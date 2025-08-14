#!/usr/bin/env python3
"""
IGED Plugin Sandbox System
Provides secure, isolated execution environment for plugins
"""

import os
import sys
import ast
import json
import hashlib
import tempfile
import subprocess
from pathlib import Path
from typing import Dict, Any, Optional, List, Callable, Set
from dataclasses import dataclass, field
import logging
import resource
import signal
from contextlib import contextmanager
from concurrent.futures import ProcessPoolExecutor, TimeoutError
import importlib.util
import importlib.machinery
from io import StringIO
import traceback

logger = logging.getLogger(__name__)

@dataclass
class PluginManifest:
    """Plugin manifest with metadata and permissions"""
    name: str
    version: str
    author: str
    description: str
    entry_point: str
    permissions: List[str] = field(default_factory=list)
    dependencies: List[str] = field(default_factory=list)
    api_version: str = "1.0.0"
    checksum: Optional[str] = None
    
    @classmethod
    def from_json(cls, json_path: Path) -> 'PluginManifest':
        """Load manifest from JSON file"""
        with open(json_path, 'r') as f:
            data = json.load(f)
        return cls(**data)
    
    def validate_checksum(self, plugin_path: Path) -> bool:
        """Validate plugin file checksum"""
        if not self.checksum:
            return True
        
        with open(plugin_path, 'rb') as f:
            file_hash = hashlib.sha256(f.read()).hexdigest()
        
        return file_hash == self.checksum

@dataclass
class SandboxPolicy:
    """Security policy for sandbox execution"""
    max_memory_mb: int = 100
    max_cpu_time_seconds: int = 10
    max_file_size_mb: int = 10
    allowed_modules: Set[str] = field(default_factory=lambda: {
        'math', 'json', 'datetime', 'collections', 'itertools',
        'functools', 'typing', 'dataclasses', 're', 'string'
    })
    denied_builtins: Set[str] = field(default_factory=lambda: {
        'eval', 'exec', 'compile', '__import__', 'open',
        'input', 'help', 'globals', 'locals', 'vars'
    })
    network_allowed: bool = False
    filesystem_allowed: bool = False
    subprocess_allowed: bool = False

class SecureImporter:
    """Custom importer with module restrictions"""
    
    def __init__(self, allowed_modules: Set[str]):
        self.allowed_modules = allowed_modules
        self.original_import = __builtins__.__import__
    
    def secure_import(self, name, *args, **kwargs):
        """Secure import function"""
        # Check if module is allowed
        base_module = name.split('.')[0]
        if base_module not in self.allowed_modules:
            raise ImportError(f"Import of '{name}' is not allowed")
        
        # Check for dangerous modules
        dangerous_modules = {'os', 'sys', 'subprocess', 'socket', 'urllib', 'requests'}
        if base_module in dangerous_modules:
            raise ImportError(f"Import of dangerous module '{name}' is blocked")
        
        return self.original_import(name, *args, **kwargs)

class RestrictedPython:
    """AST-based Python code validator and transformer"""
    
    def __init__(self, policy: SandboxPolicy):
        self.policy = policy
        self.errors = []
    
    def validate_code(self, code: str) -> bool:
        """Validate Python code against security policy"""
        try:
            tree = ast.parse(code)
            validator = SecurityValidator(self.policy)
            validator.visit(tree)
            self.errors = validator.errors
            return len(validator.errors) == 0
        except SyntaxError as e:
            self.errors.append(f"Syntax error: {e}")
            return False
    
    def transform_code(self, code: str) -> str:
        """Transform code to add security restrictions"""
        tree = ast.parse(code)
        transformer = SecurityTransformer(self.policy)
        transformed = transformer.visit(tree)
        return ast.unparse(transformed)

class SecurityValidator(ast.NodeVisitor):
    """AST visitor for security validation"""
    
    def __init__(self, policy: SandboxPolicy):
        self.policy = policy
        self.errors = []
    
    def visit_Import(self, node):
        """Check import statements"""
        for alias in node.names:
            module = alias.name.split('.')[0]
            if module not in self.policy.allowed_modules:
                self.errors.append(f"Import of '{module}' is not allowed")
        self.generic_visit(node)
    
    def visit_ImportFrom(self, node):
        """Check from-import statements"""
        if node.module:
            module = node.module.split('.')[0]
            if module not in self.policy.allowed_modules:
                self.errors.append(f"Import from '{module}' is not allowed")
        self.generic_visit(node)
    
    def visit_Call(self, node):
        """Check function calls"""
        if isinstance(node.func, ast.Name):
            func_name = node.func.id
            if func_name in self.policy.denied_builtins:
                self.errors.append(f"Call to '{func_name}' is not allowed")
        self.generic_visit(node)

class SecurityTransformer(ast.NodeTransformer):
    """AST transformer for adding security restrictions"""
    
    def __init__(self, policy: SandboxPolicy):
        self.policy = policy
    
    def visit_Call(self, node):
        """Wrap dangerous function calls"""
        if isinstance(node.func, ast.Name):
            if node.func.id == 'print':
                # Redirect print to safe logger
                node.func = ast.Attribute(
                    value=ast.Name(id='__sandbox__', ctx=ast.Load()),
                    attr='safe_print',
                    ctx=ast.Load()
                )
        return self.generic_visit(node)

class PluginSandbox:
    """Main plugin sandbox executor"""
    
    def __init__(self, policy: Optional[SandboxPolicy] = None):
        self.policy = policy or SandboxPolicy()
        self.plugins = {}
        self.plugin_dir = Path("plugins")
        self.plugin_dir.mkdir(exist_ok=True)
        self.executor = ProcessPoolExecutor(max_workers=4)
    
    def load_plugin(self, plugin_path: Path) -> bool:
        """Load and validate a plugin"""
        manifest_path = plugin_path / "manifest.json"
        
        if not manifest_path.exists():
            logger.error(f"No manifest found for plugin at {plugin_path}")
            return False
        
        try:
            manifest = PluginManifest.from_json(manifest_path)
            
            # Validate entry point exists
            entry_point = plugin_path / manifest.entry_point
            if not entry_point.exists():
                logger.error(f"Entry point {entry_point} not found")
                return False
            
            # Validate checksum
            if not manifest.validate_checksum(entry_point):
                logger.error(f"Checksum validation failed for {manifest.name}")
                return False
            
            # Validate code
            with open(entry_point, 'r') as f:
                code = f.read()
            
            validator = RestrictedPython(self.policy)
            if not validator.validate_code(code):
                logger.error(f"Security validation failed for {manifest.name}: {validator.errors}")
                return False
            
            # Store plugin
            self.plugins[manifest.name] = {
                'manifest': manifest,
                'path': plugin_path,
                'code': code
            }
            
            logger.info(f"Successfully loaded plugin: {manifest.name} v{manifest.version}")
            return True
            
        except Exception as e:
            logger.error(f"Failed to load plugin from {plugin_path}: {e}")
            return False
    
    def execute_plugin(self, plugin_name: str, 
                      method: str = 'run',
                      args: tuple = (),
                      kwargs: dict = None,
                      timeout: int = 10) -> Any:
        """Execute plugin in sandboxed environment"""
        if plugin_name not in self.plugins:
            raise ValueError(f"Plugin '{plugin_name}' not loaded")
        
        plugin = self.plugins[plugin_name]
        kwargs = kwargs or {}
        
        try:
            # Execute in separate process with timeout
            future = self.executor.submit(
                self._execute_in_sandbox,
                plugin['code'],
                plugin['manifest'],
                method,
                args,
                kwargs,
                self.policy
            )
            
            result = future.result(timeout=timeout)
            return result
            
        except TimeoutError:
            logger.error(f"Plugin '{plugin_name}' execution timed out")
            future.cancel()
            return None
        except Exception as e:
            logger.error(f"Plugin '{plugin_name}' execution failed: {e}")
            return None
    
    @staticmethod
    def _execute_in_sandbox(code: str, 
                           manifest: PluginManifest,
                           method: str,
                           args: tuple,
                           kwargs: dict,
                           policy: SandboxPolicy) -> Any:
        """Execute code in restricted environment (runs in separate process)"""
        
        # Set resource limits
        if hasattr(resource, 'RLIMIT_AS'):
            resource.setrlimit(resource.RLIMIT_AS, 
                             (policy.max_memory_mb * 1024 * 1024,
                              policy.max_memory_mb * 1024 * 1024))
        
        if hasattr(resource, 'RLIMIT_CPU'):
            resource.setrlimit(resource.RLIMIT_CPU,
                             (policy.max_cpu_time_seconds,
                              policy.max_cpu_time_seconds))
        
        # Create restricted namespace
        safe_builtins = {
            k: v for k, v in __builtins__.items()
            if k not in policy.denied_builtins
        }
        
        # Add safe utilities
        class SandboxUtils:
            @staticmethod
            def safe_print(*args, **kwargs):
                """Safe print function"""
                output = StringIO()
                print(*args, **kwargs, file=output)
                return output.getvalue()
            
            @staticmethod
            def safe_log(message: str, level: str = 'info'):
                """Safe logging function"""
                return f"[{level.upper()}] {message}"
        
        namespace = {
            '__builtins__': safe_builtins,
            '__name__': '__main__',
            '__sandbox__': SandboxUtils(),
            '__plugin_name__': manifest.name,
            '__plugin_version__': manifest.version
        }
        
        # Replace import function
        importer = SecureImporter(policy.allowed_modules)
        namespace['__import__'] = importer.secure_import
        
        try:
            # Execute plugin code
            exec(code, namespace)
            
            # Call specified method
            if method in namespace:
                func = namespace[method]
                if callable(func):
                    return func(*args, **kwargs)
                else:
                    return func
            else:
                raise AttributeError(f"Method '{method}' not found in plugin")
                
        except Exception as e:
            return {'error': str(e), 'traceback': traceback.format_exc()}
    
    def list_plugins(self) -> List[Dict[str, Any]]:
        """List all loaded plugins"""
        return [
            {
                'name': plugin['manifest'].name,
                'version': plugin['manifest'].version,
                'author': plugin['manifest'].author,
                'description': plugin['manifest'].description,
                'permissions': plugin['manifest'].permissions
            }
            for plugin in self.plugins.values()
        ]
    
    def unload_plugin(self, plugin_name: str) -> bool:
        """Unload a plugin"""
        if plugin_name in self.plugins:
            del self.plugins[plugin_name]
            logger.info(f"Unloaded plugin: {plugin_name}")
            return True
        return False
    
    def validate_plugin_api(self, plugin_name: str, 
                           api_schema: Dict[str, Any]) -> bool:
        """Validate plugin against API schema"""
        if plugin_name not in self.plugins:
            return False
        
        plugin = self.plugins[plugin_name]
        code = plugin['code']
        
        # Parse code to extract API
        tree = ast.parse(code)
        
        # Check for required functions
        functions = [node.name for node in ast.walk(tree) 
                    if isinstance(node, ast.FunctionDef)]
        
        required_functions = api_schema.get('required_functions', [])
        for func in required_functions:
            if func not in functions:
                logger.error(f"Plugin '{plugin_name}' missing required function: {func}")
                return False
        
        return True
    
    def create_plugin_template(self, name: str, 
                              output_dir: Optional[Path] = None) -> Path:
        """Create a plugin template"""
        output_dir = output_dir or self.plugin_dir
        plugin_path = output_dir / name
        plugin_path.mkdir(exist_ok=True)
        
        # Create manifest
        manifest = {
            "name": name,
            "version": "1.0.0",
            "author": "Your Name",
            "description": "Plugin description",
            "entry_point": "main.py",
            "permissions": [],
            "dependencies": [],
            "api_version": "1.0.0"
        }
        
        with open(plugin_path / "manifest.json", 'w') as f:
            json.dump(manifest, f, indent=2)
        
        # Create main.py
        template_code = '''"""
{name} Plugin
{description}
"""

def initialize():
    """Initialize plugin"""
    print(f"Initializing {name} plugin")
    return True

def run(*args, **kwargs):
    """Main plugin entry point"""
    print(f"Running {name} plugin")
    # Your plugin logic here
    return {"status": "success", "message": "Plugin executed successfully"}

def cleanup():
    """Cleanup plugin resources"""
    print(f"Cleaning up {name} plugin")
    return True
'''.format(name=name, description="Plugin description")
        
        with open(plugin_path / "main.py", 'w') as f:
            f.write(template_code)
        
        logger.info(f"Created plugin template at {plugin_path}")
        return plugin_path

class PluginManager:
    """High-level plugin management"""
    
    def __init__(self, sandbox: Optional[PluginSandbox] = None):
        self.sandbox = sandbox or PluginSandbox()
        self.plugin_registry = {}
        self.hooks = {}
    
    def discover_plugins(self, directory: Optional[Path] = None) -> List[str]:
        """Discover and load plugins from directory"""
        directory = directory or self.sandbox.plugin_dir
        loaded = []
        
        for plugin_dir in directory.iterdir():
            if plugin_dir.is_dir() and (plugin_dir / "manifest.json").exists():
                if self.sandbox.load_plugin(plugin_dir):
                    loaded.append(plugin_dir.name)
        
        return loaded
    
    def register_hook(self, hook_name: str, callback: Callable):
        """Register a hook for plugins to use"""
        if hook_name not in self.hooks:
            self.hooks[hook_name] = []
        self.hooks[hook_name].append(callback)
    
    def execute_hook(self, hook_name: str, *args, **kwargs) -> List[Any]:
        """Execute all callbacks for a hook"""
        results = []
        
        if hook_name in self.hooks:
            for callback in self.hooks[hook_name]:
                try:
                    result = callback(*args, **kwargs)
                    results.append(result)
                except Exception as e:
                    logger.error(f"Hook '{hook_name}' execution failed: {e}")
        
        # Execute plugin hooks
        for plugin_name in self.sandbox.plugins:
            try:
                result = self.sandbox.execute_plugin(
                    plugin_name,
                    f"on_{hook_name}",
                    args,
                    kwargs
                )
                if result is not None:
                    results.append(result)
            except:
                pass  # Plugin doesn't implement this hook
        
        return results
    
    def get_plugin_info(self, plugin_name: str) -> Optional[Dict[str, Any]]:
        """Get detailed plugin information"""
        if plugin_name in self.sandbox.plugins:
            plugin = self.sandbox.plugins[plugin_name]
            return {
                'manifest': plugin['manifest'].__dict__,
                'loaded': True,
                'path': str(plugin['path'])
            }
        return None

# Global plugin manager
_plugin_manager = None

def get_plugin_manager() -> PluginManager:
    """Get or create plugin manager"""
    global _plugin_manager
    if _plugin_manager is None:
        _plugin_manager = PluginManager()
    return _plugin_manager

if __name__ == "__main__":
    # Example usage
    manager = get_plugin_manager()
    
    # Create example plugin
    plugin_path = manager.sandbox.create_plugin_template("example_plugin")
    print(f"Created plugin template at: {plugin_path}")
    
    # Load plugin
    if manager.sandbox.load_plugin(plugin_path):
        print("Plugin loaded successfully")
        
        # Execute plugin
        result = manager.sandbox.execute_plugin("example_plugin", "run")
        print(f"Plugin result: {result}")
        
        # List plugins
        plugins = manager.sandbox.list_plugins()
        print(f"Loaded plugins: {plugins}")