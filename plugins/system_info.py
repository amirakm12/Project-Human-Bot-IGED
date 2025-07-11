"""
System Information Plugin for IGED
Provides system information and diagnostics
"""

import platform
from datetime import datetime

# Try to import psutil
try:
    import psutil

    PSUTIL_AVAILABLE = True
except ImportError:
    PSUTIL_AVAILABLE = False


class Plugin:
    """Plugin implementation."""

    def __init__(self) -> None:
        """Init   function."""
        self.name = "System Info"
        self.version = "1.0.0"
        self.description = "Provides system information and diagnostics"

    def run(self, input_text) -> None:
        """Run the plugin with input text"""
        try:
            if "system" in input_text.lower() or "info" in input_text.lower():
                return self._get_system_info()
            elif "memory" in input_text.lower() or "ram" in input_text.lower():
                return self._get_memory_info()
            elif "cpu" in input_text.lower() or "processor" in input_text.lower():
                return self._get_cpu_info()
            elif "disk" in input_text.lower() or "storage" in input_text.lower():
                return self._get_disk_info()
            elif "network" in input_text.lower():
                return self._get_network_info()
            else:
                return self._get_full_system_report()

        except Exception as e:
            return f"❌ System info error: {str(e)}"

    def _get_system_info(self) -> None:
        """Get basic system information"""
        info = f"🖥️ System Information\n"
        info += f"=" * 40 + "\n"
        info += f"OS: {platform.system()} {platform.release()}\n"
        info += f"Architecture: {platform.machine()}\n"
        info += f"Python: {platform.python_version()}\n"
        info += f"Hostname: {platform.node()}\n"
        info += f"Uptime: {self._get_uptime()}\n"
        return info

    def _get_memory_info(self) -> None:
        """Get memory information"""
        if not PSUTIL_AVAILABLE:
            return "💾 Memory Information\n" + "=" * 30 + "\n❌ psutil not available"

        memory = psutil.virtual_memory()
        info = f"💾 Memory Information\n"
        info += f"=" * 30 + "\n"
        info += f"Total: {memory.total / (1024**3):.2f} GB\n"
        info += f"Available: {memory.available / (1024**3):.2f} GB\n"
        info += f"Used: {memory.used / (1024**3):.2f} GB\n"
        info += f"Usage: {memory.percent}%\n"
        return info

    def _get_cpu_info(self) -> None:
        """Get CPU information"""
        if not PSUTIL_AVAILABLE:
            return "🖥️ CPU Information\n" + "=" * 25 + "\n❌ psutil not available"

        info = f"🖥️ CPU Information\n"
        info += f"=" * 25 + "\n"
        info += f"CPU Count: {psutil.cpu_count()}\n"
        info += f"CPU Usage: {psutil.cpu_percent(interval=1)}%\n"
        info += f"CPU Frequency: {psutil.cpu_freq().current:.2f} MHz\n"
        return info

    def _get_disk_info(self) -> None:
        """Get disk information"""
        if not PSUTIL_AVAILABLE:
            return "💿 Disk Information\n" + "=" * 25 + "\n❌ psutil not available"

        disk = psutil.disk_usage("/")
        info = f"💿 Disk Information\n"
        info += f"=" * 25 + "\n"
        info += f"Total: {disk.total / (1024**3):.2f} GB\n"
        info += f"Used: {disk.used / (1024**3):.2f} GB\n"
        info += f"Free: {disk.free / (1024**3):.2f} GB\n"
        info += f"Usage: {disk.percent}%\n"
        return info

    def _get_network_info(self) -> None:
        """Get network information"""
        if not PSUTIL_AVAILABLE:
            return "🌐 Network Information\n" + "=" * 30 + "\n❌ psutil not available"

        net_io = psutil.net_io_counters()
        info = f"🌐 Network Information\n"
        info += f"=" * 30 + "\n"
        info += f"Bytes Sent: {net_io.bytes_sent / (1024**2):.2f} MB\n"
        info += f"Bytes Received: {net_io.bytes_recv / (1024**2):.2f} MB\n"
        info += f"Packets Sent: {net_io.packets_sent}\n"
        info += f"Packets Received: {net_io.packets_recv}\n"
        return info

    def _get_uptime(self) -> None:
        """Get system uptime"""
        if not PSUTIL_AVAILABLE:
            return "Unknown (psutil not available)"

        uptime = datetime.now() - datetime.fromtimestamp(psutil.boot_time())
        days = uptime.days
        hours, remainder = divmod(uptime.seconds, 3600)
        minutes, seconds = divmod(remainder, 60)
        return f"{days}d {hours}h {minutes}m {seconds}s"

    def _get_full_system_report(self) -> None:
        """Get comprehensive system report"""
        report = self._get_system_info() + "\n"
        report += self._get_memory_info() + "\n"
        report += self._get_cpu_info() + "\n"
        report += self._get_disk_info() + "\n"
        report += self._get_network_info()
        return report

    def get_info(self) -> None:
        """Get plugin information"""
        return {
            "name": self.name,
            "version": self.version,
            "description": self.description,
        }
