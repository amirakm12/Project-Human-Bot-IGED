#!/usr/bin/env python3
"""
IGED Data Validator
Advanced data validation, integrity checking, and sanitization system
Ensures data accuracy, consistency, and security across the application
"""

import re
import json
import hashlib
import hmac
from pathlib import Path
from typing import Any, Dict, List, Optional, Union, Callable, Type
from dataclasses import dataclass, field
from datetime import datetime, date
from enum import Enum
import logging
from collections import defaultdict

logger = logging.getLogger(__name__)

class ValidationError(Exception):
    """Custom exception for validation errors"""
    pass

class DataType(Enum):
    """Supported data types for validation"""
    STRING = "string"
    INTEGER = "integer"
    FLOAT = "float"
    BOOLEAN = "boolean"
    DATE = "date"
    DATETIME = "datetime"
    EMAIL = "email"
    URL = "url"
    JSON = "json"
    LIST = "list"
    DICT = "dict"
    FILE_PATH = "file_path"
    IP_ADDRESS = "ip_address"
    PHONE = "phone"
    UUID = "uuid"

@dataclass
class ValidationRule:
    """Defines a validation rule for a field"""
    field_name: str
    data_type: DataType
    required: bool = True
    min_length: Optional[int] = None
    max_length: Optional[int] = None
    min_value: Optional[Union[int, float]] = None
    max_value: Optional[Union[int, float]] = None
    pattern: Optional[str] = None
    allowed_values: Optional[List[Any]] = None
    custom_validator: Optional[Callable] = None
    sanitizer: Optional[Callable] = None
    default_value: Any = None
    
class DataValidator:
    """Main data validation engine"""
    
    def __init__(self):
        self.validation_rules = {}
        self.validation_cache = {}
        self.error_log = defaultdict(list)
        self.patterns = self._compile_patterns()
        
    def _compile_patterns(self) -> Dict[str, re.Pattern]:
        """Compile common regex patterns"""
        return {
            'email': re.compile(r'^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$'),
            'url': re.compile(r'^https?://[^\s/$.?#].[^\s]*$'),
            'ip_v4': re.compile(r'^(\d{1,3}\.){3}\d{1,3}$'),
            'ip_v6': re.compile(r'^([0-9a-fA-F]{0,4}:){7}[0-9a-fA-F]{0,4}$'),
            'phone_us': re.compile(r'^\+?1?\d{10,14}$'),
            'uuid': re.compile(r'^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$'),
            'alphanumeric': re.compile(r'^[a-zA-Z0-9]+$'),
            'safe_string': re.compile(r'^[a-zA-Z0-9\s\-_.]+$')
        }
    
    def add_rule(self, rule: ValidationRule):
        """Add a validation rule"""
        self.validation_rules[rule.field_name] = rule
    
    def validate(self, data: Dict[str, Any], schema_name: Optional[str] = None) -> Dict[str, Any]:
        """Validate data against rules"""
        validated_data = {}
        errors = []
        
        # Check required fields
        for field_name, rule in self.validation_rules.items():
            if rule.required and field_name not in data:
                if rule.default_value is not None:
                    validated_data[field_name] = rule.default_value
                else:
                    errors.append(f"Required field '{field_name}' is missing")
                continue
            
            if field_name in data:
                value = data[field_name]
                
                try:
                    # Validate and sanitize
                    validated_value = self._validate_field(value, rule)
                    validated_data[field_name] = validated_value
                except ValidationError as e:
                    errors.append(f"Field '{field_name}': {str(e)}")
        
        # Check for unexpected fields
        for field_name in data:
            if field_name not in self.validation_rules:
                logger.warning(f"Unexpected field '{field_name}' in data")
        
        if errors:
            self.error_log[schema_name or 'default'].extend(errors)
            raise ValidationError(f"Validation failed: {'; '.join(errors)}")
        
        return validated_data
    
    def _validate_field(self, value: Any, rule: ValidationRule) -> Any:
        """Validate a single field"""
        # Type validation
        validated_value = self._validate_type(value, rule.data_type)
        
        # Apply sanitizer if provided
        if rule.sanitizer:
            validated_value = rule.sanitizer(validated_value)
        
        # Length validation
        if rule.min_length is not None or rule.max_length is not None:
            self._validate_length(validated_value, rule.min_length, rule.max_length)
        
        # Value range validation
        if rule.min_value is not None or rule.max_value is not None:
            self._validate_range(validated_value, rule.min_value, rule.max_value)
        
        # Pattern validation
        if rule.pattern:
            if not re.match(rule.pattern, str(validated_value)):
                raise ValidationError(f"Value does not match required pattern")
        
        # Allowed values validation
        if rule.allowed_values:
            if validated_value not in rule.allowed_values:
                raise ValidationError(f"Value must be one of {rule.allowed_values}")
        
        # Custom validation
        if rule.custom_validator:
            if not rule.custom_validator(validated_value):
                raise ValidationError(f"Custom validation failed")
        
        return validated_value
    
    def _validate_type(self, value: Any, data_type: DataType) -> Any:
        """Validate and convert data type"""
        if data_type == DataType.STRING:
            return str(value)
        
        elif data_type == DataType.INTEGER:
            try:
                return int(value)
            except (ValueError, TypeError):
                raise ValidationError(f"Invalid integer value: {value}")
        
        elif data_type == DataType.FLOAT:
            try:
                return float(value)
            except (ValueError, TypeError):
                raise ValidationError(f"Invalid float value: {value}")
        
        elif data_type == DataType.BOOLEAN:
            if isinstance(value, bool):
                return value
            if str(value).lower() in ('true', '1', 'yes'):
                return True
            if str(value).lower() in ('false', '0', 'no'):
                return False
            raise ValidationError(f"Invalid boolean value: {value}")
        
        elif data_type == DataType.DATE:
            if isinstance(value, date):
                return value
            try:
                return datetime.strptime(str(value), '%Y-%m-%d').date()
            except ValueError:
                raise ValidationError(f"Invalid date value: {value}")
        
        elif data_type == DataType.DATETIME:
            if isinstance(value, datetime):
                return value
            try:
                return datetime.fromisoformat(str(value))
            except ValueError:
                raise ValidationError(f"Invalid datetime value: {value}")
        
        elif data_type == DataType.EMAIL:
            if not self.patterns['email'].match(str(value)):
                raise ValidationError(f"Invalid email address: {value}")
            return str(value).lower()
        
        elif data_type == DataType.URL:
            if not self.patterns['url'].match(str(value)):
                raise ValidationError(f"Invalid URL: {value}")
            return str(value)
        
        elif data_type == DataType.JSON:
            if isinstance(value, (dict, list)):
                return value
            try:
                return json.loads(value)
            except (json.JSONDecodeError, TypeError):
                raise ValidationError(f"Invalid JSON: {value}")
        
        elif data_type == DataType.LIST:
            if not isinstance(value, list):
                raise ValidationError(f"Value must be a list")
            return value
        
        elif data_type == DataType.DICT:
            if not isinstance(value, dict):
                raise ValidationError(f"Value must be a dictionary")
            return value
        
        elif data_type == DataType.FILE_PATH:
            path = Path(str(value))
            if not path.exists():
                raise ValidationError(f"File does not exist: {value}")
            return str(path)
        
        elif data_type == DataType.IP_ADDRESS:
            ip_str = str(value)
            if not (self.patterns['ip_v4'].match(ip_str) or self.patterns['ip_v6'].match(ip_str)):
                raise ValidationError(f"Invalid IP address: {value}")
            return ip_str
        
        elif data_type == DataType.PHONE:
            phone = re.sub(r'[^\d+]', '', str(value))
            if not self.patterns['phone_us'].match(phone):
                raise ValidationError(f"Invalid phone number: {value}")
            return phone
        
        elif data_type == DataType.UUID:
            if not self.patterns['uuid'].match(str(value)):
                raise ValidationError(f"Invalid UUID: {value}")
            return str(value).lower()
        
        else:
            return value
    
    def _validate_length(self, value: Any, min_length: Optional[int], max_length: Optional[int]):
        """Validate length constraints"""
        if hasattr(value, '__len__'):
            length = len(value)
            if min_length is not None and length < min_length:
                raise ValidationError(f"Length must be at least {min_length}")
            if max_length is not None and length > max_length:
                raise ValidationError(f"Length must not exceed {max_length}")
    
    def _validate_range(self, value: Union[int, float], min_value: Optional[Union[int, float]], 
                       max_value: Optional[Union[int, float]]):
        """Validate numeric range"""
        if min_value is not None and value < min_value:
            raise ValidationError(f"Value must be at least {min_value}")
        if max_value is not None and value > max_value:
            raise ValidationError(f"Value must not exceed {max_value}")

class DataSanitizer:
    """Sanitizes data for security and consistency"""
    
    @staticmethod
    def sanitize_html(text: str) -> str:
        """Remove HTML tags and entities"""
        import html
        # Remove HTML tags
        text = re.sub(r'<[^>]+>', '', text)
        # Unescape HTML entities
        text = html.unescape(text)
        return text.strip()
    
    @staticmethod
    def sanitize_sql(text: str) -> str:
        """Escape SQL special characters"""
        # Basic SQL injection prevention
        sql_chars = ["'", '"', ';', '--', '/*', '*/', 'xp_', 'sp_']
        for char in sql_chars:
            text = text.replace(char, '')
        return text.strip()
    
    @staticmethod
    def sanitize_filename(filename: str) -> str:
        """Sanitize filename for safe file operations"""
        # Remove path traversal attempts
        filename = filename.replace('..', '')
        filename = filename.replace('/', '_')
        filename = filename.replace('\\', '_')
        # Keep only safe characters
        filename = re.sub(r'[^a-zA-Z0-9._-]', '_', filename)
        return filename[:255]  # Limit length
    
    @staticmethod
    def sanitize_json_key(key: str) -> str:
        """Sanitize JSON object keys"""
        # Remove special characters
        key = re.sub(r'[^a-zA-Z0-9_]', '_', key)
        # Ensure it doesn't start with a number
        if key and key[0].isdigit():
            key = '_' + key
        return key
    
    @staticmethod
    def normalize_whitespace(text: str) -> str:
        """Normalize whitespace in text"""
        # Replace multiple spaces with single space
        text = re.sub(r'\s+', ' ', text)
        return text.strip()

class DataIntegrityChecker:
    """Ensures data integrity and consistency"""
    
    def __init__(self, secret_key: str = "default_secret"):
        self.secret_key = secret_key.encode()
        self.checksums = {}
        
    def generate_checksum(self, data: Any) -> str:
        """Generate checksum for data"""
        if isinstance(data, dict):
            data_str = json.dumps(data, sort_keys=True)
        else:
            data_str = str(data)
        
        checksum = hashlib.sha256(data_str.encode()).hexdigest()
        return checksum
    
    def generate_hmac(self, data: Any) -> str:
        """Generate HMAC for data authentication"""
        if isinstance(data, dict):
            data_str = json.dumps(data, sort_keys=True)
        else:
            data_str = str(data)
        
        hmac_hash = hmac.new(self.secret_key, data_str.encode(), hashlib.sha256)
        return hmac_hash.hexdigest()
    
    def verify_checksum(self, data: Any, checksum: str) -> bool:
        """Verify data checksum"""
        calculated_checksum = self.generate_checksum(data)
        return calculated_checksum == checksum
    
    def verify_hmac(self, data: Any, hmac_hash: str) -> bool:
        """Verify HMAC for data authentication"""
        calculated_hmac = self.generate_hmac(data)
        return hmac.compare_digest(calculated_hmac, hmac_hash)
    
    def check_data_consistency(self, data1: Dict, data2: Dict) -> Dict[str, Any]:
        """Check consistency between two data sets"""
        inconsistencies = []
        
        # Check for missing keys
        keys1 = set(data1.keys())
        keys2 = set(data2.keys())
        
        missing_in_data2 = keys1 - keys2
        missing_in_data1 = keys2 - keys1
        
        if missing_in_data2:
            inconsistencies.append(f"Keys missing in data2: {missing_in_data2}")
        if missing_in_data1:
            inconsistencies.append(f"Keys missing in data1: {missing_in_data1}")
        
        # Check for value differences
        common_keys = keys1 & keys2
        for key in common_keys:
            if data1[key] != data2[key]:
                inconsistencies.append(f"Value mismatch for key '{key}': {data1[key]} != {data2[key]}")
        
        return {
            'consistent': len(inconsistencies) == 0,
            'inconsistencies': inconsistencies
        }

class SchemaValidator:
    """Validates data against predefined schemas"""
    
    def __init__(self):
        self.schemas = {}
        self.validator = DataValidator()
        
    def register_schema(self, schema_name: str, rules: List[ValidationRule]):
        """Register a validation schema"""
        self.schemas[schema_name] = rules
        
    def validate_against_schema(self, data: Dict[str, Any], schema_name: str) -> Dict[str, Any]:
        """Validate data against a named schema"""
        if schema_name not in self.schemas:
            raise ValidationError(f"Schema '{schema_name}' not found")
        
        # Apply schema rules
        validator = DataValidator()
        for rule in self.schemas[schema_name]:
            validator.add_rule(rule)
        
        return validator.validate(data, schema_name)
    
    def get_schema_info(self, schema_name: str) -> Dict[str, Any]:
        """Get information about a schema"""
        if schema_name not in self.schemas:
            return None
        
        rules = self.schemas[schema_name]
        return {
            'name': schema_name,
            'fields': [
                {
                    'name': rule.field_name,
                    'type': rule.data_type.value,
                    'required': rule.required,
                    'constraints': {
                        'min_length': rule.min_length,
                        'max_length': rule.max_length,
                        'min_value': rule.min_value,
                        'max_value': rule.max_value,
                        'pattern': rule.pattern,
                        'allowed_values': rule.allowed_values
                    }
                }
                for rule in rules
            ]
        }

# Predefined schemas for common use cases
def create_user_schema() -> List[ValidationRule]:
    """Create user data validation schema"""
    return [
        ValidationRule('username', DataType.STRING, required=True, min_length=3, max_length=50,
                      pattern=r'^[a-zA-Z0-9_]+$'),
        ValidationRule('email', DataType.EMAIL, required=True),
        ValidationRule('password', DataType.STRING, required=True, min_length=8),
        ValidationRule('age', DataType.INTEGER, required=False, min_value=0, max_value=150),
        ValidationRule('phone', DataType.PHONE, required=False),
        ValidationRule('created_at', DataType.DATETIME, required=False, default_value=datetime.now())
    ]

def create_config_schema() -> List[ValidationRule]:
    """Create configuration validation schema"""
    return [
        ValidationRule('app_name', DataType.STRING, required=True),
        ValidationRule('version', DataType.STRING, required=True, pattern=r'^\d+\.\d+\.\d+$'),
        ValidationRule('debug', DataType.BOOLEAN, required=False, default_value=False),
        ValidationRule('port', DataType.INTEGER, required=False, min_value=1, max_value=65535),
        ValidationRule('host', DataType.IP_ADDRESS, required=False),
        ValidationRule('database_url', DataType.URL, required=False),
        ValidationRule('settings', DataType.JSON, required=False)
    ]

# Global validator instance
_global_validator = None

def get_validator() -> SchemaValidator:
    """Get or create global validator instance"""
    global _global_validator
    if _global_validator is None:
        _global_validator = SchemaValidator()
        # Register default schemas
        _global_validator.register_schema('user', create_user_schema())
        _global_validator.register_schema('config', create_config_schema())
    return _global_validator

if __name__ == "__main__":
    # Test the validation system
    validator = get_validator()
    
    # Test user validation
    user_data = {
        'username': 'john_doe',
        'email': 'john@example.com',
        'password': 'securepass123',
        'age': 25
    }
    
    try:
        validated = validator.validate_against_schema(user_data, 'user')
        print("User data validated:", validated)
    except ValidationError as e:
        print(f"Validation error: {e}")
    
    # Test data integrity
    checker = DataIntegrityChecker()
    checksum = checker.generate_checksum(user_data)
    print(f"Checksum: {checksum}")
    print(f"Checksum valid: {checker.verify_checksum(user_data, checksum)}")
    
    # Test sanitization
    sanitizer = DataSanitizer()
    dirty_text = "<script>alert('XSS')</script>Hello World!"
    clean_text = sanitizer.sanitize_html(dirty_text)
    print(f"Sanitized: {clean_text}")