import React from 'react';
import { motion } from 'framer-motion';
import './AnalyticsScreen.css';

const AnalyticsScreen: React.FC = () => {
  const metrics = [
    { label: 'Processing Speed', value: '2.4 GHz', icon: '⚡', color: '#00ff88' },
    { label: 'Memory Usage', value: '78%', icon: '🧠', color: '#00ccff' },
    { label: 'Network Latency', value: '12ms', icon: '🌐', color: '#ff6b6b' },
    { label: 'Quantum Cores', value: '8/8', icon: '🔮', color: '#feca57' },
  ];

  const charts = [
    { title: 'System Performance', data: [85, 92, 78, 95, 88, 90] },
    { title: 'Memory Allocation', data: [45, 62, 38, 55, 48, 52] },
    { title: 'Network Traffic', data: [120, 135, 98, 145, 128, 142] },
  ];

  return (
    <motion.div 
      className="analytics-screen"
      initial={{ opacity: 0 }}
      animate={{ opacity: 1 }}
      transition={{ duration: 0.6 }}
    >
      <div className="analytics-header">
        <h2>System Analytics</h2>
        <p>Real-time performance monitoring and insights</p>
      </div>

      <div className="metrics-grid">
        {metrics.map((metric, index) => (
          <motion.div
            key={metric.label}
            className="metric-card"
            initial={{ scale: 0.8, opacity: 0 }}
            animate={{ scale: 1, opacity: 1 }}
            transition={{ delay: index * 0.1, duration: 0.4 }}
          >
            <div className="metric-icon" style={{ color: metric.color }}>
              {metric.icon}
            </div>
            <div className="metric-content">
              <h3>{metric.label}</h3>
              <div className="metric-value" style={{ color: metric.color }}>
                {metric.value}
              </div>
            </div>
          </motion.div>
        ))}
      </div>

      <div className="charts-section">
        {charts.map((chart, index) => (
          <motion.div
            key={chart.title}
            className="chart-card"
            initial={{ y: 50, opacity: 0 }}
            animate={{ y: 0, opacity: 1 }}
            transition={{ delay: 0.5 + index * 0.2, duration: 0.6 }}
          >
            <h3>{chart.title}</h3>
            <div className="chart-container">
              <div className="chart-bars">
                {chart.data.map((value, i) => (
                  <div
                    key={i}
                    className="chart-bar"
                    style={{ height: `${value}%` }}
                  >
                    <div className="bar-tooltip">{value}</div>
                  </div>
                ))}
              </div>
            </div>
          </motion.div>
        ))}
      </div>

      <motion.div 
        className="system-status-panel"
        initial={{ opacity: 0 }}
        animate={{ opacity: 1 }}
        transition={{ delay: 1.2, duration: 0.6 }}
      >
        <h3>System Status</h3>
        <div className="status-grid">
          <div className="status-item online">
            <div className="status-dot"></div>
            <span>Backend Services</span>
          </div>
          <div className="status-item online">
            <div className="status-dot"></div>
            <span>Database Connection</span>
          </div>
          <div className="status-item online">
            <div className="status-dot"></div>
            <span>AI Processing</span>
          </div>
          <div className="status-item online">
            <div className="status-dot"></div>
            <span>Quantum Interface</span>
          </div>
        </div>
      </motion.div>
    </motion.div>
  );
};

export default AnalyticsScreen;