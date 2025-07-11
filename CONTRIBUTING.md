# Contributing to IGED Biometric Authentication

Thank you for your interest in contributing to IGED Biometric Authentication! This document provides guidelines and information for contributors.

## 🤝 How to Contribute

### Types of Contributions

We welcome various types of contributions:

- **🐛 Bug Reports** - Report issues and bugs
- **💡 Feature Requests** - Suggest new features
- **📝 Documentation** - Improve documentation
- **🔧 Code Contributions** - Submit code changes
- **🧪 Testing** - Help with testing and quality assurance
- **🌐 Translations** - Help with internationalization

### Before You Start

1. **Check existing issues** - Your issue might already be reported
2. **Read the documentation** - Understand the project structure
3. **Set up development environment** - Follow the setup guide below
4. **Follow coding standards** - Adhere to our style guidelines

## 🛠️ Development Setup

### Prerequisites

- **Python 3.8+** with pip
- **Git** for version control
- **Windows 10+** with biometric hardware (for Windows Hello testing)
- **Android Studio** (for Android client development)
- **Code editor** (VS Code, PyCharm, etc.)

### Local Development Setup

```bash
# 1. Fork the repository on GitHub
# 2. Clone your fork
git clone https://github.com/your-username/IGED_Biometric_Auth.git
cd IGED_Biometric_Auth

# 3. Create virtual environment
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# 4. Install dependencies
pip install -r requirements.txt[dev]

# 5. Install pre-commit hooks
pre-commit install

# 6. Generate encryption key
python create_key_final.py
```

### IDE Setup

#### VS Code
```json
{
    "python.defaultInterpreter": "./venv/Scripts/python.exe",
    "python.linting.enabled": true,
    "python.linting.flake8Enabled": true,
    "python.formatting.provider": "black",
    "python.testing.pytestEnabled": true
}
```

#### PyCharm
1. Open project in PyCharm
2. Configure Python interpreter to use virtual environment
3. Enable code inspection and formatting
4. Configure pytest for testing

## 📝 Code Standards

### Python Style Guide

We follow [PEP 8](https://pep8.org/) with some modifications:

```python
# ✅ Good
def authenticate_user(user_id: str, method: str) -> bool:
    """Authenticate user using specified method."""
    try:
        result = perform_authentication(user_id, method)
        return result.success
    except AuthenticationError as e:
        logger.error(f"Authentication failed: {e}")
        return False

# ❌ Bad
def auth_user(user_id,method):
    try:
        result=perform_authentication(user_id,method)
        return result.success
    except AuthenticationError as e:
        logger.error(f"Authentication failed: {e}")
        return False
```

### Code Formatting

We use **Black** for code formatting:

```bash
# Format code
black .

# Check formatting without changes
black --check .
```

### Linting

We use **Flake8** for linting:

```bash
# Run linter
flake8 .

# Configuration in setup.cfg or pyproject.toml
[flake8]
max-line-length = 88
extend-ignore = E203, W503
```

### Type Hints

We use **mypy** for type checking:

```python
# ✅ Good - with type hints
from typing import Optional, Dict, Any

def get_user_credentials(user_id: str) -> Optional[Dict[str, Any]]:
    """Get user credentials from storage."""
    # Implementation here
    pass

# ❌ Bad - without type hints
def get_user_credentials(user_id):
    """Get user credentials from storage."""
    # Implementation here
    pass
```

### Documentation

#### Docstrings
Use Google-style docstrings:

```python
def biometric_authenticate_sync(reason: str = "Authenticate to unlock IGED") -> bool:
    """Synchronous biometric authentication using Windows Hello.
    
    Args:
        reason: Reason for authentication to display to user
        
    Returns:
        bool: True if authentication successful, False otherwise
        
    Raises:
        BiometricError: If biometric hardware is not available
    """
    pass
```

#### Comments
- Use comments to explain **why**, not **what**
- Keep comments up to date with code changes
- Use clear, concise language

## 🧪 Testing

### Running Tests

```bash
# Run all tests
pytest

# Run with coverage
pytest --cov=. --cov-report=html

# Run specific test file
pytest tests/test_windows_hello.py

# Run tests in parallel
pytest -n auto
```

### Writing Tests

```python
# Example test structure
import pytest
from windows_hello import biometric_authenticate_sync

class TestWindowsHello:
    """Test Windows Hello biometric authentication."""
    
    def test_biometric_availability(self):
        """Test biometric hardware availability check."""
        # Test implementation
        pass
    
    def test_authentication_success(self, mock_biometric):
        """Test successful biometric authentication."""
        mock_biometric.return_value = True
        result = biometric_authenticate_sync("Test authentication")
        assert result is True
    
    def test_authentication_failure(self, mock_biometric):
        """Test failed biometric authentication."""
        mock_biometric.return_value = False
        result = biometric_authenticate_sync("Test authentication")
        assert result is False
```

### Test Guidelines

- **Test coverage** should be >80%
- **Unit tests** for individual functions
- **Integration tests** for component interactions
- **Mock external dependencies** (Windows APIs, network calls)
- **Test edge cases** and error conditions

## 🔄 Contribution Workflow

### 1. Create Issue

Before making changes, create an issue to discuss:
- Bug reports with detailed steps to reproduce
- Feature requests with use cases
- Enhancement proposals with implementation ideas

### 2. Fork and Clone

```bash
# Fork on GitHub, then clone
git clone https://github.com/your-username/IGED_Biometric_Auth.git
cd IGED_Biometric_Auth

# Add upstream remote
git remote add upstream https://github.com/original-username/IGED_Biometric_Auth.git
```

### 3. Create Feature Branch

```bash
# Create and switch to feature branch
git checkout -b feature/your-feature-name

# Or for bug fixes
git checkout -b fix/your-bug-description
```

### 4. Make Changes

- Write code following our standards
- Add tests for new functionality
- Update documentation
- Update changelog if needed

### 5. Commit Changes

```bash
# Stage changes
git add .

# Commit with descriptive message
git commit -m "feat: add new biometric authentication method

- Add support for iris recognition
- Update documentation
- Add comprehensive tests
- Fix related bugs in authentication flow"
```

### 6. Push and Create Pull Request

```bash
# Push to your fork
git push origin feature/your-feature-name

# Create Pull Request on GitHub
```

### 7. Pull Request Guidelines

#### PR Title Format
```
type(scope): description

Examples:
feat(auth): add iris recognition support
fix(webauthn): resolve credential sync issue
docs(readme): update installation instructions
test(windows): add Windows Hello mock tests
```

#### PR Description Template
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation update
- [ ] Test addition
- [ ] Refactoring

## Testing
- [ ] Unit tests pass
- [ ] Integration tests pass
- [ ] Manual testing completed
- [ ] No breaking changes

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] Tests added/updated
- [ ] Changelog updated

## Related Issues
Closes #123
```

## 🚀 Release Process

### Versioning

We follow [Semantic Versioning](https://semver.org/):
- **MAJOR** - Breaking changes
- **MINOR** - New features, backward compatible
- **PATCH** - Bug fixes, backward compatible

### Release Checklist

- [ ] All tests pass
- [ ] Documentation updated
- [ ] Changelog updated
- [ ] Version bumped
- [ ] Release notes written
- [ ] GitHub release created

## 📋 Issue Templates

### Bug Report Template

```markdown
## Bug Description
Clear description of the bug

## Steps to Reproduce
1. Step 1
2. Step 2
3. Step 3

## Expected Behavior
What should happen

## Actual Behavior
What actually happens

## Environment
- OS: Windows 10/11
- Python: 3.8+
- IGED Version: 1.0.0
- Biometric Hardware: [Yes/No]

## Additional Information
Screenshots, logs, etc.
```

### Feature Request Template

```markdown
## Feature Description
Clear description of the feature

## Use Case
Why is this feature needed?

## Proposed Implementation
How should this be implemented?

## Alternatives Considered
Other approaches considered

## Additional Information
Any other relevant information
```

## 🏆 Recognition

Contributors will be recognized in:
- **README.md** - Contributors section
- **GitHub** - Contributors graph
- **Release notes** - For significant contributions
- **Documentation** - Code comments and docstrings

## 📞 Getting Help

- **GitHub Issues** - For bug reports and feature requests
- **GitHub Discussions** - For questions and general discussion
- **Documentation** - Check existing docs first
- **Code Review** - Ask questions in PR comments

## 📄 License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

**Thank you for contributing to IGED Biometric Authentication!** 🔐🤖 