import { useState, useEffect, useCallback, useRef } from 'react';
import socketService from '../services/socket';
import { CommandResponse } from '../services/api';

export interface UseSocketReturn {
  // Connection state
  isConnected: boolean;
  connectionState: 'connected' | 'disconnected' | 'connecting' | 'reconnecting';
  error: string | null;
  
  // Actions
  connect: () => void;
  disconnect: () => void;
  reconnect: () => void;
  sendCommand: (command: string) => void;
  sendVoiceStream: (audioData: ArrayBuffer) => void;
  authenticate: (token: string) => void;
  
  // Event listeners
  onCommandResponse: (callback: (response: CommandResponse) => void) => () => void;
  onVoiceTranscription: (callback: (transcription: string) => void) => () => void;
  onSystemStatusUpdate: (callback: (status: any) => void) => () => void;
  onAgentStatusUpdate: (callback: (agentId: string, status: string) => void) => () => void;
  onError: (callback: (error: string) => void) => () => void;
}

export const useSocket = (): UseSocketReturn => {
  const [isConnected, setIsConnected] = useState(false);
  const [connectionState, setConnectionState] = useState<'connected' | 'disconnected' | 'connecting' | 'reconnecting'>('disconnected');
  const [error, setError] = useState<string | null>(null);
  
  const listenersRef = useRef<Map<string, Function[]>>(new Map());

  // Initialize socket connection and event listeners
  useEffect(() => {
    // Set up connection state listeners
    const handleConnected = () => {
      setIsConnected(true);
      setConnectionState('connected');
      setError(null);
    };

    const handleDisconnected = () => {
      setIsConnected(false);
      setConnectionState('disconnected');
    };

    const handleReconnected = () => {
      setIsConnected(true);
      setConnectionState('connected');
      setError(null);
    };

    const handleError = (errorMessage: string) => {
      setError(errorMessage);
    };

    // Register socket event listeners
    socketService.on('connected', handleConnected);
    socketService.on('disconnected', handleDisconnected);
    socketService.on('reconnected', handleReconnected);
    socketService.on('error', handleError);

    // Set initial connection state
    setIsConnected(socketService.isConnected());
    setConnectionState(socketService.getConnectionState());

    // Cleanup on unmount
    return () => {
      socketService.off('connected', handleConnected);
      socketService.off('disconnected', handleDisconnected);
      socketService.off('reconnected', handleReconnected);
      socketService.off('error', handleError);
      
      // Remove all custom listeners
      listenersRef.current.clear();
    };
  }, []);

  // Update connection state periodically
  useEffect(() => {
    const interval = setInterval(() => {
      setIsConnected(socketService.isConnected());
      setConnectionState(socketService.getConnectionState());
    }, 1000);

    return () => clearInterval(interval);
  }, []);

  // Helper function to add event listener
  const addEventListener = useCallback((event: string, callback: Function): (() => void) => {
    // Add to our local listeners map
    if (!listenersRef.current.has(event)) {
      listenersRef.current.set(event, []);
    }
    listenersRef.current.get(event)!.push(callback);

    // Add to socket service
    socketService.on(event, callback);

    // Return cleanup function
    return () => {
      const listeners = listenersRef.current.get(event);
      if (listeners) {
        const index = listeners.indexOf(callback);
        if (index > -1) {
          listeners.splice(index, 1);
        }
      }
      socketService.off(event, callback);
    };
  }, []);

  // Connection actions
  const connect = useCallback(() => {
    socketService.reconnect();
  }, []);

  const disconnect = useCallback(() => {
    socketService.disconnect();
  }, []);

  const reconnect = useCallback(() => {
    socketService.reconnect();
  }, []);

  // Communication actions
  const sendCommand = useCallback((command: string) => {
    socketService.executeCommand(command);
  }, []);

  const sendVoiceStream = useCallback((audioData: ArrayBuffer) => {
    socketService.sendVoiceStream(audioData);
  }, []);

  const authenticate = useCallback((token: string) => {
    socketService.authenticate(token);
  }, []);

  // Event listener hooks
  const onCommandResponse = useCallback((callback: (response: CommandResponse) => void) => {
    return addEventListener('command_response', callback);
  }, [addEventListener]);

  const onVoiceTranscription = useCallback((callback: (transcription: string) => void) => {
    return addEventListener('voice_transcription', callback);
  }, [addEventListener]);

  const onSystemStatusUpdate = useCallback((callback: (status: any) => void) => {
    return addEventListener('system_status_update', callback);
  }, [addEventListener]);

  const onAgentStatusUpdate = useCallback((callback: (agentId: string, status: string) => void) => {
    return addEventListener('agent_status_update', callback);
  }, [addEventListener]);

  const onError = useCallback((callback: (error: string) => void) => {
    return addEventListener('error', callback);
  }, [addEventListener]);

  return {
    // Connection state
    isConnected,
    connectionState,
    error,
    
    // Actions
    connect,
    disconnect,
    reconnect,
    sendCommand,
    sendVoiceStream,
    authenticate,
    
    // Event listeners
    onCommandResponse,
    onVoiceTranscription,
    onSystemStatusUpdate,
    onAgentStatusUpdate,
    onError,
  };
};