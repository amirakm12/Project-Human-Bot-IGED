"""
Command Parser for IGED
Converts natural language to structured commands

This module provides natural language processing capabilities to parse
user commands and convert them into structured format for agent execution.
"""

import logging
from typing import Dict, Any, List, Optional, Tuple
from datetime import datetime

logger = logging.getLogger(__name__)


class CommandParser:
    """
    Natural language command parser for IGED system.
    
    This class converts natural language input into structured commands
    that can be executed by various agents in the IGED system.
    """
    
    def __init__(self):
        """Initialize the command parser with predefined patterns."""
        self.command_patterns = {
            'codegen': [
                r'(?:generate|create|make|build)\s+(?:a\s+)?(?:flask|web|python|script|api|rest|html|website)',
                r'(?:write|code)\s+(?:a\s+)?(?:flask|web|python|script|api|rest|html|website)',
                r'(?:develop|program)\s+(?:a\s+)?(?:flask|web|python|script|api|rest|html|website)'
            ],
            'secops': [
                r'(?:scan|check|test|audit)\s+(?:for\s+)?(?:vulnerabilities|security|ports|network)',
                r'(?:security|penetration|vulnerability)\s+(?:scan|test|audit)',
                r'(?:port|network)\s+(?:scan|check|test)',
                r'(?:web|http|https)\s+(?:security|vulnerability)\s+(?:scan|check)'
            ],
            'advanced_secops': [
                r'(?:penetrate|hack|exploit|breach)\s+(?:into|to|the)\s+',
                r'(?:advanced|deep|comprehensive)\s+(?:penetration|security|hacking)',
                r'(?:zero.?day|exploit|vulnerability)\s+(?:scan|test|attack)',
                r'(?:persistent|backdoor|covert)\s+(?:access|connection|control)'
            ],
            'network_intelligence': [
                r'(?:monitor|surveillance|intercept)\s+(?:network|traffic|communication)',
                r'(?:capture|analyze|decode)\s+(?:packets|traffic|protocols)',
                r'(?:device|inventory|discovery)\s+(?:network|devices|systems)',
                r'(?:intelligence|reconnaissance|gathering)\s+(?:network|system)'
            ],
            'remote_control': [
                r'(?:connect|establish|control)\s+(?:remote|to|connection)',
                r'(?:execute|run|command)\s+(?:remote|on|system)',
                r'(?:deploy|payload|backdoor)\s+(?:to|on|system)',
                r'(?:monitor|surveillance)\s+(?:remote|system|device)'
            ],
            'dataminer': [
                r'(?:analyze|process|mine|extract)\s+(?:data|dataset|file)',
                r'(?:data|statistical)\s+(?:analysis|processing|mining)',
                r'(?:visualize|plot|chart)\s+(?:data|dataset)',
                r'(?:generate|create)\s+(?:statistics|stats|report)\s+(?:for|from)'
            ]
        }
        
        self.parameter_patterns = {
            'file_path': r'["\']([^"\']*\.(?:csv|xlsx|xls|json|txt|py|html|js|css))["\']',
            'url': r'https?://[^\s]+',
            'ip_address': r'\b(?:\d{1,3}\.){3}\d{1,3}\b',
            'hostname': r'(?:scan|check|test)\s+(?:the\s+)?([a-zA-Z0-9.-]+)',
            'target': r'(?:for|on|at)\s+([a-zA-Z0-9._-]+)'
        }
    
    def parse_command(self, text: str) -> Dict[str, Any]:
        """
        Parse natural language command into structured format.
        
        Args:
            text: Natural language command string
            
        Returns:
            Dict containing parsed command structure with fields:
            - original_text: Original input text
            - command_type: Type of command identified
            - agent: Target agent for command execution
            - target: Target resource/system for command
            - parameters: Additional parameters extracted
            - confidence: Confidence score for parsing
            - timestamp: Timestamp of parsing
        """
        try:
            text = text.strip()
            if not text:
                return self._create_error_command("Empty command")
            
            logger.info(f"🔍 Parsing command: {text}")
            
            # Determine command type and agent
            command_type, agent = self._identify_command_type(text)
            
            # Extract target and parameters
            target = self._extract_target(text, command_type)
            parameters = self._extract_parameters(text)
            
            # Create command structure
            command = {
                'original_text': text,
                'command_type': command_type,
                'agent': agent,
                'target': target,
                'parameters': parameters,
                'confidence': self._calculate_confidence(text, command_type),
                'timestamp': self._get_timestamp()
            }
            
            logger.info(f"✅ Parsed command: {command_type} -> {agent}")
            return command
            
        except Exception as e:
            logger.error(f"❌ Command parsing failed: {e}")
            return self._create_error_command(f"Parsing error: {str(e)}")
    
    def _identify_command_type(self, text: str) -> Tuple[str, str]:
        """
        Identify the type of command and appropriate agent.
        
        Args:
            text: Input text to analyze
            
        Returns:
            Tuple of (command_type, agent_name)
        """
        import re
        
        text_lower = text.lower()
        
        # Check each agent's patterns
        for agent, patterns in self.command_patterns.items():
            for pattern in patterns:
                if re.search(pattern, text_lower):
                    command_type = self._get_command_type(text, agent)
                    return command_type, agent
        
        # Default to general command
        return "general", "codegen"
    
    def _get_command_type(self, text: str, agent: str) -> str:
        """
        Get specific command type for an agent.
        
        Args:
            text: Input text
            agent: Agent name
            
        Returns:
            Specific command type string
        """
        import re
        
        text_lower = text.lower()
        
        # Define command type mappings
        command_type_mappings = {
            'codegen': {
                'web': ['flask', 'web', 'website', 'html'],
                'api': ['api', 'rest', 'endpoint'],
                'script': ['script', 'python', 'code'],
                'generate': ['generate', 'create', 'make', 'build']
            },
            'secops': {
                'scan': ['scan', 'check', 'test'],
                'audit': ['audit', 'security'],
                'vulnerability': ['vulnerability', 'vuln'],
                'penetration': ['penetration', 'pentest']
            },
            'advanced_secops': {
                'exploit': ['exploit', 'hack', 'breach'],
                'persistence': ['persistent', 'backdoor'],
                'advanced': ['advanced', 'deep', 'comprehensive']
            },
            'network_intelligence': {
                'monitor': ['monitor', 'surveillance'],
                'analyze': ['analyze', 'decode'],
                'discovery': ['discovery', 'inventory'],
                'reconnaissance': ['reconnaissance', 'intel']
            },
            'remote_control': {
                'connect': ['connect', 'establish'],
                'execute': ['execute', 'run'],
                'deploy': ['deploy', 'payload'],
                'monitor': ['monitor', 'surveillance']
            },
            'dataminer': {
                'analyze': ['analyze', 'process'],
                'visualize': ['visualize', 'plot', 'chart'],
                'extract': ['extract', 'mine'],
                'statistics': ['statistics', 'stats']
            }
        }
        
        if agent in command_type_mappings:
            for cmd_type, keywords in command_type_mappings[agent].items():
                if any(keyword in text_lower for keyword in keywords):
                    return cmd_type
        
        return "general"
    
    def _extract_target(self, text: str, command_type: str) -> str:
        """
        Extract target information from command text.
        
        Args:
            text: Input text
            command_type: Type of command
            
        Returns:
            Target string or empty string if not found
        """
        import re
        
        # Try to extract common target patterns
        for pattern_name, pattern in self.parameter_patterns.items():
            if pattern_name in ['ip_address', 'hostname', 'url']:
                match = re.search(pattern, text)
                if match:
                    return match.group(1) if match.groups() else match.group(0)
        
        # Extract file paths
        file_match = re.search(self.parameter_patterns['file_path'], text)
        if file_match:
            return file_match.group(1)
        
        # Extract general targets
        target_match = re.search(self.parameter_patterns['target'], text)
        if target_match:
            return target_match.group(1)
        
        return ""
    
    def _extract_parameters(self, text: str) -> Dict[str, Any]:
        """
        Extract additional parameters from command text.
        
        Args:
            text: Input text
            
        Returns:
            Dictionary of extracted parameters
        """
        import re
        
        parameters = {}
        
        # Extract all parameter types
        for param_type, pattern in self.parameter_patterns.items():
            match = re.search(pattern, text)
            if match:
                parameters[param_type] = match.group(1) if match.groups() else match.group(0)
        
        # Extract port numbers
        port_match = re.search(r'port\s+(\d+)', text, re.IGNORECASE)
        if port_match:
            parameters['port'] = int(port_match.group(1))
        
        # Extract output formats
        format_match = re.search(r'(?:as|in|to)\s+(json|csv|html|txt|pdf)', text, re.IGNORECASE)
        if format_match:
            parameters['output_format'] = format_match.group(1).lower()
        
        # Extract verbosity levels
        if any(word in text.lower() for word in ['verbose', 'detailed', 'full']):
            parameters['verbose'] = True
        
        if any(word in text.lower() for word in ['quiet', 'silent', 'minimal']):
            parameters['verbose'] = False
        
        return parameters
    
    def _calculate_confidence(self, text: str, command_type: str) -> float:
        """
        Calculate confidence score for parsed command.
        
        Args:
            text: Input text
            command_type: Identified command type
            
        Returns:
            Confidence score between 0.0 and 1.0
        """
        import re
        
        base_confidence = 0.5
        
        # Increase confidence for specific patterns
        if command_type != "general":
            base_confidence += 0.3
        
        # Check for specific keywords
        specific_keywords = len(re.findall(r'\b(?:scan|generate|create|analyze|monitor)\b', text.lower()))
        base_confidence += min(specific_keywords * 0.1, 0.2)
        
        # Check for parameters
        parameter_count = len(self._extract_parameters(text))
        base_confidence += min(parameter_count * 0.05, 0.1)
        
        return min(base_confidence, 1.0)
    
    def _get_timestamp(self) -> str:
        """
        Get current timestamp in ISO format.
        
        Returns:
            ISO formatted timestamp string
        """
        return datetime.now().isoformat()
    
    def _create_error_command(self, error_message: str) -> Dict[str, Any]:
        """
        Create error command structure.
        
        Args:
            error_message: Error description
            
        Returns:
            Error command dictionary
        """
        return {
            'original_text': "",
            'command_type': "error",
            'agent': "none",
            'target': "",
            'parameters': {},
            'confidence': 0.0,
            'timestamp': self._get_timestamp(),
            'error': error_message
        }
    
    def get_supported_commands(self) -> Dict[str, List[str]]:
        """
        Get dictionary of supported commands by agent.
        
        Returns:
            Dictionary mapping agent names to their command patterns
        """
        supported_commands = {}
        
        for agent, patterns in self.command_patterns.items():
            # Convert regex patterns to human-readable descriptions
            descriptions = []
            for pattern in patterns:
                # Simplify regex patterns for user display
                simplified = pattern.replace(r'(?:', '').replace(r')', '')
                simplified = simplified.replace(r'\s+', ' ')
                simplified = simplified.replace(r'?:', '')
                descriptions.append(simplified)
            
            supported_commands[agent] = descriptions
        
        return supported_commands
    
    def validate_command(self, command: Dict[str, Any]) -> bool:
        """
        Validate a parsed command structure.
        
        Args:
            command: Command dictionary to validate
            
        Returns:
            True if command is valid, False otherwise
        """
        required_fields = ['original_text', 'command_type', 'agent', 'target', 'parameters', 'confidence', 'timestamp']
        
        # Check required fields
        if not all(field in command for field in required_fields):
            return False
        
        # Check data types
        if not isinstance(command['parameters'], dict):
            return False
        
        if not isinstance(command['confidence'], (int, float)):
            return False
        
        if not (0.0 <= command['confidence'] <= 1.0):
            return False
        
        return True 