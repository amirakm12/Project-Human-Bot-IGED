#!/usr/bin/env python3
"""
IGED Android Integration Module
Provides secure async WebSocket connection for Android devices
"""

import asyncio
import json
import uuid
import hashlib
import hmac
from datetime import datetime, timedelta
from typing import Dict, Any, Optional, List, Callable
from dataclasses import dataclass, field
import logging
from pathlib import Path
import aiohttp
from aiohttp import web
import socketio
import qrcode
from io import BytesIO
import base64

logger = logging.getLogger(__name__)

@dataclass
class AndroidDevice:
    """Represents a connected Android device"""
    device_id: str
    name: str
    model: str
    android_version: str
    app_version: str
    connected_at: datetime
    last_seen: datetime
    auth_token: Optional[str] = None
    session_id: Optional[str] = None
    capabilities: List[str] = field(default_factory=list)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary"""
        return {
            'device_id': self.device_id,
            'name': self.name,
            'model': self.model,
            'android_version': self.android_version,
            'app_version': self.app_version,
            'connected_at': self.connected_at.isoformat(),
            'last_seen': self.last_seen.isoformat(),
            'session_id': self.session_id,
            'capabilities': self.capabilities
        }

@dataclass
class CommandHistory:
    """Command history for Android devices"""
    device_id: str
    command: str
    timestamp: datetime
    response: Optional[Any] = None
    status: str = "pending"  # pending, success, failed
    execution_time: Optional[float] = None

class AndroidAuthenticator:
    """Handles Android device authentication"""
    
    def __init__(self, secret_key: str = None):
        self.secret_key = secret_key or uuid.uuid4().hex
        self.paired_devices = {}
        self.pending_pairs = {}
    
    def generate_pairing_code(self, device_id: str) -> str:
        """Generate a pairing code for device"""
        code = str(uuid.uuid4().hex[:8]).upper()
        self.pending_pairs[code] = {
            'device_id': device_id,
            'created_at': datetime.now(),
            'expires_at': datetime.now() + timedelta(minutes=5)
        }
        return code
    
    def generate_qr_code(self, pairing_data: Dict[str, Any]) -> str:
        """Generate QR code for pairing"""
        qr = qrcode.QRCode(version=1, box_size=10, border=5)
        qr.add_data(json.dumps(pairing_data))
        qr.make(fit=True)
        
        img = qr.make_image(fill_color="black", back_color="white")
        buffer = BytesIO()
        img.save(buffer, format='PNG')
        
        # Convert to base64
        img_str = base64.b64encode(buffer.getvalue()).decode()
        return f"data:image/png;base64,{img_str}"
    
    def verify_pairing_code(self, code: str, device_info: Dict[str, Any]) -> Optional[str]:
        """Verify pairing code and return auth token"""
        if code not in self.pending_pairs:
            return None
        
        pairing = self.pending_pairs[code]
        
        # Check expiration
        if datetime.now() > pairing['expires_at']:
            del self.pending_pairs[code]
            return None
        
        # Generate auth token
        auth_token = self._generate_auth_token(device_info)
        
        # Store paired device
        self.paired_devices[device_info['device_id']] = {
            'auth_token': auth_token,
            'paired_at': datetime.now(),
            'device_info': device_info
        }
        
        # Clean up pending pair
        del self.pending_pairs[code]
        
        return auth_token
    
    def _generate_auth_token(self, device_info: Dict[str, Any]) -> str:
        """Generate secure auth token for device"""
        data = f"{device_info['device_id']}:{datetime.now().isoformat()}"
        return hmac.new(
            self.secret_key.encode(),
            data.encode(),
            hashlib.sha256
        ).hexdigest()
    
    def verify_auth_token(self, device_id: str, token: str) -> bool:
        """Verify device auth token"""
        if device_id not in self.paired_devices:
            return False
        
        stored_token = self.paired_devices[device_id]['auth_token']
        return hmac.compare_digest(stored_token, token)

class AndroidWebSocketServer:
    """Async WebSocket server for Android communication"""
    
    def __init__(self, authenticator: Optional[AndroidAuthenticator] = None):
        self.authenticator = authenticator or AndroidAuthenticator()
        self.sio = socketio.AsyncServer(
            async_mode='aiohttp',
            cors_allowed_origins='*',
            logger=logger
        )
        self.app = web.Application()
        self.sio.attach(self.app)
        
        self.devices = {}  # Connected devices
        self.command_history = []  # Command history
        self.command_handlers = {}  # Custom command handlers
        self.cache = {}  # Memory cache for frequently accessed data
        
        self._setup_handlers()
        self._setup_routes()
    
    def _setup_handlers(self):
        """Setup WebSocket event handlers"""
        
        @self.sio.event
        async def connect(sid, environ, auth):
            """Handle device connection"""
            logger.info(f"Android device attempting connection: {sid}")
            
            if not auth or 'token' not in auth or 'device_id' not in auth:
                logger.warning(f"Connection rejected - missing auth: {sid}")
                return False
            
            # Verify authentication
            if not self.authenticator.verify_auth_token(auth['device_id'], auth['token']):
                logger.warning(f"Connection rejected - invalid auth: {sid}")
                return False
            
            # Create device instance
            device = AndroidDevice(
                device_id=auth['device_id'],
                name=auth.get('name', 'Unknown'),
                model=auth.get('model', 'Unknown'),
                android_version=auth.get('android_version', 'Unknown'),
                app_version=auth.get('app_version', 'Unknown'),
                connected_at=datetime.now(),
                last_seen=datetime.now(),
                session_id=sid,
                capabilities=auth.get('capabilities', [])
            )
            
            self.devices[sid] = device
            
            # Send welcome message
            await self.sio.emit('connected', {
                'status': 'success',
                'session_id': sid,
                'server_time': datetime.now().isoformat()
            }, to=sid)
            
            logger.info(f"Android device connected: {device.name} ({device.device_id})")
            return True
        
        @self.sio.event
        async def disconnect(sid):
            """Handle device disconnection"""
            if sid in self.devices:
                device = self.devices[sid]
                logger.info(f"Android device disconnected: {device.name} ({device.device_id})")
                del self.devices[sid]
        
        @self.sio.event
        async def command(sid, data):
            """Handle command from Android device"""
            if sid not in self.devices:
                await self.sio.emit('error', {'message': 'Not authenticated'}, to=sid)
                return
            
            device = self.devices[sid]
            device.last_seen = datetime.now()
            
            command_str = data.get('command')
            params = data.get('params', {})
            
            # Log command
            history = CommandHistory(
                device_id=device.device_id,
                command=command_str,
                timestamp=datetime.now()
            )
            
            try:
                # Execute command
                result = await self._execute_command(command_str, params, device)
                
                history.status = "success"
                history.response = result
                
                # Send response
                await self.sio.emit('command_response', {
                    'command': command_str,
                    'status': 'success',
                    'result': result
                }, to=sid)
                
            except Exception as e:
                history.status = "failed"
                history.response = str(e)
                
                await self.sio.emit('command_response', {
                    'command': command_str,
                    'status': 'error',
                    'error': str(e)
                }, to=sid)
            
            self.command_history.append(history)
        
        @self.sio.event
        async def sync_data(sid, data):
            """Handle data synchronization request"""
            if sid not in self.devices:
                return
            
            device = self.devices[sid]
            sync_type = data.get('type')
            
            if sync_type == 'memory':
                # Check cache first
                cache_key = f"memory_{device.device_id}"
                if cache_key in self.cache:
                    cached_data, cached_time = self.cache[cache_key]
                    if (datetime.now() - cached_time).seconds < 60:  # 1 minute cache
                        await self.sio.emit('sync_response', {
                            'type': 'memory',
                            'data': cached_data,
                            'cached': True
                        }, to=sid)
                        return
                
                # Get fresh data
                memory_data = await self._get_memory_data()
                
                # Update cache
                self.cache[cache_key] = (memory_data, datetime.now())
                
                await self.sio.emit('sync_response', {
                    'type': 'memory',
                    'data': memory_data,
                    'cached': False
                }, to=sid)
        
        @self.sio.event
        async def heartbeat(sid):
            """Handle heartbeat from device"""
            if sid in self.devices:
                self.devices[sid].last_seen = datetime.now()
                await self.sio.emit('heartbeat_ack', {
                    'server_time': datetime.now().isoformat()
                }, to=sid)
    
    def _setup_routes(self):
        """Setup HTTP routes for admin interface"""
        
        async def index(request):
            """Serve Android integration dashboard"""
            html = """
            <!DOCTYPE html>
            <html>
            <head>
                <title>IGED Android Integration</title>
                <style>
                    body { 
                        font-family: 'Segoe UI', Arial; 
                        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
                        color: white;
                        padding: 20px;
                    }
                    .container { 
                        max-width: 1200px; 
                        margin: 0 auto;
                        background: rgba(255,255,255,0.1);
                        border-radius: 20px;
                        padding: 30px;
                        backdrop-filter: blur(10px);
                    }
                    .device-card {
                        background: rgba(255,255,255,0.2);
                        border-radius: 10px;
                        padding: 15px;
                        margin: 10px 0;
                    }
                    .stats {
                        display: grid;
                        grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
                        gap: 20px;
                        margin: 20px 0;
                    }
                    .stat-card {
                        background: rgba(255,255,255,0.15);
                        padding: 20px;
                        border-radius: 10px;
                        text-align: center;
                    }
                    .stat-value {
                        font-size: 2em;
                        font-weight: bold;
                    }
                    button {
                        background: #4CAF50;
                        color: white;
                        border: none;
                        padding: 10px 20px;
                        border-radius: 5px;
                        cursor: pointer;
                        font-size: 16px;
                    }
                    button:hover {
                        background: #45a049;
                    }
                    #qr-code {
                        margin: 20px 0;
                        text-align: center;
                    }
                </style>
            </head>
            <body>
                <div class="container">
                    <h1>🤖 IGED Android Integration</h1>
                    
                    <div class="stats">
                        <div class="stat-card">
                            <div class="stat-value" id="device-count">0</div>
                            <div>Connected Devices</div>
                        </div>
                        <div class="stat-card">
                            <div class="stat-value" id="command-count">0</div>
                            <div>Commands Executed</div>
                        </div>
                        <div class="stat-card">
                            <div class="stat-value" id="cache-hits">0%</div>
                            <div>Cache Hit Rate</div>
                        </div>
                    </div>
                    
                    <h2>Device Pairing</h2>
                    <button onclick="generatePairingCode()">Generate Pairing Code</button>
                    <div id="pairing-info"></div>
                    <div id="qr-code"></div>
                    
                    <h2>Connected Devices</h2>
                    <div id="devices"></div>
                    
                    <h2>Recent Commands</h2>
                    <div id="commands"></div>
                </div>
                
                <script src="https://cdn.socket.io/4.5.0/socket.io.min.js"></script>
                <script>
                    const socket = io();
                    
                    function generatePairingCode() {
                        fetch('/api/pairing/generate', {method: 'POST'})
                            .then(r => r.json())
                            .then(data => {
                                document.getElementById('pairing-info').innerHTML = 
                                    `<h3>Pairing Code: ${data.code}</h3>
                                     <p>Valid for 5 minutes</p>`;
                                document.getElementById('qr-code').innerHTML = 
                                    `<img src="${data.qr_code}" alt="QR Code">`;
                            });
                    }
                    
                    function updateStats() {
                        fetch('/api/stats')
                            .then(r => r.json())
                            .then(data => {
                                document.getElementById('device-count').textContent = data.device_count;
                                document.getElementById('command-count').textContent = data.command_count;
                                document.getElementById('cache-hits').textContent = data.cache_hit_rate + '%';
                            });
                    }
                    
                    function updateDevices() {
                        fetch('/api/devices')
                            .then(r => r.json())
                            .then(devices => {
                                const html = devices.map(d => `
                                    <div class="device-card">
                                        <h3>${d.name}</h3>
                                        <p>Model: ${d.model} | Android: ${d.android_version}</p>
                                        <p>Connected: ${new Date(d.connected_at).toLocaleString()}</p>
                                        <p>Last Seen: ${new Date(d.last_seen).toLocaleString()}</p>
                                    </div>
                                `).join('');
                                document.getElementById('devices').innerHTML = html || '<p>No devices connected</p>';
                            });
                    }
                    
                    // Update every 5 seconds
                    setInterval(() => {
                        updateStats();
                        updateDevices();
                    }, 5000);
                    
                    // Initial load
                    updateStats();
                    updateDevices();
                </script>
            </body>
            </html>
            """
            return web.Response(text=html, content_type='text/html')
        
        async def generate_pairing(request):
            """Generate pairing code"""
            device_id = str(uuid.uuid4())
            code = self.authenticator.generate_pairing_code(device_id)
            
            pairing_data = {
                'server': request.host,
                'code': code,
                'device_id': device_id
            }
            
            qr_code = self.authenticator.generate_qr_code(pairing_data)
            
            return web.json_response({
                'code': code,
                'device_id': device_id,
                'qr_code': qr_code
            })
        
        async def verify_pairing(request):
            """Verify pairing code"""
            data = await request.json()
            code = data.get('code')
            device_info = data.get('device_info')
            
            token = self.authenticator.verify_pairing_code(code, device_info)
            
            if token:
                return web.json_response({
                    'status': 'success',
                    'auth_token': token
                })
            else:
                return web.json_response({
                    'status': 'error',
                    'message': 'Invalid or expired code'
                }, status=400)
        
        async def get_devices(request):
            """Get connected devices"""
            devices = [device.to_dict() for device in self.devices.values()]
            return web.json_response(devices)
        
        async def get_stats(request):
            """Get system statistics"""
            cache_hits = sum(1 for h in self.command_history if hasattr(h, 'cached') and h.cached)
            total_commands = len(self.command_history)
            
            return web.json_response({
                'device_count': len(self.devices),
                'command_count': total_commands,
                'cache_hit_rate': round((cache_hits / max(total_commands, 1)) * 100, 1),
                'paired_devices': len(self.authenticator.paired_devices)
            })
        
        # Register routes
        self.app.router.add_get('/', index)
        self.app.router.add_post('/api/pairing/generate', generate_pairing)
        self.app.router.add_post('/api/pairing/verify', verify_pairing)
        self.app.router.add_get('/api/devices', get_devices)
        self.app.router.add_get('/api/stats', get_stats)
    
    async def _execute_command(self, command: str, params: Dict[str, Any], device: AndroidDevice) -> Any:
        """Execute command from Android device"""
        # Check for custom handler
        if command in self.command_handlers:
            handler = self.command_handlers[command]
            return await handler(params, device)
        
        # Default commands
        if command == 'ping':
            return {'pong': datetime.now().isoformat()}
        
        elif command == 'get_info':
            return {
                'server_version': '1.0.0',
                'connected_devices': len(self.devices),
                'uptime': '24h'  # Placeholder
            }
        
        elif command == 'list_plugins':
            # Integration with plugin system
            try:
                from .plugin_sandbox import get_plugin_manager
                manager = get_plugin_manager()
                return manager.sandbox.list_plugins()
            except:
                return []
        
        else:
            raise ValueError(f"Unknown command: {command}")
    
    async def _get_memory_data(self) -> Dict[str, Any]:
        """Get memory data for synchronization"""
        # Placeholder - integrate with actual memory system
        return {
            'entries': [],
            'timestamp': datetime.now().isoformat()
        }
    
    def register_command_handler(self, command: str, handler: Callable):
        """Register custom command handler"""
        self.command_handlers[command] = handler
    
    async def broadcast_to_devices(self, event: str, data: Any, device_filter: Optional[Callable] = None):
        """Broadcast message to devices"""
        for sid, device in self.devices.items():
            if device_filter is None or device_filter(device):
                await self.sio.emit(event, data, to=sid)
    
    async def send_to_device(self, device_id: str, event: str, data: Any) -> bool:
        """Send message to specific device"""
        for sid, device in self.devices.items():
            if device.device_id == device_id:
                await self.sio.emit(event, data, to=sid)
                return True
        return False
    
    async def start(self, host: str = "0.0.0.0", port: int = 8765):
        """Start Android WebSocket server"""
        runner = web.AppRunner(self.app)
        await runner.setup()
        site = web.TCPSite(runner, host, port)
        await site.start()
        logger.info(f"Android WebSocket server started on {host}:{port}")

# Global Android server instance
_android_server = None

def get_android_server() -> AndroidWebSocketServer:
    """Get or create Android server"""
    global _android_server
    if _android_server is None:
        _android_server = AndroidWebSocketServer()
    return _android_server

if __name__ == "__main__":
    # Example usage
    async def main():
        server = get_android_server()
        
        # Register custom command handler
        async def custom_handler(params, device):
            return f"Custom response for {device.name}"
        
        server.register_command_handler("custom", custom_handler)
        
        # Start server
        await server.start()
        
        # Keep running
        await asyncio.Event().wait()
    
    asyncio.run(main())