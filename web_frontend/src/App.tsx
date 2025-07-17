import React, { useEffect } from 'react';
import { motion } from 'framer-motion';
import AppLayout from './components/layout/AppLayout';
import DashboardScreen from './components/screens/DashboardScreen';
import ChatScreen from './components/screens/ChatScreen';
import AnalyticsScreen from './components/screens/AnalyticsScreen';
import SettingsScreen from './components/screens/SettingsScreen';
import ErrorBoundary from './components/ErrorBoundary';
import { useUI, useUIActions, useAuthActions } from './store';
import { useSocket } from './hooks/useSocket';
import apiService from './services/api';
import './App.css';

const App: React.FC = () => {
  const { activeScreen, loading, error, notifications } = useUI();
  const { setError, setLoading, addNotification, removeNotification } = useUIActions();
  const { setAuthToken, setUser } = useAuthActions();
  const { isConnected, onError } = useSocket();

  // Initialize app
  useEffect(() => {
    const initializeApp = async () => {
      try {
        setLoading(true);
        
        // Check if user is already authenticated
        const token = localStorage.getItem('auth_token');
        if (token) {
          setAuthToken(token);
          
          // Verify token with backend
          const isValid = await apiService.verifyToken();
          if (!isValid) {
            localStorage.removeItem('auth_token');
            setAuthToken(null);
            addNotification({
              type: 'warning',
              message: 'Session expired. Please log in again.',
            });
          }
        }
        
        // Check backend health
        const isHealthy = await apiService.healthCheck();
        if (!isHealthy) {
          addNotification({
            type: 'error',
            message: 'Backend server is not responding',
          });
        }
        
      } catch (error) {
        console.error('App initialization error:', error);
        setError('Failed to initialize application');
      } finally {
        setLoading(false);
      }
    };

    initializeApp();
  }, [setAuthToken, setUser, setError, setLoading, addNotification]);

  // Handle socket errors
  useEffect(() => {
    const cleanup = onError((errorMessage: string) => {
      addNotification({
        type: 'error',
        message: `Connection error: ${errorMessage}`,
      });
    });

    return cleanup;
  }, [onError, addNotification]);

  // Auto-remove notifications after 5 seconds
  useEffect(() => {
    notifications.forEach((notification) => {
      if (Date.now() - notification.timestamp > 5000) {
        removeNotification(notification.id);
      }
    });
  }, [notifications, removeNotification]);

  // Render current screen
  const renderScreen = () => {
    switch (activeScreen) {
      case 'dashboard':
        return <DashboardScreen />;
      case 'chat':
        return <ChatScreen />;
      case 'analytics':
        return <AnalyticsScreen />;
      case 'settings':
        return <SettingsScreen />;
      default:
        return <DashboardScreen />;
    }
  };

  // Handle global error boundary errors
  const handleGlobalError = (error: Error, errorInfo: React.ErrorInfo) => {
    console.error('Global error caught:', error, errorInfo);
    addNotification({
      type: 'error',
      message: 'An unexpected error occurred. Please refresh the page.',
    });
  };

  // Loading screen
  if (loading) {
    return (
      <div className="app-loading">
        <div className="loading-container">
          <div className="loading-spinner"></div>
          <h2>Initializing IGED...</h2>
          <p>Setting up your AI assistant</p>
        </div>
      </div>
    );
  }

  return (
    <ErrorBoundary onError={handleGlobalError}>
      <div className="app">
        {/* Global Error Display */}
        {error && (
          <motion.div
            className="global-error"
            initial={{ opacity: 0, y: -50 }}
            animate={{ opacity: 1, y: 0 }}
            exit={{ opacity: 0, y: -50 }}
          >
            <div className="error-content">
              <span className="error-icon">⚠️</span>
              <span className="error-message">{error}</span>
              <button 
                className="error-dismiss"
                onClick={() => setError(null)}
              >
                ×
              </button>
            </div>
          </motion.div>
        )}

        {/* Connection Status Indicator */}
        {!isConnected && (
          <motion.div
            className="connection-warning"
            initial={{ opacity: 0, y: -30 }}
            animate={{ opacity: 1, y: 0 }}
          >
            <div className="warning-content">
              <span className="warning-icon">🔌</span>
              <span>Backend disconnected - Some features may not work</span>
            </div>
          </motion.div>
        )}

        {/* Notifications */}
        <div className="notifications-container">
          {notifications.map((notification) => (
            <motion.div
              key={notification.id}
              className={`notification ${notification.type}`}
              initial={{ opacity: 0, x: 300 }}
              animate={{ opacity: 1, x: 0 }}
              exit={{ opacity: 0, x: 300 }}
              transition={{ duration: 0.3 }}
            >
              <div className="notification-content">
                <span className="notification-icon">
                  {notification.type === 'success' && '✅'}
                  {notification.type === 'error' && '❌'}
                  {notification.type === 'warning' && '⚠️'}
                  {notification.type === 'info' && 'ℹ️'}
                </span>
                <span className="notification-message">{notification.message}</span>
                <button
                  className="notification-close"
                  onClick={() => removeNotification(notification.id)}
                >
                  ×
                </button>
              </div>
            </motion.div>
          ))}
        </div>

        {/* Main App Layout */}
        <AppLayout>
          <ErrorBoundary>
            <motion.div
              key={activeScreen}
              className="screen-container"
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              exit={{ opacity: 0, y: -20 }}
              transition={{ duration: 0.3 }}
            >
              {renderScreen()}
            </motion.div>
          </ErrorBoundary>
        </AppLayout>
      </div>
    </ErrorBoundary>
  );
};

export default App;