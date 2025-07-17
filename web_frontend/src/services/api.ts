import axios, { AxiosInstance, AxiosResponse, AxiosError } from 'axios';

// API Configuration
const API_BASE_URL = process.env.REACT_APP_API_URL || 'http://localhost:5000';
const API_TIMEOUT = 30000; // 30 seconds

// API Response Types
export interface ApiResponse<T = any> {
  success: boolean;
  data?: T;
  message?: string;
  error?: string;
}

export interface AuthResponse {
  token: string;
  user: {
    id: string;
    username: string;
    email: string;
  };
}

export interface CommandRequest {
  command: string;
  type: 'text' | 'voice';
  timestamp: number;
}

export interface CommandResponse {
  id: string;
  command: string;
  response: string;
  timestamp: number;
  execution_time: number;
  status: 'success' | 'error' | 'pending';
}

export interface SystemStatus {
  cpu_usage: number;
  memory_usage: number;
  disk_usage: number;
  uptime: number;
  active_processes: number;
  voice_pipeline_status: 'active' | 'inactive' | 'error';
  agents_status: {
    [key: string]: 'active' | 'inactive' | 'error';
  };
}

export interface AnalyticsData {
  total_commands: number;
  successful_commands: number;
  failed_commands: number;
  avg_response_time: number;
  most_used_commands: Array<{
    command: string;
    count: number;
  }>;
  usage_by_hour: Array<{
    hour: number;
    count: number;
  }>;
}

// Create axios instance
class ApiService {
  private api: AxiosInstance;
  private authToken: string | null = null;

  constructor() {
    this.api = axios.create({
      baseURL: API_BASE_URL,
      timeout: API_TIMEOUT,
      headers: {
        'Content-Type': 'application/json',
      },
    });

    // Request interceptor to add auth token
    this.api.interceptors.request.use(
      (config) => {
        if (this.authToken) {
          config.headers.Authorization = `Bearer ${this.authToken}`;
        }
        return config;
      },
      (error) => Promise.reject(error)
    );

    // Response interceptor for error handling
    this.api.interceptors.response.use(
      (response: AxiosResponse) => response,
      (error: AxiosError) => {
        if (error.response?.status === 401) {
          this.logout();
          window.location.href = '/login';
        }
        return Promise.reject(error);
      }
    );

    // Load token from localStorage
    this.loadAuthToken();
  }

  private loadAuthToken(): void {
    const token = localStorage.getItem('auth_token');
    if (token) {
      this.authToken = token;
    }
  }

  private saveAuthToken(token: string): void {
    this.authToken = token;
    localStorage.setItem('auth_token', token);
  }

  private removeAuthToken(): void {
    this.authToken = null;
    localStorage.removeItem('auth_token');
  }

  // Authentication Methods
  async login(username: string, password: string): Promise<ApiResponse<AuthResponse>> {
    try {
      const response = await this.api.post('/auth/login', { username, password });
      const { token } = response.data;
      this.saveAuthToken(token);
      return { success: true, data: response.data };
    } catch (error: any) {
      return {
        success: false,
        error: error.response?.data?.message || 'Login failed',
      };
    }
  }

  async logout(): Promise<void> {
    try {
      await this.api.post('/auth/logout');
    } catch (error) {
      console.error('Logout error:', error);
    } finally {
      this.removeAuthToken();
    }
  }

  async register(username: string, email: string, password: string): Promise<ApiResponse<AuthResponse>> {
    try {
      const response = await this.api.post('/auth/register', { username, email, password });
      const { token } = response.data;
      this.saveAuthToken(token);
      return { success: true, data: response.data };
    } catch (error: any) {
      return {
        success: false,
        error: error.response?.data?.message || 'Registration failed',
      };
    }
  }

  async verifyToken(): Promise<boolean> {
    try {
      await this.api.get('/auth/verify');
      return true;
    } catch (error) {
      this.removeAuthToken();
      return false;
    }
  }

  // Command Methods
  async executeCommand(request: CommandRequest): Promise<ApiResponse<CommandResponse>> {
    try {
      const response = await this.api.post('/commands/execute', request);
      return { success: true, data: response.data };
    } catch (error: any) {
      return {
        success: false,
        error: error.response?.data?.message || 'Command execution failed',
      };
    }
  }

  async getCommandHistory(limit = 50, offset = 0): Promise<ApiResponse<CommandResponse[]>> {
    try {
      const response = await this.api.get(`/commands/history?limit=${limit}&offset=${offset}`);
      return { success: true, data: response.data };
    } catch (error: any) {
      return {
        success: false,
        error: error.response?.data?.message || 'Failed to fetch command history',
      };
    }
  }

  async deleteCommand(commandId: string): Promise<ApiResponse> {
    try {
      await this.api.delete(`/commands/${commandId}`);
      return { success: true };
    } catch (error: any) {
      return {
        success: false,
        error: error.response?.data?.message || 'Failed to delete command',
      };
    }
  }

  // System Methods
  async getSystemStatus(): Promise<ApiResponse<SystemStatus>> {
    try {
      const response = await this.api.get('/system/status');
      return { success: true, data: response.data };
    } catch (error: any) {
      return {
        success: false,
        error: error.response?.data?.message || 'Failed to fetch system status',
      };
    }
  }

  async getAnalytics(timeRange = '24h'): Promise<ApiResponse<AnalyticsData>> {
    try {
      const response = await this.api.get(`/analytics?range=${timeRange}`);
      return { success: true, data: response.data };
    } catch (error: any) {
      return {
        success: false,
        error: error.response?.data?.message || 'Failed to fetch analytics',
      };
    }
  }

  // Voice Methods
  async uploadAudio(audioBlob: Blob): Promise<ApiResponse<CommandResponse>> {
    try {
      const formData = new FormData();
      formData.append('audio', audioBlob, 'voice_command.wav');
      
      const response = await this.api.post('/voice/process', formData, {
        headers: {
          'Content-Type': 'multipart/form-data',
        },
      });
      
      return { success: true, data: response.data };
    } catch (error: any) {
      return {
        success: false,
        error: error.response?.data?.message || 'Voice processing failed',
      };
    }
  }

  // Settings Methods
  async getSettings(): Promise<ApiResponse<any>> {
    try {
      const response = await this.api.get('/settings');
      return { success: true, data: response.data };
    } catch (error: any) {
      return {
        success: false,
        error: error.response?.data?.message || 'Failed to fetch settings',
      };
    }
  }

  async updateSettings(settings: any): Promise<ApiResponse> {
    try {
      await this.api.put('/settings', settings);
      return { success: true };
    } catch (error: any) {
      return {
        success: false,
        error: error.response?.data?.message || 'Failed to update settings',
      };
    }
  }

  // Health Check
  async healthCheck(): Promise<boolean> {
    try {
      await this.api.get('/health');
      return true;
    } catch (error) {
      return false;
    }
  }

  // Get auth status
  isAuthenticated(): boolean {
    return !!this.authToken;
  }

  getAuthToken(): string | null {
    return this.authToken;
  }
}

// Export singleton instance
export const apiService = new ApiService();
export default apiService;