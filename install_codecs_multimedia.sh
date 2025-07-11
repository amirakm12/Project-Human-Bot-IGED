#!/bin/bash

# IGED Multimedia Codecs and Libraries Installation Script
# Installs comprehensive multimedia codecs and libraries for full media support

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

# Detect OS
detect_os() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$NAME
        VERSION=$VERSION_ID
    elif type lsb_release >/dev/null 2>&1; then
        OS=$(lsb_release -si)
        VERSION=$(lsb_release -sr)
    elif [ -f /etc/redhat-release ]; then
        OS="Red Hat Enterprise Linux"
        VERSION=$(cat /etc/redhat-release | grep -oE '[0-9]+\.[0-9]+')
    else
        OS=$(uname -s)
        VERSION=$(uname -r)
    fi
    
    log_info "Detected OS: $OS $VERSION"
}

# Install Ubuntu/Debian multimedia codecs
install_ubuntu_codecs() {
    log_info "Installing multimedia codecs for Ubuntu/Debian..."
    
    # Enable universe and multiverse repositories
    sudo add-apt-repository universe -y
    sudo add-apt-repository multiverse -y
    sudo apt update
    
    # Install Ubuntu restricted extras
    echo "ttf-mscorefonts-installer msttcorefonts/accepted-mscorefonts-eula select true" | sudo debconf-set-selections
    sudo apt install -y ubuntu-restricted-extras
    
    # Install comprehensive codec pack
    sudo apt install -y \
        ffmpeg \
        gstreamer1.0-plugins-base \
        gstreamer1.0-plugins-good \
        gstreamer1.0-plugins-bad \
        gstreamer1.0-plugins-ugly \
        gstreamer1.0-libav \
        gstreamer1.0-tools \
        gstreamer1.0-x \
        gstreamer1.0-alsa \
        gstreamer1.0-gl \
        gstreamer1.0-gtk3 \
        gstreamer1.0-qt5 \
        gstreamer1.0-pulseaudio \
        libavcodec-extra \
        libavfilter-extra \
        libavformat-extra \
        libavutil-extra \
        libpostproc-extra \
        libswresample-extra \
        libswscale-extra \
        mencoder \
        mplayer \
        vlc \
        vlc-plugin-base \
        vlc-plugin-video-output \
        browser-plugin-vlc \
        handbrake \
        handbrake-cli \
        winff \
        avidemux \
        openshot \
        kdenlive \
        audacity \
        lame \
        twolame \
        gstreamer1.0-plugins-bad-multiverse \
        libdvd-pkg \
        libfaac-dev \
        libfdk-aac-dev \
        libmp3lame-dev \
        libopencore-amrnb-dev \
        libopencore-amrwb-dev \
        libopenjp2-7-dev \
        libopus-dev \
        libspeex-dev \
        libtheora-dev \
        libvorbis-dev \
        libvpx-dev \
        libwebp-dev \
        libx264-dev \
        libx265-dev \
        libxvidcore-dev \
        x264 \
        x265
    
    # Configure libdvd-pkg
    sudo dpkg-reconfigure libdvd-pkg
    
    # Install additional codecs from snap
    sudo snap install vlc
    sudo snap install obs-studio
    sudo snap install ffmpeg
}

# Install CentOS/RHEL/Fedora multimedia codecs
install_redhat_codecs() {
    log_info "Installing multimedia codecs for Red Hat based systems..."
    
    # Determine package manager
    if command -v dnf >/dev/null 2>&1; then
        PKG_MANAGER="dnf"
    else
        PKG_MANAGER="yum"
    fi
    
    # Enable EPEL repository
    sudo $PKG_MANAGER install -y epel-release
    
    # Enable RPM Fusion repositories
    if [[ "$OS" == *"Fedora"* ]]; then
        sudo $PKG_MANAGER install -y \
            https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm \
            https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm
    else
        sudo $PKG_MANAGER install -y \
            https://download1.rpmfusion.org/free/el/rpmfusion-free-release-$(rpm -E %rhel).noarch.rpm \
            https://download1.rpmfusion.org/nonfree/el/rpmfusion-nonfree-release-$(rpm -E %rhel).noarch.rpm
    fi
    
    # Update package cache
    sudo $PKG_MANAGER update -y
    
    # Install multimedia codecs
    sudo $PKG_MANAGER install -y \
        ffmpeg \
        ffmpeg-devel \
        gstreamer1 \
        gstreamer1-devel \
        gstreamer1-plugins-base \
        gstreamer1-plugins-good \
        gstreamer1-plugins-bad-free \
        gstreamer1-plugins-bad-nonfree \
        gstreamer1-plugins-ugly \
        gstreamer1-libav \
        vlc \
        vlc-devel \
        mencoder \
        mplayer \
        HandBrake-cli \
        HandBrake-gui \
        audacity \
        lame \
        lame-devel \
        twolame \
        twolame-devel \
        faac \
        faac-devel \
        faad2 \
        faad2-devel \
        fdk-aac \
        fdk-aac-devel \
        libmp3lame-devel \
        opencore-amr \
        opencore-amr-devel \
        openjpeg2 \
        openjpeg2-devel \
        opus \
        opus-devel \
        speex \
        speex-devel \
        libtheora \
        libtheora-devel \
        libvorbis \
        libvorbis-devel \
        libvpx \
        libvpx-devel \
        libwebp \
        libwebp-devel \
        x264 \
        x264-devel \
        x265 \
        x265-devel \
        xvid \
        xvid-devel
}

# Install Arch Linux multimedia codecs
install_arch_codecs() {
    log_info "Installing multimedia codecs for Arch Linux..."
    
    # Update package database
    sudo pacman -Syu --noconfirm
    
    # Install multimedia codecs
    sudo pacman -S --noconfirm \
        ffmpeg \
        gstreamer \
        gst-plugins-base \
        gst-plugins-good \
        gst-plugins-bad \
        gst-plugins-ugly \
        gst-libav \
        vlc \
        mplayer \
        mencoder \
        handbrake \
        handbrake-cli \
        audacity \
        lame \
        twolame \
        faac \
        faad2 \
        libfdk-aac \
        opencore-amr \
        openjpeg2 \
        opus \
        speex \
        libtheora \
        libvorbis \
        libvpx \
        libwebp \
        x264 \
        x265 \
        xvidcore \
        obs-studio \
        kdenlive \
        openshot
    
    # Install AUR packages (optional)
    if command -v yay >/dev/null 2>&1; then
        log_info "Installing AUR multimedia packages..."
        yay -S --noconfirm \
            google-chrome \
            spotify \
            discord \
            zoom
    fi
}

# Install additional media tools
install_media_tools() {
    log_info "Installing additional media processing tools..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt install -y \
                mediainfo \
                mediainfo-gui \
                exiftool \
                imagemagick \
                graphicsmagick \
                gimp \
                inkscape \
                blender \
                makemkv-bin \
                makemkv-oss \
                dvdbackup \
                vobcopy \
                gddccontrol \
                brasero \
                k3b \
                soundconverter \
                rhythmbox \
                clementine \
                vlc-bin \
                mpv \
                smplayer \
                parole \
                totem \
                cheese \
                guvcview \
                simplescreenrecorder \
                kazam \
                vokoscreen-ng \
                peek \
                flameshot \
                shutter
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*|*"Fedora"*)
            sudo $PKG_MANAGER install -y \
                mediainfo \
                mediainfo-gui \
                perl-Image-ExifTool \
                ImageMagick \
                GraphicsMagick \
                gimp \
                inkscape \
                blender \
                brasero \
                k3b \
                soundconverter \
                rhythmbox \
                clementine \
                mpv \
                smplayer \
                parole \
                totem \
                cheese \
                guvcview \
                simplescreenrecorder \
                vokoscreen \
                flameshot
            ;;
        *"Arch"*)
            sudo pacman -S --noconfirm \
                mediainfo \
                mediainfo-gui \
                perl-image-exiftool \
                imagemagick \
                graphicsmagick \
                gimp \
                inkscape \
                blender \
                brasero \
                k3b \
                soundkonverter \
                rhythmbox \
                clementine \
                mpv \
                smplayer \
                parole \
                totem \
                cheese \
                guvcview \
                simplescreenrecorder \
                vokoscreen \
                flameshot
            ;;
    esac
}

# Install programming language multimedia libraries
install_programming_libraries() {
    log_info "Installing programming language multimedia libraries..."
    
    # Python multimedia libraries (if venv exists)
    if [ -d "venv" ]; then
        log_info "Installing Python multimedia libraries in virtual environment..."
        source venv/bin/activate
        
        pip install --upgrade \
            opencv-python \
            opencv-contrib-python \
            imageio \
            imageio-ffmpeg \
            moviepy \
            scikit-image \
            mahotas \
            SimpleITK \
            pyaudio \
            pydub \
            librosa \
            soundfile \
            python-vlc \
            pyglet \
            pygame \
            kivy \
            pillow-heif \
            rawpy \
            exifread \
            pyexiv2 \
            face-recognition \
            dlib \
            mediapipe
    else
        log_warning "Virtual environment not found. Skipping Python multimedia libraries."
    fi
    
    # Node.js multimedia libraries (if npm is available)
    if command -v npm >/dev/null 2>&1; then
        log_info "Installing Node.js multimedia libraries..."
        sudo npm install -g \
            ffmpeg-static \
            sharp \
            imagemin \
            imagemin-cli \
            video-cli \
            audio-cli
    fi
}

# Configure multimedia environment
configure_multimedia() {
    log_info "Configuring multimedia environment..."
    
    # Set up user groups
    sudo usermod -a -G audio $USER
    sudo usermod -a -G video $USER
    sudo usermod -a -G pulse-access $USER
    
    # Configure environment variables
    cat >> ~/.bashrc << 'EOF'

# IGED Multimedia Configuration
export GST_PLUGIN_PATH="/usr/lib/gstreamer-1.0"
export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"
export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH"

# FFmpeg configuration
export FFMPEG_DATADIR="/usr/share/ffmpeg"
export FFMPEG_LIBDIR="/usr/lib/x86_64-linux-gnu"

# GStreamer configuration
export GST_DEBUG=2
export GST_DEBUG_NO_COLOR=1

# Video acceleration
export LIBVA_DRIVER_NAME=i965
export VDPAU_DRIVER=va_gl
EOF
    
    # Create multimedia test script
    cat > test_multimedia.py << 'EOF'
#!/usr/bin/env python3
"""
IGED Multimedia Test Script
Tests multimedia codec functionality
"""

import sys
import traceback

def test_image_processing():
    """Test image processing capabilities"""
    try:
        from PIL import Image
        import numpy as np
        print("✅ PIL/Pillow: OK")
        
        try:
            import cv2
            print("✅ OpenCV: OK")
        except ImportError:
            print("❌ OpenCV: Not available")
            
        try:
            import imageio
            print("✅ ImageIO: OK")
        except ImportError:
            print("❌ ImageIO: Not available")
            
    except ImportError as e:
        print(f"❌ Image processing: {e}")

def test_audio_processing():
    """Test audio processing capabilities"""
    try:
        import pyaudio
        print("✅ PyAudio: OK")
    except ImportError:
        print("❌ PyAudio: Not available")
        
    try:
        import pydub
        print("✅ Pydub: OK")
    except ImportError:
        print("❌ Pydub: Not available")
        
    try:
        import soundfile
        print("✅ SoundFile: OK")
    except ImportError:
        print("❌ SoundFile: Not available")
        
    try:
        import librosa
        print("✅ Librosa: OK")
    except ImportError:
        print("❌ Librosa: Not available")

def test_video_processing():
    """Test video processing capabilities"""
    try:
        import moviepy.editor
        print("✅ MoviePy: OK")
    except ImportError:
        print("❌ MoviePy: Not available")
        
    try:
        import ffmpeg
        print("✅ FFmpeg-python: OK")
    except ImportError:
        print("❌ FFmpeg-python: Not available")

def test_system_codecs():
    """Test system codec availability"""
    import subprocess
    import os
    
    # Test FFmpeg
    try:
        result = subprocess.run(['ffmpeg', '-version'], 
                              capture_output=True, text=True)
        if result.returncode == 0:
            print("✅ FFmpeg system binary: OK")
        else:
            print("❌ FFmpeg system binary: Not working")
    except FileNotFoundError:
        print("❌ FFmpeg system binary: Not found")
    
    # Test GStreamer
    try:
        result = subprocess.run(['gst-inspect-1.0', '--version'], 
                              capture_output=True, text=True)
        if result.returncode == 0:
            print("✅ GStreamer: OK")
        else:
            print("❌ GStreamer: Not working")
    except FileNotFoundError:
        print("❌ GStreamer: Not found")
    
    # Test VLC
    try:
        result = subprocess.run(['vlc', '--version'], 
                              capture_output=True, text=True)
        if result.returncode == 0:
            print("✅ VLC: OK")
        else:
            print("❌ VLC: Not working")
    except FileNotFoundError:
        print("❌ VLC: Not found")

def main():
    print("🎬 IGED Multimedia Test")
    print("=" * 50)
    
    print("\n📸 Image Processing:")
    test_image_processing()
    
    print("\n🎵 Audio Processing:")
    test_audio_processing()
    
    print("\n🎥 Video Processing:")
    test_video_processing()
    
    print("\n🔧 System Codecs:")
    test_system_codecs()
    
    print("\n" + "=" * 50)
    print("✅ Multimedia test completed!")

if __name__ == "__main__":
    main()
EOF
    
    chmod +x test_multimedia.py
    log_success "Multimedia test script created: test_multimedia.py"
}

# Main installation function
main() {
    log_info "Starting IGED multimedia codecs installation..."
    
    detect_os
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            install_ubuntu_codecs
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*|*"Fedora"*)
            install_redhat_codecs
            ;;
        *"Arch"*)
            install_arch_codecs
            ;;
        *)
            log_error "Unsupported operating system: $OS"
            exit 1
            ;;
    esac
    
    install_media_tools
    install_programming_libraries
    configure_multimedia
    
    log_success "Multimedia codecs installation completed!"
    log_info "Please restart your session for group changes to take effect."
    log_info "Run './test_multimedia.py' to test multimedia functionality."
}

# Run main function
main "$@"