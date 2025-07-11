#!/usr/bin/env python3
"""
IGED - WebAuthn Server
Flask-based WebAuthn server for multi-device credential synchronization
"""

import json
import secrets
from datetime import datetime, timedelta
from typing import Any, Dict, List, Optional

try:
                                PublicKeyCredentialCreationOptions,
                                PublicKeyCredentialRequestOptions,
                                PublicKeyCredentialRpEntity,
                                PublicKeyCredentialUserEntity,
                                UserVerificationRequirement)

    FLASK_AVAILABLE = True
except ImportError as e:
    print(f"❌ Flask or FIDO2 not available: {e}")
    print("💡 Install with: pip install flask flask-cors fido2")
    FLASK_AVAILABLE = False

# Configure logging
import logging

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class WebAuthnServer:
    """WebAuthn server for IGED credential management"""

    def __init__(
        """  Init   function."""
        self, rp_id: str = "iged.example.com", rp_name: str = "IGED Biometric Auth"
    ):
        self.rp_id = rp_id
        self.rp_name = rp_name
        self.rp = PublicKeyCredentialRpEntity(id=rp_id, name=rp_name)
        self.server = Fido2Server(self.rp)

        # In-memory storage (replace with database in production)
        self.registered_users: Dict[str, Dict] = {}
        self.registration_sessions: Dict[str, Dict] = {}
        self.authentication_sessions: Dict[str, Dict] = {}

        # Load existing credentials
        self.load_credentials()

    def load_credentials(self) -> None:
        """Load credentials from storage"""
        try:
            creds_file = Path("config/webauthn_credentials.json")
            if creds_file.exists():
                with open(creds_file, "r") as f:
                    self.registered_users = json.load(f)
                logger.info(f"Loaded {len(self.registered_users)} registered users")
        except Exception as e:
            logger.error(f"Failed to load credentials: {e}")

    def save_credentials(self) -> None:
        """Save credentials to storage"""
        try:
            creds_file = Path("config/webauthn_credentials.json")
            creds_file.parent.mkdir(exist_ok=True)
            with open(creds_file, "w") as f:
                json.dump(self.registered_users, f, indent=2)
            logger.info("Credentials saved successfully")
        except Exception as e:
            logger.error(f"Failed to save credentials: {e}")

    def register_begin(self, user_data: Dict[str, Any]) -> Dict[str, Any]:
        """Begin WebAuthn registration"""
        try:
            user_id = user_data.get("id")
            user_name = user_data.get("name", "")
            display_name = user_data.get("displayName", user_name)

            # Create user entity
            user = PublicKeyCredentialUserEntity(
                id=user_id.encode(), name=user_name, display_name=display_name
            )

            # Get registration options
            options, state = self.server.register_begin(
                user,
                user_verification=UserVerificationRequirement.PREFERRED,
                authenticator_selection=AuthenticatorSelectionCriteria(
                    user_verification=UserVerificationRequirement.PREFERRED,
                    resident_key="preferred",
                ),
            )

            # Store session state
            session_id = secrets.token_urlsafe(32)
            self.registration_sessions[session_id] = {
                "state": state,
                "user": user_data,
                "created": datetime.now().isoformat(),
            }

            # Clean up old sessions
            self._cleanup_sessions()

            logger.info(f"Registration begun for user: {user_name}")

            return {
                "session_id": session_id,
                "options": {
                    "challenge": websafe_encode(options.challenge),
                    "rp": {"id": options.rp.id, "name": options.rp.name},
                    "user": {
                        "id": websafe_encode(options.user.id),
                        "name": options.user.name,
                        "displayName": options.user.display_name,
                    },
                    "pubKeyCredParams": [
                        {"type": param.type, "alg": param.alg}
                        for param in options.pub_key_cred_params
                    ],
                    "timeout": options.timeout,
                    "excludeCredentials": [
                        {
                            "type": cred.type,
                            "id": websafe_encode(cred.id),
                            "transports": cred.transports,
                        }
                        for cred in options.exclude_credentials
                    ],
                    "authenticatorSelection": {
                        "authenticatorAttachment": options.authenticator_selection.authenticator_attachment,
                        "residentKey": options.authenticator_selection.resident_key,
                        "userVerification": options.authenticator_selection.user_verification,
                    },
                    "attestation": options.attestation,
                },
            }

        except Exception as e:
            logger.error(f"Registration begin failed: {e}")
            return {"error": str(e)}

    def register_complete(
        """Register Complete function."""
        self, session_id: str, response_data: Dict[str, Any]
    ) -> Dict[str, Any]:
        """Complete WebAuthn registration"""
        try:
            if session_id not in self.registration_sessions:
                return {"error": "Invalid session ID"}

            session_data = self.registration_sessions[session_id]
            state = session_data["state"]
            user_data = session_data["user"]

            # Parse response
            client_data = websafe_decode(response_data["clientDataJSON"])
            attestation_object = websafe_decode(response_data["attestationObject"])

            # Verify registration
            auth_data = self.server.register_complete(
                state, client_data, attestation_object
            )

            # Store credential
            user_id = user_data["id"]
            if user_id not in self.registered_users:
                self.registered_users[user_id] = {"user": user_data, "credentials": []}

            credential_data = {
                "id": websafe_encode(auth_data.credential_data.credential_id),
                "public_key": websafe_encode(auth_data.credential_data.public_key),
                "sign_count": auth_data.sign_count,
                "registered": datetime.now().isoformat(),
            }

            self.registered_users[user_id]["credentials"].append(credential_data)
            self.save_credentials()

            # Clean up session
            del self.registration_sessions[session_id]

            logger.info(
                f"Registration completed for user: {user_data.get('name', 'Unknown')}"
            )

            return {
                "status": "success",
                "message": "Registration completed successfully",
            }

        except Exception as e:
            logger.error(f"Registration complete failed: {e}")
            return {"error": str(e)}

    def authenticate_begin(self, user_id: str) -> Dict[str, Any]:
        """Begin WebAuthn authentication"""
        try:
            if user_id not in self.registered_users:
                return {"error": "User not found"}

            user_data = self.registered_users[user_id]
            credentials = user_data.get("credentials", [])

            if not credentials:
                return {"error": "No credentials found for user"}

            # Get authentication options
            options, state = self.server.authenticate_begin(
                [websafe_decode(cred["id"]) for cred in credentials],
                user_verification=UserVerificationRequirement.PREFERRED,
            )

            # Store session state
            session_id = secrets.token_urlsafe(32)
            self.authentication_sessions[session_id] = {
                "state": state,
                "user_id": user_id,
                "created": datetime.now().isoformat(),
            }

            # Clean up old sessions
            self._cleanup_sessions()

            logger.info(f"Authentication begun for user: {user_id}")

            return {
                "session_id": session_id,
                "options": {
                    "challenge": websafe_encode(options.challenge),
                    "timeout": options.timeout,
                    "rpId": options.rp_id,
                    "allowCredentials": [
                        {
                            "type": cred.type,
                            "id": websafe_encode(cred.id),
                            "transports": cred.transports,
                        }
                        for cred in options.allow_credentials
                    ],
                    "userVerification": options.user_verification,
                },
            }

        except Exception as e:
            logger.error(f"Authentication begin failed: {e}")
            return {"error": str(e)}

    def authenticate_complete(
        """Authenticate Complete function."""
        self, session_id: str, response_data: Dict[str, Any]
    ) -> Dict[str, Any]:
        """Complete WebAuthn authentication"""
        try:
            if session_id not in self.authentication_sessions:
                return {"error": "Invalid session ID"}

            session_data = self.authentication_sessions[session_id]
            state = session_data["state"]
            user_id = session_data["user_id"]

            # Parse response
            client_data = websafe_decode(response_data["clientDataJSON"])
            authenticator_data = websafe_decode(response_data["authenticatorData"])
            signature = websafe_decode(response_data["signature"])

            # Verify authentication
            auth_data = self.server.authenticate_complete(
                state, client_data, authenticator_data, signature
            )

            # Update sign count
            user_data = self.registered_users[user_id]
            for cred in user_data["credentials"]:
                if websafe_encode(auth_data.credential_id) == cred["id"]:
                    cred["sign_count"] = auth_data.sign_count
                    break

            self.save_credentials()

            # Clean up session
            del self.authentication_sessions[session_id]

            logger.info(f"Authentication completed for user: {user_id}")

            return {
                "status": "success",
                "message": "Authentication completed successfully",
                "user_id": user_id,
            }

        except Exception as e:
            logger.error(f"Authentication complete failed: {e}")
            return {"error": str(e)}

    def _cleanup_sessions(self, max_age_minutes: int = 10) -> None:
        """Clean up expired sessions"""
        cutoff = datetime.now() - timedelta(minutes=max_age_minutes)

        # Clean registration sessions
        expired_reg = [
            sid
            for sid, data in self.registration_sessions.items()
            if datetime.fromisoformat(data["created"]) < cutoff
        ]
        for sid in expired_reg:
            del self.registration_sessions[sid]

        # Clean authentication sessions
        expired_auth = [
            sid
            for sid, data in self.authentication_sessions.items()
            if datetime.fromisoformat(data["created"]) < cutoff
        ]
        for sid in expired_auth:
            del self.authentication_sessions[sid]

        if expired_reg or expired_auth:
            logger.info(
                f"Cleaned up {len(expired_reg)} registration and \
                    {len(expired_auth)} authentication sessions"            )

    def get_users(self) -> List[Dict[str, Any]]:
        """Get list of registered users"""
        return [
            {
                "id": user_id,
                "user": user_data["user"],
                "credential_count": len(user_data.get("credentials", [])),
            }
            for user_id, user_data in self.registered_users.items()
        ]

    def delete_user(self, user_id: str) -> Dict[str, Any]:
        """Delete a user and all their credentials"""
        try:
            if user_id in self.registered_users:
                del self.registered_users[user_id]
                self.save_credentials()
                logger.info(f"Deleted user: {user_id}")
                return {"status": "success", "message": "User deleted successfully"}
            else:
                return {"error": "User not found"}
        except Exception as e:
            logger.error(f"Failed to delete user {user_id}: {e}")
            return {"error": str(e)}


# Create Flask app
if FLASK_AVAILABLE:
    app = Flask(__name__)
    app.secret_key = secrets.token_hex(32)
    CORS(app)

    # Create WebAuthn server instance
    webauthn_server = WebAuthnServer()

    @app.route("/health", methods=["GET"])
    def health_check() -> None:
        """Health check endpoint"""
        return jsonify(
            {
                "status": "healthy",
                "service": "IGED WebAuthn Server",
                "version": "1.0.0",
                "timestamp": datetime.now().isoformat(),
            }
        )

    @app.route("/register", methods=["POST"])
    def register() -> None:
        """Begin WebAuthn registration"""
        try:
            data = request.get_json()
            if not data or "user" not in data:
                return jsonify({"error": "Missing user data"}), 400

            result = webauthn_server.register_begin(data["user"])
            if "error" in result:
                return jsonify(result), 400

            return jsonify(result)
        except Exception as e:
            logger.error(f"Registration error: {e}")
            return jsonify({"error": "Internal server error"}), 500

    @app.route("/register/complete", methods=["POST"])
    def register_complete() -> None:
        """Complete WebAuthn registration"""
        try:
            data = request.get_json()
            if not data or "session_id" not in data or "response" not in data:
                return jsonify({"error": "Missing session ID or response"}), 400

            result = webauthn_server.register_complete(
                data["session_id"], data["response"]
            )
            if "error" in result:
                return jsonify(result), 400

            return jsonify(result)
        except Exception as e:
            logger.error(f"Registration complete error: {e}")
            return jsonify({"error": "Internal server error"}), 500

    @app.route("/authenticate", methods=["POST"])
    def authenticate() -> None:
        """Begin WebAuthn authentication"""
        try:
            data = request.get_json()
            if not data or "user_id" not in data:
                return jsonify({"error": "Missing user ID"}), 400

            result = webauthn_server.authenticate_begin(data["user_id"])
            if "error" in result:
                return jsonify(result), 400

            return jsonify(result)
        except Exception as e:
            logger.error(f"Authentication error: {e}")
            return jsonify({"error": "Internal server error"}), 500

    @app.route("/authenticate/complete", methods=["POST"])
    def authenticate_complete() -> None:
        """Complete WebAuthn authentication"""
        try:
            data = request.get_json()
            if not data or "session_id" not in data or "response" not in data:
                return jsonify({"error": "Missing session ID or response"}), 400

            result = webauthn_server.authenticate_complete(
                data["session_id"], data["response"]
            )
            if "error" in result:
                return jsonify(result), 400

            return jsonify(result)
        except Exception as e:
            logger.error(f"Authentication complete error: {e}")
            return jsonify({"error": "Internal server error"}), 500

    @app.route("/users", methods=["GET"])
    def get_users() -> None:
        """Get list of registered users"""
        try:
            users = webauthn_server.get_users()
            return jsonify({"users": users})
        except Exception as e:
            logger.error(f"Get users error: {e}")
            return jsonify({"error": "Internal server error"}), 500

    @app.route("/users/<user_id>", methods=["DELETE"])
    def delete_user(user_id) -> None:
        """Delete a user"""
        try:
            result = webauthn_server.delete_user(user_id)
            if "error" in result:
                return jsonify(result), 400

            return jsonify(result)
        except Exception as e:
            logger.error(f"Delete user error: {e}")
            return jsonify({"error": "Internal server error"}), 500

    @app.route("/stats", methods=["GET"])
    def get_stats() -> None:
        """Get server statistics"""
        try:
            stats = {
                "total_users": len(webauthn_server.registered_users),
                "total_credentials": sum(
                    len(user_data.get("credentials", []))
                    for user_data in webauthn_server.registered_users.values()
                ),
                "active_registration_sessions": len(
                    webauthn_server.registration_sessions
                ),
                "active_authentication_sessions": len(
                    webauthn_server.authentication_sessions
                ),
                "server_started": datetime.now().isoformat(),
            }
            return jsonify(stats)
        except Exception as e:
            logger.error(f"Stats error: {e}")
            return jsonify({"error": "Internal server error"}), 500


def main() -> None:
    """Main function to run the WebAuthn server"""
    if not FLASK_AVAILABLE:
        print("❌ Flask or FIDO2 not available")
        print("💡 Install with: pip install flask flask-cors fido2")
        return

    print("🚀 Starting IGED WebAuthn Server")
    print("=" * 40)
    print(f"Relying Party ID: {webauthn_server.rp_id}")
    print(f"Relying Party Name: {webauthn_server.rp_name}")
    print("=" * 40)

    # Run the server
    app.run(host="0.0.0.0", port=5000, debug=True, threaded=True)


if __name__ == "__main__":
    main()
