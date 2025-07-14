"""
System Information Plugin for IGED
Gathers comprehensive system information

This plugin provides detailed system information gathering capabilities
for the IGED system, including hardware specs, OS details, and performance metrics.
"""

import logging
import platform
import subprocess
from datetime import datetime
from typing import Dict, Any, Optional, List
from pathlib import Path

# Optional imports for enhanced system information
try:
    import psutil
    PSUTIL_AVAILABLE = True
except ImportError:
    PSUTIL_AVAILABLE = False

try:
    import socket
    SOCKET_AVAILABLE = True
except ImportError:
    SOCKET_AVAILABLE = False

logger = logging.getLogger(__name__)


class SystemInfoPlugin:
    """
    System information gathering plugin for IGED.
    
    This plugin provides comprehensive system information including
    hardware specifications, operating system details, network configuration,
    and performance metrics.
    """
    
    def __init__(self):
        """Initialize the system info plugin."""
        self.plugin_name = "System Information"
        self.plugin_version = "1.0.0"
        self.plugin_description = "Comprehensive system information gathering"
        self.capabilities = [
            "hardware_info",
            "os_info",
            "network_info",
            "performance_metrics",
            "disk_usage",
            "memory_info",
            "cpu_info",
            "process_info"
        ]
        
        logger.info(f"📊 {self.plugin_name} plugin initialized")
    
    def execute(self, command: str = "", parameters: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """
        Execute system information gathering.
        
        Args:
            command: Command string (optional)
            parameters: Command parameters (optional)
            
        Returns:
            Dictionary containing system information
        """
        try:
            logger.info("🔍 Gathering system information...")
            
            # Determine what information to gather
            if parameters and 'info_type' in parameters:
                info_type = parameters['info_type']
                return self._get_specific_info(info_type)
            else:
                # Get comprehensive system information
                return self._get_comprehensive_info()
                
        except Exception as e:
            logger.error(f"❌ System info gathering failed: {e}")
            return {
                'success': False,
                'error': str(e),
                'timestamp': datetime.now().isoformat()
            }
    
    def _get_comprehensive_info(self) -> Dict[str, Any]:
        """
        Get comprehensive system information.
        
        Returns:
            Dictionary containing all available system information
        """
        try:
            system_info = {
                'success': True,
                'timestamp': datetime.now().isoformat(),
                'plugin_info': {
                    'name': self.plugin_name,
                    'version': self.plugin_version,
                    'capabilities': self.capabilities
                },
                'basic_info': self._get_basic_info(),
                'hardware_info': self._get_hardware_info(),
                'os_info': self._get_os_info(),
                'network_info': self._get_network_info(),
                'performance_info': self._get_performance_info(),
                'disk_info': self._get_disk_info(),
                'memory_info': self._get_memory_info(),
                'cpu_info': self._get_cpu_info(),
                'process_info': self._get_process_info()
            }
            
            logger.info("✅ System information gathered successfully")
            return system_info
            
        except Exception as e:
            logger.error(f"❌ Comprehensive info gathering failed: {e}")
            return {
                'success': False,
                'error': str(e),
                'timestamp': datetime.now().isoformat()
            }
    
    def _get_specific_info(self, info_type: str) -> Dict[str, Any]:
        """
        Get specific type of system information.
        
        Args:
            info_type: Type of information to gather
            
        Returns:
            Dictionary containing requested information
        """
        try:
            info_methods = {
                'basic': self._get_basic_info,
                'hardware': self._get_hardware_info,
                'os': self._get_os_info,
                'network': self._get_network_info,
                'performance': self._get_performance_info,
                'disk': self._get_disk_info,
                'memory': self._get_memory_info,
                'cpu': self._get_cpu_info,
                'process': self._get_process_info
            }
            
            if info_type in info_methods:
                info_data = info_methods[info_type]()
                return {
                    'success': True,
                    'info_type': info_type,
                    'timestamp': datetime.now().isoformat(),
                    'data': info_data
                }
            else:
                return {
                    'success': False,
                    'error': f'Unknown info type: {info_type}',
                    'available_types': list(info_methods.keys())
                }
                
        except Exception as e:
            logger.error(f"❌ Specific info gathering failed: {e}")
            return {
                'success': False,
                'error': str(e),
                'info_type': info_type
            }
    
    def _get_basic_info(self) -> Dict[str, Any]:
        """
        Get basic system information.
        
        Returns:
            Dictionary containing basic system information
        """
        try:
            return {
                'hostname': platform.node(),
                'platform': platform.platform(),
                'system': platform.system(),
                'release': platform.release(),
                'version': platform.version(),
                'machine': platform.machine(),
                'processor': platform.processor(),
                'python_version': platform.python_version(),
                'python_implementation': platform.python_implementation(),
                'python_compiler': platform.python_compiler(),
                'architecture': platform.architecture(),
                'timestamp': datetime.now().isoformat()
            }
            
        except Exception as e:
            logger.error(f"❌ Basic info gathering failed: {e}")
            return {'error': str(e)}
    
    def _get_hardware_info(self) -> Dict[str, Any]:
        """
        Get hardware information.
        
        Returns:
            Dictionary containing hardware information
        """
        try:
            hardware_info = {
                'machine': platform.machine(),
                'processor': platform.processor(),
                'architecture': platform.architecture()
            }
            
            # Enhanced hardware info with psutil
            if PSUTIL_AVAILABLE:
                try:
                    hardware_info.update({
                        'cpu_count_physical': psutil.cpu_count(logical=False),
                        'cpu_count_logical': psutil.cpu_count(logical=True),
                        'cpu_frequency': psutil.cpu_freq()._asdict() if psutil.cpu_freq() else None,
                        'memory_total': psutil.virtual_memory().total,
                        'memory_available': psutil.virtual_memory().available,
                        'swap_total': psutil.swap_memory().total,
                        'disk_partitions': [
                            {
                                'device': partition.device,
                                'mountpoint': partition.mountpoint,
                                'fstype': partition.fstype,
                                'opts': partition.opts
                            }
                            for partition in psutil.disk_partitions()
                        ]
                    })
                except Exception as e:
                    logger.warning(f"⚠️ Enhanced hardware info failed: {e}")
            
            return hardware_info
            
        except Exception as e:
            logger.error(f"❌ Hardware info gathering failed: {e}")
            return {'error': str(e)}
    
    def _get_os_info(self) -> Dict[str, Any]:
        """
        Get operating system information.
        
        Returns:
            Dictionary containing OS information
        """
        try:
            os_info = {
                'system': platform.system(),
                'release': platform.release(),
                'version': platform.version(),
                'platform': platform.platform(),
                'platform_version': platform.platform(aliased=True),
                'uname': platform.uname()._asdict()
            }
            
            # Add boot time if available
            if PSUTIL_AVAILABLE:
                try:
                    boot_time = psutil.boot_time()
                    os_info['boot_time'] = datetime.fromtimestamp(boot_time).isoformat()
                    os_info['uptime_seconds'] = datetime.now().timestamp() - boot_time
                except Exception as e:
                    logger.warning(f"⚠️ Boot time info failed: {e}")
            
            # Get additional OS-specific info
            if platform.system() == 'Linux':
                os_info.update(self._get_linux_info())
            elif platform.system() == 'Windows':
                os_info.update(self._get_windows_info())
            elif platform.system() == 'Darwin':
                os_info.update(self._get_macos_info())
            
            return os_info
            
        except Exception as e:
            logger.error(f"❌ OS info gathering failed: {e}")
            return {'error': str(e)}
    
    def _get_linux_info(self) -> Dict[str, Any]:
        """
        Get Linux-specific information.
        
        Returns:
            Dictionary containing Linux-specific information
        """
        try:
            linux_info = {}
            
            # Get distribution info
            try:
                import distro
                linux_info.update({
                    'distribution': distro.name(),
                    'distribution_version': distro.version(),
                    'distribution_codename': distro.codename()
                })
            except ImportError:
                 # Fallback for systems without distro package
                 try:
                     with open('/etc/os-release', 'r') as f:
                         os_release = f.read()
                         for line in os_release.split('\n'):
                             if line.startswith('NAME='):
                                 linux_info['distribution'] = line.split('=')[1].strip('"')
                             elif line.startswith('VERSION='):
                                 linux_info['distribution_version'] = line.split('=')[1].strip('"')
                 except:
                     pass
            
            # Get kernel version
            try:
                result = subprocess.run(['uname', '-r'], capture_output=True, text=True)
                if result.returncode == 0:
                    linux_info['kernel_version'] = result.stdout.strip()
            except Exception:
                pass
            
            return linux_info
            
        except Exception as e:
            logger.error(f"❌ Linux info gathering failed: {e}")
            return {'error': str(e)}
    
    def _get_windows_info(self) -> Dict[str, Any]:
        """
        Get Windows-specific information.
        
        Returns:
            Dictionary containing Windows-specific information
        """
        try:
            windows_info = {
                'win32_version': platform.win32_ver(),
                'win32_edition': platform.win32_edition() if hasattr(platform, 'win32_edition') else None
            }
            
            # Get Windows version details
            try:
                result = subprocess.run(['ver'], shell=True, capture_output=True, text=True)
                if result.returncode == 0:
                    windows_info['version_string'] = result.stdout.strip()
            except Exception:
                pass
            
            return windows_info
            
        except Exception as e:
            logger.error(f"❌ Windows info gathering failed: {e}")
            return {'error': str(e)}
    
    def _get_macos_info(self) -> Dict[str, Any]:
        """
        Get macOS-specific information.
        
        Returns:
            Dictionary containing macOS-specific information
        """
        try:
            macos_info = {
                'mac_version': platform.mac_ver()
            }
            
            # Get macOS version details
            try:
                result = subprocess.run(['sw_vers'], capture_output=True, text=True)
                if result.returncode == 0:
                    macos_info['sw_vers'] = result.stdout.strip()
            except Exception:
                pass
            
            return macos_info
            
        except Exception as e:
            logger.error(f"❌ macOS info gathering failed: {e}")
            return {'error': str(e)}
    
    def _get_network_info(self) -> Dict[str, Any]:
        """
        Get network information.
        
        Returns:
            Dictionary containing network information
        """
        try:
            network_info = {
                'hostname': platform.node()
            }
            
            if SOCKET_AVAILABLE:
                try:
                    network_info.update({
                        'fqdn': socket.getfqdn(),
                        'local_ip': socket.gethostbyname(socket.gethostname())
                    })
                except Exception as e:
                    logger.warning(f"⚠️ Socket network info failed: {e}")
            
            if PSUTIL_AVAILABLE:
                try:
                    # Get network interfaces
                    interfaces = psutil.net_if_addrs()
                    network_info['interfaces'] = {}
                    
                    for interface_name, addresses in interfaces.items():
                        interface_info = []
                        for address in addresses:
                            interface_info.append({
                                'family': str(address.family),
                                'address': address.address,
                                'netmask': address.netmask,
                                'broadcast': address.broadcast,
                                'ptp': address.ptp
                            })
                        network_info['interfaces'][interface_name] = interface_info
                    
                    # Get network statistics
                    net_io = psutil.net_io_counters()
                    network_info['io_counters'] = {
                        'bytes_sent': net_io.bytes_sent,
                        'bytes_recv': net_io.bytes_recv,
                        'packets_sent': net_io.packets_sent,
                        'packets_recv': net_io.packets_recv,
                        'errin': net_io.errin,
                        'errout': net_io.errout,
                        'dropin': net_io.dropin,
                        'dropout': net_io.dropout
                    }
                    
                except Exception as e:
                    logger.warning(f"⚠️ Enhanced network info failed: {e}")
            
            return network_info
            
        except Exception as e:
            logger.error(f"❌ Network info gathering failed: {e}")
            return {'error': str(e)}
    
    def _get_performance_info(self) -> Dict[str, Any]:
        """
        Get performance information.
        
        Returns:
            Dictionary containing performance information
        """
        try:
            performance_info = {}
            
            if PSUTIL_AVAILABLE:
                try:
                    # CPU usage
                    performance_info['cpu_percent'] = psutil.cpu_percent(interval=1)
                    performance_info['cpu_times'] = psutil.cpu_times()._asdict()
                    
                    # Memory usage
                    memory = psutil.virtual_memory()
                    performance_info['memory'] = {
                        'total': memory.total,
                        'available': memory.available,
                        'percent': memory.percent,
                        'used': memory.used,
                        'free': memory.free
                    }
                    
                    # Swap usage
                    swap = psutil.swap_memory()
                    performance_info['swap'] = {
                        'total': swap.total,
                        'used': swap.used,
                        'free': swap.free,
                        'percent': swap.percent
                    }
                    
                    # Load average (Unix-like systems)
                    if hasattr(psutil, 'getloadavg'):
                        performance_info['load_average'] = psutil.getloadavg()
                    
                except Exception as e:
                    logger.warning(f"⚠️ Performance info failed: {e}")
                    performance_info['error'] = str(e)
            else:
                performance_info['error'] = 'psutil not available'
            
            return performance_info
            
        except Exception as e:
            logger.error(f"❌ Performance info gathering failed: {e}")
            return {'error': str(e)}
    
    def _get_disk_info(self) -> Dict[str, Any]:
        """
        Get disk information.
        
        Returns:
            Dictionary containing disk information
        """
        try:
            disk_info = {}
            
            if PSUTIL_AVAILABLE:
                try:
                    # Disk partitions
                    partitions = psutil.disk_partitions()
                    disk_info['partitions'] = []
                    
                    for partition in partitions:
                        try:
                            partition_usage = psutil.disk_usage(partition.mountpoint)
                            partition_info = {
                                'device': partition.device,
                                'mountpoint': partition.mountpoint,
                                'fstype': partition.fstype,
                                'opts': partition.opts,
                                'total': partition_usage.total,
                                'used': partition_usage.used,
                                'free': partition_usage.free,
                                'percent': (partition_usage.used / partition_usage.total) * 100
                            }
                            disk_info['partitions'].append(partition_info)
                        except PermissionError:
                            # Skip partitions we can't access
                            continue
                    
                    # Disk I/O statistics
                    disk_io = psutil.disk_io_counters()
                    if disk_io:
                        disk_info['io_counters'] = {
                            'read_count': disk_io.read_count,
                            'write_count': disk_io.write_count,
                            'read_bytes': disk_io.read_bytes,
                            'write_bytes': disk_io.write_bytes,
                            'read_time': disk_io.read_time,
                            'write_time': disk_io.write_time
                        }
                    
                except Exception as e:
                    logger.warning(f"⚠️ Disk info failed: {e}")
                    disk_info['error'] = str(e)
            else:
                disk_info['error'] = 'psutil not available'
            
            return disk_info
            
        except Exception as e:
            logger.error(f"❌ Disk info gathering failed: {e}")
            return {'error': str(e)}
    
    def _get_memory_info(self) -> Dict[str, Any]:
        """
        Get memory information.
        
        Returns:
            Dictionary containing memory information
        """
        try:
            memory_info = {}
            
            if PSUTIL_AVAILABLE:
                try:
                    # Virtual memory
                    virtual_memory = psutil.virtual_memory()
                    memory_info['virtual'] = {
                        'total': virtual_memory.total,
                        'available': virtual_memory.available,
                        'percent': virtual_memory.percent,
                        'used': virtual_memory.used,
                        'free': virtual_memory.free,
                        'active': getattr(virtual_memory, 'active', None),
                        'inactive': getattr(virtual_memory, 'inactive', None),
                        'buffers': getattr(virtual_memory, 'buffers', None),
                        'cached': getattr(virtual_memory, 'cached', None),
                        'shared': getattr(virtual_memory, 'shared', None),
                        'slab': getattr(virtual_memory, 'slab', None)
                    }
                    
                    # Swap memory
                    swap_memory = psutil.swap_memory()
                    memory_info['swap'] = {
                        'total': swap_memory.total,
                        'used': swap_memory.used,
                        'free': swap_memory.free,
                        'percent': swap_memory.percent,
                        'sin': swap_memory.sin,
                        'sout': swap_memory.sout
                    }
                    
                except Exception as e:
                    logger.warning(f"⚠️ Memory info failed: {e}")
                    memory_info['error'] = str(e)
            else:
                memory_info['error'] = 'psutil not available'
            
            return memory_info
            
        except Exception as e:
            logger.error(f"❌ Memory info gathering failed: {e}")
            return {'error': str(e)}
    
    def _get_cpu_info(self) -> Dict[str, Any]:
        """
        Get CPU information.
        
        Returns:
            Dictionary containing CPU information
        """
        try:
            cpu_info = {
                'processor': platform.processor(),
                'machine': platform.machine(),
                'architecture': platform.architecture()
            }
            
            if PSUTIL_AVAILABLE:
                try:
                    cpu_info.update({
                        'physical_cores': psutil.cpu_count(logical=False),
                        'logical_cores': psutil.cpu_count(logical=True),
                        'max_frequency': psutil.cpu_freq().max if psutil.cpu_freq() else None,
                        'min_frequency': psutil.cpu_freq().min if psutil.cpu_freq() else None,
                        'current_frequency': psutil.cpu_freq().current if psutil.cpu_freq() else None,
                        'cpu_percent': psutil.cpu_percent(interval=1),
                        'cpu_times': psutil.cpu_times()._asdict(),
                        'cpu_stats': psutil.cpu_stats()._asdict()
                    })
                    
                    # Per-CPU statistics
                    cpu_info['per_cpu_percent'] = psutil.cpu_percent(interval=1, percpu=True)
                    cpu_info['per_cpu_times'] = [cpu_time._asdict() for cpu_time in psutil.cpu_times(percpu=True)]
                    
                except Exception as e:
                    logger.warning(f"⚠️ Enhanced CPU info failed: {e}")
            
            return cpu_info
            
        except Exception as e:
            logger.error(f"❌ CPU info gathering failed: {e}")
            return {'error': str(e)}
    
    def _get_process_info(self) -> Dict[str, Any]:
        """
        Get process information.
        
        Returns:
            Dictionary containing process information
        """
        try:
            process_info = {}
            
            if PSUTIL_AVAILABLE:
                try:
                    # Current process info
                    current_process = psutil.Process()
                    process_info['current_process'] = {
                        'pid': current_process.pid,
                        'name': current_process.name(),
                        'status': current_process.status(),
                        'cpu_percent': current_process.cpu_percent(),
                        'memory_percent': current_process.memory_percent(),
                        'memory_info': current_process.memory_info()._asdict(),
                        'create_time': current_process.create_time(),
                        'num_threads': current_process.num_threads()
                    }
                    
                    # System process summary
                    processes = list(psutil.process_iter(['pid', 'name', 'cpu_percent', 'memory_percent']))
                    process_info['process_count'] = len(processes)
                    
                    # Top processes by CPU
                    top_cpu_processes = sorted(
                        processes, 
                        key=lambda p: p.info['cpu_percent'] or 0, 
                        reverse=True
                    )[:10]
                    
                    process_info['top_cpu_processes'] = [
                        {
                            'pid': proc.info['pid'],
                            'name': proc.info['name'],
                            'cpu_percent': proc.info['cpu_percent']
                        }
                        for proc in top_cpu_processes
                    ]
                    
                    # Top processes by memory
                    top_memory_processes = sorted(
                        processes, 
                        key=lambda p: p.info['memory_percent'] or 0, 
                        reverse=True
                    )[:10]
                    
                    process_info['top_memory_processes'] = [
                        {
                            'pid': proc.info['pid'],
                            'name': proc.info['name'],
                            'memory_percent': proc.info['memory_percent']
                        }
                        for proc in top_memory_processes
                    ]
                    
                except Exception as e:
                    logger.warning(f"⚠️ Process info failed: {e}")
                    process_info['error'] = str(e)
            else:
                process_info['error'] = 'psutil not available'
            
            return process_info
            
        except Exception as e:
            logger.error(f"❌ Process info gathering failed: {e}")
            return {'error': str(e)}
    
    def get_capabilities(self) -> List[str]:
        """
        Get plugin capabilities.
        
        Returns:
            List of plugin capabilities
        """
        return self.capabilities.copy()
    
    def get_status(self) -> Dict[str, Any]:
        """
        Get plugin status.
        
        Returns:
            Dictionary containing plugin status
        """
        return {
            'name': self.plugin_name,
            'version': self.plugin_version,
            'description': self.plugin_description,
            'capabilities': self.capabilities,
            'dependencies': {
                'psutil': PSUTIL_AVAILABLE,
                'socket': SOCKET_AVAILABLE
            },
            'active': True
        }


# Plugin entry point functions
def main(command: str = "") -> Dict[str, Any]:
    """
    Main entry point for the plugin.
    
    Args:
        command: Command string
        
    Returns:
        Dictionary containing execution result
    """
    try:
        plugin = SystemInfoPlugin()
        return plugin.execute(command)
    except Exception as e:
        logger.error(f"❌ Plugin execution failed: {e}")
        return {
            'success': False,
            'error': str(e),
            'plugin': 'system_info'
        }


def execute(command: str = "", parameters: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    """
    Execute the plugin with parameters.
    
    Args:
        command: Command string
        parameters: Command parameters
        
    Returns:
        Dictionary containing execution result
    """
    try:
        plugin = SystemInfoPlugin()
        return plugin.execute(command, parameters)
    except Exception as e:
        logger.error(f"❌ Plugin execution failed: {e}")
        return {
            'success': False,
            'error': str(e),
            'plugin': 'system_info'
        }


# Create plugin instance for direct usage
plugin_instance = SystemInfoPlugin()


if __name__ == "__main__":
    # Test the plugin
    result = main("system_info")
    print(f"System Info Plugin Result: {result}") 