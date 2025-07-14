"""
Agent Orchestrator for IGED
Manages all agents and their interactions

This module serves as the central orchestrator for all IGED agents,
managing their lifecycle, communication, and coordination.
"""

import asyncio
import logging
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Any, Optional, Callable, Set
import threading
import importlib.util
import sys

logger = logging.getLogger(__name__)


class AgentOrchestrator:
    """
    Central orchestrator for all IGED agents.
    
    This class manages the lifecycle, communication, and coordination
    of all agents in the IGED system, ensuring proper task distribution
    and result aggregation.
    """
    
    def __init__(self, memory_engine=None, command_parser=None):
        """
        Initialize the agent orchestrator.
        
        Args:
            memory_engine: Memory engine for persistent storage
            command_parser: Command parser for processing requests
        """
        self.memory_engine = memory_engine
        self.command_parser = command_parser
        
        # Agent management
        self.agents: Dict[str, Any] = {}
        self.agent_status: Dict[str, Dict[str, Any]] = {}
        self.agent_threads: Dict[str, threading.Thread] = {}
        
        # Plugin management
        self.plugins: Dict[str, Any] = {}
        self.plugin_status: Dict[str, Dict[str, Any]] = {}
        
        # Task management
        self.active_tasks: Dict[str, Dict[str, Any]] = {}
        self.task_queue: List[Dict[str, Any]] = []
        self.completed_tasks: List[Dict[str, Any]] = []
        
        # Communication
        self.message_queue: List[Dict[str, Any]] = []
        self.event_listeners: Dict[str, List[Callable]] = {}
        
        # System state
        self.running = False
        self.startup_time: Optional[datetime] = None
        self.statistics: Dict[str, Any] = {
            'tasks_processed': 0,
            'agents_loaded': 0,
            'plugins_loaded': 0,
            'errors_encountered': 0,
            'uptime_seconds': 0
        }
        
        # Initialize components
        self._initialize_orchestrator()
    
    def _initialize_orchestrator(self) -> None:
        """
        Initialize the orchestrator components.
        
        Sets up the orchestrator with default settings and
        prepares it for agent and plugin management.
        """
        try:
            # Set up directories
            self.agents_dir = Path("agents")
            self.plugins_dir = Path("plugins")
            self.output_dir = Path("output")
            
            # Create directories if they don't exist
            for directory in [self.agents_dir, self.plugins_dir, self.output_dir]:
                directory.mkdir(exist_ok=True)
            
            # Initialize event system
            self.event_listeners = {
                'task_completed': [],
                'agent_started': [],
                'agent_stopped': [],
                'plugin_loaded': [],
                'error_occurred': []
            }
            
            logger.info("🎯 Agent orchestrator initialized successfully")
            
        except Exception as e:
            logger.error(f"❌ Failed to initialize orchestrator: {e}")
            raise
    
    def start(self) -> None:
        """
        Start the agent orchestrator.
        
        Initializes all components and begins managing agents and tasks.
        """
        if self.running:
            logger.warning("Orchestrator already running")
            return
        
        try:
            self.running = True
            self.startup_time = datetime.now()
            
            # Load available agents
            self._load_agents()
            
            # Load available plugins
            self._load_plugins()
            
            # Start task processing
            self._start_task_processor()
            
            logger.info("🚀 Agent orchestrator started successfully")
            
        except Exception as e:
            logger.error(f"❌ Failed to start orchestrator: {e}")
            self.running = False
            raise
    
    def stop(self) -> None:
        """
        Stop the agent orchestrator.
        
        Cleanly shuts down all agents and saves final statistics.
        """
        if not self.running:
            logger.warning("Orchestrator not running")
            return
        
        try:
            self.running = False
            
            # Stop all agents
            self._stop_all_agents()
            
            # Save final statistics
            self._save_statistics()
            
            logger.info("🛑 Agent orchestrator stopped")
            
        except Exception as e:
            logger.error(f"❌ Error stopping orchestrator: {e}")
    
    def _load_agents(self) -> None:
        """
        Load all available agents from the agents directory.
        
        Scans the agents directory and dynamically loads all
        available agent modules.
        """
        try:
            agent_count = 0
            
            for agent_dir in self.agents_dir.iterdir():
                if agent_dir.is_dir() and not agent_dir.name.startswith('_'):
                    agent_name = agent_dir.name
                    
                    try:
                        # Load agent module
                        agent_module = self._load_agent_module(agent_name, agent_dir)
                        
                        if agent_module:
                            self.agents[agent_name] = agent_module
                            self.agent_status[agent_name] = {
                                'loaded': True,
                                'running': False,
                                'last_activity': datetime.now(),
                                'tasks_processed': 0,
                                'errors': 0
                            }
                            agent_count += 1
                            
                            # Emit event
                            self._emit_event('agent_loaded', {'agent': agent_name})
                            
                            logger.info(f"✅ Loaded agent: {agent_name}")
                        
                    except Exception as e:
                        logger.error(f"❌ Failed to load agent {agent_name}: {e}")
                        self.agent_status[agent_name] = {
                            'loaded': False,
                            'error': str(e),
                            'last_attempt': datetime.now()
                        }
            
            self.statistics['agents_loaded'] = agent_count
            logger.info(f"📊 Loaded {agent_count} agents")
            
        except Exception as e:
            logger.error(f"❌ Error loading agents: {e}")
    
    def _load_agent_module(self, agent_name: str, agent_dir: Path) -> Optional[Any]:
        """
        Load a specific agent module.
        
        Args:
            agent_name: Name of the agent to load
            agent_dir: Directory containing the agent
            
        Returns:
            Loaded agent module or None if loading fails
        """
        try:
            main_file = agent_dir / "main.py"
            
            if not main_file.exists():
                logger.warning(f"⚠️ No main.py found for agent: {agent_name}")
                return None
            
            # Load module dynamically
            spec = importlib.util.spec_from_file_location(
                f"agents.{agent_name}.main",
                main_file
            )
            
            if spec is None or spec.loader is None:
                logger.error(f"❌ Failed to create spec for agent: {agent_name}")
                return None
            
            module = importlib.util.module_from_spec(spec)
            sys.modules[f"agents.{agent_name}.main"] = module
            spec.loader.exec_module(module)
            
            # Check if module has required components
            if hasattr(module, 'main') or hasattr(module, 'Agent'):
                return module
            else:
                logger.warning(f"⚠️ Agent {agent_name} missing required components")
                return None
                
        except Exception as e:
            logger.error(f"❌ Error loading agent module {agent_name}: {e}")
            return None
    
    def _load_plugins(self) -> None:
        """
        Load all available plugins from the plugins directory.
        
        Scans the plugins directory and dynamically loads all
        available plugin modules.
        """
        try:
            plugin_count = 0
            
            for plugin_file in self.plugins_dir.glob("*.py"):
                if plugin_file.name.startswith('_'):
                    continue
                
                plugin_name = plugin_file.stem
                
                try:
                    # Load plugin module
                    plugin_module = self._load_plugin_module(plugin_name, plugin_file)
                    
                    if plugin_module:
                        self.plugins[plugin_name] = plugin_module
                        self.plugin_status[plugin_name] = {
                            'loaded': True,
                            'active': False,
                            'last_used': None,
                            'usage_count': 0
                        }
                        plugin_count += 1
                        
                        # Emit event
                        self._emit_event('plugin_loaded', {'plugin': plugin_name})
                        
                        logger.info(f"🔌 Loaded plugin: {plugin_name}")
                    
                except Exception as e:
                    logger.error(f"❌ Failed to load plugin {plugin_name}: {e}")
                    self.plugin_status[plugin_name] = {
                        'loaded': False,
                        'error': str(e),
                        'last_attempt': datetime.now()
                    }
            
            self.statistics['plugins_loaded'] = plugin_count
            logger.info(f"📊 Loaded {plugin_count} plugins")
            
        except Exception as e:
            logger.error(f"❌ Error loading plugins: {e}")
    
    def _load_plugin_module(self, plugin_name: str, plugin_file: Path) -> Optional[Any]:
        """
        Load a specific plugin module.
        
        Args:
            plugin_name: Name of the plugin to load
            plugin_file: Path to the plugin file
            
        Returns:
            Loaded plugin module or None if loading fails
        """
        try:
            # Load module dynamically
            spec = importlib.util.spec_from_file_location(
                f"plugins.{plugin_name}",
                plugin_file
            )
            
            if spec is None or spec.loader is None:
                logger.error(f"❌ Failed to create spec for plugin: {plugin_name}")
                return None
            
            module = importlib.util.module_from_spec(spec)
            sys.modules[f"plugins.{plugin_name}"] = module
            spec.loader.exec_module(module)
            
            # Check if module has required components
            if hasattr(module, 'execute') or hasattr(module, 'main'):
                return module
            else:
                logger.warning(f"⚠️ Plugin {plugin_name} missing required components")
                return None
                
        except Exception as e:
            logger.error(f"❌ Error loading plugin module {plugin_name}: {e}")
            return None
    
    def _start_task_processor(self) -> None:
        """
        Start the task processing thread.
        
        Begins processing tasks from the task queue in a separate thread.
        """
        try:
            self.task_processor_thread = threading.Thread(
                target=self._task_processor_loop,
                daemon=True,
                name="TaskProcessor"
            )
            self.task_processor_thread.start()
            
            logger.info("⚙️ Task processor started")
            
        except Exception as e:
            logger.error(f"❌ Failed to start task processor: {e}")
    
    def _task_processor_loop(self) -> None:
        """
        Main task processor loop.
        
        Continuously processes tasks from the task queue while running.
        """
        import time
        
        while self.running:
            try:
                # Process pending tasks
                if self.task_queue:
                    task = self.task_queue.pop(0)
                    self._process_task(task)
                
                # Check for completed tasks
                self._check_completed_tasks()
                
                # Update statistics
                self._update_statistics()
                
                # Sleep briefly to avoid busy waiting
                time.sleep(0.1)
                
            except Exception as e:
                logger.error(f"❌ Task processor error: {e}")
                self.statistics['errors_encountered'] += 1
                time.sleep(1)
    
    def _process_task(self, task: Dict[str, Any]) -> None:
        """
        Process a single task.
        
        Args:
            task: Task dictionary containing task details
        """
        try:
            task_id = task.get('id', 'unknown')
            task_type = task.get('type', 'unknown')
            
            logger.info(f"⚡ Processing task: {task_id} ({task_type})")
            
            # Update task status
            task['status'] = 'processing'
            task['start_time'] = datetime.now()
            self.active_tasks[task_id] = task
            
            # Route task to appropriate agent
            result = self._route_task_to_agent(task)
            
            # Update task with result
            task['result'] = result
            task['end_time'] = datetime.now()
            task['status'] = 'completed' if result.get('success', False) else 'failed'
            
            # Move to completed tasks
            self.completed_tasks.append(task)
            del self.active_tasks[task_id]
            
            # Update statistics
            self.statistics['tasks_processed'] += 1
            
            # Emit event
            self._emit_event('task_completed', task)
            
            logger.info(f"✅ Task completed: {task_id}")
            
        except Exception as e:
            logger.error(f"❌ Task processing error: {e}")
            task['status'] = 'error'
            task['error'] = str(e)
            task['end_time'] = datetime.now()
            
            # Move to completed tasks
            self.completed_tasks.append(task)
            if task_id in self.active_tasks:
                del self.active_tasks[task_id]
            
            self.statistics['errors_encountered'] += 1
    
    def _route_task_to_agent(self, task: Dict[str, Any]) -> Dict[str, Any]:
        """
        Route a task to the appropriate agent.
        
        Args:
            task: Task to route
            
        Returns:
            Dictionary containing task result
        """
        try:
            task_type = task.get('type', 'unknown')
            command = task.get('command', '')
            
            # Determine target agent based on task type or command
            target_agent = self._determine_target_agent(task_type, command)
            
            if target_agent and target_agent in self.agents:
                # Execute task with agent
                return self._execute_task_with_agent(task, target_agent)
            else:
                return {
                    'success': False,
                    'error': f'No suitable agent found for task type: {task_type}',
                    'task_type': task_type
                }
                
        except Exception as e:
            logger.error(f"❌ Task routing error: {e}")
            return {
                'success': False,
                'error': f'Task routing failed: {str(e)}'
            }
    
    def _determine_target_agent(self, task_type: str, command: str) -> Optional[str]:
        """
        Determine which agent should handle a task.
        
        Args:
            task_type: Type of task
            command: Command string
            
        Returns:
            Name of target agent or None
        """
        # Agent routing logic
        routing_rules = {
            'codegen': 'codegen_agent',
            'security': 'secops',
            'network': 'network_intelligence',
            'data': 'data_miner',
            'remote': 'remote_control',
            'advanced_security': 'advanced_secops'
        }
        
        # Check direct task type mapping
        if task_type in routing_rules:
            return routing_rules[task_type]
        
        # Check command-based routing
        command_lower = command.lower()
        
        if any(keyword in command_lower for keyword in ['code', 'generate', 'script', 'program']):
            return 'codegen_agent'
        elif any(keyword in command_lower for keyword in ['security', 'scan', 'vulnerability', 'exploit']):
            return 'secops'
        elif any(keyword in command_lower for keyword in ['network', 'ping', 'port', 'nmap']):
            return 'network_intelligence'
        elif any(keyword in command_lower for keyword in ['data', 'analyze', 'csv', 'json']):
            return 'data_miner'
        elif any(keyword in command_lower for keyword in ['remote', 'control', 'execute']):
            return 'remote_control'
        elif any(keyword in command_lower for keyword in ['advanced', 'forensics', 'malware']):
            return 'advanced_secops'
        
        # Default to first available agent
        return next(iter(self.agents.keys())) if self.agents else None
    
    def _execute_task_with_agent(self, task: Dict[str, Any], agent_name: str) -> Dict[str, Any]:
        """
        Execute a task with a specific agent.
        
        Args:
            task: Task to execute
            agent_name: Name of the agent to use
            
        Returns:
            Dictionary containing execution result
        """
        try:
            agent = self.agents[agent_name]
            
            # Update agent status
            self.agent_status[agent_name]['last_activity'] = datetime.now()
            self.agent_status[agent_name]['running'] = True
            
            # Execute task
            if hasattr(agent, 'execute'):
                result = agent.execute(task)
            elif hasattr(agent, 'main'):
                result = agent.main(task.get('command', ''))
            else:
                result = {'success': False, 'error': 'Agent has no execute method'}
            
            # Update agent statistics
            self.agent_status[agent_name]['tasks_processed'] += 1
            self.agent_status[agent_name]['running'] = False
            
            return result
            
        except Exception as e:
            logger.error(f"❌ Agent execution error ({agent_name}): {e}")
            
            # Update agent error count
            if agent_name in self.agent_status:
                self.agent_status[agent_name]['errors'] += 1
                self.agent_status[agent_name]['running'] = False
            
            return {
                'success': False,
                'error': str(e),
                'agent': agent_name
            }
    
    def _check_completed_tasks(self) -> None:
        """
        Check and cleanup completed tasks.
        
        Removes old completed tasks to prevent memory buildup.
        """
        try:
            # Keep only last 100 completed tasks
            if len(self.completed_tasks) > 100:
                self.completed_tasks = self.completed_tasks[-100:]
            
        except Exception as e:
            logger.error(f"❌ Error checking completed tasks: {e}")
    
    def _update_statistics(self) -> None:
        """
        Update orchestrator statistics.
        
        Calculates and updates various performance metrics.
        """
        try:
            if self.startup_time:
                self.statistics['uptime_seconds'] = (
                    datetime.now() - self.startup_time
                ).total_seconds()
            
            self.statistics['active_tasks'] = len(self.active_tasks)
            self.statistics['queued_tasks'] = len(self.task_queue)
            self.statistics['completed_tasks'] = len(self.completed_tasks)
            
        except Exception as e:
            logger.error(f"❌ Error updating statistics: {e}")
    
    def _stop_all_agents(self) -> None:
        """
        Stop all running agents.
        
        Cleanly shuts down all active agents.
        """
        try:
            for agent_name in self.agents.keys():
                if agent_name in self.agent_status:
                    self.agent_status[agent_name]['running'] = False
            
            # Wait for agent threads to finish
            for thread in self.agent_threads.values():
                if thread.is_alive():
                    thread.join(timeout=5)
            
            logger.info("🛑 All agents stopped")
            
        except Exception as e:
            logger.error(f"❌ Error stopping agents: {e}")
    
    def _save_statistics(self) -> None:
        """
        Save final statistics to memory.
        
        Persists orchestrator statistics for later analysis.
        """
        try:
            if self.memory_engine:
                stats_entry = {
                    'type': 'orchestrator_statistics',
                    'timestamp': datetime.now().isoformat(),
                    'statistics': self.statistics,
                    'agent_status': self.agent_status,
                    'plugin_status': self.plugin_status
                }
                
                self.memory_engine.store_interaction(
                    command="orchestrator_shutdown",
                    result=stats_entry,
                    agent="orchestrator"
                )
                
        except Exception as e:
            logger.error(f"❌ Error saving statistics: {e}")
    
    def _emit_event(self, event_name: str, data: Dict[str, Any]) -> None:
        """
        Emit an event to registered listeners.
        
        Args:
            event_name: Name of the event
            data: Event data
        """
        try:
            if event_name in self.event_listeners:
                for listener in self.event_listeners[event_name]:
                    try:
                        listener(data)
                    except Exception as e:
                        logger.error(f"❌ Event listener error: {e}")
                        
        except Exception as e:
            logger.error(f"❌ Event emission error: {e}")
    
    def submit_task(self, task: Dict[str, Any]) -> str:
        """
        Submit a task for processing.
        
        Args:
            task: Task dictionary
            
        Returns:
            Task ID for tracking
        """
        try:
            import uuid
            
            task_id = str(uuid.uuid4())
            task['id'] = task_id
            task['created'] = datetime.now()
            task['status'] = 'queued'
            
            self.task_queue.append(task)
            
            logger.info(f"📝 Task submitted: {task_id}")
            return task_id
            
        except Exception as e:
            logger.error(f"❌ Task submission error: {e}")
            return ""
    
    def get_task_status(self, task_id: str) -> Dict[str, Any]:
        """
        Get the status of a task.
        
        Args:
            task_id: ID of the task
            
        Returns:
            Task status dictionary
        """
        try:
            # Check active tasks
            if task_id in self.active_tasks:
                return self.active_tasks[task_id]
            
            # Check completed tasks
            for task in self.completed_tasks:
                if task.get('id') == task_id:
                    return task
            
            # Check queued tasks
            for task in self.task_queue:
                if task.get('id') == task_id:
                    return task
            
            return {'error': 'Task not found'}
            
        except Exception as e:
            logger.error(f"❌ Error getting task status: {e}")
            return {'error': str(e)}
    
    def get_status(self) -> Dict[str, Any]:
        """
        Get orchestrator status.
        
        Returns:
            Dictionary containing orchestrator status
        """
        try:
            return {
                'running': self.running,
                'startup_time': self.startup_time.isoformat() if self.startup_time else None,
                'agents_loaded': len(self.agents),
                'plugins_loaded': len(self.plugins),
                'active_tasks': len(self.active_tasks),
                'queued_tasks': len(self.task_queue),
                'completed_tasks': len(self.completed_tasks),
                'statistics': self.statistics.copy(),
                'agent_status': self.agent_status.copy(),
                'plugin_status': self.plugin_status.copy()
            }
            
        except Exception as e:
            logger.error(f"❌ Error getting orchestrator status: {e}")
            return {'error': str(e)}
    
    def get_health(self) -> Dict[str, Any]:
        """
        Get orchestrator health status.
        
        Returns:
            Dictionary containing health information
        """
        try:
            total_agents = len(self.agents)
            healthy_agents = sum(1 for status in self.agent_status.values() 
                               if status.get('loaded', False))
            
            agent_health_ratio = (healthy_agents / total_agents * 100) if total_agents > 0 else 0
            
            # Determine overall health
            if agent_health_ratio >= 90:
                health_status = 'excellent'
            elif agent_health_ratio >= 70:
                health_status = 'good'
            elif agent_health_ratio >= 50:
                health_status = 'warning'
            else:
                health_status = 'critical'
            
            return {
                'status': health_status,
                'running': self.running,
                'agent_health_ratio': agent_health_ratio,
                'total_agents': total_agents,
                'healthy_agents': healthy_agents,
                'active_tasks': len(self.active_tasks),
                'error_rate': self.statistics.get('errors_encountered', 0),
                'uptime_seconds': self.statistics.get('uptime_seconds', 0)
            }
            
        except Exception as e:
            logger.error(f"❌ Error getting health status: {e}")
            return {
                'status': 'error',
                'error': str(e)
            } 