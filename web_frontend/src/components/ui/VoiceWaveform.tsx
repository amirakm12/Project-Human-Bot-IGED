import React, { useRef, useEffect, useState } from 'react';
import { useAudio } from '../../hooks/useAudio';
import './VoiceWaveform.css';

interface VoiceWaveformProps {
  isRecording?: boolean;
  volume?: number;
  height?: number;
  width?: number;
  color?: string;
  backgroundColor?: string;
  bars?: number;
  className?: string;
  onStartRecording?: () => void;
  onStopRecording?: () => void;
  showControls?: boolean;
  streaming?: boolean;
}

export const VoiceWaveform: React.FC<VoiceWaveformProps> = ({
  height = 100,
  width = 300,
  color = '#00ff88',
  backgroundColor = 'transparent',
  bars = 32,
  className = '',
  onStartRecording,
  onStopRecording,
  showControls = true,
  streaming = false,
}) => {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const animationRef = useRef<number>();
  const [volumes, setVolumes] = useState<number[]>(new Array(bars).fill(0));
  
  const {
    isRecording,
    volume,
    duration,
    error,
    isInitialized,
    permission,
    startRecording,
    stopRecording,
    onVolumeChange,
  } = useAudio();

  // Set up volume monitoring
  useEffect(() => {
    onVolumeChange((vol: number) => {
      setVolumes(prev => {
        const newVolumes = [...prev];
        newVolumes.shift(); // Remove first element
        newVolumes.push(vol); // Add new volume at the end
        return newVolumes;
      });
    });
  }, [onVolumeChange]);

  // Animation loop
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    const animate = () => {
      // Clear canvas
      ctx.fillStyle = backgroundColor;
      ctx.fillRect(0, 0, width, height);

      // Draw waveform bars
      const barWidth = width / bars;
      const maxBarHeight = height * 0.8;

      volumes.forEach((vol, index) => {
        const barHeight = vol * maxBarHeight;
        const x = index * barWidth;
        const y = (height - barHeight) / 2;

        // Create gradient for bars
        const gradient = ctx.createLinearGradient(0, y, 0, y + barHeight);
        gradient.addColorStop(0, color);
        gradient.addColorStop(1, color + '80'); // Add transparency

        ctx.fillStyle = gradient;
        ctx.fillRect(x, y, barWidth - 2, barHeight);

        // Add glow effect when recording
        if (isRecording && vol > 0.1) {
          ctx.shadowColor = color;
          ctx.shadowBlur = 10;
          ctx.fillRect(x, y, barWidth - 2, barHeight);
          ctx.shadowBlur = 0;
        }
      });

      // Draw recording indicator
      if (isRecording) {
        const centerX = width / 2;
        const centerY = height / 2;
        const radius = 8;

        ctx.beginPath();
        ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI);
        ctx.fillStyle = '#ff4444';
        ctx.fill();

        // Pulsing effect
        const pulseRadius = radius + Math.sin(Date.now() * 0.01) * 3;
        ctx.beginPath();
        ctx.arc(centerX, centerY, pulseRadius, 0, 2 * Math.PI);
        ctx.strokeStyle = '#ff4444';
        ctx.lineWidth = 2;
        ctx.stroke();
      }

      animationRef.current = requestAnimationFrame(animate);
    };

    animate();

    return () => {
      if (animationRef.current) {
        cancelAnimationFrame(animationRef.current);
      }
    };
  }, [volumes, isRecording, width, height, color, backgroundColor, bars]);

  // Handle recording toggle
  const handleRecordingToggle = async () => {
    if (isRecording) {
      await stopRecording();
      onStopRecording?.();
    } else {
      const success = await startRecording(streaming);
      if (success) {
        onStartRecording?.();
      }
    }
  };

  // Format duration
  const formatDuration = (seconds: number): string => {
    const mins = Math.floor(seconds / 60);
    const secs = Math.floor(seconds % 60);
    return `${mins}:${secs.toString().padStart(2, '0')}`;
  };

  return (
    <div className={`voice-waveform ${className}`}>
      <canvas
        ref={canvasRef}
        width={width}
        height={height}
        className="waveform-canvas"
      />
      
      {showControls && (
        <div className="waveform-controls">
          <button
            onClick={handleRecordingToggle}
            disabled={!isInitialized || permission === 'denied'}
            className={`record-button ${isRecording ? 'recording' : ''}`}
            title={isRecording ? 'Stop Recording' : 'Start Recording'}
          >
            {isRecording ? (
              <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor">
                <rect x="6" y="6" width="12" height="12" rx="2" />
              </svg>
            ) : (
              <svg width="24" height="24" viewBox="0 0 24 24" fill="currentColor">
                <circle cx="12" cy="12" r="8" />
              </svg>
            )}
          </button>
          
          {isRecording && (
            <div className="recording-info">
              <span className="duration">{formatDuration(duration)}</span>
              <span className="volume-indicator">
                Volume: {Math.round(volume * 100)}%
              </span>
            </div>
          )}
        </div>
      )}
      
      {error && (
        <div className="waveform-error">
          <span className="error-icon">⚠️</span>
          <span className="error-message">{error}</span>
        </div>
      )}
      
      {permission === 'denied' && (
        <div className="permission-error">
          <span className="error-icon">🎤</span>
          <span className="error-message">
            Microphone access denied. Please enable microphone permissions.
          </span>
        </div>
      )}
      
      {!isInitialized && permission !== 'denied' && (
        <div className="initializing">
          <span className="loading-icon">🔄</span>
          <span className="loading-message">Initializing audio...</span>
        </div>
      )}
    </div>
  );
};