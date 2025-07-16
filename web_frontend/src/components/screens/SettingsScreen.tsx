import React, { useState } from 'react';
import { motion } from 'framer-motion';
import './SettingsScreen.css';

const SettingsScreen: React.FC = () => {
  const [settings, setSettings] = useState({
    voiceEnabled: true,
    autoRotate: true,
    notifications: true,
    darkMode: true,
    quantumMode: true,
  });

  const handleToggle = (key: keyof typeof settings) => {
    setSettings(prev => ({
      ...prev,
      [key]: !prev[key]
    }));
  };

  const settingsGroups = [
    {
      title: 'Interface Settings',
      settings: [
        { key: 'darkMode', label: 'Dark Mode', description: 'Enable dark theme interface' },
        { key: 'autoRotate', label: 'Auto Rotate', description: 'Automatically rotate 3D elements' },
        { key: 'notifications', label: 'Notifications', description: 'Show system notifications' },
      ]
    },
    {
      title: 'AI Settings',
      settings: [
        { key: 'voiceEnabled', label: 'Voice Interface', description: 'Enable voice commands and responses' },
        { key: 'quantumMode', label: 'Quantum Processing', description: 'Enable quantum computing mode' },
      ]
    }
  ];

  return (
    <motion.div 
      className="settings-screen"
      initial={{ opacity: 0 }}
      animate={{ opacity: 1 }}
      transition={{ duration: 0.6 }}
    >
      <div className="settings-header">
        <h2>System Settings</h2>
        <p>Configure your AI dominion preferences</p>
      </div>

      <div className="settings-content">
        {settingsGroups.map((group, groupIndex) => (
          <motion.div
            key={group.title}
            className="settings-group"
            initial={{ y: 50, opacity: 0 }}
            animate={{ y: 0, opacity: 1 }}
            transition={{ delay: groupIndex * 0.2, duration: 0.6 }}
          >
            <h3>{group.title}</h3>
            
            {group.settings.map((setting, index) => (
              <motion.div
                key={setting.key}
                className="setting-item"
                initial={{ x: -50, opacity: 0 }}
                animate={{ x: 0, opacity: 1 }}
                transition={{ delay: groupIndex * 0.2 + index * 0.1, duration: 0.4 }}
              >
                <div className="setting-info">
                  <h4>{setting.label}</h4>
                  <p>{setting.description}</p>
                </div>
                
                <label className="toggle-switch">
                  <input
                    type="checkbox"
                    checked={settings[setting.key as keyof typeof settings]}
                    onChange={() => handleToggle(setting.key as keyof typeof settings)}
                  />
                  <span className="toggle-slider"></span>
                </label>
              </motion.div>
            ))}
          </motion.div>
        ))}

        <motion.div
          className="system-info"
          initial={{ y: 50, opacity: 0 }}
          animate={{ y: 0, opacity: 1 }}
          transition={{ delay: 0.8, duration: 0.6 }}
        >
          <h3>System Information</h3>
          <div className="info-grid">
            <div className="info-item">
              <span className="info-label">Version:</span>
              <span className="info-value">IGED v2.0.0</span>
            </div>
            <div className="info-item">
              <span className="info-label">Build:</span>
              <span className="info-value">2024.07.16</span>
            </div>
            <div className="info-item">
              <span className="info-label">Architecture:</span>
              <span className="info-value">Quantum-Enhanced</span>
            </div>
            <div className="info-item">
              <span className="info-label">Status:</span>
              <span className="info-value online">Operational</span>
            </div>
          </div>
        </motion.div>

        <motion.div
          className="action-buttons"
          initial={{ y: 50, opacity: 0 }}
          animate={{ y: 0, opacity: 1 }}
          transition={{ delay: 1.0, duration: 0.6 }}
        >
          <button className="btn btn-primary">Save Settings</button>
          <button className="btn btn-secondary">Reset to Default</button>
          <button className="btn btn-danger">Restart System</button>
        </motion.div>
      </div>
    </motion.div>
  );
};

export default SettingsScreen;