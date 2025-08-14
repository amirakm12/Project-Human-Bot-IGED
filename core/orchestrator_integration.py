#!/usr/bin/env python3
"""
IGED Orchestrator Integration
Connects orchestrator agent with all system components
"""

import asyncio
import logging
from typing import Dict, Any, Optional, List, Callable
from datetime import datetime
from pathlib import Path

from .orchestrator_agent import (
    OrchestratorAgent, 
    WorkflowBuilder, 
    TaskPriority,
    get_orchestrator
)

logger = logging.getLogger(__name__)

class SystemIntegration:
    """Integrates orchestrator with system components"""
    
    def __init__(self):
        self.orchestrator = get_orchestrator()
        self.components = {}
        self._register_system_handlers()
        self._setup_workflows()
    
    def _register_system_handlers(self):
        """Register handlers for system components"""
        
        # Plugin Management Handlers
        @self.orchestrator.register_task_handler
        async def load_plugin(plugin_name: str):
            """Load a plugin"""
            from .plugin_sandbox import get_plugin_manager
            manager = get_plugin_manager()
            plugin_path = Path("plugins") / plugin_name
            success = manager.sandbox.load_plugin(plugin_path)
            return {"plugin": plugin_name, "loaded": success}
        
        @self.orchestrator.register_task_handler
        async def execute_plugin(plugin_name: str, method: str = "run", **kwargs):
            """Execute a plugin"""
            from .plugin_sandbox import get_plugin_manager
            manager = get_plugin_manager()
            result = manager.sandbox.execute_plugin(plugin_name, method, kwargs=kwargs)
            return result
        
        @self.orchestrator.register_task_handler
        async def discover_plugins():
            """Discover and load all plugins"""
            from .plugin_sandbox import get_plugin_manager
            manager = get_plugin_manager()
            loaded = manager.discover_plugins()
            return {"discovered": len(loaded), "plugins": loaded}
        
        # Android Integration Handlers
        @self.orchestrator.register_task_handler
        async def start_android_server(port: int = 8765):
            """Start Android WebSocket server"""
            from .android_integration import get_android_server
            server = get_android_server()
            await server.start(port=port)
            return {"status": "started", "port": port}
        
        @self.orchestrator.register_task_handler
        async def broadcast_to_android(event: str, data: Any):
            """Broadcast to Android devices"""
            from .android_integration import get_android_server
            server = get_android_server()
            await server.broadcast_to_devices(event, data)
            return {"broadcasted": True, "event": event}
        
        @self.orchestrator.register_task_handler
        async def get_android_devices():
            """Get connected Android devices"""
            from .android_integration import get_android_server
            server = get_android_server()
            devices = [d.to_dict() for d in server.devices.values()]
            return {"count": len(devices), "devices": devices}
        
        # Security Handlers
        @self.orchestrator.register_task_handler
        async def start_secure_server(port: int = 8443):
            """Start secure WebSocket server"""
            from .secure_network import SecureWebSocketServer, get_security_config
            config = get_security_config()
            server = SecureWebSocketServer(config)
            await server.start(port=port)
            return {"status": "started", "port": port, "tls": config.enable_tls}
        
        @self.orchestrator.register_task_handler
        async def generate_certificates(hostname: str = "localhost"):
            """Generate SSL certificates"""
            from .secure_network import CertificateManager
            manager = CertificateManager()
            cert_file, key_file = manager.generate_self_signed_cert(hostname)
            return {"cert": cert_file, "key": key_file}
        
        # Configuration Handlers
        @self.orchestrator.register_task_handler
        async def load_configuration(config_file: Optional[str] = None):
            """Load system configuration"""
            from config.config_manager import get_config_manager
            manager = get_config_manager()
            config = manager.load(config_file)
            return config.to_dict()
        
        @self.orchestrator.register_task_handler
        async def update_configuration(key: str, value: Any):
            """Update configuration value"""
            from config.config_manager import get_config_manager
            manager = get_config_manager()
            manager.set(key, value)
            manager.save()
            return {"updated": key, "value": value}
        
        # Optimization Handlers
        @self.orchestrator.register_task_handler
        async def start_optimization_monitoring(interval: int = 30):
            """Start performance monitoring"""
            from .optimization_engine import get_optimization_engine
            engine = get_optimization_engine()
            engine.start_monitoring(interval)
            return {"monitoring": True, "interval": interval}
        
        @self.orchestrator.register_task_handler
        async def get_optimization_status():
            """Get optimization status"""
            from .optimization_engine import get_optimization_engine
            engine = get_optimization_engine()
            return engine.get_optimization_status()
        
        @self.orchestrator.register_task_handler
        async def apply_optimizations():
            """Apply performance optimizations"""
            from .optimization_engine import get_optimization_engine
            engine = get_optimization_engine()
            engine.apply_optimizations()
            return {"optimized": True, "settings": engine.get_optimization_status()}
        
        # Data Validation Handlers
        @self.orchestrator.register_task_handler
        async def validate_data(data: Dict[str, Any], schema_name: str):
            """Validate data against schema"""
            from .data_validator import get_validator
            validator = get_validator()
            try:
                validated = validator.validate_against_schema(data, schema_name)
                return {"valid": True, "data": validated}
            except Exception as e:
                return {"valid": False, "error": str(e)}
        
        @self.orchestrator.register_task_handler
        async def sanitize_input(data: str, sanitize_type: str = "html"):
            """Sanitize user input"""
            from .data_validator import DataSanitizer
            sanitizer = DataSanitizer()
            
            if sanitize_type == "html":
                result = sanitizer.sanitize_html(data)
            elif sanitize_type == "sql":
                result = sanitizer.sanitize_sql(data)
            elif sanitize_type == "filename":
                result = sanitizer.sanitize_filename(data)
            else:
                result = data
            
            return {"sanitized": result, "type": sanitize_type}
        
        # Testing Handlers
        @self.orchestrator.register_task_handler
        async def run_tests(test_module: str = "test_installation"):
            """Run test suite"""
            import subprocess
            result = subprocess.run(
                [sys.executable, "-m", "pytest", f"{test_module}.py", "-v"],
                capture_output=True,
                text=True
            )
            return {
                "passed": result.returncode == 0,
                "output": result.stdout,
                "errors": result.stderr
            }
        
        logger.info("System handlers registered")
    
    def _setup_workflows(self):
        """Setup predefined workflows"""
        
        # System Initialization Workflow
        builder = WorkflowBuilder(self.orchestrator)
        self.init_workflow = (builder
            .workflow("System Initialization", "Initialize all system components")
            .task("load_config", "load_configuration")
                .priority(TaskPriority.CRITICAL)
                .add()
            .task("start_monitoring", "start_optimization_monitoring")
                .params(interval=30)
                .depends_on("load_config")
                .add()
            .task("discover_plugins", "discover_plugins")
                .depends_on("load_config")
                .add()
            .task("generate_certs", "generate_certificates")
                .params(hostname="localhost")
                .add()
            .task("start_secure_server", "start_secure_server")
                .params(port=8443)
                .depends_on("generate_certs")
                .priority(TaskPriority.HIGH)
                .add()
            .task("start_android_server", "start_android_server")
                .params(port=8765)
                .depends_on("load_config")
                .priority(TaskPriority.HIGH)
                .add()
            .build()
        )
        
        # Security Audit Workflow
        builder = WorkflowBuilder(self.orchestrator)
        self.security_workflow = (builder
            .workflow("Security Audit", "Perform security checks")
            .task("check_certs", "generate_certificates")
                .priority(TaskPriority.HIGH)
                .add()
            .task("validate_config", "validate_data")
                .params(
                    data={"test": "data"},
                    schema_name="config"
                )
                .add()
            .task("run_security_tests", "run_tests")
                .params(test_module="test_installation::TestSecurity")
                .priority(TaskPriority.CRITICAL)
                .add()
            .build()
        )
        
        # Performance Optimization Workflow
        builder = WorkflowBuilder(self.orchestrator)
        self.optimization_workflow = (builder
            .workflow("Performance Optimization", "Optimize system performance")
            .task("get_status", "get_optimization_status")
                .add()
            .task("apply_opts", "apply_optimizations")
                .depends_on("get_status")
                .priority(TaskPriority.HIGH)
                .add()
            .task("verify_opts", "get_optimization_status")
                .depends_on("apply_opts")
                .add()
            .build()
        )
        
        # Plugin Execution Workflow
        builder = WorkflowBuilder(self.orchestrator)
        self.plugin_workflow = (builder
            .workflow("Plugin Execution", "Execute plugin pipeline")
            .task("discover", "discover_plugins")
                .priority(TaskPriority.HIGH)
                .add()
            .task("validate", "validate_data")
                .params(
                    data={"plugin": "test"},
                    schema_name="plugin"
                )
                .depends_on("discover")
                .add()
            .build()
        )
        
        logger.info("Predefined workflows created")
    
    async def initialize_system(self) -> Dict[str, Any]:
        """Initialize the entire system"""
        logger.info("Starting system initialization...")
        
        # Start orchestrator
        await self.orchestrator.start()
        
        # Execute initialization workflow
        result = await self.orchestrator.execute_workflow(self.init_workflow)
        
        logger.info("System initialization complete")
        return result
    
    async def perform_security_audit(self) -> Dict[str, Any]:
        """Perform security audit"""
        logger.info("Starting security audit...")
        result = await self.orchestrator.execute_workflow(self.security_workflow)
        logger.info("Security audit complete")
        return result
    
    async def optimize_performance(self) -> Dict[str, Any]:
        """Optimize system performance"""
        logger.info("Starting performance optimization...")
        result = await self.orchestrator.execute_workflow(self.optimization_workflow)
        logger.info("Performance optimization complete")
        return result
    
    async def execute_plugin_pipeline(self, plugin_name: Optional[str] = None) -> Dict[str, Any]:
        """Execute plugin pipeline"""
        logger.info(f"Starting plugin pipeline for {plugin_name or 'all plugins'}...")
        
        if plugin_name:
            # Create custom workflow for specific plugin
            builder = WorkflowBuilder(self.orchestrator)
            workflow = (builder
                .workflow(f"Execute {plugin_name}", f"Execute plugin {plugin_name}")
                .task("load", "load_plugin")
                    .params(plugin_name=plugin_name)
                    .priority(TaskPriority.HIGH)
                    .add()
                .task("execute", "execute_plugin")
                    .params(plugin_name=plugin_name, method="run")
                    .depends_on("load")
                    .add()
                .build()
            )
            result = await self.orchestrator.execute_workflow(workflow)
        else:
            result = await self.orchestrator.execute_workflow(self.plugin_workflow)
        
        logger.info("Plugin pipeline complete")
        return result
    
    async def create_custom_workflow(self, config: Dict[str, Any]) -> str:
        """Create custom workflow from configuration"""
        workflow = self.orchestrator.build_workflow_from_config(config)
        return workflow.id
    
    async def execute_custom_workflow(self, workflow_id: str) -> Dict[str, Any]:
        """Execute custom workflow"""
        return await self.orchestrator.execute_workflow(workflow_id)
    
    def get_metrics(self) -> Dict[str, Any]:
        """Get orchestrator metrics"""
        return self.orchestrator.get_metrics()
    
    def get_workflow_status(self, workflow_id: str) -> Optional[Dict[str, Any]]:
        """Get workflow status"""
        return self.orchestrator.get_workflow_status(workflow_id)
    
    async def shutdown(self):
        """Shutdown system"""
        logger.info("Shutting down system...")
        await self.orchestrator.stop()
        logger.info("System shutdown complete")

# Convenience functions for common operations
async def quick_start():
    """Quick start the entire system"""
    integration = SystemIntegration()
    return await integration.initialize_system()

async def run_security_check():
    """Run security audit"""
    integration = SystemIntegration()
    return await integration.perform_security_audit()

async def optimize_system():
    """Optimize system performance"""
    integration = SystemIntegration()
    return await integration.optimize_performance()

# Global integration instance
_integration = None

def get_system_integration() -> SystemIntegration:
    """Get or create system integration"""
    global _integration
    if _integration is None:
        _integration = SystemIntegration()
    return _integration

if __name__ == "__main__":
    import sys
    
    async def main():
        # Create system integration
        integration = get_system_integration()
        
        # Initialize system
        print("Initializing system...")
        init_result = await integration.initialize_system()
        print(f"Initialization complete: {init_result['status']}")
        
        # Run security audit
        print("\nRunning security audit...")
        security_result = await integration.perform_security_audit()
        print(f"Security audit: {security_result['status']}")
        
        # Optimize performance
        print("\nOptimizing performance...")
        opt_result = await integration.optimize_performance()
        print(f"Optimization: {opt_result['status']}")
        
        # Get metrics
        metrics = integration.get_metrics()
        print(f"\nSystem Metrics:")
        print(f"  Total Workflows: {metrics['total_workflows']}")
        print(f"  Completed: {metrics['completed_workflows']}")
        print(f"  Failed: {metrics['failed_workflows']}")
        print(f"  Total Tasks: {metrics['total_tasks']}")
        print(f"  Completed Tasks: {metrics['completed_tasks']}")
        
        # Shutdown
        await integration.shutdown()
    
    asyncio.run(main())