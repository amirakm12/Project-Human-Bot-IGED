"""
Health monitoring and alerting system for IGED
"""

import time
import psutil
import logging
import json
import requests
from datetime import datetime
from typing import Dict, List, Any
import threading
from prometheus_client import Gauge, Counter, Histogram, generate_latest

# Prometheus metrics
cpu_usage = Gauge('iged_cpu_usage_percent', 'CPU usage percentage')
memory_usage = Gauge('iged_memory_usage_mb', 'Memory usage in MB')
disk_usage = Gauge('iged_disk_usage_percent', 'Disk usage percentage')
active_agents = Gauge('iged_active_agents', 'Number of active agents')
request_count = Counter('iged_requests_total', 'Total number of requests', ['method', 'endpoint'])
request_duration = Histogram('iged_request_duration_seconds', 'Request duration', ['method', 'endpoint'])
error_count = Counter('iged_errors_total', 'Total number of errors', ['type'])
agent_execution_time = Histogram('iged_agent_execution_seconds', 'Agent execution time', ['agent'])

logger = logging.getLogger(__name__)

class HealthMonitor:
    """System health monitoring"""
    
    def __init__(self, config: Dict[str, Any] = None):
        self.config = config or {}
        self.running = False
        self.metrics = {}
        self.alerts = []
        self.thresholds = {
            'cpu_percent': 80,
            'memory_percent': 85,
            'disk_percent': 90,
            'error_rate': 0.05,
            'response_time_ms': 1000
        }
        
    def start(self):
        """Start monitoring"""
        self.running = True
        monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        monitor_thread.start()
        logger.info("Health monitoring started")
        
    def stop(self):
        """Stop monitoring"""
        self.running = False
        logger.info("Health monitoring stopped")
        
    def _monitor_loop(self):
        """Main monitoring loop"""
        while self.running:
            try:
                self.collect_metrics()
                self.check_thresholds()
                self.update_prometheus_metrics()
                time.sleep(10)  # Collect every 10 seconds
            except Exception as e:
                logger.error(f"Monitoring error: {e}")
                error_count.labels(type='monitoring').inc()
                
    def collect_metrics(self):
        """Collect system metrics"""
        try:
            # CPU metrics
            cpu_percent = psutil.cpu_percent(interval=1)
            cpu_count = psutil.cpu_count()
            
            # Memory metrics
            memory = psutil.virtual_memory()
            memory_used_mb = memory.used / (1024 * 1024)
            memory_percent = memory.percent
            
            # Disk metrics
            disk = psutil.disk_usage('/')
            disk_percent = disk.percent
            
            # Process metrics
            process = psutil.Process()
            process_memory_mb = process.memory_info().rss / (1024 * 1024)
            process_cpu_percent = process.cpu_percent()
            
            # Network metrics
            network = psutil.net_io_counters()
            bytes_sent = network.bytes_sent
            bytes_recv = network.bytes_recv
            
            self.metrics = {
                'timestamp': datetime.now().isoformat(),
                'cpu': {
                    'percent': cpu_percent,
                    'count': cpu_count,
                    'process_percent': process_cpu_percent
                },
                'memory': {
                    'used_mb': memory_used_mb,
                    'percent': memory_percent,
                    'process_mb': process_memory_mb,
                    'available_mb': memory.available / (1024 * 1024)
                },
                'disk': {
                    'percent': disk_percent,
                    'free_gb': disk.free / (1024 * 1024 * 1024)
                },
                'network': {
                    'bytes_sent': bytes_sent,
                    'bytes_recv': bytes_recv
                }
            }
            
        except Exception as e:
            logger.error(f"Failed to collect metrics: {e}")
            error_count.labels(type='metrics_collection').inc()
            
    def check_thresholds(self):
        """Check if metrics exceed thresholds"""
        alerts = []
        
        if self.metrics.get('cpu', {}).get('percent', 0) > self.thresholds['cpu_percent']:
            alerts.append({
                'level': 'warning',
                'metric': 'cpu',
                'message': f"CPU usage high: {self.metrics['cpu']['percent']}%"
            })
            
        if self.metrics.get('memory', {}).get('percent', 0) > self.thresholds['memory_percent']:
            alerts.append({
                'level': 'warning',
                'metric': 'memory',
                'message': f"Memory usage high: {self.metrics['memory']['percent']}%"
            })
            
        if self.metrics.get('disk', {}).get('percent', 0) > self.thresholds['disk_percent']:
            alerts.append({
                'level': 'critical',
                'metric': 'disk',
                'message': f"Disk usage critical: {self.metrics['disk']['percent']}%"
            })
            
        if alerts:
            self.trigger_alerts(alerts)
            
    def trigger_alerts(self, alerts: List[Dict]):
        """Trigger alerts"""
        for alert in alerts:
            logger.warning(f"Alert: {alert['message']}")
            self.alerts.append({
                **alert,
                'timestamp': datetime.now().isoformat()
            })
            
            # Send to external alerting system
            if self.config.get('alerting_webhook'):
                try:
                    requests.post(
                        self.config['alerting_webhook'],
                        json=alert,
                        timeout=5
                    )
                except Exception as e:
                    logger.error(f"Failed to send alert: {e}")
                    
    def update_prometheus_metrics(self):
        """Update Prometheus metrics"""
        if self.metrics:
            cpu_usage.set(self.metrics['cpu']['percent'])
            memory_usage.set(self.metrics['memory']['used_mb'])
            disk_usage.set(self.metrics['disk']['percent'])
            
    def get_health_status(self) -> Dict:
        """Get current health status"""
        return {
            'status': 'healthy' if not self.alerts else 'degraded',
            'metrics': self.metrics,
            'alerts': self.alerts[-10:],  # Last 10 alerts
            'timestamp': datetime.now().isoformat()
        }
        
    def get_metrics_endpoint(self):
        """Get Prometheus metrics endpoint data"""
        return generate_latest()

class PerformanceMonitor:
    """Application performance monitoring"""
    
    def __init__(self):
        self.request_times = []
        self.agent_times = {}
        
    def record_request(self, method: str, endpoint: str, duration: float):
        """Record request metrics"""
        request_count.labels(method=method, endpoint=endpoint).inc()
        request_duration.labels(method=method, endpoint=endpoint).observe(duration)
        
        self.request_times.append({
            'method': method,
            'endpoint': endpoint,
            'duration': duration,
            'timestamp': datetime.now().isoformat()
        })
        
        # Keep only last 1000 requests
        if len(self.request_times) > 1000:
            self.request_times = self.request_times[-1000:]
            
    def record_agent_execution(self, agent: str, duration: float):
        """Record agent execution metrics"""
        agent_execution_time.labels(agent=agent).observe(duration)
        
        if agent not in self.agent_times:
            self.agent_times[agent] = []
            
        self.agent_times[agent].append({
            'duration': duration,
            'timestamp': datetime.now().isoformat()
        })
        
    def get_performance_stats(self) -> Dict:
        """Get performance statistics"""
        if not self.request_times:
            avg_response_time = 0
        else:
            avg_response_time = sum(r['duration'] for r in self.request_times) / len(self.request_times)
            
        return {
            'average_response_time_ms': avg_response_time * 1000,
            'total_requests': len(self.request_times),
            'agent_stats': {
                agent: {
                    'executions': len(times),
                    'avg_duration': sum(t['duration'] for t in times) / len(times) if times else 0
                }
                for agent, times in self.agent_times.items()
            }
        }

# Global instances
health_monitor = HealthMonitor()
performance_monitor = PerformanceMonitor()

def start_monitoring(config: Dict = None):
    """Start all monitoring systems"""
    health_monitor.config = config or {}
    health_monitor.start()
    logger.info("Monitoring systems started")
    
def stop_monitoring():
    """Stop all monitoring systems"""
    health_monitor.stop()
    logger.info("Monitoring systems stopped")