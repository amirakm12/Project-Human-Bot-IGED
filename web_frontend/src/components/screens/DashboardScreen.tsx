import React, { useEffect, useState } from 'react';
import { motion } from 'framer-motion';
import AIAvatar from '../three/AIAvatar';
import { VoiceWaveform } from '../ui/VoiceWaveform';
import { useSocket } from '../../hooks/useSocket';
import { useSystemStatus, useCommands, useSystemActions, useUIActions } from '../../store';
import apiService from '../../services/api';
import './DashboardScreen.css';

const DashboardScreen: React.FC = () => {
  const [systemMetrics, setSystemMetrics] = useState({
    cpu: 0,
    memory: 0,
    disk: 0,
    uptime: 0,
  });
  
  const systemStatus = useSystemStatus();
  const commands = useCommands();
  const { setSystemStatus, setConnectionStatus } = useSystemActions();
  const { addNotification } = useUIActions();
  const { isConnected, connectionState } = useSocket();

  // Fetch system status on mount and periodically
  useEffect(() => {
    const fetchSystemStatus = async () => {
      try {
        const response = await apiService.getSystemStatus();
        if (response.success && response.data) {
          setSystemStatus(response.data);
          setSystemMetrics({
            cpu: response.data.cpu_usage,
            memory: response.data.memory_usage,
            disk: response.data.disk_usage,
            uptime: response.data.uptime,
          });
        }
      } catch (error) {
        console.error('Failed to fetch system status:', error);
        addNotification({
          type: 'error',
          message: 'Failed to fetch system status',
        });
      }
    };

    fetchSystemStatus();
    const interval = setInterval(fetchSystemStatus, 5000); // Update every 5 seconds

    return () => clearInterval(interval);
  }, [setSystemStatus, addNotification]);

  // Update connection status
  useEffect(() => {
    setConnectionStatus(connectionState);
  }, [connectionState, setConnectionStatus]);

  // Get recent commands (last 5)
  const recentCommands = commands.slice(0, 5);

  // Format uptime
  const formatUptime = (seconds: number): string => {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    return `${hours}h ${minutes}m`;
  };

  // Get status color
  const getStatusColor = (status: string): string => {
    switch (status) {
      case 'active':
      case 'connected':
        return '#00ff88';
      case 'inactive':
      case 'disconnected':
        return '#ff4444';
      case 'connecting':
      case 'reconnecting':
        return '#ffaa00';
      default:
        return '#666666';
    }
  };

  return (
    <div className="dashboard-screen">
      <motion.div
        className="dashboard-header"
        initial={{ opacity: 0, y: -20 }}
        animate={{ opacity: 1, y: 0 }}
        transition={{ duration: 0.5 }}
      >
        <h1>IGED Dashboard</h1>
        <div className="connection-status">
          <div 
            className={`status-indicator ${connectionState}`}
            style={{ backgroundColor: getStatusColor(connectionState) }}
          />
          <span>Backend: {connectionState}</span>
        </div>
      </motion.div>

      <div className="dashboard-grid">
        {/* System Status Cards */}
        <motion.div
          className="dashboard-card system-metrics"
          initial={{ opacity: 0, scale: 0.9 }}
          animate={{ opacity: 1, scale: 1 }}
          transition={{ duration: 0.5, delay: 0.1 }}
        >
          <h3>System Metrics</h3>
          <div className="metrics-grid">
            <div className="metric-item">
              <div className="metric-label">CPU Usage</div>
              <div className="metric-value">{systemMetrics.cpu.toFixed(1)}%</div>
              <div className="metric-bar">
                <div 
                  className="metric-fill"
                  style={{ width: `${systemMetrics.cpu}%`, backgroundColor: systemMetrics.cpu > 80 ? '#ff4444' : '#00ff88' }}
                />
              </div>
            </div>
            <div className="metric-item">
              <div className="metric-label">Memory</div>
              <div className="metric-value">{systemMetrics.memory.toFixed(1)}%</div>
              <div className="metric-bar">
                <div 
                  className="metric-fill"
                  style={{ width: `${systemMetrics.memory}%`, backgroundColor: systemMetrics.memory > 80 ? '#ff4444' : '#00ff88' }}
                />
              </div>
            </div>
            <div className="metric-item">
              <div className="metric-label">Disk Usage</div>
              <div className="metric-value">{systemMetrics.disk.toFixed(1)}%</div>
              <div className="metric-bar">
                <div 
                  className="metric-fill"
                  style={{ width: `${systemMetrics.disk}%`, backgroundColor: systemMetrics.disk > 90 ? '#ff4444' : '#00ff88' }}
                />
              </div>
            </div>
            <div className="metric-item">
              <div className="metric-label">Uptime</div>
              <div className="metric-value">{formatUptime(systemMetrics.uptime)}</div>
            </div>
          </div>
        </motion.div>

        {/* AI Avatar */}
        <motion.div
          className="dashboard-card avatar-container"
          initial={{ opacity: 0, scale: 0.9 }}
          animate={{ opacity: 1, scale: 1 }}
          transition={{ duration: 0.5, delay: 0.2 }}
        >
          <h3>AI Assistant</h3>
          <AIAvatar />
          <div className="avatar-status">
            <span className="status-label">Status:</span>
            <span className={`status-value ${systemStatus?.voice_pipeline_status || 'inactive'}`}>
              {systemStatus?.voice_pipeline_status || 'Inactive'}
            </span>
          </div>
        </motion.div>

        {/* Voice Interface */}
        <motion.div
          className="dashboard-card voice-interface"
          initial={{ opacity: 0, scale: 0.9 }}
          animate={{ opacity: 1, scale: 1 }}
          transition={{ duration: 0.5, delay: 0.3 }}
        >
          <h3>Voice Interface</h3>
          <VoiceWaveform 
            width={350}
            height={80}
            showControls={true}
            streaming={true}
          />
        </motion.div>

        {/* Agent Status */}
        <motion.div
          className="dashboard-card agent-status"
          initial={{ opacity: 0, scale: 0.9 }}
          animate={{ opacity: 1, scale: 1 }}
          transition={{ duration: 0.5, delay: 0.4 }}
        >
          <h3>Agent Status</h3>
          <div className="agents-grid">
            {systemStatus?.agents_status ? (
              Object.entries(systemStatus.agents_status).map(([agentId, status]) => (
                <div key={agentId} className="agent-item">
                  <div className="agent-name">{agentId}</div>
                  <div className="agent-status-indicator">
                    <div 
                      className={`status-dot ${status}`}
                      style={{ backgroundColor: getStatusColor(status) }}
                    />
                    <span className="status-text">{status}</span>
                  </div>
                </div>
              ))
            ) : (
              <div className="no-agents">No agents available</div>
            )}
          </div>
        </motion.div>

        {/* Recent Commands */}
        <motion.div
          className="dashboard-card recent-commands"
          initial={{ opacity: 0, scale: 0.9 }}
          animate={{ opacity: 1, scale: 1 }}
          transition={{ duration: 0.5, delay: 0.5 }}
        >
          <h3>Recent Commands</h3>
          <div className="commands-list">
            {recentCommands.length > 0 ? (
              recentCommands.map((command) => (
                <div key={command.id} className="command-item">
                  <div className="command-text">{command.command}</div>
                  <div className="command-meta">
                    <span className={`command-status ${command.status}`}>
                      {command.status}
                    </span>
                    <span className="command-time">
                      {new Date(command.timestamp).toLocaleTimeString()}
                    </span>
                  </div>
                </div>
              ))
            ) : (
              <div className="no-commands">No recent commands</div>
            )}
          </div>
        </motion.div>

        {/* Quick Stats */}
        <motion.div
          className="dashboard-card quick-stats"
          initial={{ opacity: 0, scale: 0.9 }}
          animate={{ opacity: 1, scale: 1 }}
          transition={{ duration: 0.5, delay: 0.6 }}
        >
          <h3>Quick Stats</h3>
          <div className="stats-grid">
            <div className="stat-item">
              <div className="stat-value">{commands.length}</div>
              <div className="stat-label">Total Commands</div>
            </div>
            <div className="stat-item">
              <div className="stat-value">
                {commands.filter(cmd => cmd.status === 'success').length}
              </div>
              <div className="stat-label">Successful</div>
            </div>
            <div className="stat-item">
              <div className="stat-value">
                {commands.filter(cmd => cmd.status === 'error').length}
              </div>
              <div className="stat-label">Failed</div>
            </div>
            <div className="stat-item">
              <div className="stat-value">
                {systemStatus?.active_processes || 0}
              </div>
              <div className="stat-label">Active Processes</div>
            </div>
          </div>
        </motion.div>
      </div>
    </div>
  );
};

export default DashboardScreen;