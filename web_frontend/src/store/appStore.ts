import { create } from 'zustand';

interface AppState {
  isConnected: boolean;
  currentAgent: string;
  voiceActive: boolean;
  currentScreen: string;
  systemStatus: {
    backend: boolean;
    voice: boolean;
    quantum: boolean;
  };
  setConnected: (connected: boolean) => void;
  setCurrentAgent: (agent: string) => void;
  setVoiceActive: (active: boolean) => void;
  setCurrentScreen: (screen: string) => void;
  updateSystemStatus: (status: Partial<AppState['systemStatus']>) => void;
}

export const useAppStore = create<AppState>((set) => ({
  isConnected: false,
  currentAgent: 'orchestrator',
  voiceActive: false,
  currentScreen: 'dashboard',
  systemStatus: {
    backend: false,
    voice: false,
    quantum: false,
  },
  setConnected: (connected) => set({ isConnected: connected }),
  setCurrentAgent: (agent) => set({ currentAgent: agent }),
  setVoiceActive: (active) => set({ voiceActive: active }),
  setCurrentScreen: (screen) => set({ currentScreen: screen }),
  updateSystemStatus: (status) => set((state) => ({
    systemStatus: { ...state.systemStatus, ...status }
  })),
}));