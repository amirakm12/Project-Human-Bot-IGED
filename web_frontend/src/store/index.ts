import { create } from 'zustand';
import { devtools, persist } from 'zustand/middleware';
import { CommandResponse, SystemStatus, AnalyticsData } from '../services/api';

// Types
export interface User {
  id: string;
  username: string;
  email: string;
}

export interface Command {
  id: string;
  command: string;
  response: string;
  timestamp: number;
  execution_time: number;
  status: 'success' | 'error' | 'pending';
  type: 'text' | 'voice';
}

export interface AppSettings {
  theme: 'dark' | 'light';
  voiceEnabled: boolean;
  autoExecute: boolean;
  notifications: boolean;
  apiUrl: string;
  socketUrl: string;
  voiceSensitivity: number;
  language: string;
}

export interface UIState {
  activeScreen: 'dashboard' | 'chat' | 'analytics' | 'settings';
  sidebarOpen: boolean;
  loading: boolean;
  error: string | null;
  notifications: Array<{
    id: string;
    type: 'info' | 'success' | 'warning' | 'error';
    message: string;
    timestamp: number;
  }>;
}

export interface AppState {
  // User & Auth
  user: User | null;
  isAuthenticated: boolean;
  authToken: string | null;
  
  // Commands & Chat
  commands: Command[];
  currentCommand: string;
  isProcessingCommand: boolean;
  
  // System Status
  systemStatus: SystemStatus | null;
  connectionStatus: 'connected' | 'disconnected' | 'connecting' | 'reconnecting';
  
  // Analytics
  analytics: AnalyticsData | null;
  
  // Settings
  settings: AppSettings;
  
  // UI State
  ui: UIState;
  
  // Actions
  setUser: (user: User | null) => void;
  setAuthToken: (token: string | null) => void;
  login: (user: User, token: string) => void;
  logout: () => void;
  
  // Command actions
  addCommand: (command: Command) => void;
  updateCommand: (id: string, updates: Partial<Command>) => void;
  removeCommand: (id: string) => void;
  clearCommands: () => void;
  setCurrentCommand: (command: string) => void;
  setProcessingCommand: (processing: boolean) => void;
  
  // System actions
  setSystemStatus: (status: SystemStatus) => void;
  setConnectionStatus: (status: 'connected' | 'disconnected' | 'connecting' | 'reconnecting') => void;
  
  // Analytics actions
  setAnalytics: (analytics: AnalyticsData) => void;
  
  // Settings actions
  updateSettings: (settings: Partial<AppSettings>) => void;
  resetSettings: () => void;
  
  // UI actions
  setActiveScreen: (screen: 'dashboard' | 'chat' | 'analytics' | 'settings') => void;
  setSidebarOpen: (open: boolean) => void;
  setLoading: (loading: boolean) => void;
  setError: (error: string | null) => void;
  addNotification: (notification: { type: 'info' | 'success' | 'warning' | 'error'; message: string }) => void;
  removeNotification: (id: string) => void;
  clearNotifications: () => void;
}

// Default settings
const defaultSettings: AppSettings = {
  theme: 'dark',
  voiceEnabled: true,
  autoExecute: false,
  notifications: true,
  apiUrl: 'http://localhost:5000',
  socketUrl: 'http://localhost:5000',
  voiceSensitivity: 0.5,
  language: 'en',
};

// Default UI state
const defaultUIState: UIState = {
  activeScreen: 'dashboard',
  sidebarOpen: true,
  loading: false,
  error: null,
  notifications: [],
};

export const useAppStore = create<AppState>()(
  devtools(
    persist(
      (set, get) => ({
        // Initial state
        user: null,
        isAuthenticated: false,
        authToken: null,
        commands: [],
        currentCommand: '',
        isProcessingCommand: false,
        systemStatus: null,
        connectionStatus: 'disconnected',
        analytics: null,
        settings: defaultSettings,
        ui: defaultUIState,

        // User & Auth actions
        setUser: (user) => set({ user, isAuthenticated: !!user }),
        
        setAuthToken: (authToken) => set({ authToken, isAuthenticated: !!authToken }),
        
        login: (user, token) => set({ 
          user, 
          authToken: token, 
          isAuthenticated: true,
          ui: { ...get().ui, error: null }
        }),
        
        logout: () => set({ 
          user: null, 
          authToken: null, 
          isAuthenticated: false,
          commands: [],
          systemStatus: null,
          analytics: null,
        }),

        // Command actions
        addCommand: (command) => set((state) => ({
          commands: [command, ...state.commands].slice(0, 1000) // Keep last 1000 commands
        })),
        
        updateCommand: (id, updates) => set((state) => ({
          commands: state.commands.map(cmd => 
            cmd.id === id ? { ...cmd, ...updates } : cmd
          )
        })),
        
        removeCommand: (id) => set((state) => ({
          commands: state.commands.filter(cmd => cmd.id !== id)
        })),
        
        clearCommands: () => set({ commands: [] }),
        
        setCurrentCommand: (currentCommand) => set({ currentCommand }),
        
        setProcessingCommand: (isProcessingCommand) => set({ isProcessingCommand }),

        // System actions
        setSystemStatus: (systemStatus) => set({ systemStatus }),
        
        setConnectionStatus: (connectionStatus) => set({ connectionStatus }),

        // Analytics actions
        setAnalytics: (analytics) => set({ analytics }),

        // Settings actions
        updateSettings: (newSettings) => set((state) => ({
          settings: { ...state.settings, ...newSettings }
        })),
        
        resetSettings: () => set({ settings: defaultSettings }),

        // UI actions
        setActiveScreen: (activeScreen) => set((state) => ({
          ui: { ...state.ui, activeScreen }
        })),
        
        setSidebarOpen: (sidebarOpen) => set((state) => ({
          ui: { ...state.ui, sidebarOpen }
        })),
        
        setLoading: (loading) => set((state) => ({
          ui: { ...state.ui, loading }
        })),
        
        setError: (error) => set((state) => ({
          ui: { ...state.ui, error }
        })),
        
        addNotification: (notification) => set((state) => ({
          ui: {
            ...state.ui,
            notifications: [
              {
                ...notification,
                id: Date.now().toString(),
                timestamp: Date.now(),
              },
              ...state.ui.notifications
            ].slice(0, 10) // Keep last 10 notifications
          }
        })),
        
        removeNotification: (id) => set((state) => ({
          ui: {
            ...state.ui,
            notifications: state.ui.notifications.filter(n => n.id !== id)
          }
        })),
        
        clearNotifications: () => set((state) => ({
          ui: { ...state.ui, notifications: [] }
        })),
      }),
      {
        name: 'iged-app-store',
        partialize: (state) => ({
          // Only persist certain parts of the state
          settings: state.settings,
          ui: {
            activeScreen: state.ui.activeScreen,
            sidebarOpen: state.ui.sidebarOpen,
          },
          authToken: state.authToken,
          user: state.user,
          isAuthenticated: state.isAuthenticated,
        }),
      }
    ),
    {
      name: 'iged-app-store',
    }
  )
);

// Selectors for common state access
export const useUser = () => useAppStore((state) => state.user);
export const useIsAuthenticated = () => useAppStore((state) => state.isAuthenticated);
export const useCommands = () => useAppStore((state) => state.commands);
export const useSystemStatus = () => useAppStore((state) => state.systemStatus);
export const useSettings = () => useAppStore((state) => state.settings);
export const useUI = () => useAppStore((state) => state.ui);
export const useConnectionStatus = () => useAppStore((state) => state.connectionStatus);
export const useAnalytics = () => useAppStore((state) => state.analytics);

// Action selectors
export const useAuthActions = () => useAppStore((state) => ({
  login: state.login,
  logout: state.logout,
  setUser: state.setUser,
  setAuthToken: state.setAuthToken,
}));

export const useCommandActions = () => useAppStore((state) => ({
  addCommand: state.addCommand,
  updateCommand: state.updateCommand,
  removeCommand: state.removeCommand,
  clearCommands: state.clearCommands,
  setCurrentCommand: state.setCurrentCommand,
  setProcessingCommand: state.setProcessingCommand,
}));

export const useUIActions = () => useAppStore((state) => ({
  setActiveScreen: state.setActiveScreen,
  setSidebarOpen: state.setSidebarOpen,
  setLoading: state.setLoading,
  setError: state.setError,
  addNotification: state.addNotification,
  removeNotification: state.removeNotification,
  clearNotifications: state.clearNotifications,
}));

export const useSystemActions = () => useAppStore((state) => ({
  setSystemStatus: state.setSystemStatus,
  setConnectionStatus: state.setConnectionStatus,
  setAnalytics: state.setAnalytics,
}));