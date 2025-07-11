#!/usr/bin/env python3
"""
IGED - Windows Hello Biometric Authentication Module
Native Windows biometric authentication using WinRT APIs
"""

import asyncio
import sys
import os
from typing import Optional, Dict, Any
from pathlib import Path
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

try:
    from winrt.windows.security.credentials.ui import (
        UserConsentVerifier, 
        UserConsentVerificationResult,
        UserConsentVerificationAvailability
    )
    from winrt.windows.foundation import IAsyncOperation
    WINDOWS_HELLO_AVAILABLE = True
except ImportError:
    logger.warning("pywinrt not available. Windows Hello features disabled.")
    WINDOWS_HELLO_AVAILABLE = False

class WindowsHelloManager:
    """Manages Windows Hello biometric authentication"""
    
    def __init__(self):
        self.availability = None
        self.verification_result = None
        self._initialized = False
    
    async def initialize(self) -> bool:
        """Initialize Windows Hello manager"""
        if not WINDOWS_HELLO_AVAILABLE:
            logger.error("Windows Hello not available - pywinrt not installed")
            return False
        
        try:
            self.availability = await UserConsentVerifier.check_availability_async()
            self._initialized = True
            logger.info(f"Windows Hello initialized. Availability: {self.availability}")
            return True
        except Exception as e:
            logger.error(f"Failed to initialize Windows Hello: {e}")
            return False
    
    async def check_availability(self) -> UserConsentVerificationAvailability:
        """Check if Windows Hello is available"""
        if not self._initialized:
            await self.initialize()
        
        if self.availability is None:
            self.availability = await UserConsentVerifier.check_availability_async()
        
        return self.availability
    
    async def request_verification(self, reason: str = "Authenticate to unlock IGED") -> UserConsentVerificationResult:
        """Request biometric verification from user"""
        if not self._initialized:
            await self.initialize()
        
        try:
            logger.info(f"Requesting Windows Hello verification: {reason}")
            result = await UserConsentVerifier.request_verification_async(reason)
            self.verification_result = result
            logger.info(f"Windows Hello verification result: {result}")
            return result
        except Exception as e:
            logger.error(f"Windows Hello verification failed: {e}")
            return UserConsentVerificationResult.CANCELED
    
    def is_available(self) -> bool:
        """Check if Windows Hello is available (synchronous)"""
        if not WINDOWS_HELLO_AVAILABLE:
            return False
        
        try:
            loop = asyncio.get_event_loop()
            if loop.is_running():
                # If we're in an async context, we need to handle this differently
                return False
            else:
                availability = asyncio.run(self.check_availability())
                return availability != UserConsentVerificationAvailability.UNAVAILABLE
        except Exception as e:
            logger.error(f"Error checking Windows Hello availability: {e}")
            return False
    
    def authenticate_sync(self, reason: str = "Authenticate to unlock IGED") -> bool:
        """Synchronous biometric authentication"""
        if not WINDOWS_HELLO_AVAILABLE:
            logger.error("Windows Hello not available")
            return False
        
        try:
            # Check availability first
            if not self.is_available():
                logger.warning("Windows Hello not available on this system")
                return False
            
            # Request verification
            result = asyncio.run(self.request_verification(reason))
            
            # Check result
            if result == UserConsentVerificationResult.VERIFIED:
                logger.info("Windows Hello authentication successful")
                return True
            elif result == UserConsentVerificationResult.DEVICE_NOT_PRESENT:
                logger.warning("Biometric device not present")
                return False
            elif result == UserConsentVerificationResult.NOT_CONFIGURED_FOR_USER:
                logger.warning("Windows Hello not configured for current user")
                return False
            elif result == UserConsentVerificationResult.DISABLED_BY_POLICY:
                logger.warning("Windows Hello disabled by policy")
                return False
            elif result == UserConsentVerificationResult.CANCELED:
                logger.info("Windows Hello authentication canceled by user")
                return False
            else:
                logger.warning(f"Windows Hello authentication failed: {result}")
                return False
                
        except Exception as e:
            logger.error(f"Windows Hello authentication error: {e}")
            return False

# Global instance
_windows_hello_manager = None

def get_windows_hello_manager() -> WindowsHelloManager:
    """Get or create Windows Hello manager instance"""
    global _windows_hello_manager
    if _windows_hello_manager is None:
        _windows_hello_manager = WindowsHelloManager()
    return _windows_hello_manager

def biometric_authenticate_sync(reason: str = "Authenticate to unlock IGED") -> bool:
    """
    Synchronous biometric authentication using Windows Hello
    
    Args:
        reason: Reason for authentication to display to user
        
    Returns:
        bool: True if authentication successful, False otherwise
    """
    manager = get_windows_hello_manager()
    return manager.authenticate_sync(reason)

async def biometric_authenticate_async(reason: str = "Authenticate to unlock IGED") -> bool:
    """
    Asynchronous biometric authentication using Windows Hello
    
    Args:
        reason: Reason for authentication to display to user
        
    Returns:
        bool: True if authentication successful, False otherwise
    """
    manager = get_windows_hello_manager()
    
    # Initialize if needed
    if not manager._initialized:
        await manager.initialize()
    
    # Check availability
    availability = await manager.check_availability()
    if availability == UserConsentVerificationAvailability.UNAVAILABLE:
        logger.warning("Windows Hello not available")
        return False
    
    # Request verification
    result = await manager.request_verification(reason)
    return result == UserConsentVerificationResult.VERIFIED

def check_biometric_availability() -> bool:
    """
    Check if biometric authentication is available
    
    Returns:
        bool: True if biometric hardware is available and configured
    """
    if not WINDOWS_HELLO_AVAILABLE:
        return False
    
    manager = get_windows_hello_manager()
    return manager.is_available()

# Legacy function names for backward compatibility
async def check_biometric_availability_async():
    """Legacy async function for checking availability"""
    if not WINDOWS_HELLO_AVAILABLE:
        return False
    
    manager = get_windows_hello_manager()
    await manager.initialize()
    availability = await manager.check_availability()
    return availability != UserConsentVerificationAvailability.UNAVAILABLE

async def request_biometric_verification(reason: str = "Authenticate to unlock IGED"):
    """Legacy async function for requesting verification"""
    if not WINDOWS_HELLO_AVAILABLE:
        return UserConsentVerificationResult.CANCELED
    
    manager = get_windows_hello_manager()
    await manager.initialize()
    return await manager.request_verification(reason)

def biometric_authenticate_sync_legacy():
    """Legacy synchronous authentication function"""
    return biometric_authenticate_sync()

# Test function
def test_windows_hello():
    """Test Windows Hello functionality"""
    print("🔐 Testing Windows Hello Biometric Authentication")
    print("=" * 50)
    
    if not WINDOWS_HELLO_AVAILABLE:
        print("❌ Windows Hello not available - pywinrt not installed")
        print("💡 Install with: pip install pywinrt")
        return False
    
    print("✅ Windows Hello module loaded")
    
    # Check availability
    if check_biometric_availability():
        print("✅ Biometric hardware available")
        
        # Test authentication
        print("🔑 Requesting biometric authentication...")
        success = biometric_authenticate_sync("Test authentication for IGED")
        
        if success:
            print("✅ Biometric authentication successful!")
            return True
        else:
            print("❌ Biometric authentication failed or canceled")
            return False
    else:
        print("❌ No biometric hardware available or not configured")
        return False

if __name__ == "__main__":
    # Run test if called directly
    test_windows_hello() 