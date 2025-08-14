#!/usr/bin/env python3
"""
IGED Configuration Manager
Centralized configuration system with YAML/ENV support and validation
"""

import os
import yaml
import json
from pathlib import Path
from typing import Dict, Any, Optional, List, Union
from dataclasses import dataclass, field, asdict
import logging
from dotenv import load_dotenv
import schema

logger = logging.getLogger(__name__)

@dataclass
class AppConfig:
    """Application configuration"""
    name: str = "IGED"
    version: str = "1.0.0"
    debug: bool = False
    log_level: str = "INFO"
    environment: str = "development"

@dataclass
class ServerConfig:
    """Server configuration"""
    host: str = "0.0.0.0"
    port: int = 8080
    workers: int = 4
    timeout: int = 30
    max_connections: int = 1000

@dataclass
class DatabaseConfig:
    """Database configuration"""
    url: str = "sqlite:///iged.db"
    pool_size: int = 10
    echo: bool = False
    auto_migrate: bool = True

@dataclass
class SecurityConfig:
    """Security configuration"""
    enable_tls: bool = True
    cert_file: Optional[str] = None
    key_file: Optional[str] = None
    jwt_secret: str = field(default_factory=lambda: os.urandom(32).hex())
    jwt_expiry: int = 3600
    enable_cors: bool = True
    allowed_origins: List[str] = field(default_factory=lambda: ["http://localhost:*"])
    rate_limit: int = 100

@dataclass
class CacheConfig:
    """Cache configuration"""
    enabled: bool = True
    backend: str = "memory"  # memory, redis, memcached
    ttl: int = 3600
    max_size: int = 1000
    redis_url: Optional[str] = None

@dataclass
class PluginConfig:
    """Plugin configuration"""
    enabled: bool = True
    directory: str = "plugins"
    sandbox: bool = True
    auto_load: bool = True
    allowed_modules: List[str] = field(default_factory=lambda: ["math", "json", "datetime"])

@dataclass
class MonitoringConfig:
    """Monitoring configuration"""
    enabled: bool = True
    metrics_enabled: bool = True
    tracing_enabled: bool = False
    metrics_port: int = 9090
    health_check_path: str = "/health"
    metrics_path: str = "/metrics"

@dataclass
class Configuration:
    """Main configuration container"""
    app: AppConfig = field(default_factory=AppConfig)
    server: ServerConfig = field(default_factory=ServerConfig)
    database: DatabaseConfig = field(default_factory=DatabaseConfig)
    security: SecurityConfig = field(default_factory=SecurityConfig)
    cache: CacheConfig = field(default_factory=CacheConfig)
    plugins: PluginConfig = field(default_factory=PluginConfig)
    monitoring: MonitoringConfig = field(default_factory=MonitoringConfig)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert configuration to dictionary"""
        return asdict(self)
    
    def to_yaml(self) -> str:
        """Convert configuration to YAML"""
        return yaml.dump(self.to_dict(), default_flow_style=False)
    
    def to_json(self) -> str:
        """Convert configuration to JSON"""
        return json.dumps(self.to_dict(), indent=2)

class ConfigurationManager:
    """Manages application configuration"""
    
    def __init__(self, config_dir: Path = Path("config")):
        self.config_dir = config_dir
        self.config_dir.mkdir(exist_ok=True)
        self.config = Configuration()
        self.config_files = []
        self.env_prefix = "IGED_"
        self.schema = self._create_schema()
    
    def _create_schema(self) -> schema.Schema:
        """Create validation schema"""
        return schema.Schema({
            'app': {
                'name': str,
                'version': schema.Regex(r'^\d+\.\d+\.\d+$'),
                'debug': bool,
                'log_level': schema.Or('DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'),
                'environment': schema.Or('development', 'testing', 'staging', 'production')
            },
            'server': {
                'host': str,
                'port': schema.And(int, lambda n: 1 <= n <= 65535),
                'workers': schema.And(int, lambda n: 1 <= n <= 100),
                'timeout': schema.And(int, lambda n: 1 <= n <= 300),
                'max_connections': schema.And(int, lambda n: 1 <= n <= 10000)
            },
            'database': {
                'url': str,
                'pool_size': schema.And(int, lambda n: 1 <= n <= 100),
                'echo': bool,
                'auto_migrate': bool
            },
            'security': {
                'enable_tls': bool,
                schema.Optional('cert_file'): schema.Or(None, str),
                schema.Optional('key_file'): schema.Or(None, str),
                'jwt_secret': str,
                'jwt_expiry': schema.And(int, lambda n: 60 <= n <= 86400),
                'enable_cors': bool,
                'allowed_origins': [str],
                'rate_limit': schema.And(int, lambda n: 1 <= n <= 10000)
            },
            'cache': {
                'enabled': bool,
                'backend': schema.Or('memory', 'redis', 'memcached'),
                'ttl': schema.And(int, lambda n: 1 <= n <= 86400),
                'max_size': schema.And(int, lambda n: 1 <= n <= 100000),
                schema.Optional('redis_url'): schema.Or(None, str)
            },
            'plugins': {
                'enabled': bool,
                'directory': str,
                'sandbox': bool,
                'auto_load': bool,
                'allowed_modules': [str]
            },
            'monitoring': {
                'enabled': bool,
                'metrics_enabled': bool,
                'tracing_enabled': bool,
                'metrics_port': schema.And(int, lambda n: 1 <= n <= 65535),
                'health_check_path': str,
                'metrics_path': str
            }
        })
    
    def load(self, config_file: Optional[str] = None) -> Configuration:
        """Load configuration from files and environment"""
        # Load .env file
        env_file = self.config_dir / ".env"
        if env_file.exists():
            load_dotenv(env_file)
            logger.info(f"Loaded environment from {env_file}")
        
        # Load default configuration
        default_file = self.config_dir / "default.yaml"
        if default_file.exists():
            self._load_yaml_file(default_file)
        
        # Load environment-specific configuration
        env = os.getenv(f"{self.env_prefix}ENVIRONMENT", "development")
        env_file = self.config_dir / f"{env}.yaml"
        if env_file.exists():
            self._load_yaml_file(env_file)
        
        # Load specific configuration file
        if config_file:
            config_path = Path(config_file)
            if config_path.exists():
                if config_path.suffix in ['.yaml', '.yml']:
                    self._load_yaml_file(config_path)
                elif config_path.suffix == '.json':
                    self._load_json_file(config_path)
        
        # Override with environment variables
        self._load_from_env()
        
        # Validate configuration
        self.validate()
        
        return self.config
    
    def _load_yaml_file(self, file_path: Path):
        """Load configuration from YAML file"""
        try:
            with open(file_path, 'r') as f:
                data = yaml.safe_load(f)
                if data:
                    self._merge_config(data)
                    self.config_files.append(str(file_path))
                    logger.info(f"Loaded configuration from {file_path}")
        except Exception as e:
            logger.error(f"Failed to load {file_path}: {e}")
    
    def _load_json_file(self, file_path: Path):
        """Load configuration from JSON file"""
        try:
            with open(file_path, 'r') as f:
                data = json.load(f)
                if data:
                    self._merge_config(data)
                    self.config_files.append(str(file_path))
                    logger.info(f"Loaded configuration from {file_path}")
        except Exception as e:
            logger.error(f"Failed to load {file_path}: {e}")
    
    def _load_from_env(self):
        """Load configuration from environment variables"""
        # App configuration
        self.config.app.name = os.getenv(f"{self.env_prefix}APP_NAME", self.config.app.name)
        self.config.app.version = os.getenv(f"{self.env_prefix}APP_VERSION", self.config.app.version)
        self.config.app.debug = os.getenv(f"{self.env_prefix}DEBUG", "false").lower() == "true"
        self.config.app.log_level = os.getenv(f"{self.env_prefix}LOG_LEVEL", self.config.app.log_level)
        self.config.app.environment = os.getenv(f"{self.env_prefix}ENVIRONMENT", self.config.app.environment)
        
        # Server configuration
        self.config.server.host = os.getenv(f"{self.env_prefix}HOST", self.config.server.host)
        self.config.server.port = int(os.getenv(f"{self.env_prefix}PORT", self.config.server.port))
        self.config.server.workers = int(os.getenv(f"{self.env_prefix}WORKERS", self.config.server.workers))
        
        # Database configuration
        self.config.database.url = os.getenv(f"{self.env_prefix}DATABASE_URL", self.config.database.url)
        
        # Security configuration
        self.config.security.jwt_secret = os.getenv(f"{self.env_prefix}JWT_SECRET", self.config.security.jwt_secret)
        self.config.security.enable_tls = os.getenv(f"{self.env_prefix}TLS_ENABLED", "true").lower() == "true"
        
        # Cache configuration
        self.config.cache.redis_url = os.getenv(f"{self.env_prefix}REDIS_URL", self.config.cache.redis_url)
    
    def _merge_config(self, data: Dict[str, Any]):
        """Merge configuration data into current config"""
        if 'app' in data:
            for key, value in data['app'].items():
                if hasattr(self.config.app, key):
                    setattr(self.config.app, key, value)
        
        if 'server' in data:
            for key, value in data['server'].items():
                if hasattr(self.config.server, key):
                    setattr(self.config.server, key, value)
        
        if 'database' in data:
            for key, value in data['database'].items():
                if hasattr(self.config.database, key):
                    setattr(self.config.database, key, value)
        
        if 'security' in data:
            for key, value in data['security'].items():
                if hasattr(self.config.security, key):
                    setattr(self.config.security, key, value)
        
        if 'cache' in data:
            for key, value in data['cache'].items():
                if hasattr(self.config.cache, key):
                    setattr(self.config.cache, key, value)
        
        if 'plugins' in data:
            for key, value in data['plugins'].items():
                if hasattr(self.config.plugins, key):
                    setattr(self.config.plugins, key, value)
        
        if 'monitoring' in data:
            for key, value in data['monitoring'].items():
                if hasattr(self.config.monitoring, key):
                    setattr(self.config.monitoring, key, value)
    
    def validate(self):
        """Validate configuration against schema"""
        try:
            config_dict = self.config.to_dict()
            self.schema.validate(config_dict)
            logger.info("Configuration validation successful")
        except schema.SchemaError as e:
            logger.error(f"Configuration validation failed: {e}")
            raise ValueError(f"Invalid configuration: {e}")
    
    def save(self, file_path: Optional[Path] = None):
        """Save configuration to file"""
        if file_path is None:
            file_path = self.config_dir / "config.yaml"
        
        try:
            with open(file_path, 'w') as f:
                yaml.dump(self.config.to_dict(), f, default_flow_style=False)
            logger.info(f"Configuration saved to {file_path}")
        except Exception as e:
            logger.error(f"Failed to save configuration: {e}")
    
    def get(self, key: str, default: Any = None) -> Any:
        """Get configuration value by dot notation key"""
        keys = key.split('.')
        value = self.config
        
        for k in keys:
            if hasattr(value, k):
                value = getattr(value, k)
            elif isinstance(value, dict) and k in value:
                value = value[k]
            else:
                return default
        
        return value
    
    def set(self, key: str, value: Any):
        """Set configuration value by dot notation key"""
        keys = key.split('.')
        target = self.config
        
        for k in keys[:-1]:
            if hasattr(target, k):
                target = getattr(target, k)
            else:
                raise KeyError(f"Invalid configuration key: {key}")
        
        if hasattr(target, keys[-1]):
            setattr(target, keys[-1], value)
        else:
            raise KeyError(f"Invalid configuration key: {key}")
    
    def reload(self):
        """Reload configuration from files"""
        self.config = Configuration()
        self.config_files = []
        return self.load()
    
    def watch(self, callback: Callable):
        """Watch configuration files for changes"""
        import watchdog.observers
        import watchdog.events
        
        class ConfigFileHandler(watchdog.events.FileSystemEventHandler):
            def __init__(self, manager, callback):
                self.manager = manager
                self.callback = callback
            
            def on_modified(self, event):
                if not event.is_directory and event.src_path in self.manager.config_files:
                    logger.info(f"Configuration file changed: {event.src_path}")
                    self.manager.reload()
                    self.callback(self.manager.config)
        
        observer = watchdog.observers.Observer()
        handler = ConfigFileHandler(self, callback)
        
        for config_file in self.config_files:
            observer.schedule(handler, os.path.dirname(config_file), recursive=False)
        
        observer.start()
        return observer

# Global configuration instance
_config_manager = None

def get_config_manager() -> ConfigurationManager:
    """Get or create configuration manager"""
    global _config_manager
    if _config_manager is None:
        _config_manager = ConfigurationManager()
        _config_manager.load()
    return _config_manager

def get_config() -> Configuration:
    """Get current configuration"""
    return get_config_manager().config

if __name__ == "__main__":
    # Example usage
    manager = get_config_manager()
    config = manager.config
    
    print("Current Configuration:")
    print(config.to_yaml())
    
    # Save example configuration
    example_config = Path("config/example.yaml")
    manager.save(example_config)
    print(f"Example configuration saved to {example_config}")