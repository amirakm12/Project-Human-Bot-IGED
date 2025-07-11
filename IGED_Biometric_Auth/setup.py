#!/usr/bin/env python3
"""
IGED Biometric Authentication Setup
Setup script for IGED Biometric Authentication Integration Suite
"""

from setuptools import setup, find_packages
from pathlib import Path

# Read README file
readme_path = Path("README.md")
long_description = ""
if readme_path.exists():
    with open(readme_path, "r", encoding="utf-8") as f:
        long_description = f.read()

# Read requirements
requirements_path = Path("requirements.txt")
install_requires = []
if requirements_path.exists():
    with open(requirements_path, "r", encoding="utf-8") as f:
        install_requires = [
            line.strip() 
            for line in f.readlines() 
            if line.strip() and not line.startswith("#")
        ]

setup(
    name="iged-biometric-auth",
    version="1.0.0",
    author="IGED Development Team",
    author_email="dev@iged.example.com",
    description="Enterprise-grade biometric authentication for IGED",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/iged/biometric-auth",
    packages=find_packages(),
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Intended Audience :: Developers",
        "Intended Audience :: System Administrators",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Programming Language :: Python :: 3.10",
        "Programming Language :: Python :: 3.11",
        "Topic :: Security",
        "Topic :: System :: Systems Administration",
        "Topic :: Software Development :: Libraries :: Python Modules",
    ],
    python_requires=">=3.8",
    install_requires=install_requires,
    extras_require={
        "dev": [
            "pytest>=7.4.3",
            "pytest-asyncio>=0.21.1",
            "black>=23.11.0",
            "flake8>=6.1.0",
            "mypy>=1.7.1",
        ],
        "gui": [
            "PySide6>=6.6.1",
        ],
        "webauthn": [
            "flask>=2.3.2",
            "flask-cors>=3.0.10",
            "fido2>=1.20.0",
        ],
        "windows": [
            "pywinrt>=1.0.0",
            "pywin32>=306",
        ],
    },
    entry_points={
        "console_scripts": [
            "iged-biometric-launcher=iged_launcher:main",
            "iged-biometric-manager=biometric_manager_gui:main",
            "iged-webauthn-server=webauthn_server:main",
            "iged-windows-hello-test=windows_hello:test_windows_hello",
        ],
    },
    include_package_data=True,
    package_data={
        "": ["*.md", "*.txt", "*.json", "*.yaml", "*.yml"],
    },
    keywords=[
        "biometric",
        "authentication",
        "webauthn",
        "windows-hello",
        "security",
        "fido2",
        "iged",
    ],
    project_urls={
        "Bug Reports": "https://github.com/iged/biometric-auth/issues",
        "Source": "https://github.com/iged/biometric-auth",
        "Documentation": "https://iged-biometric-auth.readthedocs.io/",
    },
) 