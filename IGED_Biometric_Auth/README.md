# IGED Biometric Authentication Integration Suite

A comprehensive biometric and WebAuthn authentication system for IGED across Windows, Android, and web platforms.

## 🚀 Features

- **Windows Hello Integration** - Native biometric authentication using WinRT APIs
- **WebAuthn Support** - Cross-platform credential syncing between devices
- **Android Biometric Client** - Mobile authentication with AndroidX BiometricPrompt
- **Secure Launcher** - Biometric unlock required before IGED startup
- **GUI Manager** - PySide6-based credential management interface
- **Flask WebAuthn Server** - Multi-device credential synchronization
- **Enterprise Security** - AES-256 encryption, secure key management

## 🏗️ Architecture

### Core Components
- **Windows Hello Module** (`windows_hello.py`) - WinRT-based biometric authentication
- **Biometric Manager GUI** (`biometric_manager_gui.py`) - Desktop credential management
- **WebAuthn Server** (`webauthn_server.py`) - Flask server for credential sync
- **Secure Launcher** (`iged_launcher.py`) - Biometric-protected IGED startup
- **Android Client** (`android_client/`) - Mobile biometric and WebAuthn support
- **Crypto Utils** (`utils/crypto_utils.py`) - Secure cryptographic operations
- **Logger** (`utils/logger.py`) - Unified logging across all modules

## 🛠️ Installation

### Quick Start (Windows)
```bash
# Install dependencies
pip install -r requirements.txt

# Generate encryption key
python create_key_final.py

# Test biometric authentication
python biometric_manager_gui.py

# Launch IGED with biometric protection
python iged_launcher.py
```

### Development Setup
```bash
# Clone and setup
git clone <repository>
cd IGED_Biometric_Auth

# Install development dependencies
pip install -r requirements.txt

# Run WebAuthn server
python webauthn_server.py

# Build Android client (requires Android Studio)
cd android_client
# Open in Android Studio and build
```

### Production Build
```bash
# Build Windows EXE
installer/build_installer.bat

# Build Android APK
cd android_client
# Use Android Studio to build APK
```

## 🎯 Usage

### Windows Biometric Authentication
```python
from windows_hello import biometric_authenticate_sync

# Check if biometric hardware is available
if biometric_authenticate_sync():
    print("✅ Biometric authentication successful")
    # Launch IGED
else:
    print("❌ Authentication failed")
```

### WebAuthn Credential Management
```python
# Start WebAuthn server
python webauthn_server.py

# Register new credential
curl -X POST http://localhost:5000/register \
  -H "Content-Type: application/json" \
  -d '{"user": {"id": "user123", "name": "test@example.com", "displayName": "Test User"}}'
```

### Android Integration
- Install the APK on Android device
- Configure biometric authentication
- Sync credentials with desktop via WebAuthn server

## 🔐 Security Features

- **Windows Hello Integration** - Native Windows biometric authentication
- **WebAuthn Standards** - FIDO2-compliant multi-factor authentication
- **AES-256 Encryption** - Secure credential storage
- **Atomic File Operations** - Prevents key corruption
- **Secure Key Management** - Hardware-backed key storage
- **Audit Logging** - Comprehensive security event logging

## 📱 Platform Support

### Windows
- Windows 10+ with biometric hardware
- Windows Hello fingerprint/face recognition
- Native WinRT API integration

### Android
- Android 6.0+ (API 23+)
- AndroidX BiometricPrompt
- FIDO2 WebAuthn support

### Web
- Flask-based WebAuthn server
- Cross-platform credential sync
- RESTful API for client integration

## 🔧 Configuration

### Environment Variables
```bash
# WebAuthn server configuration
WEBAUTHN_HOST=0.0.0.0
WEBAUTHN_PORT=5000
WEBAUTHN_RP_ID=iged.example.com

# Biometric settings
BIOMETRIC_TIMEOUT=30
BIOMETRIC_RETRY_COUNT=3
```

### WebAuthn Server Settings
```python
# In webauthn_server.py
rp = PublicKeyCredentialRpEntity(
    id="iged.example.com", 
    name="IGED Biometric Auth"
)
```

## 🚨 Troubleshooting

### Windows Hello Issues
- Ensure biometric hardware is properly configured
- Check Windows Hello settings in Windows Settings
- Verify pywinrt installation: `pip install pywinrt`

### WebAuthn Server Issues
- Check if port 5000 is available
- Verify Flask and fido2 dependencies
- Check firewall settings for network access

### Android Client Issues
- Ensure Android SDK 33+ is installed
- Check biometric hardware availability
- Verify WebAuthn server connectivity

## 📊 Monitoring

- **Log Files** - All operations logged to `logs/`
- **WebAuthn Server** - Flask debug mode for development
- **Biometric Events** - Windows Event Log integration
- **Android Logs** - Logcat integration for debugging

## 🤝 Contributing

1. Fork the repository
2. Create feature branch
3. Add biometric/WebAuthn features
4. Update documentation
5. Submit pull request

## 📄 License

This project is designed for educational and research purposes. Use responsibly and in accordance with local laws.

## ⚠️ Disclaimer

IGED Biometric Auth is a powerful security tool. Users are responsible for ensuring compliance with applicable laws and regulations regarding biometric data collection and storage.

---

**Built for enterprise-grade biometric security and seamless multi-device authentication.** 🔐🤖 