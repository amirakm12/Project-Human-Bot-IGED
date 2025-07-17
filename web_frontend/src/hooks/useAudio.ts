import { useState, useEffect, useCallback, useRef } from 'react';
import audioService, { VoiceRecordingState } from '../services/audio';
import apiService from '../services/api';

export interface UseAudioReturn {
  // State
  isRecording: boolean;
  isProcessing: boolean;
  volume: number;
  duration: number;
  error: string | null;
  isInitialized: boolean;
  permission: PermissionState | null;
  
  // Actions
  startRecording: (streaming?: boolean) => Promise<boolean>;
  stopRecording: () => Promise<void>;
  processLastRecording: () => Promise<void>;
  initialize: () => Promise<boolean>;
  cleanup: () => void;
  
  // Callbacks
  onTranscription: (callback: (text: string) => void) => void;
  onVolumeChange: (callback: (volume: number) => void) => void;
}

export const useAudio = (): UseAudioReturn => {
  const [isRecording, setIsRecording] = useState(false);
  const [isProcessing, setIsProcessing] = useState(false);
  const [volume, setVolume] = useState(0);
  const [duration, setDuration] = useState(0);
  const [error, setError] = useState<string | null>(null);
  const [isInitialized, setIsInitialized] = useState(false);
  const [permission, setPermission] = useState<PermissionState | null>(null);
  
  const durationIntervalRef = useRef<NodeJS.Timeout | null>(null);
  const startTimeRef = useRef<number>(0);
  const lastRecordingRef = useRef<Blob | null>(null);
  const transcriptionCallbackRef = useRef<((text: string) => void) | null>(null);
  const volumeCallbackRef = useRef<((volume: number) => void) | null>(null);

  // Initialize audio service on mount
  useEffect(() => {
    const initAudio = async () => {
      try {
        // Check microphone permission
        const permissionState = await audioService.checkMicrophonePermission();
        setPermission(permissionState);

        // Initialize audio service
        const initialized = await audioService.initialize();
        setIsInitialized(initialized);

        if (!initialized) {
          setError('Failed to initialize audio service');
        }
      } catch (err) {
        setError('Audio initialization failed');
        console.error('Audio initialization error:', err);
      }
    };

    initAudio();

    // Cleanup on unmount
    return () => {
      cleanup();
    };
  }, []);

  // Set up audio service callbacks
  useEffect(() => {
    audioService.setVolumeCallback((vol: number) => {
      setVolume(vol);
      if (volumeCallbackRef.current) {
        volumeCallbackRef.current(vol);
      }
    });

    audioService.setTranscriptionCallback((text: string) => {
      if (transcriptionCallbackRef.current) {
        transcriptionCallbackRef.current(text);
      }
    });
  }, []);

  // Start recording
  const startRecording = useCallback(async (streaming = false): Promise<boolean> => {
    if (!isInitialized) {
      setError('Audio service not initialized');
      return false;
    }

    try {
      setError(null);
      const success = await audioService.startRecording(streaming);
      
      if (success) {
        setIsRecording(true);
        setDuration(0);
        startTimeRef.current = Date.now();
        
        // Start duration tracking
        durationIntervalRef.current = setInterval(() => {
          setDuration((Date.now() - startTimeRef.current) / 1000);
        }, 100);
      } else {
        setError('Failed to start recording');
      }
      
      return success;
    } catch (err) {
      setError('Recording start failed');
      console.error('Recording error:', err);
      return false;
    }
  }, [isInitialized]);

  // Stop recording
  const stopRecording = useCallback(async (): Promise<void> => {
    if (!isRecording) return;

    try {
      const audioBlob = audioService.stopRecording();
      setIsRecording(false);
      setVolume(0);
      
      // Clear duration interval
      if (durationIntervalRef.current) {
        clearInterval(durationIntervalRef.current);
        durationIntervalRef.current = null;
      }

      if (audioBlob) {
        lastRecordingRef.current = audioBlob;
      }
    } catch (err) {
      setError('Failed to stop recording');
      console.error('Stop recording error:', err);
    }
  }, [isRecording]);

  // Process the last recording
  const processLastRecording = useCallback(async (): Promise<void> => {
    if (!lastRecordingRef.current) {
      setError('No recording to process');
      return;
    }

    try {
      setIsProcessing(true);
      setError(null);

      // Process the audio blob
      const processedBlob = await audioService.processAudioBlob(lastRecordingRef.current);
      
      // Upload to backend for processing
      const response = await apiService.uploadAudio(processedBlob);
      
      if (!response.success) {
        setError(response.error || 'Failed to process audio');
      }
      
    } catch (err) {
      setError('Audio processing failed');
      console.error('Processing error:', err);
    } finally {
      setIsProcessing(false);
    }
  }, []);

  // Initialize audio service manually
  const initialize = useCallback(async (): Promise<boolean> => {
    try {
      setError(null);
      const success = await audioService.initialize();
      setIsInitialized(success);
      
      if (!success) {
        setError('Failed to initialize audio service');
      }
      
      return success;
    } catch (err) {
      setError('Initialization failed');
      console.error('Initialization error:', err);
      return false;
    }
  }, []);

  // Cleanup resources
  const cleanup = useCallback((): void => {
    if (durationIntervalRef.current) {
      clearInterval(durationIntervalRef.current);
      durationIntervalRef.current = null;
    }
    
    audioService.cleanup();
    setIsRecording(false);
    setIsProcessing(false);
    setVolume(0);
    setDuration(0);
    setIsInitialized(false);
    lastRecordingRef.current = null;
  }, []);

  // Set transcription callback
  const onTranscription = useCallback((callback: (text: string) => void): void => {
    transcriptionCallbackRef.current = callback;
  }, []);

  // Set volume change callback
  const onVolumeChange = useCallback((callback: (volume: number) => void): void => {
    volumeCallbackRef.current = callback;
  }, []);

  return {
    // State
    isRecording,
    isProcessing,
    volume,
    duration,
    error,
    isInitialized,
    permission,
    
    // Actions
    startRecording,
    stopRecording,
    processLastRecording,
    initialize,
    cleanup,
    
    // Callbacks
    onTranscription,
    onVolumeChange,
  };
};