#!/bin/bash

# IGED Python Requirements Installation Script
# Installs all Python packages with proper error handling and fallbacks

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check Python version
check_python() {
    log_info "Checking Python installation..."
    
    if ! command -v python3 &> /dev/null; then
        log_error "Python 3 is not installed. Please install Python 3.8 or newer."
        exit 1
    fi
    
    PYTHON_VERSION=$(python3 -c 'import sys; print(".".join(map(str, sys.version_info[:2])))')
    REQUIRED_VERSION="3.8"
    
    if [ "$(printf '%s\n' "$REQUIRED_VERSION" "$PYTHON_VERSION" | sort -V | head -n1)" != "$REQUIRED_VERSION" ]; then
        log_error "Python $PYTHON_VERSION found, but Python $REQUIRED_VERSION or newer is required."
        exit 1
    fi
    
    log_success "Python $PYTHON_VERSION found"
}

# Upgrade pip and essential tools
upgrade_pip() {
    log_info "Upgrading pip and essential tools..."
    
    python3 -m pip install --upgrade pip
    python3 -m pip install --upgrade setuptools wheel
}

# Create virtual environment
create_venv() {
    log_info "Creating virtual environment..."
    
    if [ -d "venv" ]; then
        log_warning "Virtual environment already exists. Removing and recreating..."
        rm -rf venv
    fi
    
    python3 -m venv venv
    source venv/bin/activate
    
    # Upgrade pip in venv
    pip install --upgrade pip setuptools wheel
    
    log_success "Virtual environment created and activated"
}

# Install packages with error handling
install_package() {
    local package=$1
    local max_retries=3
    local retry=0
    
    while [ $retry -lt $max_retries ]; do
        log_info "Installing $package (attempt $((retry + 1))/$max_retries)..."
        
        if pip install "$package"; then
            log_success "Successfully installed $package"
            return 0
        else
            log_warning "Failed to install $package (attempt $((retry + 1))/$max_retries)"
            retry=$((retry + 1))
            sleep 2
        fi
    done
    
    log_error "Failed to install $package after $max_retries attempts"
    echo "$package" >> failed_packages.txt
    return 1
}

# Install core packages first
install_core_packages() {
    log_info "Installing core packages..."
    
    local core_packages=(
        "cryptography>=3.4.8"
        "flask>=2.3.3"
        "flask-cors>=4.0.0"
        "requests>=2.31.0"
        "pandas>=2.0.0"
        "numpy>=1.24.0"
        "matplotlib>=3.7.0"
        "Pillow>=10.0.0"
    )
    
    for package in "${core_packages[@]}"; do
        install_package "$package"
    done
}

# Install audio packages
install_audio_packages() {
    log_info "Installing audio processing packages..."
    
    local audio_packages=(
        "SpeechRecognition>=3.10.0"
        "PyAudio>=0.2.11"
        "pydub>=0.25.1"
        "soundfile>=0.12.1"
    )
    
    for package in "${audio_packages[@]}"; do
        install_package "$package" || log_warning "Audio package $package failed - some audio features may not work"
    done
    
    # Try to install Whisper
    log_info "Installing OpenAI Whisper..."
    if ! install_package "openai-whisper>=20231117"; then
        log_warning "Whisper installation failed - offline speech recognition will not be available"
    fi
}

# Install networking packages
install_networking_packages() {
    log_info "Installing networking packages..."
    
    local network_packages=(
        "scapy>=2.5.0"
        "python-nmap>=0.7.1"
        "psutil>=5.9.0"
        "netaddr>=0.8.0"
        "netifaces>=0.11.0"
    )
    
    for package in "${network_packages[@]}"; do
        install_package "$package" || log_warning "Network package $package failed - some network features may not work"
    done
}

# Install GUI packages
install_gui_packages() {
    log_info "Installing GUI packages..."
    
    local gui_packages=(
        "pyautogui>=0.9.54"
        "pynput>=1.7.6"
        "keyboard>=0.13.5"
        "mouse>=0.7.1"
        "screeninfo>=0.8.1"
    )
    
    for package in "${gui_packages[@]}"; do
        install_package "$package" || log_warning "GUI package $package failed - some GUI features may not work"
    done
}

# Install security packages
install_security_packages() {
    log_info "Installing security packages..."
    
    local security_packages=(
        "pycryptodome>=3.18.0"
        "bcrypt>=4.0.1"
        "pyotp>=2.8.0"
        "python-jose>=3.3.0"
    )
    
    for package in "${security_packages[@]}"; do
        install_package "$package"
    done
}

# Install web packages
install_web_packages() {
    log_info "Installing web packages..."
    
    local web_packages=(
        "flask-socketio>=5.3.4"
        "gunicorn>=21.2.0"
        "aiohttp>=3.8.5"
        "websockets>=11.0.3"
    )
    
    for package in "${web_packages[@]}"; do
        install_package "$package"
    done
}

# Install data analysis packages
install_data_packages() {
    log_info "Installing data analysis packages..."
    
    local data_packages=(
        "seaborn>=0.12.0"
        "scipy>=1.11.0"
        "scikit-learn>=1.3.0"
        "plotly>=5.15.0"
    )
    
    for package in "${data_packages[@]}"; do
        install_package "$package"
    done
}

# Install development packages
install_dev_packages() {
    log_info "Installing development packages..."
    
    local dev_packages=(
        "pyinstaller>=5.13.0"
        "pytest>=7.4.0"
        "black>=23.7.0"
        "flake8>=6.0.0"
    )
    
    for package in "${dev_packages[@]}"; do
        install_package "$package"
    done
}

# Install utility packages
install_utility_packages() {
    log_info "Installing utility packages..."
    
    local utility_packages=(
        "click>=8.1.7"
        "colorama>=0.4.6"
        "tqdm>=4.65.0"
        "rich>=13.4.2"
        "python-dotenv>=1.0.0"
        "schedule>=1.2.0"
        "watchdog>=3.0.0"
    )
    
    for package in "${utility_packages[@]}"; do
        install_package "$package"
    done
}

# Install optional advanced packages
install_advanced_packages() {
    log_info "Installing advanced packages (optional)..."
    
    local advanced_packages=(
        "opencv-python>=4.8.0"
        "imageio>=2.31.1"
        "ffmpeg-python>=0.2.0"
        "paramiko>=3.3.1"
        "fabric>=3.2.2"
        "pexpect>=4.8.0"
    )
    
    for package in "${advanced_packages[@]}"; do
        install_package "$package" || log_warning "Advanced package $package failed - advanced features may not work"
    done
}

# Install AI/ML packages (optional)
install_ml_packages() {
    log_info "Installing AI/ML packages (optional - this may take a while)..."
    
    local ml_packages=(
        "torch>=2.0.0"
        "transformers>=4.32.0"
        "sentence-transformers>=2.2.2"
    )
    
    for package in "${ml_packages[@]}"; do
        install_package "$package" || log_warning "ML package $package failed - AI features may not work"
    done
}

# Install from requirements file
install_from_requirements() {
    log_info "Installing remaining packages from requirements..."
    
    if [ -f "requirements_complete.txt" ]; then
        log_info "Installing from requirements_complete.txt..."
        pip install -r requirements_complete.txt || log_warning "Some packages from requirements_complete.txt failed"
    elif [ -f "requirements.txt" ]; then
        log_info "Installing from requirements.txt..."
        pip install -r requirements.txt || log_warning "Some packages from requirements.txt failed"
    else
        log_warning "No requirements file found"
    fi
}

# Verify installation
verify_installation() {
    log_info "Verifying installation..."
    
    # Test core imports
    python3 -c "
import sys
import traceback

packages_to_test = [
    'cryptography',
    'flask',
    'requests',
    'pandas',
    'numpy',
    'matplotlib',
    'PIL',
    'psutil'
]

failed_imports = []

for package in packages_to_test:
    try:
        __import__(package)
        print(f'✅ {package}')
    except ImportError as e:
        print(f'❌ {package}: {e}')
        failed_imports.append(package)

if failed_imports:
    print(f'\\n⚠️ Failed imports: {failed_imports}')
    print('Some features may not work properly.')
else:
    print('\\n🎉 All core packages imported successfully!')
"
}

# Create activation script
create_activation_script() {
    log_info "Creating activation script..."
    
    cat > activate_iged.sh << 'EOF'
#!/bin/bash

# IGED Environment Activation Script

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}🚀 Activating IGED Environment...${NC}"

# Activate virtual environment
if [ -f "venv/bin/activate" ]; then
    source venv/bin/activate
    echo -e "${GREEN}✅ Virtual environment activated${NC}"
else
    echo -e "❌ Virtual environment not found. Please run install_python_requirements.sh first."
    exit 1
fi

# Set environment variables
export IGED_HOME="$(pwd)"
export PYTHONPATH="$IGED_HOME:$PYTHONPATH"

# Create necessary directories
mkdir -p logs
mkdir -p config
mkdir -p memory
mkdir -p output/{data_analysis,security,network_intelligence,remote_control,exploits,codegen}

# Generate encryption key if it doesn't exist
if [ ! -f "config/secret.key" ]; then
    echo "🔑 Generating encryption key..."
    python3 -c "
from cryptography.fernet import Fernet
key = Fernet.generate_key()
with open('config/secret.key', 'wb') as f:
    f.write(key)
print('✅ Encryption key generated')
"
fi

echo -e "${GREEN}🎯 IGED environment ready!${NC}"
echo "To start IGED, run: python3 launcher.py"
EOF

    chmod +x activate_iged.sh
    log_success "Activation script created: activate_iged.sh"
}

# Main installation function
main() {
    log_info "Starting IGED Python requirements installation..."
    
    # Initialize failed packages file
    > failed_packages.txt
    
    check_python
    upgrade_pip
    create_venv
    
    # Activate virtual environment for the rest of the script
    source venv/bin/activate
    
    install_core_packages
    install_audio_packages
    install_networking_packages
    install_gui_packages
    install_security_packages
    install_web_packages
    install_data_packages
    install_dev_packages
    install_utility_packages
    install_advanced_packages
    install_ml_packages
    install_from_requirements
    
    verify_installation
    create_activation_script
    
    log_success "Python requirements installation completed!"
    
    if [ -s failed_packages.txt ]; then
        log_warning "Some packages failed to install. Check failed_packages.txt for details."
        echo "Failed packages:"
        cat failed_packages.txt
    fi
    
    log_info "To activate the environment, run: source activate_iged.sh"
    log_info "To start IGED, run: python3 launcher.py"
}

# Run main function
main "$@"