import React, { useEffect, useRef, useState } from 'react';
import './VoiceWaveform.css';

const VoiceWaveform: React.FC = () => {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [isListening, setIsListening] = useState(false);
  const animationRef = useRef<number>();

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    const animate = () => {
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      
      const bars = 50;
      const barWidth = canvas.width / bars;
      const centerY = canvas.height / 2;
      
      for (let i = 0; i < bars; i++) {
        const height = isListening 
          ? Math.random() * canvas.height * 0.8
          : Math.sin(Date.now() * 0.001 + i * 0.1) * 20 + 20;
        
        const x = i * barWidth;
        const y = centerY - height / 2;
        
        // Create gradient
        const gradient = ctx.createLinearGradient(x, 0, x + barWidth, 0);
        gradient.addColorStop(0, '#00ff88');
        gradient.addColorStop(0.5, '#00ccff');
        gradient.addColorStop(1, '#00ff88');
        
        ctx.fillStyle = gradient;
        ctx.fillRect(x, y, barWidth - 2, height);
        
        // Add glow effect
        ctx.shadowColor = '#00ff88';
        ctx.shadowBlur = 5;
        ctx.fillRect(x, y, barWidth - 2, height);
        ctx.shadowBlur = 0;
      }
      
      animationRef.current = requestAnimationFrame(animate);
    };

    animate();

    return () => {
      if (animationRef.current) {
        cancelAnimationFrame(animationRef.current);
      }
    };
  }, [isListening]);

  const toggleListening = () => {
    setIsListening(!isListening);
  };

  return (
    <div className="voice-waveform">
      <h3>Voice Interface</h3>
      
      <div className="waveform-container">
        <canvas
          ref={canvasRef}
          width={400}
          height={100}
          className="waveform-canvas"
        />
      </div>
      
      <div className="voice-controls">
        <button 
          className={`voice-btn ${isListening ? 'listening' : ''}`}
          onClick={toggleListening}
          title={isListening ? 'Stop Listening' : 'Start Listening'}
        >
          🎤
        </button>
        <span className="voice-status">
          {isListening ? 'Listening...' : 'Voice Ready'}
        </span>
      </div>
      
      <div className="voice-info">
        <div className="info-item">
          <span className="info-label">Status:</span>
          <span className={`info-value ${isListening ? 'active' : ''}`}>
            {isListening ? 'Active' : 'Standby'}
          </span>
        </div>
        <div className="info-item">
          <span className="info-label">Sensitivity:</span>
          <span className="info-value">High</span>
        </div>
      </div>
    </div>
  );
};

export default VoiceWaveform;