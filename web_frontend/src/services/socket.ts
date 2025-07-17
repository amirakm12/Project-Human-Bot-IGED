import { io, Socket } from 'socket.io-client';
import { CommandResponse } from './api';

// Socket Event Types
export interface SocketEvents {
  // Client to Server
  'voice_stream': (audioData: ArrayBuffer) => void;
  'command_execute': (command: string) => void;
  'join_room': (roomId: string) => void;
  'leave_room': (roomId: string) => void;

  // Server to Client
  'command_response': (response: CommandResponse) => void;
  'voice_transcription': (transcription: string) => void;
  'system_status_update': (status: any) => void;
  'agent_status_update': (agentId: string, status: string) => void;
  'error': (error: string) => void;
  'connected': () => void;
  'disconnected': () => void;
  'reconnected': () => void;
}

class SocketService {
  private socket: Socket | null = null;
  private reconnectAttempts = 0;
  private maxReconnectAttempts = 5;
  private reconnectDelay = 1000; // Start with 1 second
  private isConnecting = false;
  private listeners: Map<string, Function[]> = new Map();

  constructor() {
    this.connect();
  }

  private connect(): void {
    if (this.isConnecting || this.socket?.connected) {
      return;
    }

    this.isConnecting = true;
    const socketUrl = process.env.REACT_APP_SOCKET_URL || 'http://localhost:5000';

    this.socket = io(socketUrl, {
      transports: ['websocket', 'polling'],
      autoConnect: true,
      reconnection: true,
      reconnectionAttempts: this.maxReconnectAttempts,
      reconnectionDelay: this.reconnectDelay,
      timeout: 10000,
    });

    this.setupEventHandlers();
    this.isConnecting = false;
  }

  private setupEventHandlers(): void {
    if (!this.socket) return;

    this.socket.on('connect', () => {
      console.log('Socket connected:', this.socket?.id);
      this.reconnectAttempts = 0;
      this.reconnectDelay = 1000;
      this.emit('connected');
    });

    this.socket.on('disconnect', (reason) => {
      console.log('Socket disconnected:', reason);
      this.emit('disconnected');
    });

    this.socket.on('reconnect', () => {
      console.log('Socket reconnected');
      this.emit('reconnected');
    });

    this.socket.on('reconnect_attempt', (attemptNumber) => {
      console.log(`Socket reconnection attempt ${attemptNumber}`);
      this.reconnectAttempts = attemptNumber;
    });

    this.socket.on('reconnect_failed', () => {
      console.log('Socket reconnection failed');
      this.emit('error', 'Failed to reconnect to server');
    });

    this.socket.on('connect_error', (error) => {
      console.error('Socket connection error:', error);
      this.emit('error', `Connection error: ${error.message}`);
    });

    // Application-specific events
    this.socket.on('command_response', (response: CommandResponse) => {
      this.emit('command_response', response);
    });

    this.socket.on('voice_transcription', (transcription: string) => {
      this.emit('voice_transcription', transcription);
    });

    this.socket.on('system_status_update', (status: any) => {
      this.emit('system_status_update', status);
    });

    this.socket.on('agent_status_update', (agentId: string, status: string) => {
      this.emit('agent_status_update', agentId, status);
    });

    this.socket.on('error', (error: string) => {
      this.emit('error', error);
    });
  }

  // Event emitter methods
  private emit(event: string, ...args: any[]): void {
    const eventListeners = this.listeners.get(event);
    if (eventListeners) {
      eventListeners.forEach(listener => {
        try {
          listener(...args);
        } catch (error) {
          console.error(`Error in socket event listener for ${event}:`, error);
        }
      });
    }
  }

  public on(event: string, listener: Function): void {
    if (!this.listeners.has(event)) {
      this.listeners.set(event, []);
    }
    this.listeners.get(event)!.push(listener);
  }

  public off(event: string, listener?: Function): void {
    if (!listener) {
      this.listeners.delete(event);
      return;
    }

    const eventListeners = this.listeners.get(event);
    if (eventListeners) {
      const index = eventListeners.indexOf(listener);
      if (index > -1) {
        eventListeners.splice(index, 1);
      }
    }
  }

  // Socket methods
  public sendVoiceStream(audioData: ArrayBuffer): void {
    if (this.socket?.connected) {
      this.socket.emit('voice_stream', audioData);
    } else {
      console.warn('Socket not connected, cannot send voice stream');
    }
  }

  public executeCommand(command: string): void {
    if (this.socket?.connected) {
      this.socket.emit('command_execute', command);
    } else {
      console.warn('Socket not connected, cannot execute command');
    }
  }

  public joinRoom(roomId: string): void {
    if (this.socket?.connected) {
      this.socket.emit('join_room', roomId);
    }
  }

  public leaveRoom(roomId: string): void {
    if (this.socket?.connected) {
      this.socket.emit('leave_room', roomId);
    }
  }

  public isConnected(): boolean {
    return this.socket?.connected || false;
  }

  public getConnectionState(): 'connected' | 'disconnected' | 'connecting' | 'reconnecting' {
    if (!this.socket) return 'disconnected';
    
    if (this.socket.connected) return 'connected';
    if (this.isConnecting) return 'connecting';
    if (this.reconnectAttempts > 0) return 'reconnecting';
    
    return 'disconnected';
  }

  public disconnect(): void {
    if (this.socket) {
      this.socket.disconnect();
      this.socket = null;
    }
    this.listeners.clear();
  }

  public reconnect(): void {
    if (this.socket) {
      this.socket.disconnect();
    }
    this.connect();
  }

  // Authentication
  public authenticate(token: string): void {
    if (this.socket?.connected) {
      this.socket.emit('authenticate', { token });
    }
  }

  // Voice streaming methods
  public startVoiceStream(): void {
    if (this.socket?.connected) {
      this.socket.emit('start_voice_stream');
    }
  }

  public stopVoiceStream(): void {
    if (this.socket?.connected) {
      this.socket.emit('stop_voice_stream');
    }
  }

  // System monitoring
  public subscribeToSystemUpdates(): void {
    if (this.socket?.connected) {
      this.socket.emit('subscribe_system_updates');
    }
  }

  public unsubscribeFromSystemUpdates(): void {
    if (this.socket?.connected) {
      this.socket.emit('unsubscribe_system_updates');
    }
  }

  // Get socket instance (for advanced usage)
  public getSocket(): Socket | null {
    return this.socket;
  }
}

// Export singleton instance
export const socketService = new SocketService();
export default socketService;