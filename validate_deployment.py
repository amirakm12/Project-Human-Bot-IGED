#!/usr/bin/env python3
"""
IGED Deployment Validation Script
Validates that all components are properly configured for enterprise deployment
"""

import os
import sys
import json
import subprocess
from pathlib import Path
from datetime import datetime

# Colors for terminal output
class Colors:
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BLUE = '\033[94m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'

def print_status(status, message):
    """Print colored status message"""
    if status == "success":
        print(f"{Colors.GREEN}✓{Colors.ENDC} {message}")
    elif status == "warning":
        print(f"{Colors.YELLOW}⚠{Colors.ENDC} {message}")
    elif status == "error":
        print(f"{Colors.RED}✗{Colors.ENDC} {message}")
    elif status == "info":
        print(f"{Colors.BLUE}ℹ{Colors.ENDC} {message}")

def check_directories():
    """Check if all required directories exist"""
    print(f"\n{Colors.BOLD}Checking Directory Structure...{Colors.ENDC}")
    
    required_dirs = [
        "logs",
        "output",
        "output/codegen",
        "output/data_analysis",
        "output/security",
        "output/network_intelligence",
        "output/remote_control",
        "templates",
        "config",
        "memory",
        "tests",
        "tests/unit",
        "tests/integration",
        "tests/security",
        "tests/performance",
        ".github/workflows"
    ]
    
    all_exist = True
    for dir_path in required_dirs:
        if Path(dir_path).exists():
            print_status("success", f"Directory exists: {dir_path}")
        else:
            print_status("error", f"Missing directory: {dir_path}")
            all_exist = False
    
    return all_exist

def check_files():
    """Check if all critical files exist"""
    print(f"\n{Colors.BOLD}Checking Critical Files...{Colors.ENDC}")
    
    required_files = [
        ".env",
        "requirements.txt",
        "requirements-dev.txt",
        "Dockerfile",
        "docker-compose.yml",
        "pytest.ini",
        ".github/workflows/ci.yml",
        "prometheus.yml",
        "deploy.sh"
    ]
    
    all_exist = True
    for file_path in required_files:
        if Path(file_path).exists():
            print_status("success", f"File exists: {file_path}")
        else:
            print_status("error", f"Missing file: {file_path}")
            all_exist = False
    
    return all_exist

def check_security():
    """Check security configurations"""
    print(f"\n{Colors.BOLD}Checking Security Configuration...{Colors.ENDC}")
    
    issues = []
    
    # Check for hardcoded secrets
    dangerous_patterns = ["password=", "secret=", "api_key=", "token="]
    python_files = Path(".").rglob("*.py")
    
    for py_file in python_files:
        if "test" in str(py_file) or ".env" in str(py_file) or "validate_deployment" in str(py_file):
            continue
        
        try:
            with open(py_file, 'r') as f:
                content = f.read()
                for pattern in dangerous_patterns:
                    if pattern in content.lower():
                        lines = content.split('\n')
                        for i, line in enumerate(lines):
                            if pattern in line.lower() and '"' in line and "os.environ" not in line:
                                issues.append(f"Potential hardcoded secret in {py_file}:{i+1}")
        except Exception as e:
            print_status("warning", f"Could not check {py_file}: {e}")
    
    if not issues:
        print_status("success", "No hardcoded secrets detected")
    else:
        for issue in issues[:5]:  # Show first 5 issues
            print_status("error", issue)
    
    # Check .env file
    if Path(".env").exists():
        print_status("success", "Environment configuration file exists")
        
        # Check if critical env vars are set
        with open(".env", 'r') as f:
            env_content = f.read()
            critical_vars = ["FLASK_SECRET_KEY", "DATABASE_URL", "LOG_LEVEL"]
            for var in critical_vars:
                if var in env_content:
                    print_status("success", f"Environment variable set: {var}")
                else:
                    print_status("warning", f"Missing environment variable: {var}")
    else:
        print_status("error", ".env file missing")
    
    return len(issues) == 0

def check_tests():
    """Check test coverage"""
    print(f"\n{Colors.BOLD}Checking Test Coverage...{Colors.ENDC}")
    
    test_files = list(Path("tests").rglob("test_*.py"))
    
    if len(test_files) > 0:
        print_status("success", f"Found {len(test_files)} test files")
        
        # Count test functions
        test_count = 0
        for test_file in test_files:
            with open(test_file, 'r') as f:
                content = f.read()
                test_count += content.count("def test_")
        
        print_status("info", f"Total test functions: {test_count}")
        
        if test_count >= 30:
            print_status("success", "Adequate test coverage")
        else:
            print_status("warning", "Consider adding more tests")
    else:
        print_status("error", "No test files found")
        return False
    
    return True

def check_dependencies():
    """Check Python dependencies"""
    print(f"\n{Colors.BOLD}Checking Dependencies...{Colors.ENDC}")
    
    try:
        result = subprocess.run(
            ["python3", "-m", "pip", "check"],
            capture_output=True,
            text=True
        )
        
        if result.returncode == 0:
            print_status("success", "No dependency conflicts")
        else:
            print_status("error", "Dependency conflicts detected")
            print(result.stdout)
    except Exception as e:
        print_status("warning", f"Could not check dependencies: {e}")
    
    return True

def check_docker():
    """Check Docker configuration"""
    print(f"\n{Colors.BOLD}Checking Docker Configuration...{Colors.ENDC}")
    
    if Path("Dockerfile").exists():
        print_status("success", "Dockerfile exists")
        
        # Check Dockerfile best practices
        with open("Dockerfile", 'r') as f:
            dockerfile_content = f.read()
            
            if "USER" in dockerfile_content:
                print_status("success", "Non-root user configured")
            else:
                print_status("warning", "Consider running as non-root user")
            
            if "HEALTHCHECK" in dockerfile_content:
                print_status("success", "Health check configured")
            else:
                print_status("warning", "Consider adding HEALTHCHECK")
    
    if Path("docker-compose.yml").exists():
        print_status("success", "Docker Compose configuration exists")
    
    return True

def check_ci_cd():
    """Check CI/CD configuration"""
    print(f"\n{Colors.BOLD}Checking CI/CD Pipeline...{Colors.ENDC}")
    
    ci_file = Path(".github/workflows/ci.yml")
    
    if ci_file.exists():
        print_status("success", "GitHub Actions workflow exists")
        
        with open(ci_file, 'r') as f:
            ci_content = f.read()
            
            # Check for essential jobs
            jobs = ["lint", "test", "security", "build"]
            for job in jobs:
                if f"name: {job}" in ci_content.lower() or f"{job}:" in ci_content:
                    print_status("success", f"CI job configured: {job}")
                else:
                    print_status("warning", f"Missing CI job: {job}")
    else:
        print_status("error", "CI/CD pipeline not configured")
        return False
    
    return True

def check_monitoring():
    """Check monitoring configuration"""
    print(f"\n{Colors.BOLD}Checking Monitoring Configuration...{Colors.ENDC}")
    
    if Path("prometheus.yml").exists():
        print_status("success", "Prometheus configuration exists")
    else:
        print_status("warning", "Prometheus configuration missing")
    
    if Path("monitoring/health_monitor.py").exists():
        print_status("success", "Health monitoring module exists")
    else:
        print_status("warning", "Health monitoring module missing")
    
    return True

def generate_report():
    """Generate deployment readiness report"""
    print(f"\n{Colors.BOLD}{'='*60}{Colors.ENDC}")
    print(f"{Colors.BOLD}DEPLOYMENT READINESS REPORT{Colors.ENDC}")
    print(f"{Colors.BOLD}{'='*60}{Colors.ENDC}")
    
    checks = {
        "Directory Structure": check_directories(),
        "Critical Files": check_files(),
        "Security Configuration": check_security(),
        "Test Coverage": check_tests(),
        "Dependencies": check_dependencies(),
        "Docker Configuration": check_docker(),
        "CI/CD Pipeline": check_ci_cd(),
        "Monitoring": check_monitoring()
    }
    
    passed = sum(1 for v in checks.values() if v)
    total = len(checks)
    
    print(f"\n{Colors.BOLD}Summary:{Colors.ENDC}")
    print(f"Passed: {passed}/{total}")
    print(f"Score: {(passed/total)*100:.1f}%")
    
    if passed == total:
        print(f"\n{Colors.GREEN}{Colors.BOLD}✓ APPLICATION IS READY FOR DEPLOYMENT!{Colors.ENDC}")
    elif passed >= total * 0.8:
        print(f"\n{Colors.YELLOW}{Colors.BOLD}⚠ APPLICATION IS NEARLY READY{Colors.ENDC}")
        print("Address the remaining issues before deployment.")
    else:
        print(f"\n{Colors.RED}{Colors.BOLD}✗ APPLICATION IS NOT READY FOR DEPLOYMENT{Colors.ENDC}")
        print("Critical issues must be resolved.")
    
    # Save report
    report = {
        "timestamp": datetime.now().isoformat(),
        "checks": {k: v for k, v in checks.items()},
        "score": (passed/total)*100,
        "ready": passed == total
    }
    
    with open("deployment_validation_report.json", 'w') as f:
        json.dump(report, f, indent=2)
    
    print(f"\n{Colors.BLUE}Report saved to deployment_validation_report.json{Colors.ENDC}")

if __name__ == "__main__":
    print(f"{Colors.BOLD}{Colors.BLUE}IGED Deployment Validation Script{Colors.ENDC}")
    print(f"Validating deployment readiness...")
    
    generate_report()