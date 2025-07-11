#!/usr/bin/env python3
"""
IGED - Cryptographic Utilities
Secure cryptographic operations for biometric authentication
"""

import os
import base64
import hashlib
import secrets
import json
from pathlib import Path
from typing import Optional, Dict, Any, Union
from datetime import datetime, timedelta

try:
    from cryptography.fernet import Fernet
    from cryptography.hazmat.primitives import hashes, serialization
    from cryptography.hazmat.primitives.asymmetric import rsa, padding
    from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
    from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
    from cryptography.hazmat.backends import default_backend
    CRYPTOGRAPHY_AVAILABLE = True
except ImportError:
    print("⚠️ Cryptography library not available")
    CRYPTOGRAPHY_AVAILABLE = False

class CryptoManager:
    """Manages cryptographic operations for IGED biometric authentication"""
    
    def __init__(self, key_file: Optional[Path] = None):
        self.key_file = key_file or Path("config/secret.key")
        self.fernet = None
        self.rsa_private_key = None
        self.rsa_public_key = None
        self.initialized = False
        
        # Initialize cryptography
        self._initialize_crypto()
    
    def _initialize_crypto(self):
        """Initialize cryptographic components"""
        if not CRYPTOGRAPHY_AVAILABLE:
            print("❌ Cryptography library not available")
            return
        
        try:
            # Load or generate Fernet key
            if self.key_file.exists():
                with open(self.key_file, 'rb') as f:
                    key = f.read()
                self.fernet = Fernet(key)
            else:
                # Generate new key
                key = Fernet.generate_key()
                self.key_file.parent.mkdir(exist_ok=True)
                with open(self.key_file, 'wb') as f:
                    f.write(key)
                self.fernet = Fernet(key)
            
            # Generate RSA key pair for asymmetric operations
            self._generate_rsa_keys()
            
            self.initialized = True
            print("✅ Cryptographic manager initialized")
            
        except Exception as e:
            print(f"❌ Failed to initialize cryptography: {e}")
    
    def _generate_rsa_keys(self):
        """Generate RSA key pair"""
        try:
            # Generate private key
            self.rsa_private_key = rsa.generate_private_key(
                public_exponent=65537,
                key_size=2048,
                backend=default_backend()
            )
            
            # Get public key
            self.rsa_public_key = self.rsa_private_key.public_key()
            
        except Exception as e:
            print(f"❌ Failed to generate RSA keys: {e}")
    
    def encrypt_data(self, data: Union[str, bytes]) -> Optional[str]:
        """Encrypt data using Fernet"""
        if not self.initialized or not self.fernet:
            return None
        
        try:
            if isinstance(data, str):
                data = data.encode('utf-8')
            
            encrypted = self.fernet.encrypt(data)
            return base64.b64encode(encrypted).decode('utf-8')
            
        except Exception as e:
            print(f"❌ Encryption failed: {e}")
            return None
    
    def decrypt_data(self, encrypted_data: str) -> Optional[str]:
        """Decrypt data using Fernet"""
        if not self.initialized or not self.fernet:
            return None
        
        try:
            encrypted_bytes = base64.b64decode(encrypted_data.encode('utf-8'))
            decrypted = self.fernet.decrypt(encrypted_bytes)
            return decrypted.decode('utf-8')
            
        except Exception as e:
            print(f"❌ Decryption failed: {e}")
            return None
    
    def encrypt_asymmetric(self, data: Union[str, bytes]) -> Optional[Dict[str, str]]:
        """Encrypt data using RSA public key"""
        if not self.initialized or not self.rsa_public_key:
            return None
        
        try:
            if isinstance(data, str):
                data = data.encode('utf-8')
            
            encrypted = self.rsa_public_key.encrypt(
                data,
                padding.OAEP(
                    mgf=padding.MGF1(algorithm=hashes.SHA256()),
                    algorithm=hashes.SHA256(),
                    label=None
                )
            )
            
            return {
                'encrypted_data': base64.b64encode(encrypted).decode('utf-8'),
                'algorithm': 'RSA-OAEP-SHA256'
            }
            
        except Exception as e:
            print(f"❌ Asymmetric encryption failed: {e}")
            return None
    
    def decrypt_asymmetric(self, encrypted_data: str) -> Optional[str]:
        """Decrypt data using RSA private key"""
        if not self.initialized or not self.rsa_private_key:
            return None
        
        try:
            encrypted_bytes = base64.b64decode(encrypted_data.encode('utf-8'))
            decrypted = self.rsa_private_key.decrypt(
                encrypted_bytes,
                padding.OAEP(
                    mgf=padding.MGF1(algorithm=hashes.SHA256()),
                    algorithm=hashes.SHA256(),
                    label=None
                )
            )
            
            return decrypted.decode('utf-8')
            
        except Exception as e:
            print(f"❌ Asymmetric decryption failed: {e}")
            return None
    
    def hash_data(self, data: Union[str, bytes], algorithm: str = 'sha256') -> Optional[str]:
        """Hash data using specified algorithm"""
        try:
            if isinstance(data, str):
                data = data.encode('utf-8')
            
            if algorithm.lower() == 'sha256':
                hash_obj = hashlib.sha256(data)
            elif algorithm.lower() == 'sha512':
                hash_obj = hashlib.sha512(data)
            elif algorithm.lower() == 'md5':
                hash_obj = hashlib.md5(data)
            else:
                print(f"❌ Unsupported hash algorithm: {algorithm}")
                return None
            
            return hash_obj.hexdigest()
            
        except Exception as e:
            print(f"❌ Hashing failed: {e}")
            return None
    
    def generate_secure_token(self, length: int = 32) -> str:
        """Generate a secure random token"""
        return secrets.token_urlsafe(length)
    
    def derive_key_from_password(self, password: str, salt: Optional[bytes] = None) -> Optional[bytes]:
        """Derive a key from password using PBKDF2"""
        if not CRYPTOGRAPHY_AVAILABLE:
            return None
        
        try:
            if salt is None:
                salt = os.urandom(16)
            
            kdf = PBKDF2HMAC(
                algorithm=hashes.SHA256(),
                length=32,
                salt=salt,
                iterations=100000,
                backend=default_backend()
            )
            
            key = kdf.derive(password.encode('utf-8'))
            return key
            
        except Exception as e:
            print(f"❌ Key derivation failed: {e}")
            return None
    
    def encrypt_credentials(self, credentials: Dict[str, Any]) -> Optional[str]:
        """Encrypt credential data"""
        try:
            # Convert to JSON
            json_data = json.dumps(credentials, sort_keys=True)
            
            # Encrypt
            encrypted = self.encrypt_data(json_data)
            return encrypted
            
        except Exception as e:
            print(f"❌ Credential encryption failed: {e}")
            return None
    
    def decrypt_credentials(self, encrypted_credentials: str) -> Optional[Dict[str, Any]]:
        """Decrypt credential data"""
        try:
            # Decrypt
            decrypted = self.decrypt_data(encrypted_credentials)
            if decrypted is None:
                return None
            
            # Parse JSON
            credentials = json.loads(decrypted)
            return credentials
            
        except Exception as e:
            print(f"❌ Credential decryption failed: {e}")
            return None
    
    def sign_data(self, data: Union[str, bytes]) -> Optional[Dict[str, str]]:
        """Sign data using RSA private key"""
        if not self.initialized or not self.rsa_private_key:
            return None
        
        try:
            if isinstance(data, str):
                data = data.encode('utf-8')
            
            signature = self.rsa_private_key.sign(
                data,
                padding.PSS(
                    mgf=padding.MGF1(hashes.SHA256()),
                    salt_length=padding.PSS.MAX_LENGTH
                ),
                hashes.SHA256()
            )
            
            return {
                'signature': base64.b64encode(signature).decode('utf-8'),
                'algorithm': 'RSA-PSS-SHA256'
            }
            
        except Exception as e:
            print(f"❌ Signing failed: {e}")
            return None
    
    def verify_signature(self, data: Union[str, bytes], signature: str) -> bool:
        """Verify signature using RSA public key"""
        if not self.initialized or not self.rsa_public_key:
            return False
        
        try:
            if isinstance(data, str):
                data = data.encode('utf-8')
            
            signature_bytes = base64.b64decode(signature.encode('utf-8'))
            
            self.rsa_public_key.verify(
                signature_bytes,
                data,
                padding.PSS(
                    mgf=padding.MGF1(hashes.SHA256()),
                    salt_length=padding.PSS.MAX_LENGTH
                ),
                hashes.SHA256()
            )
            
            return True
            
        except Exception as e:
            print(f"❌ Signature verification failed: {e}")
            return False
    
    def export_public_key(self) -> Optional[str]:
        """Export RSA public key as PEM"""
        if not self.initialized or not self.rsa_public_key:
            return None
        
        try:
            pem = self.rsa_public_key.public_bytes(
                encoding=serialization.Encoding.PEM,
                format=serialization.PublicFormat.SubjectPublicKeyInfo
            )
            return pem.decode('utf-8')
            
        except Exception as e:
            print(f"❌ Public key export failed: {e}")
            return None
    
    def import_public_key(self, pem_data: str) -> bool:
        """Import RSA public key from PEM"""
        if not CRYPTOGRAPHY_AVAILABLE:
            return False
        
        try:
            self.rsa_public_key = serialization.load_pem_public_key(
                pem_data.encode('utf-8'),
                backend=default_backend()
            )
            return True
            
        except Exception as e:
            print(f"❌ Public key import failed: {e}")
            return False
    
    def get_key_fingerprint(self) -> Optional[str]:
        """Get fingerprint of the current encryption key"""
        if not self.initialized or not self.fernet:
            return None
        
        try:
            # Get the key and hash it
            key = base64.urlsafe_b64decode(self.fernet._encryption_key + b'=')
            return hashlib.sha256(key).hexdigest()[:16]
            
        except Exception as e:
            print(f"❌ Key fingerprint generation failed: {e}")
            return None

def create_secure_backup(credentials: Dict[str, Any], backup_path: Path) -> bool:
    """Create a secure backup of credentials"""
    try:
        crypto = CryptoManager()
        if not crypto.initialized:
            return False
        
        # Encrypt credentials
        encrypted = crypto.encrypt_credentials(credentials)
        if encrypted is None:
            return False
        
        # Create backup data
        backup_data = {
            'timestamp': datetime.now().isoformat(),
            'version': '1.0.0',
            'encrypted_credentials': encrypted,
            'key_fingerprint': crypto.get_key_fingerprint()
        }
        
        # Save backup
        backup_path.parent.mkdir(exist_ok=True)
        with open(backup_path, 'w') as f:
            json.dump(backup_data, f, indent=2)
        
        return True
        
    except Exception as e:
        print(f"❌ Backup creation failed: {e}")
        return False

def restore_secure_backup(backup_path: Path) -> Optional[Dict[str, Any]]:
    """Restore credentials from secure backup"""
    try:
        crypto = CryptoManager()
        if not crypto.initialized:
            return None
        
        # Load backup
        with open(backup_path, 'r') as f:
            backup_data = json.load(f)
        
        # Verify key fingerprint
        if backup_data.get('key_fingerprint') != crypto.get_key_fingerprint():
            print("❌ Key fingerprint mismatch - backup cannot be restored")
            return None
        
        # Decrypt credentials
        encrypted_credentials = backup_data['encrypted_credentials']
        credentials = crypto.decrypt_credentials(encrypted_credentials)
        
        return credentials
        
    except Exception as e:
        print(f"❌ Backup restoration failed: {e}")
        return None

# Test functions
def test_crypto_manager():
    """Test cryptographic manager functionality"""
    print("🔐 Testing Cryptographic Manager")
    print("=" * 40)
    
    if not CRYPTOGRAPHY_AVAILABLE:
        print("❌ Cryptography library not available")
        return False
    
    try:
        crypto = CryptoManager()
        
        if not crypto.initialized:
            print("❌ Crypto manager not initialized")
            return False
        
        # Test symmetric encryption
        test_data = "Hello, IGED!"
        encrypted = crypto.encrypt_data(test_data)
        decrypted = crypto.decrypt_data(encrypted)
        
        if decrypted == test_data:
            print("✅ Symmetric encryption/decryption: OK")
        else:
            print("❌ Symmetric encryption/decryption: FAILED")
            return False
        
        # Test asymmetric encryption
        asym_encrypted = crypto.encrypt_asymmetric(test_data)
        if asym_encrypted:
            asym_decrypted = crypto.decrypt_asymmetric(asym_encrypted['encrypted_data'])
            if asym_decrypted == test_data:
                print("✅ Asymmetric encryption/decryption: OK")
            else:
                print("❌ Asymmetric encryption/decryption: FAILED")
                return False
        
        # Test hashing
        hash_result = crypto.hash_data(test_data)
        if hash_result:
            print("✅ Hashing: OK")
        else:
            print("❌ Hashing: FAILED")
            return False
        
        # Test signing
        signature = crypto.sign_data(test_data)
        if signature:
            verified = crypto.verify_signature(test_data, signature['signature'])
            if verified:
                print("✅ Digital signing/verification: OK")
            else:
                print("❌ Digital signing/verification: FAILED")
                return False
        
        print("🎉 All cryptographic tests passed!")
        return True
        
    except Exception as e:
        print(f"❌ Crypto test failed: {e}")
        return False

if __name__ == "__main__":
    test_crypto_manager() 