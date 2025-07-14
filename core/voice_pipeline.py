"""
Voice Pipeline for IGED
Handles speech recognition using Whisper

This module provides voice processing capabilities for the IGED system,
including speech recognition, audio processing, and command delegation.
"""

import logging
import queue
import threading
from typing import Optional, Dict, Any

# Voice recognition imports (optional)
try:
    import speech_recognition as sr
    SPEECH_RECOGNITION_AVAILABLE = True
except ImportError:
    SPEECH_RECOGNITION_AVAILABLE = False

try:
    import whisper
    WHISPER_AVAILABLE = True
except ImportError:
    WHISPER_AVAILABLE = False

logger = logging.getLogger(__name__)


class VoicePipeline:
    """
    Voice processing pipeline for IGED system.
    
    This class manages speech recognition, audio processing, and command
    delegation for voice-controlled interactions with the IGED system.
    """
    
    def __init__(self, command_parser, orchestrator, memory_engine):
        """
        Initialize the voice pipeline.
        
        Args:
            command_parser: Command parser instance
            orchestrator: Orchestrator instance for command execution
            memory_engine: Memory engine for storing interactions
        """
        self.parser = command_parser
        self.orchestrator = orchestrator
        self.memory = memory_engine
        self.recognizer: Optional[sr.Recognizer] = None
        self.whisper_model: Optional[Any] = None
        self.is_listening = False
        self.audio_queue: queue.Queue = queue.Queue()
        self.callback_queue: queue.Queue = queue.Queue()
        
        # Initialize voice recognition components
        if SPEECH_RECOGNITION_AVAILABLE:
            self.recognizer = sr.Recognizer()
        
        # Initialize Whisper model for offline recognition
        self._initialize_whisper()
    
    def _initialize_whisper(self) -> None:
        """
        Initialize Whisper model for offline speech recognition.
        
        Loads the Whisper model if available, falls back gracefully if not.
        """
        if not WHISPER_AVAILABLE:
            logger.warning("🎤 Whisper not available, offline recognition disabled")
            return
            
        try:
            logger.info("🎤 Initializing Whisper model...")
            self.whisper_model = whisper.load_model("base")
            logger.info("✅ Whisper model loaded successfully")
        except Exception as e:
            logger.error(f"❌ Failed to load Whisper model: {e}")
            self.whisper_model = None
    
    def start_listening(self) -> None:
        """
        Start continuous voice listening.
        
        Initiates audio processing threads and microphone listening.
        """
        if self.is_listening:
            logger.warning("🎤 Already listening")
            return
        
        self.is_listening = True
        logger.info("🎤 Starting voice listening...")
        
        # Start audio processing thread
        audio_thread = threading.Thread(target=self._audio_processing_loop, daemon=True)
        audio_thread.start()
        
        # Start callback processing thread
        callback_thread = threading.Thread(target=self._callback_processing_loop, daemon=True)
        callback_thread.start()
        
        # Start microphone listening
        self._listen_microphone()
    
    def stop_listening(self) -> None:
        """
        Stop voice listening.
        
        Stops all audio processing threads and microphone listening.
        """
        self.is_listening = False
        logger.info("🛑 Voice listening stopped")
    
    def _listen_microphone(self) -> None:
        """
        Listen to microphone input continuously.
        
        Captures audio from the microphone and queues it for processing.
        """
        if not SPEECH_RECOGNITION_AVAILABLE or not self.recognizer:
            logger.error("❌ Speech recognition not available")
            return
            
        try:
            with sr.Microphone() as source:
                # Adjust for ambient noise
                logger.info("🎤 Adjusting for ambient noise...")
                self.recognizer.adjust_for_ambient_noise(source, duration=1)
                logger.info("🎤 Microphone ready")
                
                while self.is_listening:
                    try:
                        logger.debug("🎤 Listening for speech...")
                        audio = self.recognizer.listen(source, timeout=1, phrase_time_limit=10)
                        self.audio_queue.put(audio)
                    except sr.WaitTimeoutError:
                        continue
                    except Exception as e:
                        logger.error(f"❌ Microphone error: {e}")
                        break
        except Exception as e:
            logger.error(f"❌ Microphone initialization failed: {e}")
    
    def _audio_processing_loop(self) -> None:
        """
        Process audio from the audio queue.
        
        Continuously processes audio data from the queue and attempts
        speech recognition using available methods.
        """
        logger.info("🎤 Audio processing loop started")
        
        while self.is_listening:
            try:
                # Get audio from queue with timeout
                audio = self.audio_queue.get(timeout=1)
                self._process_audio(audio)
                self.audio_queue.task_done()
            except queue.Empty:
                continue
            except Exception as e:
                logger.error(f"❌ Audio processing error: {e}")
    
    def _process_audio(self, audio) -> None:
        """
        Process individual audio sample.
        
        Args:
            audio: Audio data from speech recognition
        """
        try:
            # Try Whisper first (offline)
            if self.whisper_model:
                transcription = self._whisper_transcribe(audio)
                if transcription:
                    self._handle_transcription(transcription)
                    return
            
            # Fallback to online speech recognition
            if self.recognizer:
                transcription = self._speech_recognition_transcribe(audio)
                if transcription:
                    self._handle_transcription(transcription)
                    
        except Exception as e:
            logger.error(f"❌ Audio processing failed: {e}")
    
    def _whisper_transcribe(self, audio) -> Optional[str]:
        """
        Transcribe audio using Whisper model.
        
        Args:
            audio: Audio data to transcribe
            
        Returns:
            Transcribed text or None if transcription fails
        """
        try:
            if not self.whisper_model:
                return None
                
            # Convert audio to numpy array for Whisper
            audio_data = audio.get_wav_data()
            
            # Use Whisper to transcribe
            result = self.whisper_model.transcribe(audio_data)
            text = result.get('text', '').strip()
            
            if text:
                logger.info(f"🎤 Whisper transcription: {text}")
                return text
                
        except Exception as e:
            logger.error(f"❌ Whisper transcription failed: {e}")
            
        return None
    
    def _speech_recognition_transcribe(self, audio) -> Optional[str]:
        """
        Transcribe audio using online speech recognition.
        
        Args:
            audio: Audio data to transcribe
            
        Returns:
            Transcribed text or None if transcription fails
        """
        try:
            if not self.recognizer:
                return None
                
            # Try Google Speech Recognition
            text = self.recognizer.recognize_google(audio)
            logger.info(f"🎤 Google transcription: {text}")
            return text
            
        except sr.UnknownValueError:
            logger.debug("🎤 Could not understand audio")
        except sr.RequestError as e:
            logger.error(f"❌ Speech recognition error: {e}")
        except Exception as e:
            logger.error(f"❌ Transcription failed: {e}")
            
        return None
    
    def _handle_transcription(self, text: str) -> None:
        """
        Handle transcribed text by parsing and executing commands.
        
        Args:
            text: Transcribed text to process
        """
        try:
            if not text or not text.strip():
                return
                
            logger.info(f"🎤 Processing voice command: {text}")
            
            # Parse command
            command = self.parser.parse_command(text)
            
            if command and command.get('command_type') != 'error':
                # Queue command for execution
                callback = {
                    'type': 'command_execution',
                    'command': command,
                    'original_text': text
                }
                self.callback_queue.put(callback)
                
                # Store in memory
                self.memory.add_entry(
                    command=text,
                    result="Voice command queued",
                    agent="voice_pipeline",
                    metadata={'confidence': command.get('confidence', 0.0)}
                )
            else:
                logger.warning(f"🎤 Could not parse voice command: {text}")
                
        except Exception as e:
            logger.error(f"❌ Voice command processing failed: {e}")
    
    def _callback_processing_loop(self) -> None:
        """
        Process callbacks from the callback queue.
        
        Continuously processes queued callbacks and executes commands.
        """
        logger.info("🎤 Callback processing loop started")
        
        while self.is_listening:
            try:
                # Get callback from queue with timeout
                callback = self.callback_queue.get(timeout=1)
                self._process_callback(callback)
                self.callback_queue.task_done()
            except queue.Empty:
                continue
            except Exception as e:
                logger.error(f"❌ Callback processing error: {e}")
    
    def _process_callback(self, callback: Dict[str, Any]) -> None:
        """
        Process individual callback.
        
        Args:
            callback: Callback data dictionary
        """
        try:
            callback_type = callback.get('type')
            
            if callback_type == 'command_execution':
                command = callback.get('command')
                if command:
                    result = self.orchestrator.execute_command(command)
                    logger.info(f"🎤 Voice command executed: {result}")
                    
        except Exception as e:
            logger.error(f"❌ Callback processing failed: {e}")
    
    def process_text_command(self, text: str) -> Optional[str]:
        """
        Process text command directly (for testing/debugging).
        
        Args:
            text: Text command to process
            
        Returns:
            Command execution result or None
        """
        try:
            command = self.parser.parse_command(text)
            if command and command.get('command_type') != 'error':
                return self.orchestrator.execute_command(command)
        except Exception as e:
            logger.error(f"❌ Text command processing failed: {e}")
        return None
    
    def get_status(self) -> Dict[str, Any]:
        """
        Get current voice pipeline status.
        
        Returns:
            Dictionary containing pipeline status information
        """
        return {
            'is_listening': self.is_listening,
            'speech_recognition_available': SPEECH_RECOGNITION_AVAILABLE,
            'whisper_available': WHISPER_AVAILABLE,
            'whisper_model_loaded': self.whisper_model is not None,
            'audio_queue_size': self.audio_queue.qsize(),
            'callback_queue_size': self.callback_queue.qsize()
        } 