#!/usr/bin/env python3
"""
IGED - Unified Logging Module
Consistent logging across all biometric authentication components
"""

import json
import logging
import os
import sys
import threading
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, Optional

# Configure colorama for colored output
try:
    from colorama import Back, Fore, Style, init

    init(autoreset=True)
    COLORAMA_AVAILABLE = True
except ImportError:
    COLORAMA_AVAILABLE = False


class IGEDLogger:
    """Unified logger for IGED biometric authentication"""

    def __init__(self, name: str, log_file: Optional[Path] = None):
        self.name = name
        self.log_file = log_file or Path("logs/iged_biometric.log")
        self.logger = logging.getLogger(name)
        self._setup_logger()

    def _setup_logger(self):
        """Setup logger configuration"""
        # Create logs directory
        self.log_file.parent.mkdir(exist_ok=True)

        # Set log level
        self.logger.setLevel(logging.DEBUG)

        # Clear existing handlers
        self.logger.handlers.clear()

        # Create formatters
        console_formatter = logging.Formatter(
            "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
        )

        file_formatter = logging.Formatter(
            "%(asctime)s - %(name)s - %(levelname)s - %(funcName)s:%(lineno)d - %(message)s"
        )

        # Console handler
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(logging.INFO)
        console_handler.setFormatter(console_formatter)
        self.logger.addHandler(console_handler)

        # File handler
        file_handler = logging.FileHandler(self.log_file, encoding="utf-8")
        file_handler.setLevel(logging.DEBUG)
        file_handler.setFormatter(file_formatter)
        self.logger.addHandler(file_handler)

    def debug(self, message: str, **kwargs):
        """Log debug message"""
        self.logger.debug(self._format_message(message, **kwargs))

    def info(self, message: str, **kwargs):
        """Log info message"""
        formatted_msg = self._format_message(message, **kwargs)
        self.logger.info(formatted_msg)
        if COLORAMA_AVAILABLE:
            print(f"{Fore.GREEN}{formatted_msg}{Style.RESET_ALL}")

    def warning(self, message: str, **kwargs):
        """Log warning message"""
        formatted_msg = self._format_message(message, **kwargs)
        self.logger.warning(formatted_msg)
        if COLORAMA_AVAILABLE:
            print(f"{Fore.YELLOW}{formatted_msg}{Style.RESET_ALL}")

    def error(self, message: str, **kwargs):
        """Log error message"""
        formatted_msg = self._format_message(message, **kwargs)
        self.logger.error(formatted_msg)
        if COLORAMA_AVAILABLE:
            print(f"{Fore.RED}{formatted_msg}{Style.RESET_ALL}")

    def critical(self, message: str, **kwargs):
        """Log critical message"""
        formatted_msg = self._format_message(message, **kwargs)
        self.logger.critical(formatted_msg)
        if COLORAMA_AVAILABLE:
            print(f"{Fore.RED}{Back.WHITE}{formatted_msg}{Style.RESET_ALL}")

    def _format_message(self, message: str, **kwargs) -> str:
        """Format message with additional context"""
        if kwargs:
            context = " | ".join([f"{k}={v}" for k, v in kwargs.items()])
            return f"{message} | {context}"
        return message

    def log_biometric_event(
        self, event_type: str, success: bool, details: Optional[Dict[str, Any]] = None
    ):
        """Log biometric authentication event"""
        status = "SUCCESS" if success else "FAILED"
        message = f"Biometric {event_type}: {status}"

        if details:
            self.info(message, **details)
        else:
            self.info(message)

    def log_webauthn_event(
        self,
        event_type: str,
        user_id: str,
        success: bool,
        details: Optional[Dict[str, Any]] = None,
    ):
        """Log WebAuthn event"""
        status = "SUCCESS" if success else "FAILED"
        message = f"WebAuthn {event_type}: {status}"

        event_details = {"user_id": user_id}
        if details:
            event_details.update(details)

        self.info(message, **event_details)

    def log_security_event(
        self, event_type: str, severity: str, details: Optional[Dict[str, Any]] = None
    ):
        """Log security event"""
        message = f"Security Event: {event_type}"

        event_details = {"severity": severity}
        if details:
            event_details.update(details)

        if severity.upper() == "HIGH":
            self.critical(message, **event_details)
        elif severity.upper() == "MEDIUM":
            self.warning(message, **event_details)
        else:
            self.info(message, **event_details)


class ThreadSafeLogger(IGEDLogger):
    """Thread-safe logger for multi-threaded applications"""

    def __init__(self, name: str, log_file: Optional[Path] = None):
        super().__init__(name, log_file)
        self._lock = threading.Lock()

    def _log_with_lock(self, level: str, message: str, **kwargs):
        """Log message with thread lock"""
        with self._lock:
            if level == "debug":
                self.debug(message, **kwargs)
            elif level == "info":
                self.info(message, **kwargs)
            elif level == "warning":
                self.warning(message, **kwargs)
            elif level == "error":
                self.error(message, **kwargs)
            elif level == "critical":
                self.critical(message, **kwargs)

    def debug(self, message: str, **kwargs):
        """Thread-safe debug logging"""
        self._log_with_lock("debug", message, **kwargs)

    def info(self, message: str, **kwargs):
        """Thread-safe info logging"""
        self._log_with_lock("info", message, **kwargs)

    def warning(self, message: str, **kwargs):
        """Thread-safe warning logging"""
        self._log_with_lock("warning", message, **kwargs)

    def error(self, message: str, **kwargs):
        """Thread-safe error logging"""
        self._log_with_lock("error", message, **kwargs)

    def critical(self, message: str, **kwargs):
        """Thread-safe critical logging"""
        self._log_with_lock("critical", message, **kwargs)


class AuditLogger(IGEDLogger):
    """Audit logger for security and compliance events"""

    def __init__(self, audit_file: Optional[Path] = None):
        audit_file = audit_file or Path("logs/audit.log")
        super().__init__("IGED_AUDIT", audit_file)
        self._setup_audit_logger()

    def _setup_audit_logger(self):
        """Setup audit-specific logging"""

        # Create JSON formatter for audit logs
        class JSONFormatter(logging.Formatter):
            def format(self, record):
                log_entry = {
                    "timestamp": datetime.now().isoformat(),
                    "level": record.levelname,
                    "logger": record.name,
                    "message": record.getMessage(),
                    "module": record.module,
                    "function": record.funcName,
                    "line": record.lineno,
                }

                # Add extra fields if present
                if hasattr(record, "user_id"):
                    log_entry["user_id"] = record.user_id
                if hasattr(record, "event_type"):
                    log_entry["event_type"] = record.event_type
                if hasattr(record, "ip_address"):
                    log_entry["ip_address"] = record.ip_address

                return json.dumps(log_entry)

        # Replace file handler with JSON formatter
        for handler in self.logger.handlers:
            if isinstance(handler, logging.FileHandler):
                handler.setFormatter(JSONFormatter())

    def log_authentication_attempt(
        self, user_id: str, method: str, success: bool, ip_address: Optional[str] = None
    ):
        """Log authentication attempt"""
        extra = {
            "user_id": user_id,
            "event_type": "authentication_attempt",
            "method": method,
            "success": success,
        }
        if ip_address:
            extra["ip_address"] = ip_address

        if success:
            self.info("Authentication successful", **extra)
        else:
            self.warning("Authentication failed", **extra)

    def log_credential_operation(
        self,
        user_id: str,
        operation: str,
        success: bool,
        details: Optional[Dict[str, Any]] = None,
    ):
        """Log credential operation"""
        extra = {
            "user_id": user_id,
            "event_type": "credential_operation",
            "operation": operation,
            "success": success,
        }
        if details:
            extra.update(details)

        if success:
            self.info("Credential operation completed", **extra)
        else:
            self.warning("Credential operation failed", **extra)

    def log_security_violation(
        self,
        violation_type: str,
        user_id: Optional[str] = None,
        details: Optional[Dict[str, Any]] = None,
    ):
        """Log security violation"""
        extra = {"event_type": "security_violation", "violation_type": violation_type}
        if user_id:
            extra["user_id"] = user_id
        if details:
            extra.update(details)

        self.critical("Security violation detected", **extra)


def setup_logger(
    name: str, log_file: Optional[Path] = None, thread_safe: bool = False
) -> IGEDLogger:
    """Setup and return a logger instance"""
    if thread_safe:
        return ThreadSafeLogger(name, log_file)
    else:
        return IGEDLogger(name, log_file)


def setup_audit_logger(audit_file: Optional[Path] = None) -> AuditLogger:
    """Setup and return an audit logger instance"""
    return AuditLogger(audit_file)


def get_logger(name: str) -> IGEDLogger:
    """Get existing logger or create new one"""
    return setup_logger(name)


# Global logger instances
_biometric_logger = None
_webauthn_logger = None
_audit_logger = None


def get_biometric_logger() -> IGEDLogger:
    """Get biometric authentication logger"""
    global _biometric_logger
    if _biometric_logger is None:
        _biometric_logger = setup_logger("IGED_BIOMETRIC", Path("logs/biometric.log"))
    return _biometric_logger


def get_webauthn_logger() -> IGEDLogger:
    """Get WebAuthn logger"""
    global _webauthn_logger
    if _webauthn_logger is None:
        _webauthn_logger = setup_logger("IGED_WEBAUTHN", Path("logs/webauthn.log"))
    return _webauthn_logger


def get_audit_logger() -> AuditLogger:
    """Get audit logger"""
    global _audit_logger
    if _audit_logger is None:
        _audit_logger = setup_audit_logger(Path("logs/audit.log"))
    return _audit_logger


def log_biometric_event(
    event_type: str, success: bool, details: Optional[Dict[str, Any]] = None
):
    """Log biometric event using global logger"""
    logger = get_biometric_logger()
    logger.log_biometric_event(event_type, success, details)


def log_webauthn_event(
    event_type: str,
    user_id: str,
    success: bool,
    details: Optional[Dict[str, Any]] = None,
):
    """Log WebAuthn event using global logger"""
    logger = get_webauthn_logger()
    logger.log_webauthn_event(event_type, user_id, success, details)


def log_security_event(
    event_type: str, severity: str, details: Optional[Dict[str, Any]] = None
):
    """Log security event using global logger"""
    logger = get_audit_logger()
    logger.log_security_event(event_type, severity, details)


# Test functions
def test_logger():
    """Test logger functionality"""
    print("📝 Testing IGED Logger")
    print("=" * 40)

    # Test basic logger
    logger = setup_logger("TEST_LOGGER")
    logger.info("Test info message")
    logger.warning("Test warning message")
    logger.error("Test error message")

    # Test biometric logger
    bio_logger = get_biometric_logger()
    bio_logger.log_biometric_event(
        "fingerprint_scan", True, {"device": "Windows Hello"}
    )
    bio_logger.log_biometric_event(
        "face_recognition", False, {"reason": "no_face_detected"}
    )

    # Test WebAuthn logger
    webauthn_logger = get_webauthn_logger()
    webauthn_logger.log_webauthn_event(
        "registration", "user123", True, {"device": "YubiKey"}
    )
    webauthn_logger.log_webauthn_event(
        "authentication", "user123", False, {"reason": "invalid_signature"}
    )

    # Test audit logger
    audit_logger = get_audit_logger()
    audit_logger.log_authentication_attempt(
        "user123", "biometric", True, "192.168.1.100"
    )
    audit_logger.log_credential_operation(
        "user123", "create", True, {"credential_type": "webauthn"}
    )
    audit_logger.log_security_violation(
        "multiple_failed_attempts", "user123", {"attempts": 5}
    )

    print("✅ Logger tests completed")


if __name__ == "__main__":
    test_logger()
