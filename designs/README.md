# IGED Design System & Assets

## 🎨 Design Philosophy

IGED employs a **sophisticated dark theme** aesthetic that conveys:
- **Professional Authority** - Deep charcoal palettes suggesting enterprise-grade capability
- **Modern Elegance** - Clean lines and subtle highlights for contemporary appeal  
- **Accessibility** - High contrast ratios ensuring readability and WCAG compliance
- **Technical Sophistication** - Monospace fonts and terminal-inspired elements

## 🌑 Color Palette

### Primary Dark Theme
```
Background Colors:
- Primary Background: #1a1a1a    (Deep charcoal)
- Secondary Background: #2b2b2b  (Medium charcoal) 
- Card/Panel Background: #3b3b3b (Light charcoal)
- Input Background: #1e1e1e      (Terminal black)

Accent Colors:
- Primary Accent: #00ff41         (Matrix green)
- Secondary Accent: #667eea       (Professional blue)
- Success: #27ae60               (Green)
- Warning: #f39c12               (Orange)
- Danger: #e74c3c                (Red)
- Info: #2196F3                  (Blue)

Text Colors:
- Primary Text: #ffffff           (Pure white)
- Secondary Text: #b8b8b8         (Light gray)
- Muted Text: #888888            (Medium gray)
- Terminal Text: #00ff41          (Matrix green)
```

### Highlight Colors
```
Interactive States:
- Hover: rgba(102, 126, 234, 0.1)
- Focus: rgba(0, 255, 65, 0.2) 
- Active: rgba(102, 126, 234, 0.3)
- Disabled: rgba(255, 255, 255, 0.1)
```

## 📐 Typography

### Font Hierarchy
```
Primary Font: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif
Monospace Font: 'JetBrains Mono', 'Consolas', 'Monaco', monospace
Code Font: 'Fira Code', 'Consolas', monospace

Sizes:
- h1: 32px / 2rem (Page titles)
- h2: 24px / 1.5rem (Section headers)  
- h3: 20px / 1.25rem (Subsections)
- body: 16px / 1rem (Main content)
- small: 14px / 0.875rem (Captions)
- xs: 12px / 0.75rem (Labels)
```

## 🎯 Iconography

### Icon Style Guidelines
- **Style**: Minimalist line icons with 2px stroke weight
- **Size**: 24px standard, 16px small, 32px large
- **Format**: SVG for scalability and crispness
- **Color**: Inherit from parent or use accent colors

### Icon Categories
```
Navigation: home, settings, dashboard, search
Actions: play, pause, stop, refresh, save, delete
Status: success, warning, error, info, loading
Features: microphone, shield, brain, robot, terminal
Files: folder, file, image, code, download
```

## 🖼️ Asset Organization

```
/designs/
├── icons/                 # SVG icons
│   ├── navigation/        # Nav icons
│   ├── actions/          # Action buttons
│   ├── status/           # Status indicators  
│   └── features/         # Feature-specific icons
├── illustrations/        # Hero graphics & illustrations
├── logos/               # Brand assets
├── patterns/            # Background patterns & textures
├── mockups/             # UI/UX wireframes
└── exports/             # Optimized production assets
```

## 🎭 Component Specifications

### Buttons
```css
Primary Button:
- Background: linear-gradient(135deg, #667eea, #764ba2)
- Border-radius: 8px
- Padding: 12px 24px
- Font-weight: 600
- Transition: all 0.3s ease
- Hover: transform: translateY(-2px)

Secondary Button:
- Background: transparent
- Border: 2px solid #667eea
- Color: #667eea
```

### Cards
```css
Standard Card:
- Background: #2b2b2b
- Border-radius: 12px
- Box-shadow: 0 4px 20px rgba(0, 0, 0, 0.4)
- Border: 1px solid rgba(255, 255, 255, 0.1)
- Padding: 24px
```

### Input Fields
```css
Text Input:
- Background: #1e1e1e
- Border: 2px solid #3b3b3b
- Border-radius: 6px
- Padding: 12px 16px
- Focus: border-color: #00ff41
```

## 🚀 Asset Specifications

### Icon Requirements
- **Format**: SVG (vector)
- **Viewbox**: 0 0 24 24 (standardized)
- **Stroke width**: 2px
- **Fill**: none (stroke-based)
- **Colors**: Use currentColor for inheritance

### Image Requirements  
- **Format**: WebP (primary), PNG (fallback)
- **Optimization**: Compressed for web
- **Sizes**: 1x, 2x, 3x for different pixel densities
- **Max file size**: 100KB per asset

### Logo Specifications
- **Formats**: SVG (primary), PNG (raster backup)
- **Variants**: Full logo, icon mark, wordmark
- **Sizes**: 32px, 64px, 128px, 256px
- **Background**: Transparent

## ⚡ Performance Guidelines

### Optimization Standards
- SVG icons: Optimized with SVGO
- Images: WebP format with PNG fallback
- File sizes: <50KB per icon, <200KB per illustration
- Loading: Lazy load non-critical assets
- Caching: Set appropriate cache headers

### Accessibility Requirements
- Color contrast: Minimum 4.5:1 ratio
- Alternative text: All images have descriptive alt text
- Scalability: Assets scale cleanly from 50%-200%
- High contrast mode: Compatible with system preferences

## 📱 Responsive Considerations

### Breakpoints
```
Mobile: 320px - 768px
Tablet: 768px - 1024px  
Desktop: 1024px+
```

### Scaling Rules
- Icons: Maintain aspect ratio, never pixelate
- Text: Minimum 14px on mobile
- Touch targets: Minimum 44px tap area
- Spacing: Use relative units (rem, em)

## 🔄 Version Control

### Naming Conventions
```
Icons: icon-[category]-[name].svg
Illustrations: illus-[context]-[name].webp
Logos: logo-[variant]-[size].svg
Patterns: pattern-[name]-[variant].svg
```

### Change Management
- Major changes: Increment version number
- Minor updates: Maintain backward compatibility  
- Archive: Keep previous versions in `/archives/`
- Documentation: Update this README for any changes

---

*Last updated: $(date)*
*Version: 1.0.0*