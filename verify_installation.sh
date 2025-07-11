#!/bin/bash

# IGED Installation Verification Script
# Comprehensive testing of all dependencies and codecs

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

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Test function
run_test() {
    local test_name="$1"
    local test_command="$2"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    log_info "Testing: $test_name"
    
    if eval "$test_command"; then
        log_success "✅ $test_name: PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        return 0
    else
        log_error "❌ $test_name: FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi
}

# Test Python environment
test_python_environment() {
    log_info "Testing Python environment..."
    
    run_test "Python 3.8+" "python3 -c 'import sys; assert sys.version_info >= (3, 8)'"
    run_test "Virtual environment" "[ -d 'venv' ] && [ -f 'venv/bin/activate' ]"
    run_test "Pip in venv" "source venv/bin/activate && pip --version"
}

# Test core Python packages
test_core_packages() {
    log_info "Testing core Python packages..."
    
    source venv/bin/activate 2>/dev/null || true
    
    local packages=(
        "cryptography"
        "flask"
        "requests"
        "pandas"
        "numpy"
        "matplotlib"
        "PIL"
        "psutil"
    )
    
    for package in "${packages[@]}"; do
        run_test "Import $package" "python3 -c 'import $package'"
    done
}

# Test audio packages
test_audio_packages() {
    log_info "Testing audio packages..."
    
    source venv/bin/activate 2>/dev/null || true
    
    local packages=(
        "speech_recognition"
        "pyaudio"
        "pydub"
        "soundfile"
    )
    
    for package in "${packages[@]}"; do
        run_test "Import $package" "python3 -c 'import $package' 2>/dev/null"
    done
    
    # Test Whisper separately as it's optional
    if python3 -c 'import whisper' 2>/dev/null; then
        log_success "✅ OpenAI Whisper: Available"
    else
        log_warning "⚠️ OpenAI Whisper: Not available (offline speech recognition disabled)"
    fi
}

# Test multimedia packages
test_multimedia_packages() {
    log_info "Testing multimedia packages..."
    
    source venv/bin/activate 2>/dev/null || true
    
    local packages=(
        "cv2"
        "imageio"
        "moviepy"
    )
    
    for package in "${packages[@]}"; do
        run_test "Import $package" "python3 -c 'import $package' 2>/dev/null"
    done
}

# Test networking packages
test_networking_packages() {
    log_info "Testing networking packages..."
    
    source venv/bin/activate 2>/dev/null || true
    
    # Test scapy (requires root for some functions)
    if python3 -c 'import scapy' 2>/dev/null; then
        log_success "✅ Scapy: Available"
    else
        log_warning "⚠️ Scapy: Not available (packet capture limited)"
    fi
    
    # Test other networking packages
    local packages=(
        "nmap"
        "netaddr"
        "netifaces"
    )
    
    for package in "${packages[@]}"; do
        run_test "Import $package" "python3 -c 'import $package' 2>/dev/null"
    done
}

# Test GUI packages
test_gui_packages() {
    log_info "Testing GUI packages..."
    
    source venv/bin/activate 2>/dev/null || true
    
    local packages=(
        "pyautogui"
        "pynput"
        "keyboard"
        "mouse"
    )
    
    for package in "${packages[@]}"; do
        run_test "Import $package" "python3 -c 'import $package' 2>/dev/null"
    done
    
    # Test tkinter (usually built-in)
    run_test "Import tkinter" "python3 -c 'import tkinter'"
}

# Test system codecs
test_system_codecs() {
    log_info "Testing system codecs..."
    
    # Test FFmpeg
    run_test "FFmpeg binary" "ffmpeg -version >/dev/null 2>&1"
    
    # Test GStreamer
    run_test "GStreamer" "gst-inspect-1.0 --version >/dev/null 2>&1"
    
    # Test VLC
    run_test "VLC" "vlc --version >/dev/null 2>&1"
    
    # Test audio system
    run_test "ALSA" "aplay -l >/dev/null 2>&1"
    run_test "PulseAudio" "pulseaudio --check -v >/dev/null 2>&1"
}

# Test system tools
test_system_tools() {
    log_info "Testing system tools..."
    
    local tools=(
        "nmap"
        "netcat"
        "tcpdump"
        "wireshark"
        "htop"
        "git"
        "curl"
        "wget"
    )
    
    for tool in "${tools[@]}"; do
        run_test "$tool binary" "command -v $tool >/dev/null 2>&1"
    done
}

# Test IGED specific files
test_iged_files() {
    log_info "Testing IGED specific files..."
    
    local files=(
        "launcher.py"
        "core/voice_pipeline.py"
        "core/command_parser.py"
        "core/memory_engine.py"
        "ui/win_gui/main_window.py"
        "requirements_complete.txt"
        "activate_iged.sh"
    )
    
    for file in "${files[@]}"; do
        run_test "File $file exists" "[ -f '$file' ]"
    done
    
    # Test directories
    local dirs=(
        "config"
        "logs"
        "output"
        "agents"
        "plugins"
    )
    
    for dir in "${dirs[@]}"; do
        run_test "Directory $dir exists" "[ -d '$dir' ]"
    done
}

# Test configuration
test_configuration() {
    log_info "Testing configuration..."
    
    # Test encryption key
    run_test "Encryption key exists" "[ -f 'config/secret.key' ]"
    
    # Test permissions
    run_test "Scripts executable" "[ -x 'activate_iged.sh' ] && [ -x 'install_system_dependencies.sh' ]"
    
    # Test Python path
    source venv/bin/activate 2>/dev/null || true
    run_test "IGED importable" "python3 -c 'import sys; sys.path.insert(0, \".\"); import launcher'"
}

# Test network connectivity (optional)
test_network_connectivity() {
    log_info "Testing network connectivity (optional)..."
    
    # Test internet connectivity
    if ping -c 1 8.8.8.8 >/dev/null 2>&1; then
        log_success "✅ Internet connectivity: Available"
    else
        log_warning "⚠️ Internet connectivity: Limited (some features may not work)"
    fi
    
    # Test DNS resolution
    if nslookup google.com >/dev/null 2>&1; then
        log_success "✅ DNS resolution: Working"
    else
        log_warning "⚠️ DNS resolution: Issues detected"
    fi
}

# Performance test
performance_test() {
    log_info "Running performance tests..."
    
    source venv/bin/activate 2>/dev/null || true
    
    # Test numpy performance
    python3 -c "
import time
import numpy as np
start = time.time()
a = np.random.random((1000, 1000))
b = np.random.random((1000, 1000))
c = np.dot(a, b)
end = time.time()
print(f'NumPy matrix multiplication (1000x1000): {end-start:.3f}s')
"
    
    # Test basic system performance
    log_info "System information:"
    echo "  CPU: $(lscpu | grep 'Model name' | sed 's/Model name: *//')"
    echo "  Memory: $(free -h | grep Mem | awk '{print $2}')"
    echo "  Disk: $(df -h . | tail -1 | awk '{print $4}') free"
}

# Generate report
generate_report() {
    log_info "Generating verification report..."
    
    cat > verification_report.txt << EOF
IGED Installation Verification Report
=====================================
Date: $(date)
OS: $(lsb_release -d 2>/dev/null | cut -f2 || uname -a)
Python: $(python3 --version)

Test Results:
-------------
Total Tests: $TOTAL_TESTS
Passed: $PASSED_TESTS
Failed: $FAILED_TESTS
Success Rate: $(( PASSED_TESTS * 100 / TOTAL_TESTS ))%

Status: $([ $FAILED_TESTS -eq 0 ] && echo "ALL TESTS PASSED ✅" || echo "SOME TESTS FAILED ❌")

Recommendations:
$([ $FAILED_TESTS -gt 0 ] && echo "- Review failed tests and reinstall missing dependencies" || echo "- System is ready for IGED operation")
$([ ! -f "venv/bin/activate" ] && echo "- Create virtual environment: ./install_python_requirements.sh")
$(! command -v ffmpeg >/dev/null 2>&1 && echo "- Install multimedia codecs: ./install_codecs_multimedia.sh")

Next Steps:
- Activate environment: source activate_iged.sh
- Start IGED: python3 launcher.py
- Check web interface: http://localhost:8080
EOF
    
    log_success "Verification report saved to: verification_report.txt"
}

# Main verification function
main() {
    echo "🔍 IGED Installation Verification"
    echo "=================================="
    echo ""
    
    test_python_environment
    test_core_packages
    test_audio_packages
    test_multimedia_packages
    test_networking_packages
    test_gui_packages
    test_system_codecs
    test_system_tools
    test_iged_files
    test_configuration
    test_network_connectivity
    performance_test
    
    echo ""
    echo "📊 VERIFICATION SUMMARY"
    echo "======================="
    echo "Total Tests: $TOTAL_TESTS"
    echo "Passed: $PASSED_TESTS"
    echo "Failed: $FAILED_TESTS"
    echo "Success Rate: $(( PASSED_TESTS * 100 / TOTAL_TESTS ))%"
    echo ""
    
    if [ $FAILED_TESTS -eq 0 ]; then
        log_success "🎉 ALL TESTS PASSED! IGED is ready for operation."
        echo ""
        echo "To start IGED:"
        echo "1. source activate_iged.sh"
        echo "2. python3 launcher.py"
    else
        log_warning "⚠️ Some tests failed. Please review and fix issues before running IGED."
        echo ""
        echo "Common fixes:"
        echo "- Run: ./install_system_dependencies.sh"
        echo "- Run: ./install_python_requirements.sh"
        echo "- Run: ./install_codecs_multimedia.sh"
    fi
    
    generate_report
}

# Run main function
main "$@"