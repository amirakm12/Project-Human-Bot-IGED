"""
Memory Engine for IGED
Handles persistent encrypted storage of commands and results

This module provides comprehensive memory management for the IGED system,
including encrypted storage, retrieval, and analysis of command history.
"""

import json
import logging
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Any, Optional
import uuid

logger = logging.getLogger(__name__)


class MemoryEngine:
    """
    Encrypted memory management system for IGED.
    
    This class handles persistent storage of commands, results, and system
    interactions using encrypted files for security.
    """
    
    def __init__(self, encryption_manager):
        """
        Initialize the memory engine.
        
        Args:
            encryption_manager: EncryptionManager instance for data security
        """
        self.encryption = encryption_manager
        self.memory_file = Path("memory/memory_log.json")
        self.memory_file.parent.mkdir(parents=True, exist_ok=True)
        self.memory_data: List[Dict[str, Any]] = self._load_memory()
    
    def _load_memory(self) -> List[Dict[str, Any]]:
        """
        Load memory from encrypted file.
        
        Returns:
            List of memory entries, empty list if file doesn't exist
        """
        try:
            if self.memory_file.exists():
                with open(self.memory_file, 'r', encoding='utf-8') as f:
                    data = f.read()
                    if data.strip():
                        # Try to decrypt if encrypted
                        try:
                            decrypted = self.encryption.decrypt(data)
                            return json.loads(decrypted)
                        except Exception:
                            # If decryption fails, try as plain JSON
                            return json.loads(data)
            return []
        except Exception as e:
            logger.error(f"❌ Failed to load memory: {e}")
            return []
    
    def _save_memory(self) -> None:
        """
        Save memory to encrypted file.
        
        Raises:
            Exception: If memory saving fails
        """
        try:
            data = json.dumps(self.memory_data, indent=2, ensure_ascii=False)
            encrypted_data = self.encryption.encrypt(data)
            
            with open(self.memory_file, 'w', encoding='utf-8') as f:
                f.write(encrypted_data)
            
            logger.debug("💾 Memory saved successfully")
            
        except Exception as e:
            logger.error(f"❌ Failed to save memory: {e}")
            raise
    
    def add_entry(self, command: str, result: str, agent: str = "unknown", 
                  success: bool = True, metadata: Optional[Dict] = None) -> Optional[str]:
        """
        Add a new memory entry.
        
        Args:
            command: Command that was executed
            result: Result of the command execution
            agent: Agent that executed the command
            success: Whether the command was successful
            metadata: Optional metadata dictionary
            
        Returns:
            Entry ID if successful, None otherwise
        """
        try:
            entry = {
                "id": self._generate_id(),
                "timestamp": datetime.now().isoformat(),
                "command": command,
                "result": result,
                "agent": agent,
                "success": success,
                "metadata": metadata or {}
            }
            
            self.memory_data.append(entry)
            self._save_memory()
            
            logger.info(f"📝 Added memory entry: {entry['id']}")
            return entry['id']
            
        except Exception as e:
            logger.error(f"❌ Failed to add memory entry: {e}")
            return None
    
    def get_entry(self, entry_id: str) -> Optional[Dict[str, Any]]:
        """
        Get a specific memory entry by ID.
        
        Args:
            entry_id: ID of the entry to retrieve
            
        Returns:
            Memory entry dictionary or None if not found
        """
        for entry in self.memory_data:
            if entry.get('id') == entry_id:
                return entry
        return None
    
    def search_entries(self, query: str, limit: int = 10) -> List[Dict[str, Any]]:
        """
        Search memory entries by command or result content.
        
        Args:
            query: Search query string
            limit: Maximum number of results to return
            
        Returns:
            List of matching memory entries
        """
        results = []
        query_lower = query.lower()
        
        for entry in reversed(self.memory_data):
            if (query_lower in entry.get('command', '').lower() or 
                query_lower in entry.get('result', '').lower()):
                results.append(entry)
                if len(results) >= limit:
                    break
        
        return results
    
    def get_recent_entries(self, limit: int = 10) -> List[Dict[str, Any]]:
        """
        Get the most recent memory entries.
        
        Args:
            limit: Maximum number of entries to return
            
        Returns:
            List of recent memory entries
        """
        return list(reversed(self.memory_data[-limit:]))
    
    def get_entries_by_agent(self, agent: str, limit: int = 10) -> List[Dict[str, Any]]:
        """
        Get memory entries by specific agent.
        
        Args:
            agent: Agent name to filter by
            limit: Maximum number of entries to return
            
        Returns:
            List of entries from the specified agent
        """
        results = []
        for entry in reversed(self.memory_data):
            if entry.get('agent') == agent:
                results.append(entry)
                if len(results) >= limit:
                    break
        return results
    
    def delete_entry(self, entry_id: str) -> bool:
        """
        Delete a memory entry by ID.
        
        Args:
            entry_id: ID of the entry to delete
            
        Returns:
            True if entry was deleted, False otherwise
        """
        try:
            for i, entry in enumerate(self.memory_data):
                if entry.get('id') == entry_id:
                    del self.memory_data[i]
                    self._save_memory()
                    logger.info(f"🗑️ Deleted memory entry: {entry_id}")
                    return True
            return False
        except Exception as e:
            logger.error(f"❌ Failed to delete memory entry: {e}")
            return False
    
    def clear_memory(self) -> None:
        """
        Clear all memory entries.
        
        Raises:
            Exception: If memory clearing fails
        """
        try:
            self.memory_data.clear()
            self._save_memory()
            logger.info("🗑️ Memory cleared")
        except Exception as e:
            logger.error(f"❌ Failed to clear memory: {e}")
            raise
    
    def export_memory(self, file_path: str) -> bool:
        """
        Export memory to a file.
        
        Args:
            file_path: Path to export file
            
        Returns:
            True if export successful, False otherwise
        """
        try:
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(self.memory_data, f, indent=2, ensure_ascii=False)
            logger.info(f"📤 Memory exported to: {file_path}")
            return True
        except Exception as e:
            logger.error(f"❌ Failed to export memory: {e}")
            return False
    
    def import_memory(self, file_path: str) -> bool:
        """
        Import memory from a file.
        
        Args:
            file_path: Path to import file
            
        Returns:
            True if import successful, False otherwise
        """
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                imported_data = json.load(f)
            
            # Validate imported data
            if isinstance(imported_data, list):
                self.memory_data.extend(imported_data)
                self._save_memory()
                logger.info(f"📥 Memory imported from: {file_path}")
                return True
            else:
                logger.error("❌ Invalid memory file format")
                return False
                
        except Exception as e:
            logger.error(f"❌ Failed to import memory: {e}")
            return False
    
    def get_statistics(self) -> Dict[str, Any]:
        """
        Get memory statistics.
        
        Returns:
            Dictionary containing memory statistics
        """
        try:
            total_entries = len(self.memory_data)
            successful_entries = sum(1 for entry in self.memory_data if entry.get('success', False))
            failed_entries = total_entries - successful_entries
            
            # Count entries by agent
            agent_counts = {}
            for entry in self.memory_data:
                agent = entry.get('agent', 'unknown')
                agent_counts[agent] = agent_counts.get(agent, 0) + 1
            
            # Recent activity (last 24 hours)
            recent_cutoff = datetime.now().timestamp() - 86400  # 24 hours
            recent_entries = 0
            for entry in self.memory_data:
                try:
                    entry_time = datetime.fromisoformat(entry.get('timestamp', ''))
                    if entry_time.timestamp() > recent_cutoff:
                        recent_entries += 1
                except (ValueError, TypeError):
                    pass
            
            return {
                'total_entries': total_entries,
                'successful_entries': successful_entries,
                'failed_entries': failed_entries,
                'success_rate': (successful_entries / total_entries * 100) if total_entries > 0 else 0,
                'agent_counts': agent_counts,
                'recent_entries_24h': recent_entries,
                'memory_file_size': self.memory_file.stat().st_size if self.memory_file.exists() else 0
            }
            
        except Exception as e:
            logger.error(f"❌ Failed to get statistics: {e}")
            return {'error': str(e)}
    
    def _generate_id(self) -> str:
        """
        Generate a unique ID for memory entries.
        
        Returns:
            Unique identifier string
        """
        return str(uuid.uuid4())[:8]
    
    def get_memory_size(self) -> int:
        """
        Get the total number of memory entries.
        
        Returns:
            Number of entries in memory
        """
        return len(self.memory_data)
    
    def get_memory_health(self) -> Dict[str, Any]:
        """
        Get memory system health information.
        
        Returns:
            Dictionary containing memory health metrics
        """
        try:
            file_exists = self.memory_file.exists()
            file_size = self.memory_file.stat().st_size if file_exists else 0
            
            return {
                'memory_file_exists': file_exists,
                'memory_file_size': file_size,
                'entries_count': len(self.memory_data),
                'encryption_available': self.encryption is not None,
                'last_save_status': 'healthy' if file_exists else 'no_file'
            }
            
        except Exception as e:
            logger.error(f"❌ Failed to get memory health: {e}")
            return {'error': str(e), 'status': 'unhealthy'} 