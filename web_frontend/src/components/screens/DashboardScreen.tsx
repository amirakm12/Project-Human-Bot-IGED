import React from 'react';
import { motion } from 'framer-motion';
import AIAvatar from '../three/AIAvatar';
import VoiceWaveform from '../ui/VoiceWaveform';
import './DashboardScreen.css';

const DashboardScreen: React.FC = () => {
  const stats = [
    { label: 'Processing Power', value: '99.9%', icon: '⚡' },
    { label: 'Memory Usage', value: '2.4TB', icon: '🧠' },
    { label: 'Active Agents', value: '12', icon: '🤖' },
    { label: 'Quantum Cores', value: '8', icon: '🔮' },
  ];

  return (
    <motion.div 
      className="dashboard-screen"
      initial={{ opacity: 0, y: 20 }}
      animate={{ opacity: 1, y: 0 }}
      transition={{ duration: 0.6 }}
    >
      <div className="dashboard-header">
        <h1>Welcome to IGED</h1>
        <p>Your AI Dominion Awaits</p>
      </div>
      
      <div className="dashboard-grid">
        <motion.div 
          className="ai-avatar-section"
          initial={{ scale: 0.8, opacity: 0 }}
          animate={{ scale: 1, opacity: 1 }}
          transition={{ delay: 0.2, duration: 0.6 }}
        >
          <AIAvatar />
        </motion.div>
        
        <motion.div 
          className="voice-section"
          initial={{ x: 50, opacity: 0 }}
          animate={{ x: 0, opacity: 1 }}
          transition={{ delay: 0.4, duration: 0.6 }}
        >
          <VoiceWaveform />
        </motion.div>
        
        <motion.div 
          className="stats-section"
          initial={{ y: 50, opacity: 0 }}
          animate={{ y: 0, opacity: 1 }}
          transition={{ delay: 0.6, duration: 0.6 }}
        >
          {stats.map((stat, index) => (
            <motion.div 
              key={stat.label}
              className="stat-card"
              initial={{ scale: 0.8, opacity: 0 }}
              animate={{ scale: 1, opacity: 1 }}
              transition={{ delay: 0.8 + index * 0.1, duration: 0.4 }}
            >
              <div className="stat-icon">{stat.icon}</div>
              <div className="stat-content">
                <h3>{stat.label}</h3>
                <div className="stat-value">{stat.value}</div>
              </div>
            </motion.div>
          ))}
        </motion.div>
      </div>

      <motion.div 
        className="system-status"
        initial={{ opacity: 0 }}
        animate={{ opacity: 1 }}
        transition={{ delay: 1.2, duration: 0.6 }}
      >
        <div className="status-item">
          <div className="status-dot online"></div>
          <span>Backend Connected</span>
        </div>
        <div className="status-item">
          <div className="status-dot online"></div>
          <span>Voice Pipeline Active</span>
        </div>
        <div className="status-item">
          <div className="status-dot online"></div>
          <span>Quantum Processing Online</span>
        </div>
      </motion.div>
    </motion.div>
  );
};

export default DashboardScreen;