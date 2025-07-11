# IGED Biometric Authentication Integration Suite

[![Python](https://img.shields.io/badge/Python-3.8+-blue.svg)](https://python.org)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Android-blue.svg)](https://github.com/amirakm12/Project-Human-Bot-IGED)

A comprehensive, enterprise-grade biometric and WebAuthn authentication system for IGED across Windows, Android, and web platforms.

## 🚀 Features

- **🔐 Windows Hello Integration** - Native biometric authentication using WinRT APIs
- **🌐 WebAuthn Support** - Cross-platform credential syncing between devices
- **📱 Android Biometric Client** - Mobile authentication with AndroidX BiometricPrompt
- **🛡️ Secure Launcher** - Biometric unlock required before IGED startup
- **🖥️ GUI Manager** - PySide6-based credential management interface
- **⚡ Flask WebAuthn Server** - Multi-device credential synchronization
- **🔒 Enterprise Security** - AES-256 encryption, secure key management

## 📋 Table of Contents

- [Installation](#installation)
- [Quick Start](#quick-start)
- [Usage](#usage)
- [Architecture](#architecture)
- [API Reference](#api-reference)
- [Contributing](#contributing)
- [License](#license)

## 🛠️ Installation

### Prerequisites

- **Windows 10+** with biometric hardware (fingerprint/face recognition)
- **Python 3.8+** with pip
- **Git** for cloning the repository
- **Android Studio** (for Android client development)

### Clone the Repository

```bash
git clone https://github.com/amirakm12/Project-Human-Bot-IGED.git
cd Project-Human-Bot-IGED
```

### Install Dependencies

```bash
# Install all dependencies
pip install -r requirements.txt

# Or install specific components
pip install -r requirements.txt[gui]      # GUI components
pip install -r requirements.txt[webauthn] # WebAuthn server
pip install -r requirements.txt[windows]  # Windows Hello
```

### Generate Encryption Key

```bash
# Generate encryption key for secure storage
python create_key_final.py
```

## 🚀 Quick Start

### 1. Test Biometric Authentication

```bash
# Test Windows Hello integration
python biometric_manager_gui.py
```

### 2. Launch IGED with Biometric Protection

```bash
# Launch IGED with biometric authentication
python iged_launcher.py
```

### 3. Start WebAuthn Server

```bash
# Start WebAuthn server for credential sync
python webauthn_server.py
```

### 4. Build Windows Executable

```bash
# Build standalone executable
installer/build_installer.bat
```

## 📖 Usage

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

# Register new credential via API
curl -X POST http://localhost:5000/register \
  -H "Content-Type: application/json" \
  -d '{
    "user": {
      "id": "user123",
      "name": "test@example.com",
      "displayName": "Test User"
    }
  }'
```

### Android Integration

1. Open `android_client/` in Android Studio
2. Build and install APK on Android device
3. Configure biometric authentication
4. Sync credentials with desktop via WebAuthn server

## 🏗️ Architecture

### Core Components

```
Project-Human-Bot-IGED/
├── windows_hello.py              # Windows Hello biometric integration
├── biometric_manager_gui.py      # PySide6 GUI manager
├── webauthn_server.py           # Flask WebAuthn server
├── iged_launcher.py             # Secure biometric launcher
├── utils/
│   ├── crypto_utils.py          # Cryptographic operations
│   └── logger.py                # Unified logging system
├── android_client/              # Android biometric client
└── installer/                   # Build and packaging scripts
```

### Security Features

- **🔐 Hardware-backed Authentication** - Windows Hello integration
- **🌐 FIDO2 WebAuthn** - Standards-compliant multi-factor authentication
- **🔒 AES-256 Encryption** - Secure credential storage
- **🛡️ Account Lockout** - Protection against brute force attacks
- **📊 Audit Logging** - Comprehensive security event tracking

## 🔧 Configuration

### Environment Variables

```bash
# WebAuthn server configuration
export WEBAUTHN_HOST=0.0.0.0
export WEBAUTHN_PORT=5000
export WEBAUTHN_RP_ID=iged.example.com

# Biometric settings
export BIOMETRIC_TIMEOUT=30
export BIOMETRIC_RETRY_COUNT=3
```

### WebAuthn Server Settings

```python
# In webauthn_server.py
rp = PublicKeyCredentialRpEntity(
    id="iged.example.com", 
    name="IGED Biometric Auth"
)
```

## 📱 Platform Support

| Platform | Biometric | WebAuthn | GUI | Status |
|----------|-----------|----------|-----|--------|
| Windows 10+ | ✅ Windows Hello | ✅ Server | ✅ PySide6 | Production Ready |
| Android 6.0+ | ✅ BiometricPrompt | ✅ Client | ✅ Native | Production Ready |
| Linux | 🔄 Planned | ✅ Server | ✅ PySide6 | In Development |
| macOS | 🔄 Planned | ✅ Server | ✅ PySide6 | In Development |

## 🧪 Testing

### Run Test Suite

```bash
# Install test dependencies
pip install -r requirements.txt[dev]

# Run all tests
pytest

# Run specific test modules
pytest tests/test_windows_hello.py
pytest tests/test_webauthn.py
pytest tests/test_crypto.py
```

### Manual Testing

```bash
# Test Windows Hello
python windows_hello.py

# Test cryptographic utilities
python utils/crypto_utils.py

# Test logging system
python utils/logger.py
```

## 🚨 Troubleshooting

### Common Issues

#### Windows Hello Not Working
```bash
# Check Windows Hello settings
# 1. Open Windows Settings > Accounts > Sign-in options
# 2. Ensure Windows Hello is configured
# 3. Verify biometric hardware is working

# Check pywinrt installation
pip install pywinrt
```

#### WebAuthn Server Issues
```bash
# Check if port 5000 is available
netstat -an | findstr :5000

# Verify Flask dependencies
pip install flask flask-cors fido2

# Check firewall settings
# Allow Python/Flask through Windows Firewall
```

#### Android Client Issues
```bash
# Ensure Android SDK 33+ is installed
# Check biometric hardware availability
# Verify WebAuthn server connectivity
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

### Development Setup

```bash
# Fork and clone the repository
git clone https://github.com/amirakm12/Project-Human-Bot-IGED.git
cd Project-Human-Bot-IGED

# Create virtual environment
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install development dependencies
pip install -r requirements.txt[dev]

# Run pre-commit hooks
pre-commit install
```

### Code Style

```bash
# Format code
black .

# Lint code
flake8 .

# Type checking
mypy .
```

### Testing

```bash
# Run tests with coverage
pytest --cov=.

# Generate coverage report
coverage html
```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## ⚠️ Disclaimer

IGED Biometric Auth is designed for educational and research purposes. Users are responsible for ensuring compliance with applicable laws and regulations regarding biometric data collection and storage.

## 🙏 Acknowledgments

- **Windows Hello** - Microsoft for WinRT APIs
- **WebAuthn** - FIDO Alliance for FIDO2 standards
- **PySide6** - Qt Company for Qt bindings
- **Cryptography** - Python Cryptography Authority

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/amirakm12/Project-Human-Bot-IGED/issues)
- **Discussions**: [GitHub Discussions](https://github.com/amirakm12/Project-Human-Bot-IGED/discussions)
- **Documentation**: [Wiki](https://github.com/amirakm12/Project-Human-Bot-IGED/wiki)

---

**Built with ❤️ for enterprise-grade biometric security and seamless multi-device authentication.** 🔐🤖

[![GitHub stars](https://img.shields.io/github/stars/amirakm12/Project-Human-Bot-IGED?style=social)](https://github.com/amirakm12/Project-Human-Bot-IGED/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/amirakm12/Project-Human-Bot-IGED?style=social)](https://github.com/amirakm12/Project-Human-Bot-IGED/network/members)
[![GitHub issues](https://img.shields.io/github/issues/amirakm12/Project-Human-Bot-IGED)](https://github.com/amirakm12/Project-Human-Bot-IGED/issues) 