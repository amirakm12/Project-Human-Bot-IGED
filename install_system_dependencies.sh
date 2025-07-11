#!/bin/bash

# IGED System Dependencies Installation Script
# Installs all required system packages, codecs, and tools for IGED
# Supports Ubuntu/Debian, CentOS/RHEL/Fedora, and Arch Linux

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

# Check if running as root
check_root() {
    if [[ $EUID -eq 0 ]]; then
        log_warning "Running as root. This script should be run as a regular user with sudo privileges."
        read -p "Continue anyway? (y/n): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
}

# Update package manager
update_packages() {
    log_info "Updating package manager..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt update && sudo apt upgrade -y
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*)
            sudo yum update -y || sudo dnf update -y
            ;;
        *"Fedora"*)
            sudo dnf update -y
            ;;
        *"Arch"*)
            sudo pacman -Syu --noconfirm
            ;;
        *)
            log_warning "Unknown OS. Skipping package manager update."
            ;;
    esac
}

# Install base development tools
install_base_tools() {
    log_info "Installing base development tools..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt install -y \
                build-essential \
                cmake \
                git \
                curl \
                wget \
                unzip \
                zip \
                tar \
                gzip \
                software-properties-common \
                apt-transport-https \
                ca-certificates \
                gnupg \
                lsb-release
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*)
            sudo yum groupinstall -y "Development Tools" || sudo dnf groupinstall -y "Development Tools"
            sudo yum install -y git curl wget unzip zip tar gzip || sudo dnf install -y git curl wget unzip zip tar gzip
            ;;
        *"Fedora"*)
            sudo dnf groupinstall -y "Development Tools"
            sudo dnf install -y git curl wget unzip zip tar gzip
            ;;
        *"Arch"*)
            sudo pacman -S --noconfirm base-devel git curl wget unzip zip tar gzip
            ;;
    esac
}

# Install Python and development headers
install_python() {
    log_info "Installing Python and development headers..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt install -y \
                python3 \
                python3-dev \
                python3-pip \
                python3-venv \
                python3-setuptools \
                python3-wheel \
                python3-tk \
                idle3
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*)
            sudo yum install -y python3 python3-devel python3-pip python3-tkinter || \
            sudo dnf install -y python3 python3-devel python3-pip python3-tkinter
            ;;
        *"Fedora"*)
            sudo dnf install -y python3 python3-devel python3-pip python3-tkinter
            ;;
        *"Arch"*)
            sudo pacman -S --noconfirm python python-pip tk
            ;;
    esac
}

# Install audio codecs and libraries
install_audio_codecs() {
    log_info "Installing audio codecs and libraries..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt install -y \
                alsa-utils \
                alsa-base \
                alsa-firmware-loaders \
                pulseaudio \
                pulseaudio-utils \
                pavucontrol \
                libasound2-dev \
                libpulse-dev \
                portaudio19-dev \
                libportaudio2 \
                libportaudiocpp0 \
                ffmpeg \
                libavcodec-dev \
                libavformat-dev \
                libavutil-dev \
                libswscale-dev \
                libavresample-dev \
                gstreamer1.0-tools \
                gstreamer1.0-alsa \
                gstreamer1.0-pulseaudio \
                gstreamer1.0-plugins-base \
                gstreamer1.0-plugins-good \
                gstreamer1.0-plugins-bad \
                gstreamer1.0-plugins-ugly \
                libgstreamer1.0-dev \
                libgstreamer-plugins-base1.0-dev \
                sox \
                libsox-dev \
                libsndfile1-dev \
                libasound2-plugins
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*)
            # Enable EPEL repository
            sudo yum install -y epel-release || sudo dnf install -y epel-release
            # Enable RPM Fusion for multimedia codecs
            sudo yum install -y https://download1.rpmfusion.org/free/el/rpmfusion-free-release-$(rpm -E %rhel).noarch.rpm || \
            sudo dnf install -y https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm
            
            sudo yum install -y \
                alsa-utils \
                alsa-lib-devel \
                pulseaudio \
                pulseaudio-utils \
                portaudio-devel \
                ffmpeg \
                ffmpeg-devel \
                gstreamer1 \
                gstreamer1-devel \
                gstreamer1-plugins-base-devel \
                sox \
                sox-devel \
                libsndfile-devel || \
            sudo dnf install -y \
                alsa-utils \
                alsa-lib-devel \
                pulseaudio \
                pulseaudio-utils \
                portaudio-devel \
                ffmpeg \
                ffmpeg-devel \
                gstreamer1 \
                gstreamer1-devel \
                gstreamer1-plugins-base-devel \
                sox \
                sox-devel \
                libsndfile-devel
            ;;
        *"Fedora"*)
            # Enable RPM Fusion
            sudo dnf install -y https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm
            sudo dnf install -y https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm
            
            sudo dnf install -y \
                alsa-utils \
                alsa-lib-devel \
                pulseaudio \
                pulseaudio-utils \
                portaudio-devel \
                ffmpeg \
                ffmpeg-devel \
                gstreamer1 \
                gstreamer1-devel \
                gstreamer1-plugins-base-devel \
                sox \
                sox-devel \
                libsndfile-devel
            ;;
        *"Arch"*)
            sudo pacman -S --noconfirm \
                alsa-utils \
                alsa-lib \
                pulseaudio \
                pulseaudio-alsa \
                pavucontrol \
                portaudio \
                ffmpeg \
                gstreamer \
                gst-plugins-base \
                gst-plugins-good \
                gst-plugins-bad \
                gst-plugins-ugly \
                sox \
                libsndfile
            ;;
    esac
}

# Install video codecs and libraries
install_video_codecs() {
    log_info "Installing video codecs and libraries..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt install -y \
                libx264-dev \
                libx265-dev \
                libvpx-dev \
                libfdk-aac-dev \
                libmp3lame-dev \
                libopus-dev \
                libvorbis-dev \
                libtheora-dev \
                libopencore-amrnb-dev \
                libopencore-amrwb-dev \
                libxvidcore-dev \
                libv4l-dev \
                v4l-utils \
                libgphoto2-dev \
                libgstreamer-plugins-base1.0-dev \
                libgstreamer1.0-dev \
                libgtk-3-dev \
                libatlas-base-dev \
                gfortran \
                libhdf5-dev \
                libhdf5-serial-dev \
                libatlas3-base \
                libjasper-dev \
                libqtgui4 \
                libqt4-test
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*|*"Fedora"*)
            sudo ${PKG_MANAGER} install -y \
                x264-devel \
                x265-devel \
                libvpx-devel \
                fdk-aac-devel \
                lame-devel \
                opus-devel \
                libvorbis-devel \
                libtheora-devel \
                opencore-amr-devel \
                xvid-devel \
                libv4l-devel \
                v4l-utils \
                libgphoto2-devel \
                gtk3-devel \
                atlas-devel \
                hdf5-devel
            ;;
        *"Arch"*)
            sudo pacman -S --noconfirm \
                x264 \
                x265 \
                libvpx \
                libfdk-aac \
                lame \
                opus \
                libvorbis \
                libtheora \
                opencore-amr \
                xvidcore \
                v4l-utils \
                libgphoto2 \
                gtk3 \
                blas \
                lapack \
                hdf5
            ;;
    esac
}

# Install image processing libraries
install_image_libraries() {
    log_info "Installing image processing libraries..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt install -y \
                libjpeg-dev \
                libpng-dev \
                libtiff-dev \
                libwebp-dev \
                libopenjp2-7-dev \
                libjpeg8-dev \
                libjpeg-turbo8-dev \
                libpng12-dev \
                libtiff5-dev \
                libavcodec-dev \
                libavformat-dev \
                libswscale-dev \
                libdc1394-22-dev \
                libxine2-dev \
                libv4l-dev \
                libgtkglext1-dev \
                libqt4-dev \
                libfaac-dev \
                libmp3lame-dev \
                libopencore-amrnb-dev \
                libopencore-amrwb-dev \
                libtheora-dev \
                libvorbis-dev \
                libxvidcore-dev \
                x264 \
                v4l-utils \
                imagemagick \
                libmagick++-dev \
                graphicsmagick \
                libgraphicsmagick1-dev
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*|*"Fedora"*)
            sudo ${PKG_MANAGER} install -y \
                libjpeg-turbo-devel \
                libpng-devel \
                libtiff-devel \
                libwebp-devel \
                openjpeg2-devel \
                ImageMagick \
                ImageMagick-devel \
                GraphicsMagick \
                GraphicsMagick-devel
            ;;
        *"Arch"*)
            sudo pacman -S --noconfirm \
                libjpeg-turbo \
                libpng \
                libtiff \
                libwebp \
                openjpeg2 \
                imagemagick \
                graphicsmagick
            ;;
    esac
}

# Install networking and security tools
install_network_security_tools() {
    log_info "Installing networking and security tools..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt install -y \
                nmap \
                netcat \
                netcat-openbsd \
                socat \
                tcpdump \
                wireshark-common \
                tshark \
                libpcap-dev \
                libnetfilter-queue-dev \
                iptables \
                iptables-persistent \
                arping \
                dnsutils \
                bind9-utils \
                whois \
                traceroute \
                mtr \
                netdiscover \
                masscan \
                zmap \
                hping3 \
                ncat \
                nikto \
                dirb \
                gobuster \
                hydra \
                john \
                hashcat \
                aircrack-ng \
                reaver \
                pixiewps \
                bully \
                macchanger \
                ettercap-text-only \
                dsniff \
                sslstrip \
                ssldump \
                openssl \
                stunnel4 \
                proxychains \
                tor \
                privoxy
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*|*"Fedora"*)
            sudo ${PKG_MANAGER} install -y \
                nmap \
                netcat \
                socat \
                tcpdump \
                wireshark \
                libpcap-devel \
                libnetfilter_queue-devel \
                iptables \
                bind-utils \
                whois \
                traceroute \
                mtr \
                hping3 \
                nikto \
                hydra \
                john \
                hashcat \
                aircrack-ng \
                macchanger \
                ettercap \
                openssl \
                stunnel \
                proxychains-ng \
                tor \
                privoxy
            ;;
        *"Arch"*)
            sudo pacman -S --noconfirm \
                nmap \
                netcat \
                socat \
                tcpdump \
                wireshark-cli \
                libpcap \
                libnetfilter_queue \
                iptables \
                bind-tools \
                whois \
                traceroute \
                mtr \
                hping \
                nikto \
                hydra \
                john \
                hashcat \
                aircrack-ng \
                macchanger \
                ettercap \
                openssl \
                stunnel \
                proxychains-ng \
                tor \
                privoxy
            ;;
    esac
}

# Install system monitoring tools
install_monitoring_tools() {
    log_info "Installing system monitoring tools..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt install -y \
                htop \
                iotop \
                nethogs \
                iftop \
                bmon \
                vnstat \
                sysstat \
                lsof \
                strace \
                ltrace \
                gdb \
                valgrind \
                perf-tools-unstable \
                linux-tools-common \
                systemd-coredump \
                crash \
                kdump-tools
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*|*"Fedora"*)
            sudo ${PKG_MANAGER} install -y \
                htop \
                iotop \
                nethogs \
                iftop \
                bmon \
                vnstat \
                sysstat \
                lsof \
                strace \
                ltrace \
                gdb \
                valgrind \
                perf \
                systemd-devel \
                crash \
                kexec-tools
            ;;
        *"Arch"*)
            sudo pacman -S --noconfirm \
                htop \
                iotop \
                nethogs \
                iftop \
                bmon \
                vnstat \
                sysstat \
                lsof \
                strace \
                ltrace \
                gdb \
                valgrind \
                perf \
                systemd \
                crash
            ;;
    esac
}

# Install GUI and X11 libraries
install_gui_libraries() {
    log_info "Installing GUI and X11 libraries..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt install -y \
                xorg-dev \
                libx11-dev \
                libxext-dev \
                libxrandr-dev \
                libxinerama-dev \
                libxcursor-dev \
                libxi-dev \
                libxss-dev \
                libxxf86vm-dev \
                libxkbfile-dev \
                libxtst-dev \
                xvfb \
                x11vnc \
                fluxbox \
                openbox \
                gtk2-engines-pixbuf \
                gtk2-engines-murrine \
                gnome-themes-standard \
                ubuntu-desktop-minimal
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*|*"Fedora"*)
            sudo ${PKG_MANAGER} install -y \
                xorg-x11-server-devel \
                libX11-devel \
                libXext-devel \
                libXrandr-devel \
                libXinerama-devel \
                libXcursor-devel \
                libXi-devel \
                libXScrnSaver-devel \
                libXxf86vm-devel \
                libxkbfile-devel \
                libXtst-devel \
                xorg-x11-server-Xvfb \
                x11vnc \
                fluxbox \
                openbox \
                gtk2-engines \
                gnome-themes-standard
            ;;
        *"Arch"*)
            sudo pacman -S --noconfirm \
                xorg-server-devel \
                libx11 \
                libxext \
                libxrandr \
                libxinerama \
                libxcursor \
                libxi \
                libxss \
                libxxf86vm \
                libxkbfile \
                libxtst \
                xorg-server-xvfb \
                x11vnc \
                fluxbox \
                openbox \
                gtk-engines \
                gnome-themes-standard
            ;;
    esac
}

# Install database systems
install_databases() {
    log_info "Installing database systems..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt install -y \
                sqlite3 \
                libsqlite3-dev \
                redis-server \
                redis-tools \
                mongodb \
                mysql-server \
                mysql-client \
                libmysqlclient-dev \
                postgresql \
                postgresql-contrib \
                libpq-dev
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*|*"Fedora"*)
            sudo ${PKG_MANAGER} install -y \
                sqlite \
                sqlite-devel \
                redis \
                mongodb-server \
                mysql-server \
                mysql-devel \
                postgresql \
                postgresql-server \
                postgresql-devel
            ;;
        *"Arch"*)
            sudo pacman -S --noconfirm \
                sqlite \
                redis \
                mongodb \
                mysql \
                postgresql
            ;;
    esac
}

# Install additional utilities
install_utilities() {
    log_info "Installing additional utilities..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt install -y \
                vim \
                nano \
                emacs \
                tmux \
                screen \
                tree \
                jq \
                yq \
                xmlstarlet \
                lynx \
                w3m \
                elinks \
                curl \
                wget \
                aria2 \
                rsync \
                rclone \
                parallel \
                pv \
                progress \
                ncdu \
                fd-find \
                ripgrep \
                silversearcher-ag \
                bat \
                exa \
                zsh \
                fish \
                powerline \
                fonts-powerline \
                figlet \
                cowsay \
                fortune \
                sl
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*|*"Fedora"*)
            sudo ${PKG_MANAGER} install -y \
                vim \
                nano \
                emacs \
                tmux \
                screen \
                tree \
                jq \
                lynx \
                w3m \
                elinks \
                curl \
                wget \
                aria2 \
                rsync \
                parallel \
                pv \
                ncdu \
                ripgrep \
                zsh \
                fish \
                powerline \
                powerline-fonts \
                figlet \
                cowsay \
                fortune-mod
            ;;
        *"Arch"*)
            sudo pacman -S --noconfirm \
                vim \
                nano \
                emacs \
                tmux \
                screen \
                tree \
                jq \
                lynx \
                w3m \
                elinks \
                curl \
                wget \
                aria2 \
                rsync \
                parallel \
                pv \
                ncdu \
                fd \
                ripgrep \
                the_silver_searcher \
                bat \
                exa \
                zsh \
                fish \
                powerline \
                powerline-fonts \
                figlet \
                cowsay \
                fortune-mod
            ;;
    esac
}

# Configure audio system
configure_audio() {
    log_info "Configuring audio system..."
    
    # Add user to audio group
    sudo usermod -a -G audio $USER
    
    # Configure ALSA
    if [ ! -f ~/.asoundrc ]; then
        cat > ~/.asoundrc << EOF
pcm.!default {
    type pulse
}
ctl.!default {
    type pulse
}
EOF
    fi
    
    # Start and enable PulseAudio
    systemctl --user enable pulseaudio
    systemctl --user start pulseaudio
}

# Configure networking permissions
configure_networking() {
    log_info "Configuring networking permissions..."
    
    # Add user to necessary groups
    sudo usermod -a -G wireshark $USER
    sudo usermod -a -G netdev $USER
    
    # Configure Wireshark for non-root capture
    sudo dpkg-reconfigure wireshark-common 2>/dev/null || true
    
    # Set capabilities for network tools
    sudo setcap cap_net_raw,cap_net_admin=eip /usr/bin/nmap 2>/dev/null || true
    sudo setcap cap_net_raw=ep /usr/bin/hping3 2>/dev/null || true
}

# Install and configure firewall
configure_firewall() {
    log_info "Configuring firewall..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo ufw enable
            sudo ufw default deny incoming
            sudo ufw default allow outgoing
            sudo ufw allow ssh
            sudo ufw allow 8080/tcp  # Web admin panel
            sudo ufw allow 9090/tcp  # Android client
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*|*"Fedora"*)
            sudo systemctl enable firewalld
            sudo systemctl start firewalld
            sudo firewall-cmd --permanent --add-service=ssh
            sudo firewall-cmd --permanent --add-port=8080/tcp
            sudo firewall-cmd --permanent --add-port=9090/tcp
            sudo firewall-cmd --reload
            ;;
        *"Arch"*)
            sudo systemctl enable ufw
            sudo ufw enable
            sudo ufw default deny incoming
            sudo ufw default allow outgoing
            sudo ufw allow ssh
            sudo ufw allow 8080/tcp
            sudo ufw allow 9090/tcp
            ;;
    esac
}

# Clean up
cleanup() {
    log_info "Cleaning up..."
    
    case "$OS" in
        *"Ubuntu"*|*"Debian"*)
            sudo apt autoremove -y
            sudo apt autoclean
            ;;
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*)
            sudo yum autoremove -y || sudo dnf autoremove -y
            sudo yum clean all || sudo dnf clean all
            ;;
        *"Fedora"*)
            sudo dnf autoremove -y
            sudo dnf clean all
            ;;
        *"Arch"*)
            sudo pacman -Rns $(pacman -Qtdq) --noconfirm 2>/dev/null || true
            sudo pacman -Scc --noconfirm
            ;;
    esac
}

# Main installation function
main() {
    log_info "Starting IGED system dependencies installation..."
    
    # Set package manager variable
    case "$OS" in
        *"CentOS"*|*"Red Hat"*|*"Rocky"*|*"AlmaLinux"*)
            if command -v dnf >/dev/null 2>&1; then
                PKG_MANAGER="dnf"
            else
                PKG_MANAGER="yum"
            fi
            ;;
        *"Fedora"*)
            PKG_MANAGER="dnf"
            ;;
    esac
    
    detect_os
    check_root
    
    update_packages
    install_base_tools
    install_python
    install_audio_codecs
    install_video_codecs
    install_image_libraries
    install_network_security_tools
    install_monitoring_tools
    install_gui_libraries
    install_databases
    install_utilities
    
    configure_audio
    configure_networking
    configure_firewall
    cleanup
    
    log_success "System dependencies installation completed!"
    log_info "Please reboot your system to ensure all changes take effect."
    log_info "After reboot, run: ./install_python_requirements.sh"
}

# Run main function
main "$@"