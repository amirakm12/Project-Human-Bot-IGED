#!/usr/bin/env python3
"""
IGED Secure Network Module
Provides TLS/HTTPS support, authentication, and websocket communication
"""

import ssl
import asyncio
import json
import jwt
import hashlib
import hmac
import secrets
from pathlib import Path
from typing import Dict, Any, Optional, List, Callable
from datetime import datetime, timedelta
import logging
from dataclasses import dataclass
import aiohttp
from aiohttp import web
import socketio
from cryptography import x509
from cryptography.x509.oid import NameOID
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.backends import default_backend

logger = logging.getLogger(__name__)

@dataclass
class SecurityConfig:
    """Security configuration for network endpoints"""
    enable_tls: bool = True
    cert_file: Optional[str] = None
    key_file: Optional[str] = None
    ca_file: Optional[str] = None
    verify_mode: ssl.VerifyMode = ssl.CERT_REQUIRED
    jwt_secret: str = None
    jwt_algorithm: str = "HS256"
    token_expiry: int = 3600  # seconds
    enable_cors: bool = True
    allowed_origins: List[str] = None
    rate_limit: int = 100  # requests per minute
    
    def __post_init__(self):
        if self.jwt_secret is None:
            self.jwt_secret = secrets.token_urlsafe(32)
        if self.allowed_origins is None:
            self.allowed_origins = ["http://localhost:*", "https://localhost:*"]

class CertificateManager:
    """Manages SSL/TLS certificates"""
    
    def __init__(self, cert_dir: Path = Path("certs")):
        self.cert_dir = cert_dir
        self.cert_dir.mkdir(exist_ok=True)
    
    def generate_self_signed_cert(self, hostname: str = "localhost") -> tuple:
        """Generate self-signed certificate for development"""
        # Generate private key
        private_key = rsa.generate_private_key(
            public_exponent=65537,
            key_size=2048,
            backend=default_backend()
        )
        
        # Generate certificate
        subject = issuer = x509.Name([
            x509.NameAttribute(NameOID.COUNTRY_NAME, "US"),
            x509.NameAttribute(NameOID.STATE_OR_PROVINCE_NAME, "State"),
            x509.NameAttribute(NameOID.LOCALITY_NAME, "City"),
            x509.NameAttribute(NameOID.ORGANIZATION_NAME, "IGED"),
            x509.NameAttribute(NameOID.COMMON_NAME, hostname),
        ])
        
        cert = x509.CertificateBuilder().subject_name(
            subject
        ).issuer_name(
            issuer
        ).public_key(
            private_key.public_key()
        ).serial_number(
            x509.random_serial_number()
        ).not_valid_before(
            datetime.utcnow()
        ).not_valid_after(
            datetime.utcnow() + timedelta(days=365)
        ).add_extension(
            x509.SubjectAlternativeName([
                x509.DNSName(hostname),
                x509.DNSName("*.localhost"),
                x509.IPAddress(ipaddress.IPv4Address("127.0.0.1")),
            ]),
            critical=False,
        ).sign(private_key, hashes.SHA256(), default_backend())
        
        # Save certificate and key
        cert_path = self.cert_dir / f"{hostname}.crt"
        key_path = self.cert_dir / f"{hostname}.key"
        
        with open(cert_path, "wb") as f:
            f.write(cert.public_bytes(serialization.Encoding.PEM))
        
        with open(key_path, "wb") as f:
            f.write(private_key.private_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PrivateFormat.TraditionalOpenSSL,
                encryption_algorithm=serialization.NoEncryption()
            ))
        
        return str(cert_path), str(key_path)
    
    def create_ssl_context(self, config: SecurityConfig) -> ssl.SSLContext:
        """Create SSL context for secure connections"""
        context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
        
        if config.cert_file and config.key_file:
            context.load_cert_chain(config.cert_file, config.key_file)
        else:
            # Generate self-signed cert if not provided
            cert_file, key_file = self.generate_self_signed_cert()
            context.load_cert_chain(cert_file, key_file)
        
        if config.ca_file:
            context.load_verify_locations(config.ca_file)
        
        context.verify_mode = config.verify_mode
        context.check_hostname = False  # For development
        
        # Set strong ciphers
        context.set_ciphers('ECDHE+AESGCM:ECDHE+CHACHA20:DHE+AESGCM:DHE+CHACHA20:!aNULL:!MD5:!DSS')
        
        return context

class AuthenticationManager:
    """Manages authentication and authorization"""
    
    def __init__(self, config: SecurityConfig):
        self.config = config
        self.active_tokens = {}
        self.refresh_tokens = {}
    
    def generate_token(self, user_id: str, role: str = "user") -> Dict[str, str]:
        """Generate JWT access and refresh tokens"""
        now = datetime.utcnow()
        
        # Access token
        access_payload = {
            "user_id": user_id,
            "role": role,
            "iat": now,
            "exp": now + timedelta(seconds=self.config.token_expiry)
        }
        access_token = jwt.encode(access_payload, self.config.jwt_secret, self.config.jwt_algorithm)
        
        # Refresh token
        refresh_payload = {
            "user_id": user_id,
            "iat": now,
            "exp": now + timedelta(days=7)
        }
        refresh_token = jwt.encode(refresh_payload, self.config.jwt_secret + "_refresh", self.config.jwt_algorithm)
        
        # Store tokens
        self.active_tokens[access_token] = user_id
        self.refresh_tokens[refresh_token] = user_id
        
        return {
            "access_token": access_token,
            "refresh_token": refresh_token,
            "token_type": "Bearer",
            "expires_in": self.config.token_expiry
        }
    
    def verify_token(self, token: str) -> Optional[Dict[str, Any]]:
        """Verify JWT token"""
        try:
            payload = jwt.decode(token, self.config.jwt_secret, algorithms=[self.config.jwt_algorithm])
            
            # Check if token is still active
            if token not in self.active_tokens:
                return None
            
            return payload
        except jwt.ExpiredSignatureError:
            logger.warning("Token expired")
            return None
        except jwt.InvalidTokenError as e:
            logger.warning(f"Invalid token: {e}")
            return None
    
    def revoke_token(self, token: str):
        """Revoke a token"""
        if token in self.active_tokens:
            del self.active_tokens[token]
    
    def authenticate_request(self, request: web.Request) -> Optional[Dict[str, Any]]:
        """Authenticate HTTP request"""
        auth_header = request.headers.get("Authorization", "")
        
        if not auth_header.startswith("Bearer "):
            return None
        
        token = auth_header[7:]  # Remove "Bearer " prefix
        return self.verify_token(token)

class SecureWebSocketServer:
    """Secure WebSocket server with authentication"""
    
    def __init__(self, config: SecurityConfig):
        self.config = config
        self.auth_manager = AuthenticationManager(config)
        self.cert_manager = CertificateManager()
        self.sio = socketio.AsyncServer(
            async_mode='aiohttp',
            cors_allowed_origins=config.allowed_origins if config.enable_cors else []
        )
        self.app = web.Application()
        self.sio.attach(self.app)
        self.clients = {}
        
        # Setup event handlers
        self.setup_handlers()
    
    def setup_handlers(self):
        """Setup WebSocket event handlers"""
        
        @self.sio.event
        async def connect(sid, environ, auth):
            """Handle client connection"""
            logger.info(f"Client {sid} connected")
            
            # Verify authentication
            if auth and 'token' in auth:
                user_data = self.auth_manager.verify_token(auth['token'])
                if user_data:
                    self.clients[sid] = user_data
                    await self.sio.emit('authenticated', {'status': 'success'}, to=sid)
                    return True
            
            logger.warning(f"Client {sid} authentication failed")
            return False
        
        @self.sio.event
        async def disconnect(sid):
            """Handle client disconnection"""
            logger.info(f"Client {sid} disconnected")
            if sid in self.clients:
                del self.clients[sid]
        
        @self.sio.event
        async def message(sid, data):
            """Handle incoming message"""
            if sid not in self.clients:
                await self.sio.emit('error', {'message': 'Not authenticated'}, to=sid)
                return
            
            logger.info(f"Message from {sid}: {data}")
            
            # Process message
            response = await self.process_message(sid, data)
            await self.sio.emit('response', response, to=sid)
        
        @self.sio.event
        async def subscribe(sid, channel):
            """Subscribe to a channel"""
            if sid not in self.clients:
                return
            
            await self.sio.enter_room(sid, channel)
            logger.info(f"Client {sid} subscribed to {channel}")
    
    async def process_message(self, sid: str, data: Dict[str, Any]) -> Dict[str, Any]:
        """Process incoming WebSocket message"""
        # Implement message processing logic
        return {
            "status": "success",
            "echo": data,
            "timestamp": datetime.utcnow().isoformat()
        }
    
    async def broadcast(self, event: str, data: Any, room: Optional[str] = None):
        """Broadcast message to clients"""
        await self.sio.emit(event, data, room=room)
    
    async def start(self, host: str = "0.0.0.0", port: int = 8443):
        """Start secure WebSocket server"""
        # Setup SSL
        ssl_context = None
        if self.config.enable_tls:
            ssl_context = self.cert_manager.create_ssl_context(self.config)
        
        # Setup routes
        self.setup_routes()
        
        # Start server
        runner = web.AppRunner(self.app)
        await runner.setup()
        
        site = web.TCPSite(runner, host, port, ssl_context=ssl_context)
        await site.start()
        
        logger.info(f"Secure WebSocket server started on {host}:{port}")
    
    def setup_routes(self):
        """Setup HTTP routes"""
        
        async def index(request):
            """Serve index page"""
            return web.Response(text="IGED Secure WebSocket Server", content_type="text/html")
        
        async def auth(request):
            """Authentication endpoint"""
            data = await request.json()
            username = data.get("username")
            password = data.get("password")
            
            # Implement actual authentication logic
            if username and password:  # Simplified for example
                tokens = self.auth_manager.generate_token(username)
                return web.json_response(tokens)
            
            return web.json_response({"error": "Invalid credentials"}, status=401)
        
        self.app.router.add_get("/", index)
        self.app.router.add_post("/auth", auth)

class SecureAdminPanel:
    """Secure Flask admin panel with HTTPS and authentication"""
    
    def __init__(self, config: SecurityConfig):
        from flask import Flask, request, jsonify, render_template_string
        from flask_cors import CORS
        
        self.config = config
        self.app = Flask(__name__)
        self.auth_manager = AuthenticationManager(config)
        self.cert_manager = CertificateManager()
        
        # Setup CORS
        if config.enable_cors:
            CORS(self.app, origins=config.allowed_origins)
        
        # Setup routes
        self.setup_routes()
    
    def require_auth(self, f):
        """Decorator for routes requiring authentication"""
        from functools import wraps
        from flask import request, jsonify
        
        @wraps(f)
        def decorated(*args, **kwargs):
            auth_header = request.headers.get("Authorization", "")
            
            if not auth_header.startswith("Bearer "):
                return jsonify({"error": "Missing authentication"}), 401
            
            token = auth_header[7:]
            user_data = self.auth_manager.verify_token(token)
            
            if not user_data:
                return jsonify({"error": "Invalid token"}), 401
            
            request.user = user_data
            return f(*args, **kwargs)
        
        return decorated
    
    def setup_routes(self):
        """Setup admin panel routes"""
        
        @self.app.route("/")
        def index():
            """Admin panel index"""
            return """
            <!DOCTYPE html>
            <html>
            <head>
                <title>IGED Admin Panel</title>
                <style>
                    body { font-family: Arial; padding: 20px; background: #1a1a1a; color: #fff; }
                    .container { max-width: 1200px; margin: 0 auto; }
                    .status { padding: 10px; background: #2a2a2a; border-radius: 5px; margin: 10px 0; }
                </style>
            </head>
            <body>
                <div class="container">
                    <h1>IGED Secure Admin Panel</h1>
                    <div class="status">
                        <h2>System Status</h2>
                        <p>🔒 Secure connection (HTTPS)</p>
                        <p>✅ Authentication enabled</p>
                        <p>🛡️ CORS protection active</p>
                    </div>
                </div>
            </body>
            </html>
            """
        
        @self.app.route("/api/login", methods=["POST"])
        def login():
            """Login endpoint"""
            from flask import request, jsonify
            
            data = request.json
            username = data.get("username")
            password = data.get("password")
            
            # Implement actual authentication
            if username and password:  # Simplified
                tokens = self.auth_manager.generate_token(username, role="admin")
                return jsonify(tokens)
            
            return jsonify({"error": "Invalid credentials"}), 401
        
        @self.app.route("/api/status")
        @self.require_auth
        def status():
            """System status endpoint"""
            from flask import jsonify
            
            return jsonify({
                "status": "online",
                "timestamp": datetime.utcnow().isoformat(),
                "user": request.user["user_id"],
                "role": request.user["role"]
            })
        
        @self.app.route("/api/config")
        @self.require_auth
        def config():
            """Configuration endpoint"""
            from flask import jsonify
            
            if request.user["role"] != "admin":
                return jsonify({"error": "Admin access required"}), 403
            
            return jsonify({
                "tls_enabled": self.config.enable_tls,
                "cors_enabled": self.config.enable_cors,
                "rate_limit": self.config.rate_limit
            })
    
    def run(self, host: str = "0.0.0.0", port: int = 8443):
        """Run secure admin panel"""
        ssl_context = None
        
        if self.config.enable_tls:
            ssl_context = self.cert_manager.create_ssl_context(self.config)
            # Convert SSLContext to tuple for Flask
            if not self.config.cert_file:
                cert_file, key_file = self.cert_manager.generate_self_signed_cert()
            else:
                cert_file = self.config.cert_file
                key_file = self.config.key_file
            ssl_context = (cert_file, key_file)
        
        self.app.run(
            host=host,
            port=port,
            ssl_context=ssl_context,
            debug=False
        )

class AsyncSecureClient:
    """Async secure client for network communication"""
    
    def __init__(self, config: SecurityConfig):
        self.config = config
        self.session = None
        self.auth_token = None
    
    async def connect(self, url: str, username: str, password: str):
        """Connect to secure server"""
        # Create SSL context
        ssl_context = ssl.create_default_context()
        
        if not self.config.enable_tls:
            ssl_context = None
        elif self.config.ca_file:
            ssl_context.load_verify_locations(self.config.ca_file)
        
        # Create session
        connector = aiohttp.TCPConnector(ssl=ssl_context)
        self.session = aiohttp.ClientSession(connector=connector)
        
        # Authenticate
        async with self.session.post(f"{url}/auth", json={
            "username": username,
            "password": password
        }) as resp:
            if resp.status == 200:
                data = await resp.json()
                self.auth_token = data["access_token"]
                return True
        
        return False
    
    async def request(self, method: str, url: str, **kwargs):
        """Make authenticated request"""
        if not self.session:
            raise RuntimeError("Not connected")
        
        headers = kwargs.get("headers", {})
        if self.auth_token:
            headers["Authorization"] = f"Bearer {self.auth_token}"
        kwargs["headers"] = headers
        
        async with self.session.request(method, url, **kwargs) as resp:
            return await resp.json()
    
    async def close(self):
        """Close connection"""
        if self.session:
            await self.session.close()

# Global security configuration
_security_config = None

def get_security_config() -> SecurityConfig:
    """Get or create security configuration"""
    global _security_config
    if _security_config is None:
        _security_config = SecurityConfig()
    return _security_config

if __name__ == "__main__":
    import ipaddress
    
    # Test certificate generation
    cert_manager = CertificateManager()
    cert_file, key_file = cert_manager.generate_self_signed_cert()
    print(f"Generated certificate: {cert_file}")
    print(f"Generated key: {key_file}")
    
    # Test authentication
    config = get_security_config()
    auth_manager = AuthenticationManager(config)
    
    tokens = auth_manager.generate_token("test_user", "admin")
    print(f"Generated tokens: {tokens}")
    
    # Verify token
    user_data = auth_manager.verify_token(tokens["access_token"])
    print(f"Verified user: {user_data}")