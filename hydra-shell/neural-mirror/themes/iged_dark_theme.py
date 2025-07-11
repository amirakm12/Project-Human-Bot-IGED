"""
HYDRA-SHELL Module 5: NEURAL UX MIRROR
Windows GUI Dark Theme Synchronization System

Bridges the IGED design system from web CSS to PyQt6/Tkinter styling,
ensuring perfect visual parity across all interface platforms.
"""

import json
import colorsys
from pathlib import Path
from typing import Dict, Any, Optional, Tuple

class IGEDThemeEngine:
    """
    Core theme engine that converts IGED design tokens 
    between CSS and Python GUI frameworks
    """
    
    def __init__(self):
        self.design_tokens = self._load_design_tokens()
        self.color_palette = self._extract_color_palette()
        
    def _load_design_tokens(self) -> Dict[str, Any]:
        """Load IGED design tokens from CSS/JSON source"""
        return {
            # Background Colors - Deep Charcoal Palette
            "bg_primary": "#1a1a1a",      # Deep charcoal
            "bg_secondary": "#2b2b2b",    # Medium charcoal  
            "bg_tertiary": "#3b3b3b",     # Light charcoal
            "bg_input": "#1e1e1e",        # Terminal black
            
            # Accent Colors - Sovereign AI Palette
            "accent_primary": "#00ff41",     # Matrix green
            "accent_secondary": "#667eea",   # Professional blue
            "accent_tertiary": "#764ba2",    # Deep purple
            
            # Status Colors
            "success": "#27ae60",
            "warning": "#f39c12", 
            "danger": "#e74c3c",
            "info": "#2196F3",
            
            # Text Colors
            "text_primary": "#ffffff",       # Pure white
            "text_secondary": "#b8b8b8",     # Light gray
            "text_muted": "#888888",         # Medium gray
            "text_terminal": "#00ff41",      # Matrix green
            
            # Interactive States
            "hover": "rgba(102, 126, 234, 0.1)",
            "focus": "rgba(0, 255, 65, 0.2)",
            "active": "rgba(102, 126, 234, 0.3)",
            "disabled": "rgba(255, 255, 255, 0.1)",
            
            # Typography
            "font_primary": "Inter",
            "font_mono": "JetBrains Mono",
            
            # Spacing (converted to pixels)
            "spacing_xs": 4,
            "spacing_sm": 8,
            "spacing_md": 12,
            "spacing_lg": 16,
            "spacing_xl": 24,
            "spacing_2xl": 32,
            "spacing_3xl": 48,
            
            # Border Radius
            "radius_sm": 4,
            "radius_md": 6,
            "radius_lg": 8,
            "radius_xl": 12,
            "radius_2xl": 16
        }
    
    def _extract_color_palette(self) -> Dict[str, str]:
        """Extract and organize color palette for GUI frameworks"""
        return {
            key: value for key, value in self.design_tokens.items()
            if isinstance(value, str) and (value.startswith('#') or value.startswith('rgba'))
        }
    
    def hex_to_rgb(self, hex_color: str) -> Tuple[int, int, int]:
        """Convert hex color to RGB tuple"""
        hex_color = hex_color.lstrip('#')
        r = int(hex_color[0:2], 16)
        g = int(hex_color[2:4], 16) 
        b = int(hex_color[4:6], 16)
        return (r, g, b)
    
    def rgb_to_hex(self, r: int, g: int, b: int) -> str:
        """Convert RGB values to hex string"""
        return f"#{r:02x}{g:02x}{b:02x}"
    
    def lighten_color(self, hex_color: str, factor: float = 0.1) -> str:
        """Lighten a color by the given factor"""
        r, g, b = self.hex_to_rgb(hex_color)
        h, l, s = colorsys.rgb_to_hls(r/255, g/255, b/255)
        l = min(1.0, l + factor)
        r, g, b = colorsys.hls_to_rgb(h, l, s)
        return self.rgb_to_hex(int(r*255), int(g*255), int(b*255))
    
    def darken_color(self, hex_color: str, factor: float = 0.1) -> str:
        """Darken a color by the given factor"""
        r, g, b = self.hex_to_rgb(hex_color)
        h, l, s = colorsys.rgb_to_hls(r/255, g/255, b/255)
        l = max(0.0, l - factor)
        r, g, b = colorsys.hls_to_rgb(h, l, s)
        return self.rgb_to_hex(int(r*255), int(g*255), int(b*255))

class PyQt6ThemeGenerator(IGEDThemeEngine):
    """
    Generate PyQt6 stylesheets that match IGED design system
    """
    
    def generate_base_stylesheet(self) -> str:
        """Generate comprehensive PyQt6 stylesheet"""
        tokens = self.design_tokens
        
        return f"""
/* IGED Sovereign AI Theme for PyQt6 */
/* Neural UX Mirror - Perfect Design System Synchronization */

/* Global Application Styling */
QApplication {{
    background-color: {tokens['bg_primary']};
    color: {tokens['text_primary']};
    font-family: "{tokens['font_primary']}", system-ui, sans-serif;
    font-size: 14px;
}}

/* Main Window */
QMainWindow {{
    background-color: {tokens['bg_primary']};
    color: {tokens['text_primary']};
    border: none;
}}

QWidget {{
    background-color: {tokens['bg_primary']};
    color: {tokens['text_primary']};
    selection-background-color: {tokens['accent_secondary']};
    selection-color: {tokens['text_primary']};
}}

/* Buttons - Sovereign AI Styling */
QPushButton {{
    background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1,
                stop: 0 {tokens['accent_primary']}, 
                stop: 0.5 {tokens['accent_secondary']}, 
                stop: 1 {tokens['accent_tertiary']});
    color: {tokens['text_primary']};
    border: none;
    border-radius: {tokens['radius_lg']}px;
    padding: {tokens['spacing_md']}px {tokens['spacing_xl']}px;
    font-weight: 600;
    font-size: 14px;
}}

QPushButton:hover {{
    background: {self.lighten_color(tokens['accent_secondary'], 0.1)};
    transform: translateY(-2px);
}}

QPushButton:pressed {{
    background: {self.darken_color(tokens['accent_secondary'], 0.1)};
    transform: translateY(0px);
}}

QPushButton:disabled {{
    background-color: {tokens['bg_tertiary']};
    color: {tokens['text_muted']};
}}

/* Matrix Terminal Style Button */
QPushButton[class="matrix"] {{
    background-color: {tokens['accent_primary']};
    color: {tokens['bg_primary']};
    font-family: "{tokens['font_mono']}", monospace;
    border: 1px solid {tokens['accent_primary']};
}}

/* Neural Network Style Button */
QPushButton[class="neural"] {{
    background-color: {tokens['accent_secondary']};
    color: {tokens['text_primary']};
    border: 1px solid {tokens['accent_secondary']};
}}

/* Ghost Button */
QPushButton[class="ghost"] {{
    background-color: transparent;
    color: {tokens['accent_primary']};
    border: 2px solid {tokens['accent_primary']};
}}

QPushButton[class="ghost"]:hover {{
    background-color: {tokens['accent_primary']};
    color: {tokens['bg_primary']};
}}

/* Input Fields */
QLineEdit, QTextEdit, QPlainTextEdit {{
    background-color: {tokens['bg_input']};
    color: {tokens['text_primary']};
    border: 2px solid {tokens['bg_tertiary']};
    border-radius: {tokens['radius_md']}px;
    padding: {tokens['spacing_md']}px;
    font-size: 14px;
}}

QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {{
    border-color: {tokens['accent_primary']};
    background-color: {self.lighten_color(tokens['bg_input'], 0.05)};
}}

/* Terminal Style Text Areas */
QTextEdit[class="terminal"], QPlainTextEdit[class="terminal"] {{
    background-color: {tokens['bg_input']};
    color: {tokens['text_terminal']};
    font-family: "{tokens['font_mono']}", monospace;
    border: 1px solid {tokens['bg_tertiary']};
    border-radius: {tokens['radius_md']}px;
}}

/* Cards/Panels */
QFrame {{
    background-color: {tokens['bg_secondary']};
    border: 1px solid rgba(255, 255, 255, 26); /* 10% opacity */
    border-radius: {tokens['radius_xl']}px;
    padding: {tokens['spacing_xl']}px;
}}

QFrame[class="card"] {{
    background-color: {tokens['bg_secondary']};
    border: 1px solid rgba(255, 255, 255, 26);
    border-radius: {tokens['radius_xl']}px;
}}

QFrame[class="neural"] {{
    background-color: {tokens['bg_secondary']};
    border: 1px solid {self.hex_to_rgba(tokens['accent_secondary'], 0.3)};
}}

QFrame[class="glass"] {{
    background-color: rgba(43, 43, 43, 204); /* 80% opacity */
    border: 1px solid rgba(255, 255, 255, 26);
}}

/* Labels */
QLabel {{
    color: {tokens['text_primary']};
    background-color: transparent;
}}

QLabel[class="secondary"] {{
    color: {tokens['text_secondary']};
}}

QLabel[class="muted"] {{
    color: {tokens['text_muted']};
}}

QLabel[class="terminal"] {{
    color: {tokens['text_terminal']};
    font-family: "{tokens['font_mono']}", monospace;
}}

/* Progress Bars */
QProgressBar {{
    background-color: {tokens['bg_tertiary']};
    border: 1px solid {tokens['bg_tertiary']};
    border-radius: {tokens['radius_md']}px;
    text-align: center;
    color: {tokens['text_primary']};
}}

QProgressBar::chunk {{
    background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                stop: 0 {tokens['accent_primary']}, 
                stop: 1 {tokens['accent_secondary']});
    border-radius: {tokens['radius_md']}px;
}}

/* Tabs */
QTabWidget::pane {{
    background-color: {tokens['bg_secondary']};
    border: 1px solid {tokens['bg_tertiary']};
    border-radius: {tokens['radius_lg']}px;
}}

QTabBar::tab {{
    background-color: {tokens['bg_tertiary']};
    color: {tokens['text_muted']};
    padding: {tokens['spacing_lg']}px {tokens['spacing_xl']}px;
    margin-right: 2px;
    border-top-left-radius: {tokens['radius_lg']}px;
    border-top-right-radius: {tokens['radius_lg']}px;
}}

QTabBar::tab:selected {{
    background-color: {tokens['bg_secondary']};
    color: {tokens['accent_primary']};
    border-bottom: 2px solid {tokens['accent_primary']};
}}

QTabBar::tab:hover {{
    background-color: {self.lighten_color(tokens['bg_tertiary'], 0.1)};
    color: {tokens['text_secondary']};
}}

/* Menu Bar */
QMenuBar {{
    background-color: {tokens['bg_secondary']};
    color: {tokens['text_primary']};
    border-bottom: 1px solid {tokens['bg_tertiary']};
}}

QMenuBar::item {{
    background-color: transparent;
    padding: {tokens['spacing_sm']}px {tokens['spacing_lg']}px;
}}

QMenuBar::item:selected {{
    background-color: {tokens['accent_secondary']};
    color: {tokens['text_primary']};
}}

/* Context Menus */
QMenu {{
    background-color: {tokens['bg_secondary']};
    color: {tokens['text_primary']};
    border: 1px solid {tokens['bg_tertiary']};
    border-radius: {tokens['radius_md']}px;
}}

QMenu::item {{
    padding: {tokens['spacing_sm']}px {tokens['spacing_lg']}px;
}}

QMenu::item:selected {{
    background-color: {tokens['accent_secondary']};
    color: {tokens['text_primary']};
}}

/* Scrollbars */
QScrollBar:vertical {{
    background-color: {tokens['bg_secondary']};
    width: 12px;
    border-radius: 6px;
}}

QScrollBar::handle:vertical {{
    background-color: {tokens['bg_tertiary']};
    border-radius: 6px;
    margin: 2px;
}}

QScrollBar::handle:vertical:hover {{
    background-color: {self.lighten_color(tokens['bg_tertiary'], 0.2)};
}}

QScrollBar:horizontal {{
    background-color: {tokens['bg_secondary']};
    height: 12px;
    border-radius: 6px;
}}

QScrollBar::handle:horizontal {{
    background-color: {tokens['bg_tertiary']};
    border-radius: 6px;
    margin: 2px;
}}

/* Status Bar */
QStatusBar {{
    background-color: {tokens['bg_secondary']};
    color: {tokens['text_secondary']};
    border-top: 1px solid {tokens['bg_tertiary']};
}}

/* Splitters */
QSplitter::handle {{
    background-color: {tokens['bg_tertiary']};
}}

QSplitter::handle:hover {{
    background-color: {tokens['accent_secondary']};
}}

/* Checkboxes and Radio Buttons */
QCheckBox, QRadioButton {{
    color: {tokens['text_primary']};
    spacing: {tokens['spacing_sm']}px;
}}

QCheckBox::indicator, QRadioButton::indicator {{
    width: 16px;
    height: 16px;
    border: 2px solid {tokens['bg_tertiary']};
    border-radius: 3px;
    background-color: {tokens['bg_input']};
}}

QCheckBox::indicator:checked, QRadioButton::indicator:checked {{
    background-color: {tokens['accent_primary']};
    border-color: {tokens['accent_primary']};
}}

/* Combo Boxes */
QComboBox {{
    background-color: {tokens['bg_input']};
    color: {tokens['text_primary']};
    border: 2px solid {tokens['bg_tertiary']};
    border-radius: {tokens['radius_md']}px;
    padding: {tokens['spacing_sm']}px {tokens['spacing_md']}px;
}}

QComboBox:focus {{
    border-color: {tokens['accent_primary']};
}}

QComboBox::drop-down {{
    border: none;
    width: 20px;
}}

QComboBox::down-arrow {{
    image: none;
    border-left: 5px solid transparent;
    border-right: 5px solid transparent;
    border-top: 5px solid {tokens['text_primary']};
}}

/* Tool Tips */
QToolTip {{
    background-color: {tokens['bg_tertiary']};
    color: {tokens['text_primary']};
    border: 1px solid {tokens['accent_primary']};
    border-radius: {tokens['radius_sm']}px;
    padding: {tokens['spacing_sm']}px;
    font-size: 12px;
}}
"""
    
    def hex_to_rgba(self, hex_color: str, alpha: float) -> str:
        """Convert hex color to rgba string for PyQt"""
        r, g, b = self.hex_to_rgb(hex_color)
        return f"rgba({r}, {g}, {b}, {int(alpha * 255)})"

class TkinterThemeGenerator(IGEDThemeEngine):
    """
    Generate Tkinter theme configuration that matches IGED design system
    """
    
    def generate_theme_config(self) -> Dict[str, Any]:
        """Generate Tkinter theme configuration dictionary"""
        tokens = self.design_tokens
        
        return {
            'configure': {
                # Main window styling
                'TFrame': {
                    'background': tokens['bg_primary'],
                    'borderwidth': 0
                },
                
                # Labels
                'TLabel': {
                    'background': tokens['bg_primary'],
                    'foreground': tokens['text_primary'],
                    'font': (tokens['font_primary'], 12)
                },
                
                # Buttons - IGED styling
                'TButton': {
                    'background': tokens['accent_secondary'],
                    'foreground': tokens['text_primary'],
                    'borderwidth': 0,
                    'focuscolor': tokens['accent_primary'],
                    'font': (tokens['font_primary'], 12, 'bold'),
                    'relief': 'flat'
                },
                
                # Entry fields
                'TEntry': {
                    'fieldbackground': tokens['bg_input'],
                    'background': tokens['bg_input'],
                    'foreground': tokens['text_primary'],
                    'borderwidth': 2,
                    'relief': 'flat',
                    'insertcolor': tokens['accent_primary']
                },
                
                # Text widgets
                'Text': {
                    'background': tokens['bg_input'],
                    'foreground': tokens['text_primary'],
                    'insertbackground': tokens['accent_primary'],
                    'selectbackground': tokens['accent_secondary'],
                    'selectforeground': tokens['text_primary'],
                    'font': (tokens['font_mono'], 11)
                },
                
                # Notebook (tabs)
                'TNotebook': {
                    'background': tokens['bg_primary'],
                    'borderwidth': 0,
                    'tabmargins': [2, 5, 2, 0]
                },
                
                'TNotebook.Tab': {
                    'background': tokens['bg_tertiary'],
                    'foreground': tokens['text_muted'],
                    'padding': [12, 8],
                    'borderwidth': 0
                }
            },
            
            'map': {
                # Button state mappings
                'TButton': {
                    'background': [
                        ('active', tokens['accent_primary']),
                        ('pressed', self.darken_color(tokens['accent_secondary'], 0.1))
                    ],
                    'foreground': [
                        ('disabled', tokens['text_muted'])
                    ]
                },
                
                # Entry field state mappings
                'TEntry': {
                    'focuscolor': [('focus', tokens['accent_primary'])],
                    'bordercolor': [('focus', tokens['accent_primary'])]
                },
                
                # Tab state mappings
                'TNotebook.Tab': {
                    'background': [
                        ('selected', tokens['bg_secondary']),
                        ('active', self.lighten_color(tokens['bg_tertiary'], 0.1))
                    ],
                    'foreground': [
                        ('selected', tokens['accent_primary']),
                        ('active', tokens['text_secondary'])
                    ]
                }
            }
        }

def apply_iged_theme_to_window(window, framework='pyqt6'):
    """
    Apply IGED theme to a GUI window
    
    Args:
        window: The GUI window object (QMainWindow or Tk root)
        framework: 'pyqt6' or 'tkinter'
    """
    if framework.lower() == 'pyqt6':
        generator = PyQt6ThemeGenerator()
        stylesheet = generator.generate_base_stylesheet()
        window.setStyleSheet(stylesheet)
        
    elif framework.lower() == 'tkinter':
        import tkinter.ttk as ttk
        generator = TkinterThemeGenerator()
        theme_config = generator.generate_theme_config()
        
        style = ttk.Style()
        
        # Apply configurations
        for widget, config in theme_config['configure'].items():
            style.configure(widget, **config)
            
        # Apply state mappings
        for widget, mappings in theme_config['map'].items():
            style.map(widget, **mappings)

def export_theme_tokens(output_path: Optional[str] = None):
    """
    Export IGED design tokens to JSON file for use in other applications
    """
    engine = IGEDThemeEngine()
    
    if output_path is None:
        resolved_path = Path(__file__).parent / "iged_design_tokens.json"
    else:
        resolved_path = Path(output_path)
    
    with open(resolved_path, 'w') as f:
        json.dump(engine.design_tokens, f, indent=2)
    
    print(f"IGED design tokens exported to: {resolved_path}")

if __name__ == "__main__":
    # Export design tokens
    export_theme_tokens()
    
    # Generate and save PyQt6 stylesheet
    pyqt_generator = PyQt6ThemeGenerator()
    stylesheet = pyqt_generator.generate_base_stylesheet()
    
    with open("iged_pyqt6_theme.qss", "w") as f:
        f.write(stylesheet)
    
    print("✅ NEURAL UX MIRROR: IGED theme synchronization complete")
    print("🎨 PyQt6 stylesheet generated: iged_pyqt6_theme.qss")
    print("🧠 Design tokens exported: iged_design_tokens.json")