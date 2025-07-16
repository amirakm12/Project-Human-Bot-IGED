import React from 'react';
import { motion } from 'framer-motion';
import './Header.css';

const Header: React.FC = () => {
  return (
    <motion.header 
      className="header"
      initial={{ y: -60 }}
      animate={{ y: 0 }}
      transition={{ duration: 0.5, ease: "easeOut" }}
    >
      <div className="header-left">
        <h2 className="page-title">Dashboard</h2>
        <p className="page-subtitle">Welcome back to your AI dominion</p>
      </div>

      <div className="header-right">
        <div className="header-controls">
          <button className="control-btn" title="Voice Control">
            🎤
          </button>
          <button className="control-btn" title="Notifications">
            🔔
          </button>
          <button className="control-btn" title="Fullscreen">
            ⛶
          </button>
        </div>

        <div className="user-info">
          <div className="user-avatar">
            <span>👤</span>
          </div>
          <div className="user-details">
            <span className="user-name">Admin</span>
            <span className="user-role">AI Controller</span>
          </div>
        </div>
      </div>
    </motion.header>
  );
};

export default Header;