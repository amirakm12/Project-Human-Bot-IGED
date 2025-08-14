#!/usr/bin/env python3
"""
IGED Orchestrator Agent
Advanced workflow management and task coordination system
"""

import asyncio
import uuid
import json
import time
from enum import Enum
from datetime import datetime, timedelta
from typing import Dict, Any, Optional, List, Callable, Set, Tuple, Union
from dataclasses import dataclass, field, asdict
from collections import defaultdict, deque
import logging
import heapq
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import networkx as nx
import inspect

logger = logging.getLogger(__name__)

class TaskStatus(Enum):
    """Task execution status"""
    PENDING = "pending"
    QUEUED = "queued"
    RUNNING = "running"
    COMPLETED = "completed"
    FAILED = "failed"
    CANCELLED = "cancelled"
    RETRYING = "retrying"
    SKIPPED = "skipped"

class TaskPriority(Enum):
    """Task priority levels"""
    CRITICAL = 1
    HIGH = 2
    NORMAL = 3
    LOW = 4
    BACKGROUND = 5

@dataclass
class Task:
    """Represents a workflow task"""
    id: str = field(default_factory=lambda: str(uuid.uuid4()))
    name: str = ""
    description: str = ""
    handler: Optional[Callable] = None
    params: Dict[str, Any] = field(default_factory=dict)
    dependencies: List[str] = field(default_factory=list)
    priority: TaskPriority = TaskPriority.NORMAL
    status: TaskStatus = TaskStatus.PENDING
    result: Optional[Any] = None
    error: Optional[str] = None
    created_at: datetime = field(default_factory=datetime.now)
    started_at: Optional[datetime] = None
    completed_at: Optional[datetime] = None
    retry_count: int = 0
    max_retries: int = 3
    timeout: Optional[int] = None  # seconds
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert task to dictionary"""
        return {
            'id': self.id,
            'name': self.name,
            'description': self.description,
            'status': self.status.value,
            'priority': self.priority.value,
            'dependencies': self.dependencies,
            'created_at': self.created_at.isoformat(),
            'started_at': self.started_at.isoformat() if self.started_at else None,
            'completed_at': self.completed_at.isoformat() if self.completed_at else None,
            'retry_count': self.retry_count,
            'metadata': self.metadata
        }
    
    def __lt__(self, other):
        """Compare tasks by priority for heap operations"""
        return self.priority.value < other.priority.value

@dataclass
class Workflow:
    """Represents a workflow containing multiple tasks"""
    id: str = field(default_factory=lambda: str(uuid.uuid4()))
    name: str = ""
    description: str = ""
    tasks: Dict[str, Task] = field(default_factory=dict)
    graph: Optional[nx.DiGraph] = None
    status: TaskStatus = TaskStatus.PENDING
    created_at: datetime = field(default_factory=datetime.now)
    started_at: Optional[datetime] = None
    completed_at: Optional[datetime] = None
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def add_task(self, task: Task):
        """Add task to workflow"""
        self.tasks[task.id] = task
        if self.graph is None:
            self.graph = nx.DiGraph()
        self.graph.add_node(task.id, task=task)
        
        # Add dependency edges
        for dep_id in task.dependencies:
            if dep_id in self.tasks:
                self.graph.add_edge(dep_id, task.id)
    
    def get_execution_order(self) -> List[str]:
        """Get topological order for task execution"""
        if self.graph is None or len(self.graph) == 0:
            return list(self.tasks.keys())
        
        try:
            return list(nx.topological_sort(self.graph))
        except nx.NetworkXError:
            # Cycle detected
            raise ValueError("Circular dependency detected in workflow")
    
    def get_ready_tasks(self, completed_tasks: Set[str]) -> List[str]:
        """Get tasks that are ready to execute"""
        ready = []
        for task_id, task in self.tasks.items():
            if task.status == TaskStatus.PENDING:
                # Check if all dependencies are completed
                if all(dep in completed_tasks for dep in task.dependencies):
                    ready.append(task_id)
        return ready
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert workflow to dictionary"""
        return {
            'id': self.id,
            'name': self.name,
            'description': self.description,
            'status': self.status.value,
            'tasks': {tid: t.to_dict() for tid, t in self.tasks.items()},
            'created_at': self.created_at.isoformat(),
            'started_at': self.started_at.isoformat() if self.started_at else None,
            'completed_at': self.completed_at.isoformat() if self.completed_at else None,
            'metadata': self.metadata
        }

class TaskExecutor:
    """Executes individual tasks"""
    
    def __init__(self, max_workers: int = 4):
        self.thread_pool = ThreadPoolExecutor(max_workers=max_workers)
        self.process_pool = ProcessPoolExecutor(max_workers=max_workers)
        self.running_tasks = {}
    
    async def execute_task(self, task: Task) -> Any:
        """Execute a single task"""
        task.status = TaskStatus.RUNNING
        task.started_at = datetime.now()
        
        try:
            # Set timeout if specified
            if task.timeout:
                result = await asyncio.wait_for(
                    self._run_task_handler(task),
                    timeout=task.timeout
                )
            else:
                result = await self._run_task_handler(task)
            
            task.result = result
            task.status = TaskStatus.COMPLETED
            task.completed_at = datetime.now()
            
            logger.info(f"Task {task.name} completed successfully")
            return result
            
        except asyncio.TimeoutError:
            task.error = f"Task timed out after {task.timeout} seconds"
            task.status = TaskStatus.FAILED
            task.completed_at = datetime.now()
            logger.error(f"Task {task.name} timed out")
            raise
            
        except Exception as e:
            task.error = str(e)
            task.status = TaskStatus.FAILED
            task.completed_at = datetime.now()
            logger.error(f"Task {task.name} failed: {e}")
            raise
    
    async def _run_task_handler(self, task: Task) -> Any:
        """Run the task handler function"""
        if task.handler is None:
            # Default handler - just return params
            return task.params
        
        # Check if handler is async
        if inspect.iscoroutinefunction(task.handler):
            return await task.handler(**task.params)
        else:
            # Run sync function in thread pool
            loop = asyncio.get_event_loop()
            return await loop.run_in_executor(
                self.thread_pool,
                task.handler,
                **task.params
            )
    
    def shutdown(self):
        """Shutdown executor pools"""
        self.thread_pool.shutdown(wait=True)
        self.process_pool.shutdown(wait=True)

class WorkflowScheduler:
    """Schedules and manages workflow execution"""
    
    def __init__(self, max_concurrent_tasks: int = 10):
        self.max_concurrent_tasks = max_concurrent_tasks
        self.task_queue = []  # Priority queue
        self.running_tasks = {}
        self.completed_tasks = set()
        self.failed_tasks = set()
        self.workflows = {}
        self.executor = TaskExecutor(max_workers=max_concurrent_tasks)
    
    async def schedule_workflow(self, workflow: Workflow) -> str:
        """Schedule a workflow for execution"""
        workflow.status = TaskStatus.QUEUED
        self.workflows[workflow.id] = workflow
        
        # Get initial ready tasks
        ready_tasks = workflow.get_ready_tasks(self.completed_tasks)
        
        for task_id in ready_tasks:
            task = workflow.tasks[task_id]
            heapq.heappush(self.task_queue, (task.priority.value, task_id, workflow.id))
        
        logger.info(f"Scheduled workflow {workflow.name} with {len(workflow.tasks)} tasks")
        return workflow.id
    
    async def execute_workflows(self):
        """Main execution loop for workflows"""
        while self.task_queue or self.running_tasks:
            # Start new tasks if capacity available
            while self.task_queue and len(self.running_tasks) < self.max_concurrent_tasks:
                _, task_id, workflow_id = heapq.heappop(self.task_queue)
                
                if workflow_id not in self.workflows:
                    continue
                
                workflow = self.workflows[workflow_id]
                if task_id not in workflow.tasks:
                    continue
                
                task = workflow.tasks[task_id]
                
                # Start task execution
                asyncio.create_task(self._execute_task(task, workflow))
                self.running_tasks[task_id] = task
            
            # Wait a bit before checking again
            await asyncio.sleep(0.1)
    
    async def _execute_task(self, task: Task, workflow: Workflow):
        """Execute a single task and handle completion"""
        try:
            # Update workflow status if first task
            if workflow.status == TaskStatus.QUEUED:
                workflow.status = TaskStatus.RUNNING
                workflow.started_at = datetime.now()
            
            # Execute task
            await self.executor.execute_task(task)
            
            # Mark as completed
            self.completed_tasks.add(task.id)
            
            # Find newly ready tasks
            ready_tasks = workflow.get_ready_tasks(self.completed_tasks)
            for new_task_id in ready_tasks:
                if new_task_id not in self.running_tasks and new_task_id not in self.completed_tasks:
                    new_task = workflow.tasks[new_task_id]
                    heapq.heappush(self.task_queue, (new_task.priority.value, new_task_id, workflow.id))
            
            # Check if workflow is complete
            if all(t.status in [TaskStatus.COMPLETED, TaskStatus.SKIPPED] 
                   for t in workflow.tasks.values()):
                workflow.status = TaskStatus.COMPLETED
                workflow.completed_at = datetime.now()
                logger.info(f"Workflow {workflow.name} completed")
                
        except Exception as e:
            # Handle task failure
            self.failed_tasks.add(task.id)
            
            # Retry if possible
            if task.retry_count < task.max_retries:
                task.retry_count += 1
                task.status = TaskStatus.RETRYING
                logger.info(f"Retrying task {task.name} (attempt {task.retry_count})")
                
                # Re-queue task
                heapq.heappush(self.task_queue, (task.priority.value, task.id, workflow.id))
            else:
                # Mark dependent tasks as skipped
                self._skip_dependent_tasks(task.id, workflow)
                
                # Check if workflow failed
                if any(t.status == TaskStatus.FAILED for t in workflow.tasks.values()):
                    workflow.status = TaskStatus.FAILED
                    workflow.completed_at = datetime.now()
                    logger.error(f"Workflow {workflow.name} failed")
        
        finally:
            # Remove from running tasks
            if task.id in self.running_tasks:
                del self.running_tasks[task.id]
    
    def _skip_dependent_tasks(self, failed_task_id: str, workflow: Workflow):
        """Skip tasks that depend on a failed task"""
        for task_id, task in workflow.tasks.items():
            if failed_task_id in task.dependencies and task.status == TaskStatus.PENDING:
                task.status = TaskStatus.SKIPPED
                logger.info(f"Skipping task {task.name} due to failed dependency")

class OrchestratorAgent:
    """Main orchestrator agent for workflow management"""
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        self.config = config or {}
        self.scheduler = WorkflowScheduler(
            max_concurrent_tasks=self.config.get('max_concurrent_tasks', 10)
        )
        self.workflows = {}
        self.task_handlers = {}
        self.hooks = defaultdict(list)
        self.metrics = {
            'total_workflows': 0,
            'completed_workflows': 0,
            'failed_workflows': 0,
            'total_tasks': 0,
            'completed_tasks': 0,
            'failed_tasks': 0,
            'average_task_duration': 0
        }
        self.running = False
    
    def register_task_handler(self, name: str, handler: Callable):
        """Register a task handler function"""
        self.task_handlers[name] = handler
        logger.info(f"Registered task handler: {name}")
    
    def register_hook(self, event: str, callback: Callable):
        """Register event hook"""
        self.hooks[event].append(callback)
    
    async def trigger_hooks(self, event: str, data: Any = None):
        """Trigger event hooks"""
        for callback in self.hooks[event]:
            try:
                if inspect.iscoroutinefunction(callback):
                    await callback(data)
                else:
                    callback(data)
            except Exception as e:
                logger.error(f"Hook error for event {event}: {e}")
    
    def create_workflow(self, name: str, description: str = "") -> Workflow:
        """Create a new workflow"""
        workflow = Workflow(name=name, description=description)
        self.workflows[workflow.id] = workflow
        self.metrics['total_workflows'] += 1
        return workflow
    
    def create_task(self, 
                   name: str,
                   handler: Optional[Union[str, Callable]] = None,
                   params: Optional[Dict[str, Any]] = None,
                   dependencies: Optional[List[str]] = None,
                   priority: TaskPriority = TaskPriority.NORMAL,
                   timeout: Optional[int] = None,
                   max_retries: int = 3) -> Task:
        """Create a new task"""
        # Resolve handler
        if isinstance(handler, str):
            handler = self.task_handlers.get(handler)
        
        task = Task(
            name=name,
            handler=handler,
            params=params or {},
            dependencies=dependencies or [],
            priority=priority,
            timeout=timeout,
            max_retries=max_retries
        )
        
        self.metrics['total_tasks'] += 1
        return task
    
    async def execute_workflow(self, workflow: Union[str, Workflow]) -> Dict[str, Any]:
        """Execute a workflow"""
        if isinstance(workflow, str):
            workflow = self.workflows.get(workflow)
            if not workflow:
                raise ValueError(f"Workflow {workflow} not found")
        
        # Trigger pre-execution hook
        await self.trigger_hooks('workflow_start', workflow)
        
        # Schedule workflow
        workflow_id = await self.scheduler.schedule_workflow(workflow)
        
        # Execute
        await self.scheduler.execute_workflows()
        
        # Update metrics
        if workflow.status == TaskStatus.COMPLETED:
            self.metrics['completed_workflows'] += 1
        elif workflow.status == TaskStatus.FAILED:
            self.metrics['failed_workflows'] += 1
        
        # Count task completions
        for task in workflow.tasks.values():
            if task.status == TaskStatus.COMPLETED:
                self.metrics['completed_tasks'] += 1
            elif task.status == TaskStatus.FAILED:
                self.metrics['failed_tasks'] += 1
        
        # Trigger post-execution hook
        await self.trigger_hooks('workflow_complete', workflow)
        
        return workflow.to_dict()
    
    def build_workflow_from_config(self, config: Dict[str, Any]) -> Workflow:
        """Build workflow from configuration dictionary"""
        workflow = self.create_workflow(
            name=config.get('name', 'Unnamed Workflow'),
            description=config.get('description', '')
        )
        
        # Create tasks
        task_map = {}
        for task_config in config.get('tasks', []):
            task = self.create_task(
                name=task_config['name'],
                handler=task_config.get('handler'),
                params=task_config.get('params', {}),
                dependencies=[task_map[dep] for dep in task_config.get('dependencies', [])],
                priority=TaskPriority[task_config.get('priority', 'NORMAL')],
                timeout=task_config.get('timeout'),
                max_retries=task_config.get('max_retries', 3)
            )
            task_map[task_config['name']] = task.id
            workflow.add_task(task)
        
        return workflow
    
    async def start(self):
        """Start the orchestrator agent"""
        self.running = True
        logger.info("Orchestrator Agent started")
        
        # Start background monitoring
        asyncio.create_task(self._monitor_workflows())
    
    async def stop(self):
        """Stop the orchestrator agent"""
        self.running = False
        self.scheduler.executor.shutdown()
        logger.info("Orchestrator Agent stopped")
    
    async def _monitor_workflows(self):
        """Monitor workflow execution and collect metrics"""
        while self.running:
            # Update metrics
            active_workflows = sum(1 for w in self.workflows.values() 
                                 if w.status == TaskStatus.RUNNING)
            
            # Log status
            logger.debug(f"Active workflows: {active_workflows}, "
                        f"Completed: {self.metrics['completed_workflows']}, "
                        f"Failed: {self.metrics['failed_workflows']}")
            
            # Trigger monitoring hook
            await self.trigger_hooks('monitor', self.metrics)
            
            await asyncio.sleep(10)  # Monitor every 10 seconds
    
    def get_metrics(self) -> Dict[str, Any]:
        """Get orchestrator metrics"""
        return self.metrics.copy()
    
    def get_workflow_status(self, workflow_id: str) -> Optional[Dict[str, Any]]:
        """Get workflow status"""
        if workflow_id in self.workflows:
            return self.workflows[workflow_id].to_dict()
        return None

class WorkflowBuilder:
    """Fluent API for building workflows"""
    
    def __init__(self, orchestrator: OrchestratorAgent):
        self.orchestrator = orchestrator
        self.workflow = None
        self.current_task = None
        self.task_map = {}
    
    def workflow(self, name: str, description: str = "") -> 'WorkflowBuilder':
        """Start building a workflow"""
        self.workflow = self.orchestrator.create_workflow(name, description)
        return self
    
    def task(self, name: str, handler: Optional[Union[str, Callable]] = None) -> 'WorkflowBuilder':
        """Add a task to the workflow"""
        if not self.workflow:
            raise ValueError("Must create workflow first")
        
        self.current_task = self.orchestrator.create_task(name, handler)
        self.task_map[name] = self.current_task.id
        return self
    
    def params(self, **kwargs) -> 'WorkflowBuilder':
        """Set parameters for current task"""
        if not self.current_task:
            raise ValueError("No current task")
        
        self.current_task.params = kwargs
        return self
    
    def depends_on(self, *task_names) -> 'WorkflowBuilder':
        """Set dependencies for current task"""
        if not self.current_task:
            raise ValueError("No current task")
        
        self.current_task.dependencies = [self.task_map[name] for name in task_names]
        return self
    
    def priority(self, priority: TaskPriority) -> 'WorkflowBuilder':
        """Set priority for current task"""
        if not self.current_task:
            raise ValueError("No current task")
        
        self.current_task.priority = priority
        return self
    
    def timeout(self, seconds: int) -> 'WorkflowBuilder':
        """Set timeout for current task"""
        if not self.current_task:
            raise ValueError("No current task")
        
        self.current_task.timeout = seconds
        return self
    
    def add(self) -> 'WorkflowBuilder':
        """Add current task to workflow"""
        if not self.current_task or not self.workflow:
            raise ValueError("No current task or workflow")
        
        self.workflow.add_task(self.current_task)
        self.current_task = None
        return self
    
    def build(self) -> Workflow:
        """Build and return the workflow"""
        if self.current_task:
            self.add()
        return self.workflow

# Built-in task handlers
async def parallel_execute(tasks: List[Callable], max_workers: int = 4) -> List[Any]:
    """Execute tasks in parallel"""
    results = await asyncio.gather(*[task() for task in tasks])
    return results

async def sequential_execute(tasks: List[Callable]) -> List[Any]:
    """Execute tasks sequentially"""
    results = []
    for task in tasks:
        result = await task() if inspect.iscoroutinefunction(task) else task()
        results.append(result)
    return results

async def conditional_execute(condition: Callable, true_task: Callable, false_task: Callable) -> Any:
    """Execute task based on condition"""
    if condition():
        return await true_task() if inspect.iscoroutinefunction(true_task) else true_task()
    else:
        return await false_task() if inspect.iscoroutinefunction(false_task) else false_task()

# Global orchestrator instance
_orchestrator = None

def get_orchestrator() -> OrchestratorAgent:
    """Get or create orchestrator instance"""
    global _orchestrator
    if _orchestrator is None:
        _orchestrator = OrchestratorAgent()
    return _orchestrator

if __name__ == "__main__":
    # Example usage
    async def main():
        # Create orchestrator
        orchestrator = get_orchestrator()
        await orchestrator.start()
        
        # Register task handlers
        async def fetch_data(url: str):
            # Simulate data fetching
            await asyncio.sleep(1)
            return f"Data from {url}"
        
        def process_data(data: str):
            # Simulate data processing
            return f"Processed: {data}"
        
        async def save_data(data: str, path: str):
            # Simulate saving data
            await asyncio.sleep(0.5)
            return f"Saved to {path}"
        
        orchestrator.register_task_handler('fetch', fetch_data)
        orchestrator.register_task_handler('process', process_data)
        orchestrator.register_task_handler('save', save_data)
        
        # Build workflow using fluent API
        builder = WorkflowBuilder(orchestrator)
        workflow = (builder
            .workflow("Data Pipeline", "Fetch, process and save data")
            .task("fetch_task", "fetch")
                .params(url="https://api.example.com/data")
                .priority(TaskPriority.HIGH)
                .timeout(30)
                .add()
            .task("process_task", "process")
                .params(data="{{fetch_task.result}}")  # Reference previous result
                .depends_on("fetch_task")
                .add()
            .task("save_task", "save")
                .params(data="{{process_task.result}}", path="/tmp/output.json")
                .depends_on("process_task")
                .add()
            .build()
        )
        
        # Execute workflow
        result = await orchestrator.execute_workflow(workflow)
        print(f"Workflow result: {json.dumps(result, indent=2)}")
        
        # Get metrics
        metrics = orchestrator.get_metrics()
        print(f"Metrics: {json.dumps(metrics, indent=2)}")
        
        await orchestrator.stop()
    
    asyncio.run(main())