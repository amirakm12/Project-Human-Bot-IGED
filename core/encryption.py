"""
Encryption Manager for IGED
Handles AES-256 encryption for sensitive data

This module provides comprehensive encryption functionality for the IGED system,
including file encryption, data encryption, and secure key management.
"""

import base64
import json
import logging
from pathlib import Path
from typing import Optional, Union, Tuple

# Cryptography imports
from cryptography.fernet import Fernet
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC

logger = logging.getLogger(__name__)


class EncryptionManager:
    """
    Comprehensive encryption manager for IGED system.
    
    This class provides AES-256 encryption capabilities for protecting
    sensitive data, files, and communications within the IGED system.
    """
    
    def __init__(self, key_path: str = "config/secret.key"):
        """
        Initialize the encryption manager.
        
        Args:
            key_path: Path to the encryption key file
        """
        self.key_path = Path(key_path)
        self.key: Optional[bytes] = None
        self.cipher: Optional[Fernet] = None
        self.initialize_encryption()
    
    def initialize_encryption(self) -> None:
        """
        Initialize encryption with key generation or loading.
        
        Raises:
            Exception: If encryption initialization fails
        """
        try:
            # Create config directory if it doesn't exist
            self.key_path.parent.mkdir(parents=True, exist_ok=True)
            
            # Load or generate encryption key
            if self.key_path.exists():
                self._load_key()
            else:
                self._generate_key()
            
            # Initialize Fernet cipher
            self.cipher = Fernet(self.key)
            logger.info("✅ Encryption initialized successfully")
            
        except Exception as e:
            logger.error(f"❌ Failed to initialize encryption: {e}")
            raise
    
    def _generate_key(self) -> None:
        """
        Generate a new encryption key.
        
        Raises:
            Exception: If key generation fails
        """
        try:
            # Generate a new Fernet key
            self.key = Fernet.generate_key()
            
            # Save key to file
            if self.key is not None:
                with open(self.key_path, 'wb') as f:
                    f.write(self.key)
            
            logger.info(f"🔑 Generated new encryption key: {self.key_path}")
            
        except Exception as e:
            logger.error(f"❌ Failed to generate encryption key: {e}")
            raise
    
    def _load_key(self) -> None:
        """
        Load encryption key from file.
        
        Raises:
            Exception: If key loading fails
        """
        try:
            with open(self.key_path, 'rb') as f:
                self.key = f.read()
            
            logger.info(f"🔑 Loaded encryption key from: {self.key_path}")
            
        except Exception as e:
            logger.error(f"❌ Failed to load encryption key: {e}")
            raise
    
    def encrypt(self, data: Union[str, bytes]) -> str:
        """
        Encrypt data using AES-256 encryption.
        
        Args:
            data: Data to encrypt (string or bytes)
            
        Returns:
            Base64 encoded encrypted data
            
        Raises:
            ValueError: If encryption is not initialized
            Exception: If encryption fails
        """
        try:
            if not self.cipher:
                raise ValueError("Encryption not initialized")
            
            # Convert string to bytes if necessary
            if isinstance(data, str):
                data = data.encode('utf-8')
            
            # Encrypt using Fernet
            encrypted_data = self.cipher.encrypt(data)
            
            # Return base64 encoded string
            return base64.b64encode(encrypted_data).decode('utf-8')
            
        except Exception as e:
            logger.error(f"❌ Encryption failed: {e}")
            raise
    
    def decrypt(self, encrypted_data: Union[str, bytes]) -> str:
        """
        Decrypt data using AES-256 encryption.
        
        Args:
            encrypted_data: Base64 encoded encrypted data
            
        Returns:
            Decrypted data as string
            
        Raises:
            ValueError: If encryption is not initialized
            Exception: If decryption fails
        """
        try:
            if not self.cipher:
                raise ValueError("Encryption not initialized")
            
            # Convert string to bytes if necessary
            if isinstance(encrypted_data, str):
                encrypted_data = base64.b64decode(encrypted_data.encode('utf-8'))
            
            # Decrypt using Fernet
            decrypted_data = self.cipher.decrypt(encrypted_data)
            
            # Return as string
            return decrypted_data.decode('utf-8')
            
        except Exception as e:
            logger.error(f"❌ Decryption failed: {e}")
            raise
    
    def encrypt_file(self, file_path: str, output_path: Optional[str] = None) -> str:
        """
        Encrypt a file.
        
        Args:
            file_path: Path to file to encrypt
            output_path: Optional output path for encrypted file
            
        Returns:
            Path to encrypted file
            
        Raises:
            Exception: If file encryption fails
        """
        try:
            file_path_obj = Path(file_path)
            
            if not file_path_obj.exists():
                raise FileNotFoundError(f"File not found: {file_path_obj}")
            
            # Determine output path
            if output_path is None:
                output_path_obj = file_path_obj.with_suffix(file_path_obj.suffix + '.encrypted')
            else:
                output_path_obj = Path(output_path)
            
            # Read and encrypt file
            with open(file_path_obj, 'rb') as f:
                file_data = f.read()
            
            encrypted_data = self.encrypt(file_data)
            
            # Write encrypted file
            with open(output_path_obj, 'w', encoding='utf-8') as f:
                f.write(encrypted_data)
            
            logger.info(f"📄 File encrypted: {file_path_obj} -> {output_path_obj}")
            return str(output_path_obj)
            
        except Exception as e:
            logger.error(f"❌ File encryption failed: {e}")
            raise
    
    def decrypt_file(self, file_path: str, output_path: Optional[str] = None) -> str:
        """
        Decrypt a file.
        
        Args:
            file_path: Path to encrypted file
            output_path: Optional output path for decrypted file
            
        Returns:
            Path to decrypted file
            
        Raises:
            Exception: If file decryption fails
        """
        try:
            file_path_obj = Path(file_path)
            
            if not file_path_obj.exists():
                raise FileNotFoundError(f"File not found: {file_path_obj}")
            
            # Determine output path
            if output_path is None:
                if file_path_obj.suffix == '.encrypted':
                    output_path_obj = file_path_obj.with_suffix('')
                else:
                    output_path_obj = file_path_obj.with_suffix('.decrypted')
            else:
                output_path_obj = Path(output_path)
            
            # Read and decrypt file
            with open(file_path_obj, 'r', encoding='utf-8') as f:
                encrypted_data = f.read()
            
            decrypted_data = self.decrypt(encrypted_data)
            
            # Write decrypted file
            with open(output_path_obj, 'w', encoding='utf-8') as f:
                f.write(decrypted_data)
            
            logger.info(f"📄 File decrypted: {file_path_obj} -> {output_path_obj}")
            return str(output_path_obj)
            
        except Exception as e:
            logger.error(f"❌ File decryption failed: {e}")
            raise
    
    def encrypt_dict(self, data: dict) -> str:
        """
        Encrypt a dictionary as JSON.
        
        Args:
            data: Dictionary to encrypt
            
        Returns:
            Encrypted JSON string
            
        Raises:
            Exception: If dictionary encryption fails
        """
        try:
            json_data = json.dumps(data, ensure_ascii=False)
            return self.encrypt(json_data)
        except Exception as e:
            logger.error(f"❌ Dictionary encryption failed: {e}")
            raise
    
    def decrypt_dict(self, encrypted_data: str) -> dict:
        """
        Decrypt a JSON dictionary.
        
        Args:
            encrypted_data: Encrypted JSON string
            
        Returns:
            Decrypted dictionary
            
        Raises:
            Exception: If dictionary decryption fails
        """
        try:
            json_data = self.decrypt(encrypted_data)
            return json.loads(json_data)
        except Exception as e:
            logger.error(f"❌ Dictionary decryption failed: {e}")
            raise
    
    def generate_password_hash(self, password: str, salt: Optional[bytes] = None) -> Tuple[bytes, bytes]:
        """
        Generate a password hash using PBKDF2.
        
        Args:
            password: Password to hash
            salt: Optional salt bytes
            
        Returns:
            Tuple of (hashed_password, salt)
            
        Raises:
            Exception: If password hashing fails
        """
        try:
            # Generate salt if not provided
            if salt is None:
                import os
                salt = os.urandom(16)
            
            # Create PBKDF2 key derivation function
            kdf = PBKDF2HMAC(
                algorithm=hashes.SHA256(),
                length=32,
                salt=salt,
                iterations=100000,
            )
            
            # Generate hash
            key = kdf.derive(password.encode('utf-8'))
            
            return key, salt
            
        except Exception as e:
            logger.error(f"❌ Password hashing failed: {e}")
            raise
    
    def verify_password(self, password: str, key: bytes, salt: bytes) -> bool:
        """
        Verify a password against a hash.
        
        Args:
            password: Password to verify
            key: Stored password hash
            salt: Password salt
            
        Returns:
            True if password matches, False otherwise
        """
        try:
            # Create PBKDF2 key derivation function
            kdf = PBKDF2HMAC(
                algorithm=hashes.SHA256(),
                length=32,
                salt=salt,
                iterations=100000,
            )
            
            # Verify password
            kdf.verify(password.encode('utf-8'), key)
            return True
            
        except Exception:
            return False
    
    def secure_delete(self, file_path: str, passes: int = 3) -> None:
        """
        Securely delete a file by overwriting it multiple times.
        
        Args:
            file_path: Path to file to delete
            passes: Number of overwrite passes
            
        Raises:
            Exception: If secure deletion fails
        """
        try:
            import os
            
            file_path_obj = Path(file_path)
            
            if not file_path_obj.exists():
                logger.warning(f"File not found for secure deletion: {file_path_obj}")
                return
            
            # Get file size
            file_size = file_path_obj.stat().st_size
            
            # Overwrite file multiple times
            with open(file_path_obj, 'r+b') as f:
                for pass_num in range(passes):
                    f.seek(0)
                    f.write(os.urandom(file_size))
                    f.flush()
                    os.fsync(f.fileno())
            
            # Remove file
            file_path_obj.unlink()
            logger.info(f"🗑️ Securely deleted file: {file_path_obj}")
            
        except Exception as e:
            logger.error(f"❌ Secure deletion failed: {e}")
            raise
    
    def get_key_info(self) -> dict:
        """
        Get information about the encryption key.
        
        Returns:
            Dictionary containing key information
        """
        return {
            'key_path': str(self.key_path),
            'key_exists': self.key_path.exists(),
            'cipher_initialized': self.cipher is not None,
            'key_size': len(self.key) if self.key else 0
        }
    
    def rotate_key(self, new_key_path: Optional[str] = None) -> None:
        """
        Rotate the encryption key.
        
        Args:
            new_key_path: Optional path for new key file
            
        Raises:
            Exception: If key rotation fails
        """
        try:
            # Backup old key
            old_key_path = self.key_path.with_suffix('.old')
            if self.key_path.exists():
                self.key_path.rename(old_key_path)
            
            # Generate new key
            if new_key_path:
                self.key_path = Path(new_key_path)
            
            self._generate_key()
            self.cipher = Fernet(self.key)
            
            logger.info("🔄 Encryption key rotated successfully")
            
        except Exception as e:
            logger.error(f"❌ Key rotation failed: {e}")
            raise
    
    def test_encryption(self) -> bool:
        """
        Test encryption/decryption functionality.
        
        Returns:
            True if test passes, False otherwise
        """
        try:
            test_data = "Test encryption data 🔐"
            
            # Test basic encryption/decryption
            encrypted = self.encrypt(test_data)
            decrypted = self.decrypt(encrypted)
            
            if decrypted != test_data:
                return False
            
            # Test dictionary encryption/decryption
            test_dict = {"test": "data", "number": 42}
            encrypted_dict = self.encrypt_dict(test_dict)
            decrypted_dict = self.decrypt_dict(encrypted_dict)
            
            if decrypted_dict != test_dict:
                return False
            
            logger.info("✅ Encryption test passed")
            return True
            
        except Exception as e:
            logger.error(f"❌ Encryption test failed: {e}")
            return False 