#!/usr/bin/env python3
"""
IGED - Add ALL Files to GitHub Repository
Comprehensive script to add all project files and folders
"""

import os
import subprocess
import sys
from pathlib import Path

def run_command(command, description):
    """Run a git command and return success status"""
    print(f"🔄 {description}...")
    try:
        result = subprocess.run(command, shell=True, capture_output=True, text=True)
        if result.returncode == 0:
            print(f"✅ {description} completed successfully")
            return True, result.stdout
        else:
            print(f"❌ {description} failed: {result.stderr}")
            return False, result.stderr
    except Exception as e:
        print(f"❌ {description} error: {e}")
        return False, str(e)

def get_file_count():
    """Count total files in the project"""
    count = 0
    for root, dirs, files in os.walk('.'):
        # Skip .git directory
        if '.git' in root:
            continue
        count += len(files)
    return count

def main():
    print("🚀 IGED - Add ALL Files to GitHub Repository")
    print("=" * 60)
    print()
    
    # Check if we're in the right directory
    if not os.path.exists('launcher.py'):
        print("❌ Error: launcher.py not found. Please run this script from the IGED project root directory.")
        sys.exit(1)
    
    # Count files
    print("📊 Counting files...")
    file_count = get_file_count()
    print(f"📁 Found {file_count} files to add")
    print()
    
    # Initialize git if needed
    if not os.path.exists('.git'):
        success, _ = run_command('git init', 'Initializing git repository')
        if not success:
            sys.exit(1)
        print()
    
    # Add remote if needed
    success, output = run_command('git remote -v', 'Checking remote repository')
    if not success or 'origin' not in output:
        success, _ = run_command(
            'git remote add origin https://github.com/amirakm12/Project-Human-Bot-IGED.git',
            'Adding remote repository'
        )
        if not success:
            sys.exit(1)
        print()
    
    # Add all files
    print("📦 Adding all files and folders...")
    success, output = run_command('git add .', 'Adding all files to git')
    if not success:
        sys.exit(1)
    print()
    
    # Check if there are changes to commit
    success, output = run_command('git status --porcelain', 'Checking for changes')
    if not success:
        sys.exit(1)
    
    if not output.strip():
        print("ℹ️ No changes to commit. All files are already up to date.")
        return
    
    # Commit changes
    commit_message = """Add complete IGED project with all files and folders

- Core system components (voice pipeline, encryption, memory engine)
- Agent modules (codegen, secops, data miner, network intelligence) 
- Plugin system with exploit developer and system info
- Windows GUI and web admin panel
- Android client integration
- Build scripts and installation tools
- Documentation and configuration files
- Biometric authentication integration suite
- Complete project structure with all dependencies

Total files: {file_count}"""
    
    success, _ = run_command(f'git commit -m "{commit_message}"', 'Committing all changes')
    if not success:
        sys.exit(1)
    print()
    
    # Push to GitHub
    print("🚀 Pushing to GitHub...")
    success, _ = run_command('git push -u origin main', 'Pushing to main branch')
    
    if not success:
        print("🔄 Trying master branch...")
        success, _ = run_command('git push -u origin master', 'Pushing to master branch')
        if not success:
            print("❌ Failed to push to GitHub. Please check your credentials and try again.")
            sys.exit(1)
    
    print()
    print("🎉 SUCCESS! All files have been added to GitHub!")
    print("=" * 60)
    print()
    print("📋 Repository: https://github.com/amirakm12/Project-Human-Bot-IGED")
    print(f"📁 Files added: {file_count}")
    print()
    print("📦 Included components:")
    print("   ✅ Core system (core/)")
    print("   ✅ Agent modules (agents/)")
    print("   ✅ Plugin system (plugins/)")
    print("   ✅ User interfaces (ui/, admin_panel/)")
    print("   ✅ Android client (android_client/, android-client/)")
    print("   ✅ Build scripts and installers")
    print("   ✅ Documentation files")
    print("   ✅ Configuration and logs")
    print("   ✅ Biometric authentication suite")
    print("   ✅ All subdirectories and their contents")
    print()
    print("🚀 Your IGED project is now fully uploaded to GitHub!")

if __name__ == "__main__":
    main() 