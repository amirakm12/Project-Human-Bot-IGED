"""
Watchdog for IGED
System monitoring and health checks

This module provides comprehensive system monitoring capabilities for the IGED
system, including resource monitoring, health checks, and performance tracking.
"""

import logging
import threading
from datetime import datetime
from typing import Dict, Any, List, Callable, Optional

# Optional psutil import for system monitoring
try:
    import psutil
    PSUTIL_AVAILABLE = True
except ImportError:
    PSUTIL_AVAILABLE = False

logger = logging.getLogger(__name__)


class Watchdog:
    """
    System monitoring and health check manager for IGED.
    
    This class provides comprehensive monitoring of system resources,
    component health, and performance metrics for the IGED system.
    """
    
    def __init__(self, components: Dict[str, Any]):
        """
        Initialize the watchdog with system components.
        
        Args:
            components: Dictionary of system components to monitor
        """
        self.components = components
        self.running = False
        self.monitoring_thread: Optional[threading.Thread] = None
        self.health_checks: List[Callable] = []
        self.system_stats: Dict[str, Any] = {}
        
        # Initialize health checks
        self._setup_health_checks()
    
    def _setup_health_checks(self) -> None:
        """
        Setup system health checks.
        
        Initializes all health check functions that will be executed
        during monitoring cycles.
        """
        self.health_checks = [
            self._check_system_resources,
            self._check_component_health,
            self._check_memory_usage,
            self._check_disk_space
        ]
    
    def run(self) -> None:
        """
        Start the watchdog monitoring.
        
        Initiates the monitoring thread and begins system health checks.
        """
        if self.running:
            logger.warning("Watchdog already running")
            return
        
        self.running = True
        self.monitoring_thread = threading.Thread(target=self._monitoring_loop, daemon=True)
        self.monitoring_thread.start()
        logger.info("🔄 Watchdog monitoring started")
    
    def stop(self) -> None:
        """
        Stop the watchdog monitoring.
        
        Cleanly stops the monitoring thread and saves final statistics.
        """
        self.running = False
        logger.info("🛑 Watchdog monitoring stopped")
    
    def _monitoring_loop(self) -> None:
        """
        Main monitoring loop.
        
        Continuously executes health checks and updates system statistics
        while the watchdog is running.
        """
        import time
        
        while self.running:
            try:
                # Run all health checks
                for health_check in self.health_checks:
                    if not self.running:
                        break
                    
                    try:
                        health_check()
                    except Exception as e:
                        logger.error(f"❌ Health check failed: {health_check.__name__}: {e}")
                
                # Update system statistics
                self._update_system_stats()
                
                # Sleep for monitoring interval
                time.sleep(30)  # 30 seconds monitoring interval
                
            except Exception as e:
                logger.error(f"❌ Monitoring loop error: {e}")
                time.sleep(5)
    
    def _check_system_resources(self) -> None:
        """
        Check system resource usage.
        
        Monitors CPU, memory, and disk usage if psutil is available.
        """
        if not PSUTIL_AVAILABLE:
            logger.warning("⚠️ psutil not available, skipping system resource check")
            return
        
        try:
            # Check CPU usage
            cpu_percent = psutil.cpu_percent(interval=1)
            if cpu_percent > 80:
                logger.warning(f"⚠️ High CPU usage: {cpu_percent:.1f}%")
            
            # Check memory usage
            memory = psutil.virtual_memory()
            if memory.percent > 85:
                logger.warning(f"⚠️ High memory usage: {memory.percent:.1f}%")
            
            # Check disk usage
            disk = psutil.disk_usage('/')
            disk_percent = (disk.used / disk.total) * 100
            if disk_percent > 90:
                logger.warning(f"⚠️ High disk usage: {disk_percent:.1f}%")
            
            # Store resource stats
            self.system_stats.update({
                'cpu_percent': cpu_percent,
                'memory_percent': memory.percent,
                'disk_percent': disk_percent,
                'last_resource_check': datetime.now().isoformat()
            })
            
        except Exception as e:
            logger.error(f"❌ System resource check failed: {e}")
    
    def _check_component_health(self) -> None:
        """
        Check health of IGED components.
        
        Monitors the status and health of all registered components.
        """
        try:
            component_status = {}
            healthy_components = 0
            total_components = len(self.components)
            
            for component_name, component in self.components.items():
                try:
                    # Check if component has a health check method
                    if hasattr(component, 'get_health'):
                        health = component.get_health()
                        component_status[component_name] = health
                        if health.get('status') == 'healthy':
                            healthy_components += 1
                    elif hasattr(component, 'get_status'):
                        status = component.get_status()
                        component_status[component_name] = status
                        if status.get('running', False):
                            healthy_components += 1
                    else:
                        # Basic availability check
                        component_status[component_name] = {'status': 'available'}
                        healthy_components += 1
                        
                except Exception as e:
                    logger.error(f"❌ Component health check failed for {component_name}: {e}")
                    component_status[component_name] = {'status': 'error', 'error': str(e)}
            
            # Calculate health ratio
            health_ratio = (healthy_components / total_components * 100) if total_components > 0 else 0
            
            if health_ratio < 70:
                logger.warning(f"⚠️ Low system health: {health_ratio:.1f}%")
            
            # Store component stats
            self.system_stats.update({
                'component_status': component_status,
                'health_ratio': health_ratio,
                'healthy_components': healthy_components,
                'total_components': total_components,
                'last_component_check': datetime.now().isoformat()
            })
            
        except Exception as e:
            logger.error(f"❌ Component health check failed: {e}")
    
    def _check_memory_usage(self) -> None:
        """
        Check memory usage and detect potential memory leaks.
        
        Monitors process memory usage and alerts on unusual patterns.
        """
        if not PSUTIL_AVAILABLE:
            logger.warning("⚠️ psutil not available, skipping memory check")
            return
        
        try:
            import os
            
            # Get current process info
            process = psutil.Process(os.getpid())
            memory_info = process.memory_info()
            memory_percent = process.memory_percent()
            
            if memory_percent > 50:  # Alert if using more than 50% of system memory
                logger.warning(f"⚠️ High process memory usage: {memory_percent:.1f}%")
            
            # Store memory stats
            self.system_stats.update({
                'process_memory_mb': memory_info.rss / 1024 / 1024,
                'process_memory_percent': memory_percent,
                'last_memory_check': datetime.now().isoformat()
            })
            
        except Exception as e:
            logger.error(f"❌ Memory usage check failed: {e}")
    
    def _check_disk_space(self) -> None:
        """
        Check available disk space.
        
        Monitors disk space usage and alerts when space is running low.
        """
        if not PSUTIL_AVAILABLE:
            logger.warning("⚠️ psutil not available, skipping disk space check")
            return
        
        try:
            # Check disk usage for current directory
            disk_usage = psutil.disk_usage('.')
            free_space_gb = disk_usage.free / (1024**3)
            total_space_gb = disk_usage.total / (1024**3)
            used_percent = (disk_usage.used / disk_usage.total) * 100
            
            if free_space_gb < 1.0:  # Alert if less than 1GB free
                logger.warning(f"⚠️ Low disk space: {free_space_gb:.1f}GB free")
            
            if used_percent > 95:  # Alert if more than 95% used
                logger.warning(f"⚠️ Disk almost full: {used_percent:.1f}% used")
            
            # Store disk stats
            self.system_stats.update({
                'disk_free_gb': free_space_gb,
                'disk_total_gb': total_space_gb,
                'disk_used_percent': used_percent,
                'last_disk_check': datetime.now().isoformat()
            })
            
        except Exception as e:
            logger.error(f"❌ Disk space check failed: {e}")
    
    def _check_success_rate(self) -> None:
        """
        Check system success rate based on component performance.
        
        Calculates and monitors the overall success rate of system operations.
        """
        try:
            # Get memory statistics if available
            if 'memory' in self.components:
                memory_component = self.components['memory']
                if hasattr(memory_component, 'get_statistics'):
                    memory_stats = memory_component.get_statistics()
                    success_rate = memory_stats.get('success_rate', 0)
                    
                    if success_rate < 80:
                        logger.warning(f"⚠️ Low success rate: {success_rate:.1f}%")
                    
                    self.system_stats['success_rate'] = success_rate
                    
        except Exception as e:
            logger.error(f"❌ Success rate check failed: {e}")
    
    def _update_system_stats(self) -> None:
        """
        Update system statistics.
        
        Collects and updates comprehensive system statistics for monitoring.
        """
        try:
            # Get basic system info
            self.system_stats.update({
                'monitoring_active': self.running,
                'last_update': datetime.now().isoformat(),
                'uptime_seconds': self._get_uptime(),
                'component_count': len(self.components)
            })
            
            # Add process statistics if available
            if PSUTIL_AVAILABLE:
                try:
                    import os
                    
                    process = psutil.Process(os.getpid())
                    self.system_stats.update({
                        'cpu_times': process.cpu_times()._asdict(),
                        'threads_count': process.num_threads(),
                        'open_files_count': len(process.open_files()),
                        'connections_count': len(process.connections())
                    })
                    
                except Exception as e:
                    logger.debug(f"Failed to get process stats: {e}")
            
        except Exception as e:
            logger.error(f"❌ Failed to update system stats: {e}")
    
    def _get_uptime(self) -> float:
        """
        Get system uptime in seconds.
        
        Returns:
            System uptime in seconds, or 0 if unavailable
        """
        try:
            if PSUTIL_AVAILABLE:
                import time
                boot_time = psutil.boot_time()
                return time.time() - boot_time
            return 0.0
        except Exception:
            return 0.0
    
    def get_health_report(self) -> Dict[str, Any]:
        """
        Get comprehensive health report.
        
        Returns:
            Dictionary containing complete system health information
        """
        try:
            report = {
                'monitoring_status': 'active' if self.running else 'inactive',
                'timestamp': datetime.now().isoformat(),
                'system_stats': self.system_stats.copy(),
                'psutil_available': PSUTIL_AVAILABLE,
                'health_checks_count': len(self.health_checks),
                'components_monitored': list(self.components.keys())
            }
            
            # Add summary status
            if 'health_ratio' in self.system_stats:
                health_ratio = self.system_stats['health_ratio']
                if health_ratio >= 90:
                    report['overall_status'] = 'excellent'
                elif health_ratio >= 70:
                    report['overall_status'] = 'good'
                elif health_ratio >= 50:
                    report['overall_status'] = 'warning'
                else:
                    report['overall_status'] = 'critical'
            else:
                report['overall_status'] = 'unknown'
            
            return report
            
        except Exception as e:
            logger.error(f"❌ Failed to generate health report: {e}")
            return {
                'monitoring_status': 'error',
                'error': str(e),
                'timestamp': datetime.now().isoformat()
            }
    
    def get_status(self) -> Dict[str, Any]:
        """
        Get current watchdog status.
        
        Returns:
            Dictionary containing current status information
        """
        return {
            'running': self.running,
            'monitoring_thread_alive': self.monitoring_thread.is_alive() if self.monitoring_thread else False,
            'health_checks_count': len(self.health_checks),
            'components_count': len(self.components),
            'last_update': self.system_stats.get('last_update'),
            'psutil_available': PSUTIL_AVAILABLE
        } 