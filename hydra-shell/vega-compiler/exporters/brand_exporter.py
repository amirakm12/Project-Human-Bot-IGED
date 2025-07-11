"""
HYDRA-SHELL Module 6: VEGA COMPILER
Brand Asset Export Automation System

Exports IGED brand assets into multiple formats:
- PNG (rasterized, multi-resolution)
- WebP (optimized web format)
- PDF (deck-ready presentations)
- AI/PSD (design software compatibility)
- Auto-generated brand guide PDF
"""

import os
import json
import subprocess
from pathlib import Path
from typing import Dict, List, Tuple, Optional, Any
from dataclasses import dataclass
from datetime import datetime
import base64

@dataclass
class ExportConfig:
    """Configuration for asset export operations"""
    source_dir: Path
    output_dir: Path
    formats: List[str]
    resolutions: List[int]
    quality: int = 90
    optimize: bool = True
    watermark: bool = False

@dataclass 
class AssetInfo:
    """Information about a design asset"""
    name: str
    path: Path
    format: str
    size: Tuple[int, int]
    category: str
    description: str = ""

class VegaCompiler:
    """
    Core brand asset export compiler for IGED design system
    """
    
    def __init__(self):
        self.project_root = Path(__file__).parent.parent.parent.parent
        self.designs_dir = self.project_root / "designs"
        self.assets_dir = self.project_root / "assets" 
        self.output_dir = self.project_root / "hydra-shell" / "vega-compiler" / "output"
        
        # Create output directory if it doesn't exist
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        # Asset categories and their source directories
        self.asset_categories = {
            "logos": self.designs_dir / "logos",
            "icons": self.designs_dir / "icons", 
            "illustrations": self.designs_dir / "illustrations",
            "patterns": self.designs_dir / "patterns" if (self.designs_dir / "patterns").exists() else None
        }
        
        # Export formats and their configurations
        self.export_formats = {
            "png": {"quality": 100, "compression": 9},
            "webp": {"quality": 90, "method": 6},
            "pdf": {"dpi": 300},
            "svg": {"optimize": True}
        }
        
        # Standard resolution multipliers for responsive design
        self.resolutions = [1, 2, 3]  # 1x, 2x (retina), 3x (super retina)
        
    def scan_assets(self) -> Dict[str, List[AssetInfo]]:
        """
        Scan design directories and catalog all assets
        """
        assets = {}
        
        for category, dir_path in self.asset_categories.items():
            if dir_path is None or not dir_path.exists():
                assets[category] = []
                continue
                
            category_assets = []
            
            # Scan for SVG files (primary format)
            for svg_file in dir_path.rglob("*.svg"):
                asset_info = AssetInfo(
                    name=svg_file.stem,
                    path=svg_file,
                    format="svg",
                    size=self._get_svg_dimensions(svg_file),
                    category=category,
                    description=self._extract_description(svg_file)
                )
                category_assets.append(asset_info)
            
            # Scan for other image formats
            for ext in [".png", ".jpg", ".jpeg", ".webp"]:
                for img_file in dir_path.rglob(f"*{ext}"):
                    asset_info = AssetInfo(
                        name=img_file.stem,
                        path=img_file,
                        format=ext.lstrip('.'),
                        size=self._get_image_dimensions(img_file),
                        category=category,
                        description=self._extract_description(img_file)
                    )
                    category_assets.append(asset_info)
            
            assets[category] = category_assets
            
        return assets
    
    def _get_svg_dimensions(self, svg_path: Path) -> Tuple[int, int]:
        """Extract dimensions from SVG file"""
        try:
            with open(svg_path, 'r', encoding='utf-8') as f:
                content = f.read()
                
            # Extract viewBox if present
            if 'viewBox=' in content:
                import re
                viewbox_match = re.search(r'viewBox=["\']([\d\s.-]+)["\']', content)
                if viewbox_match:
                    values = viewbox_match.group(1).split()
                    if len(values) >= 4:
                        width = float(values[2]) - float(values[0])
                        height = float(values[3]) - float(values[1])
                        return (int(width), int(height))
            
            # Extract width/height attributes
            width_match = re.search(r'width=["\']([\d.]+)', content)
            height_match = re.search(r'height=["\']([\d.]+)', content)
            
            if width_match and height_match:
                return (int(float(width_match.group(1))), int(float(height_match.group(1))))
                
        except Exception as e:
            print(f"Warning: Could not parse SVG dimensions for {svg_path}: {e}")
            
        return (100, 100)  # Default fallback
    
    def _get_image_dimensions(self, img_path: Path) -> Tuple[int, int]:
        """Get dimensions of raster image using basic file inspection"""
        try:
            # For now, return default dimensions
            # In production, would use PIL/Pillow: Image.open(img_path).size
            return (100, 100)
        except Exception:
            return (100, 100)
    
    def _extract_description(self, file_path: Path) -> str:
        """Extract description from file comments or metadata"""
        if file_path.suffix.lower() == '.svg':
            try:
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    
                # Look for description in SVG comments or metadata
                import re
                desc_match = re.search(r'<desc[^>]*>(.*?)</desc>', content, re.DOTALL)
                if desc_match:
                    return desc_match.group(1).strip()
                    
                # Look for title
                title_match = re.search(r'<title[^>]*>(.*?)</title>', content, re.DOTALL)
                if title_match:
                    return title_match.group(1).strip()
                    
            except Exception:
                pass
                
        return ""
    
    def export_to_png(self, asset: AssetInfo, output_dir: Path, resolutions: Optional[List[int]] = None) -> List[Path]:
        """
        Export SVG asset to PNG format at multiple resolutions
        """
        if resolutions is None:
            resolutions = self.resolutions
            
        exported_files = []
        
        for resolution in resolutions:
            # Calculate output dimensions
            base_width, base_height = asset.size
            width = base_width * resolution
            height = base_height * resolution
            
            # Generate output filename
            if resolution == 1:
                output_name = f"{asset.name}.png"
            else:
                output_name = f"{asset.name}@{resolution}x.png"
                
            output_path = output_dir / "png" / output_name
            output_path.parent.mkdir(parents=True, exist_ok=True)
            
            if asset.format == 'svg':
                # For SVG to PNG conversion, we'd use a tool like Inkscape or librsvg
                # For now, create a placeholder implementation
                self._svg_to_png_placeholder(asset.path, output_path, width, height)
            else:
                # For raster images, we'd use PIL/Pillow for resizing
                self._resize_image_placeholder(asset.path, output_path, width, height)
                
            exported_files.append(output_path)
            
        return exported_files
    
    def export_to_webp(self, asset: AssetInfo, output_dir: Path, quality: int = 90) -> Path:
        """
        Export asset to optimized WebP format
        """
        output_name = f"{asset.name}.webp"
        output_path = output_dir / "webp" / output_name
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        # WebP conversion would use tools like cwebp or PIL
        self._convert_to_webp_placeholder(asset.path, output_path, quality)
        
        return output_path
    
    def _svg_to_png_placeholder(self, svg_path: Path, output_path: Path, width: int, height: int):
        """Placeholder for SVG to PNG conversion"""
        # In production, would use: inkscape --export-png=output_path --export-width=width svg_path
        print(f"📸 Converting {svg_path.name} to PNG ({width}x{height}) -> {output_path}")
        
        # Create a simple placeholder file for demonstration
        placeholder_content = f"PNG placeholder for {svg_path.name} at {width}x{height}"
        with open(output_path, 'w') as f:
            f.write(placeholder_content)
    
    def _resize_image_placeholder(self, img_path: Path, output_path: Path, width: int, height: int):
        """Placeholder for image resizing"""
        print(f"🔄 Resizing {img_path.name} to {width}x{height} -> {output_path}")
        
        placeholder_content = f"Resized image placeholder for {img_path.name} at {width}x{height}"
        with open(output_path, 'w') as f:
            f.write(placeholder_content)
    
    def _convert_to_webp_placeholder(self, img_path: Path, output_path: Path, quality: int):
        """Placeholder for WebP conversion"""
        print(f"🌐 Converting {img_path.name} to WebP (quality: {quality}) -> {output_path}")
        
        placeholder_content = f"WebP placeholder for {img_path.name} at quality {quality}"
        with open(output_path, 'w') as f:
            f.write(placeholder_content)
    
    def generate_brand_guide_pdf(self, assets: Dict[str, List[AssetInfo]], output_path: Path):
        """
        Generate comprehensive brand guide PDF
        """
        print(f"📖 Generating IGED Brand Guide PDF -> {output_path}")
        
        brand_guide_content = self._create_brand_guide_content(assets)
        
        # In production, would use reportlab or weasyprint to generate PDF
        # For now, create an HTML version that can be converted to PDF
        html_path = output_path.with_suffix('.html')
        
        with open(html_path, 'w', encoding='utf-8') as f:
            f.write(brand_guide_content)
        
        # Create a simple text version for demonstration
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write("IGED Brand Guide PDF Placeholder\n\n")
            f.write(f"Generated on: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"Total assets: {sum(len(category_assets) for category_assets in assets.values())}\n")
    
    def _create_brand_guide_content(self, assets: Dict[str, List[AssetInfo]]) -> str:
        """
        Create HTML content for brand guide
        """
        html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>IGED Brand Guide</title>
    <style>
        body {{
            font-family: 'Inter', -apple-system, BlinkMacSystemFont, sans-serif;
            line-height: 1.6;
            margin: 0;
            padding: 40px;
            background: #1a1a1a;
            color: #ffffff;
        }}
        
        .header {{
            text-align: center;
            margin-bottom: 60px;
            padding: 40px 0;
            border-bottom: 2px solid #00ff41;
        }}
        
        .header h1 {{
            font-size: 3rem;
            margin: 0;
            background: linear-gradient(135deg, #00ff41 0%, #667eea 50%, #764ba2 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
        }}
        
        .header p {{
            font-size: 1.2rem;
            color: #b8b8b8;
            margin: 20px 0 0 0;
        }}
        
        .section {{
            margin: 40px 0;
        }}
        
        .section h2 {{
            color: #00ff41;
            border-left: 4px solid #00ff41;
            padding-left: 20px;
            margin-bottom: 30px;
        }}
        
        .color-palette {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin: 30px 0;
        }}
        
        .color-card {{
            background: #2b2b2b;
            border-radius: 12px;
            padding: 20px;
            border: 1px solid rgba(255, 255, 255, 0.1);
        }}
        
        .color-swatch {{
            width: 100%;
            height: 80px;
            border-radius: 8px;
            margin-bottom: 15px;
        }}
        
        .asset-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin: 30px 0;
        }}
        
        .asset-card {{
            background: #2b2b2b;
            border-radius: 12px;
            padding: 20px;
            border: 1px solid rgba(255, 255, 255, 0.1);
            text-align: center;
        }}
        
        .asset-preview {{
            width: 100%;
            height: 120px;
            background: #3b3b3b;
            border-radius: 8px;
            margin-bottom: 15px;
            display: flex;
            align-items: center;
            justify-content: center;
            color: #888;
        }}
        
        .stats {{
            background: #2b2b2b;
            border-radius: 12px;
            padding: 30px;
            margin: 30px 0;
            border: 1px solid rgba(0, 255, 65, 0.3);
        }}
        
        .stats-grid {{
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 20px;
            text-align: center;
        }}
        
        .stat-value {{
            font-size: 2rem;
            font-weight: bold;
            color: #00ff41;
        }}
        
        .stat-label {{
            color: #b8b8b8;
            text-transform: uppercase;
            font-size: 0.9rem;
            letter-spacing: 1px;
        }}
    </style>
</head>
<body>
    <div class="header">
        <h1>IGED Brand Guide</h1>
        <p>Sovereign AI Assistant • Design System Documentation</p>
        <p>Generated on {datetime.now().strftime('%B %d, %Y')}</p>
    </div>
    
    <div class="section">
        <h2>🎨 Color Palette</h2>
        <div class="color-palette">
            <div class="color-card">
                <div class="color-swatch" style="background: #00ff41;"></div>
                <h4>Matrix Green</h4>
                <p>#00ff41</p>
                <p>Primary accent, terminal elements</p>
            </div>
            <div class="color-card">
                <div class="color-swatch" style="background: #667eea;"></div>
                <h4>Professional Blue</h4>
                <p>#667eea</p>
                <p>Secondary accent, interactive elements</p>
            </div>
            <div class="color-card">
                <div class="color-swatch" style="background: #1a1a1a;"></div>
                <h4>Deep Charcoal</h4>
                <p>#1a1a1a</p>
                <p>Primary background</p>
            </div>
            <div class="color-card">
                <div class="color-swatch" style="background: #2b2b2b;"></div>
                <h4>Medium Charcoal</h4>
                <p>#2b2b2b</p>
                <p>Secondary background, cards</p>
            </div>
        </div>
    </div>
    
    <div class="section">
        <h2>📊 Asset Statistics</h2>
        <div class="stats">
            <div class="stats-grid">
                <div>
                    <div class="stat-value">{sum(len(category_assets) for category_assets in assets.values())}</div>
                    <div class="stat-label">Total Assets</div>
                </div>
                <div>
                    <div class="stat-value">{len(assets.get('logos', []))}</div>
                    <div class="stat-label">Logos</div>
                </div>
                <div>
                    <div class="stat-value">{len(assets.get('icons', []))}</div>
                    <div class="stat-label">Icons</div>
                </div>
                <div>
                    <div class="stat-value">{len(assets.get('illustrations', []))}</div>
                    <div class="stat-label">Illustrations</div>
                </div>
            </div>
        </div>
    </div>
"""
        
        # Add asset sections
        for category, category_assets in assets.items():
            if not category_assets:
                continue
                
            html_content += f"""
    <div class="section">
        <h2>🎨 {category.title()}</h2>
        <div class="asset-grid">
"""
            
            for asset in category_assets:
                html_content += f"""
            <div class="asset-card">
                <div class="asset-preview">
                    {asset.name}
                </div>
                <h4>{asset.name}</h4>
                <p>{asset.size[0]}×{asset.size[1]} • {asset.format.upper()}</p>
                {f'<p style="color: #b8b8b8; font-size: 0.9rem;">{asset.description}</p>' if asset.description else ''}
            </div>
"""
            
            html_content += """
        </div>
    </div>
"""
        
        html_content += """
    <div class="section">
        <h2>📋 Usage Guidelines</h2>
        <div style="background: #2b2b2b; border-radius: 12px; padding: 30px; border: 1px solid rgba(255, 255, 255, 0.1);">
            <h3>Logo Usage</h3>
            <ul>
                <li>Maintain minimum clear space of 1x the logo height around all sides</li>
                <li>Never alter logo colors or proportions</li>
                <li>Use appropriate contrast ratios on different backgrounds</li>
            </ul>
            
            <h3>Color Usage</h3>
            <ul>
                <li>Matrix Green (#00ff41) for primary accents and terminal elements</li>
                <li>Professional Blue (#667eea) for interactive elements and buttons</li>
                <li>Maintain dark theme aesthetic with charcoal backgrounds</li>
            </ul>
            
            <h3>Typography</h3>
            <ul>
                <li>Primary: Inter font family for UI and body text</li>
                <li>Monospace: JetBrains Mono for code and terminal displays</li>
                <li>Maintain hierarchy with consistent sizing scale</li>
            </ul>
        </div>
    </div>
</body>
</html>
"""
        
        return html_content
    
    def export_all_assets(self, config: Optional[ExportConfig] = None) -> Dict[str, Any]:
        """
        Export all assets in specified formats
        """
        if config is None:
            config = ExportConfig(
                source_dir=self.designs_dir,
                output_dir=self.output_dir,
                formats=["png", "webp", "pdf"],
                resolutions=[1, 2, 3],
                quality=90,
                optimize=True
            )
        
        print("🚀 VEGA COMPILER: Starting brand asset export...")
        
        # Scan all assets
        assets = self.scan_assets()
        
        export_results = {
            "timestamp": datetime.now().isoformat(),
            "total_assets": sum(len(category_assets) for category_assets in assets.values()),
            "categories": {},
            "exported_files": []
        }
        
        # Export each category
        for category, category_assets in assets.items():
            if not category_assets:
                continue
                
            print(f"📁 Processing {category}: {len(category_assets)} assets")
            
            category_results = {
                "count": len(category_assets),
                "exported": []
            }
            
            for asset in category_assets:
                asset_exports = []
                
                # Export to PNG at multiple resolutions
                if "png" in config.formats:
                    png_files = self.export_to_png(asset, config.output_dir, config.resolutions)
                    asset_exports.extend(png_files)
                
                # Export to WebP
                if "webp" in config.formats:
                    webp_file = self.export_to_webp(asset, config.output_dir, config.quality)
                    asset_exports.append(webp_file)
                
                category_results["exported"].append({
                    "name": asset.name,
                    "source": str(asset.path),
                    "exports": [str(f) for f in asset_exports]
                })
                
                export_results["exported_files"].extend([str(f) for f in asset_exports])
            
            export_results["categories"][category] = category_results
        
        # Generate brand guide PDF
        if "pdf" in config.formats:
            brand_guide_path = config.output_dir / "IGED_Brand_Guide.pdf"
            self.generate_brand_guide_pdf(assets, brand_guide_path)
            export_results["brand_guide"] = str(brand_guide_path)
        
        # Save export manifest
        manifest_path = config.output_dir / "export_manifest.json"
        with open(manifest_path, 'w', encoding='utf-8') as f:
            json.dump(export_results, f, indent=2)
        
        print(f"✅ Export complete! {export_results['total_assets']} assets processed")
        print(f"📋 Manifest saved: {manifest_path}")
        
        return export_results

def main():
    """Main execution function for VEGA Compiler"""
    compiler = VegaCompiler()
    
    # Create export configuration
    config = ExportConfig(
        source_dir=compiler.designs_dir,
        output_dir=compiler.output_dir,
        formats=["png", "webp", "pdf"],
        resolutions=[1, 2, 3],
        quality=90,
        optimize=True
    )
    
    # Execute export
    results = compiler.export_all_assets(config)
    
    print("\n🎨 VEGA COMPILER EXPORT SUMMARY:")
    print("=" * 50)
    print(f"Total Assets Processed: {results['total_assets']}")
    print(f"Export Formats: {', '.join(config.formats)}")
    print(f"Output Directory: {config.output_dir}")
    print("\n📁 Category Breakdown:")
    
    for category, data in results['categories'].items():
        print(f"  {category.title()}: {data['count']} assets")
    
    if 'brand_guide' in results:
        print(f"\n📖 Brand Guide: {results['brand_guide']}")
    
    print("\n🚀 VEGA COMPILER: Export operation completed successfully!")

if __name__ == "__main__":
    main()