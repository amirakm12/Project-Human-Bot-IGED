/** @type {import('tailwindcss').Config} */
import { fontFamily } from "tailwindcss/defaultTheme"

export default {
  content: [
    "./src/**/*.{js,ts,jsx,tsx}",
    "./components/**/*.{js,ts,jsx,tsx}",
    "./stories/**/*.{js,ts,jsx,tsx}",
  ],
  darkMode: ["class"],
  theme: {
    container: {
      center: true,
      padding: "2rem",
      screens: {
        "2xl": "1400px",
      },
    },
    extend: {
      colors: {
        // IGED Sovereign AI Color System
        iged: {
          // Background Layers
          'bg-primary': '#1a1a1a',    // Deep charcoal
          'bg-secondary': '#2b2b2b',  // Medium charcoal  
          'bg-tertiary': '#3b3b3b',   // Light charcoal
          'bg-input': '#1e1e1e',      // Terminal black
          
          // Accent Colors
          'accent-primary': '#00ff41',    // Matrix green
          'accent-secondary': '#667eea',  // Professional blue
          'accent-tertiary': '#764ba2',   // Deep purple
          
          // Status Colors
          'success': '#27ae60',
          'warning': '#f39c12', 
          'danger': '#e74c3c',
          'info': '#2196F3',
          
          // Text Colors
          'text-primary': '#ffffff',     // Pure white
          'text-secondary': '#b8b8b8',   // Light gray
          'text-muted': '#888888',       // Medium gray
          'text-terminal': '#00ff41',    // Matrix green
          
          // Interactive States
          'hover': 'rgba(102, 126, 234, 0.1)',
          'focus': 'rgba(0, 255, 65, 0.2)',
          'active': 'rgba(102, 126, 234, 0.3)',
          'disabled': 'rgba(255, 255, 255, 0.1)',
        },
        
        // ShadCN compatible colors
        border: "hsl(var(--border))",
        input: "hsl(var(--input))",
        ring: "hsl(var(--ring))",
        background: "hsl(var(--background))",
        foreground: "hsl(var(--foreground))",
        primary: {
          DEFAULT: "hsl(var(--primary))",
          foreground: "hsl(var(--primary-foreground))",
        },
        secondary: {
          DEFAULT: "hsl(var(--secondary))",
          foreground: "hsl(var(--secondary-foreground))",
        },
        destructive: {
          DEFAULT: "hsl(var(--destructive))",
          foreground: "hsl(var(--destructive-foreground))",
        },
        muted: {
          DEFAULT: "hsl(var(--muted))",
          foreground: "hsl(var(--muted-foreground))",
        },
        accent: {
          DEFAULT: "hsl(var(--accent))",
          foreground: "hsl(var(--accent-foreground))",
        },
        popover: {
          DEFAULT: "hsl(var(--popover))",
          foreground: "hsl(var(--popover-foreground))",
        },
        card: {
          DEFAULT: "hsl(var(--card))",
          foreground: "hsl(var(--card-foreground))",
        },
      },
      backgroundImage: {
        'iged-gradient': 'linear-gradient(135deg, #00ff41 0%, #667eea 50%, #764ba2 100%)',
        'iged-radial': 'radial-gradient(circle at center, #00ff41 0%, transparent 70%)',
        'matrix-rain': 'linear-gradient(180deg, transparent 0%, #00ff41 50%, transparent 100%)',
        'neural-grid': 'radial-gradient(circle at 20% 20%, #667eea 0%, transparent 50%), radial-gradient(circle at 80% 80%, #00ff41 0%, transparent 50%)',
      },
      borderRadius: {
        lg: "var(--radius)",
        md: "calc(var(--radius) - 2px)",
        sm: "calc(var(--radius) - 4px)",
        // IGED specific radius
        'iged-sm': '4px',
        'iged-md': '6px', 
        'iged-lg': '8px',
        'iged-xl': '12px',
        'iged-2xl': '16px',
      },
      fontFamily: {
        sans: ["Inter", ...fontFamily.sans],
        mono: ["JetBrains Mono", "Consolas", "Monaco", ...fontFamily.mono],
        'iged-primary': ["Inter", "system-ui", "sans-serif"],
        'iged-mono': ["JetBrains Mono", "Consolas", "monospace"],
      },
      fontSize: {
        'iged-xs': '0.75rem',    // 12px
        'iged-sm': '0.875rem',   // 14px  
        'iged-base': '1rem',     // 16px
        'iged-lg': '1.125rem',   // 18px
        'iged-xl': '1.25rem',    // 20px
        'iged-2xl': '1.5rem',    // 24px
        'iged-3xl': '2rem',      // 32px
      },
      spacing: {
        'iged-xs': '0.25rem',   // 4px
        'iged-sm': '0.5rem',    // 8px
        'iged-md': '0.75rem',   // 12px
        'iged-lg': '1rem',      // 16px
        'iged-xl': '1.5rem',    // 24px
        'iged-2xl': '2rem',     // 32px
        'iged-3xl': '3rem',     // 48px
      },
      boxShadow: {
        'iged-sm': '0 1px 2px rgba(0, 0, 0, 0.2)',
        'iged-md': '0 4px 6px rgba(0, 0, 0, 0.3)',
        'iged-lg': '0 4px 20px rgba(0, 0, 0, 0.4)',
        'iged-xl': '0 8px 25px rgba(0, 0, 0, 0.5)',
        'iged-glow': '0 0 20px rgba(0, 255, 65, 0.3)',
        'iged-glow-blue': '0 0 20px rgba(102, 126, 234, 0.3)',
      },
      animation: {
        "accordion-down": "accordion-down 0.2s ease-out",
        "accordion-up": "accordion-up 0.2s ease-out",
        "fade-in": "fade-in 0.5s ease-out",
        "slide-in": "slide-in 0.3s ease-out", 
        "matrix-rain": "matrix-rain 2s linear infinite",
        "neural-pulse": "neural-pulse 2s ease-in-out infinite",
        "sovereign-glow": "sovereign-glow 3s ease-in-out infinite alternate",
      },
      keyframes: {
        "accordion-down": {
          from: { height: "0" },
          to: { height: "var(--radix-accordion-content-height)" },
        },
        "accordion-up": {
          from: { height: "var(--radix-accordion-content-height)" },
          to: { height: "0" },
        },
        "fade-in": {
          "0%": { opacity: "0", transform: "translateY(10px)" },
          "100%": { opacity: "1", transform: "translateY(0)" },
        },
        "slide-in": {
          "0%": { transform: "translateX(-100%)" },
          "100%": { transform: "translateX(0)" },
        },
        "matrix-rain": {
          "0%": { transform: "translateY(-100%)" },
          "100%": { transform: "translateY(100vh)" },
        },
        "neural-pulse": {
          "0%, 100%": { opacity: "1" },
          "50%": { opacity: "0.5" },
        },
        "sovereign-glow": {
          "0%": { 
            "box-shadow": "0 0 20px rgba(0, 255, 65, 0.3)",
            "border-color": "rgba(0, 255, 65, 0.3)"
          },
          "100%": { 
            "box-shadow": "0 0 40px rgba(102, 126, 234, 0.5)",
            "border-color": "rgba(102, 126, 234, 0.5)"
          },
        },
      },
      backdropBlur: {
        'iged': '12px',
      },
      transitionTimingFunction: {
        'iged-ease': 'cubic-bezier(0.4, 0, 0.2, 1)',
        'sovereign': 'cubic-bezier(0.25, 0.46, 0.45, 0.94)',
      },
      transitionDuration: {
        'iged-fast': '150ms',
        'iged-normal': '300ms', 
        'iged-slow': '500ms',
      }
    },
  },
  plugins: [
    require("tailwindcss-animate"),
    // Custom IGED utilities
    function({ addUtilities, addComponents, theme }) {
      // Add IGED specific utilities
      addUtilities({
        '.text-glow': {
          'text-shadow': '0 0 10px currentColor',
        },
        '.text-glow-green': {
          'text-shadow': '0 0 10px #00ff41',
        },
        '.text-glow-blue': {
          'text-shadow': '0 0 10px #667eea',
        },
        '.neural-grid-bg': {
          'background-image': 'radial-gradient(circle at 25px 25px, rgba(255,255,255,0.1) 2px, transparent 0)',
          'background-size': '50px 50px',
        },
        '.matrix-text': {
          'font-family': theme('fontFamily.iged-mono'),
          'color': theme('colors.iged.text-terminal'),
          'text-shadow': '0 0 5px currentColor',
        },
        '.sovereign-border': {
          'border': '1px solid rgba(0, 255, 65, 0.3)',
          'box-shadow': '0 0 20px rgba(0, 255, 65, 0.1)',
        },
        '.glass-effect': {
          'backdrop-filter': 'blur(12px)',
          'background': 'rgba(43, 43, 43, 0.8)',
          'border': '1px solid rgba(255, 255, 255, 0.1)',
        }
      });

      // Add IGED component classes
      addComponents({
        '.iged-button': {
          'padding': `${theme('spacing.iged-md')} ${theme('spacing.iged-xl')}`,
          'border-radius': theme('borderRadius.iged-lg'),
          'font-weight': '600',
          'transition': 'all 300ms ease',
          'position': 'relative',
          'overflow': 'hidden',
          '&:before': {
            'content': '""',
            'position': 'absolute',
            'top': '0',
            'left': '-100%',
            'width': '100%',
            'height': '100%',
            'background': 'linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.1), transparent)',
            'transition': 'left 500ms ease',
          },
          '&:hover:before': {
            'left': '100%',
          },
        },
        '.iged-card': {
          'background': theme('colors.iged.bg-secondary'),
          'border-radius': theme('borderRadius.iged-xl'),
          'padding': theme('spacing.iged-xl'),
          'box-shadow': theme('boxShadow.iged-lg'),
          'border': '1px solid rgba(255, 255, 255, 0.1)',
          'transition': 'all 300ms ease',
          '&:hover': {
            'box-shadow': theme('boxShadow.iged-xl'),
            'border-color': 'rgba(255, 255, 255, 0.15)',
          },
        },
        '.iged-terminal': {
          'background': theme('colors.iged.bg-input'),
          'color': theme('colors.iged.text-terminal'),
          'font-family': theme('fontFamily.iged-mono'),
          'padding': theme('spacing.iged-lg'),
          'border-radius': theme('borderRadius.iged-md'),
          'border': '1px solid rgba(59, 59, 59, 1)',
          'white-space': 'pre-wrap',
          'overflow': 'auto',
        },
      });
    },
  ],
}