import socketService from './socket';

export interface AudioConfig {
  sampleRate: number;
  channels: number;
  bitDepth: number;
  bufferSize: number;
}

export interface VoiceRecordingState {
  isRecording: boolean;
  isProcessing: boolean;
  duration: number;
  volume: number;
  error: string | null;
}

class AudioService {
  private mediaRecorder: MediaRecorder | null = null;
  private audioContext: AudioContext | null = null;
  private analyser: AnalyserNode | null = null;
  private microphone: MediaStreamAudioSourceNode | null = null;
  private stream: MediaStream | null = null;
  private recordedChunks: Blob[] = [];
  private isRecording = false;
  private isStreaming = false;
  private volumeCallback: ((volume: number) => void) | null = null;
  private transcriptionCallback: ((text: string) => void) | null = null;
  
  private config: AudioConfig = {
    sampleRate: 44100,
    channels: 1,
    bitDepth: 16,
    bufferSize: 4096,
  };

  constructor() {
    this.setupSocketListeners();
  }

  private setupSocketListeners(): void {
    socketService.on('voice_transcription', (transcription: string) => {
      if (this.transcriptionCallback) {
        this.transcriptionCallback(transcription);
      }
    });
  }

  // Initialize audio context and get microphone access
  async initialize(): Promise<boolean> {
    try {
      // Request microphone permission
      this.stream = await navigator.mediaDevices.getUserMedia({
        audio: {
          sampleRate: this.config.sampleRate,
          channelCount: this.config.channels,
          echoCancellation: true,
          noiseSuppression: true,
          autoGainControl: true,
        },
      });

      // Create audio context
      this.audioContext = new (window.AudioContext || (window as any).webkitAudioContext)();
      
      // Create analyser for volume monitoring
      this.analyser = this.audioContext.createAnalyser();
      this.analyser.fftSize = 256;
      this.analyser.smoothingTimeConstant = 0.8;

      // Connect microphone to analyser
      this.microphone = this.audioContext.createMediaStreamSource(this.stream);
      this.microphone.connect(this.analyser);

      // Create media recorder
      this.mediaRecorder = new MediaRecorder(this.stream, {
        mimeType: this.getSupportedMimeType(),
      });

      this.setupMediaRecorderEvents();
      
      return true;
    } catch (error) {
      console.error('Failed to initialize audio:', error);
      return false;
    }
  }

  private getSupportedMimeType(): string {
    const types = [
      'audio/webm;codecs=opus',
      'audio/webm',
      'audio/mp4',
      'audio/wav',
    ];

    for (const type of types) {
      if (MediaRecorder.isTypeSupported(type)) {
        return type;
      }
    }

    return '';
  }

  private setupMediaRecorderEvents(): void {
    if (!this.mediaRecorder) return;

    this.mediaRecorder.ondataavailable = (event) => {
      if (event.data.size > 0) {
        this.recordedChunks.push(event.data);
        
        // Stream audio data via socket if streaming is enabled
        if (this.isStreaming) {
          event.data.arrayBuffer().then((buffer) => {
            socketService.sendVoiceStream(buffer);
          });
        }
      }
    };

    this.mediaRecorder.onstop = () => {
      this.isRecording = false;
      if (this.isStreaming) {
        socketService.stopVoiceStream();
        this.isStreaming = false;
      }
    };

    this.mediaRecorder.onerror = (event) => {
      console.error('MediaRecorder error:', event);
      this.isRecording = false;
      this.isStreaming = false;
    };
  }

  // Start recording audio
  async startRecording(streaming = false): Promise<boolean> {
    if (!this.mediaRecorder || this.isRecording) {
      return false;
    }

    try {
      this.recordedChunks = [];
      this.isRecording = true;
      this.isStreaming = streaming;

      if (streaming) {
        socketService.startVoiceStream();
      }

      // Start recording with time slice for streaming
      if (streaming) {
        this.mediaRecorder.start(100); // 100ms chunks for real-time streaming
      } else {
        this.mediaRecorder.start();
      }

      // Start volume monitoring
      this.startVolumeMonitoring();

      return true;
    } catch (error) {
      console.error('Failed to start recording:', error);
      this.isRecording = false;
      this.isStreaming = false;
      return false;
    }
  }

  // Stop recording audio
  stopRecording(): Blob | null {
    if (!this.mediaRecorder || !this.isRecording) {
      return null;
    }

    this.mediaRecorder.stop();
    this.stopVolumeMonitoring();

    if (this.recordedChunks.length > 0) {
      const audioBlob = new Blob(this.recordedChunks, {
        type: this.mediaRecorder.mimeType,
      });
      this.recordedChunks = [];
      return audioBlob;
    }

    return null;
  }

  // Start volume monitoring
  private startVolumeMonitoring(): void {
    if (!this.analyser || !this.volumeCallback) return;

    const bufferLength = this.analyser.frequencyBinCount;
    const dataArray = new Uint8Array(bufferLength);

    const updateVolume = () => {
      if (!this.isRecording || !this.analyser) return;

      this.analyser.getByteFrequencyData(dataArray);
      
      // Calculate average volume
      let sum = 0;
      for (let i = 0; i < bufferLength; i++) {
        sum += dataArray[i];
      }
      const average = sum / bufferLength;
      const volume = average / 255; // Normalize to 0-1

      if (this.volumeCallback) {
        this.volumeCallback(volume);
      }

      requestAnimationFrame(updateVolume);
    };

    updateVolume();
  }

  private stopVolumeMonitoring(): void {
    // Volume monitoring will stop automatically when isRecording becomes false
  }

  // Get current recording state
  getRecordingState(): VoiceRecordingState {
    return {
      isRecording: this.isRecording,
      isProcessing: false, // This would be set by the calling component
      duration: 0, // This would be tracked by the calling component
      volume: 0, // This is provided via callback
      error: null,
    };
  }

  // Set volume callback
  setVolumeCallback(callback: (volume: number) => void): void {
    this.volumeCallback = callback;
  }

  // Set transcription callback
  setTranscriptionCallback(callback: (text: string) => void): void {
    this.transcriptionCallback = callback;
  }

  // Process audio blob (convert to different formats if needed)
  async processAudioBlob(blob: Blob): Promise<Blob> {
    // For now, return as-is. In the future, we could add:
    // - Format conversion
    // - Compression
    // - Noise reduction
    return blob;
  }

  // Convert audio blob to base64
  async blobToBase64(blob: Blob): Promise<string> {
    return new Promise((resolve, reject) => {
      const reader = new FileReader();
      reader.onload = () => {
        const base64 = (reader.result as string).split(',')[1];
        resolve(base64);
      };
      reader.onerror = reject;
      reader.readAsDataURL(blob);
    });
  }

  // Get audio duration from blob
  async getAudioDuration(blob: Blob): Promise<number> {
    return new Promise((resolve) => {
      const audio = new Audio();
      audio.onloadedmetadata = () => {
        resolve(audio.duration);
      };
      audio.src = URL.createObjectURL(blob);
    });
  }

  // Check if audio is supported
  isAudioSupported(): boolean {
    return !!(navigator.mediaDevices && navigator.mediaDevices.getUserMedia);
  }

  // Check microphone permission
  async checkMicrophonePermission(): Promise<PermissionState> {
    try {
      const permission = await navigator.permissions.query({ name: 'microphone' as PermissionName });
      return permission.state;
    } catch (error) {
      console.warn('Permission API not supported');
      return 'prompt';
    }
  }

  // Cleanup resources
  cleanup(): void {
    if (this.isRecording) {
      this.stopRecording();
    }

    if (this.stream) {
      this.stream.getTracks().forEach(track => track.stop());
      this.stream = null;
    }

    if (this.audioContext) {
      this.audioContext.close();
      this.audioContext = null;
    }

    this.mediaRecorder = null;
    this.analyser = null;
    this.microphone = null;
    this.recordedChunks = [];
    this.volumeCallback = null;
    this.transcriptionCallback = null;
  }

  // Get supported audio formats
  getSupportedFormats(): string[] {
    const formats = [
      'audio/webm;codecs=opus',
      'audio/webm',
      'audio/mp4',
      'audio/wav',
      'audio/ogg',
    ];

    return formats.filter(format => MediaRecorder.isTypeSupported(format));
  }

  // Update configuration
  updateConfig(newConfig: Partial<AudioConfig>): void {
    this.config = { ...this.config, ...newConfig };
  }

  // Get current configuration
  getConfig(): AudioConfig {
    return { ...this.config };
  }
}

// Export singleton instance
export const audioService = new AudioService();
export default audioService;