#!/usr/bin/env python3
"""
IGED Static Analysis Runner
Comprehensive code quality analysis and fixing script
"""

import subprocess
import sys


def run_command(cmd: str, capture_output: bool = True) -> tuple[int, str]:
    """Run a shell command and return exit code and output"""
    try:
        result = subprocess.run(
            cmd, shell=True, capture_output=capture_output, text=True
        )
        return result.returncode, result.stdout + result.stderr
    except Exception as e:
        return 1, str(e)


def main() -> None:
    """Run comprehensive static analysis"""
    print("🔍 IGED Static Analysis Suite")
    print("=" * 50)

    # Check if virtual environment is active
    if not Path("venv").exists():
        print(
            "❌ Virtual environment not found. Please run install_dependencies.py first."
        )
        return 1

    # Install analysis tools
    print("📦 Installing analysis tools...")
    code, output = run_command(
        "source venv/bin/activate && pip install -q pylint black isort flake8 mypy bandit safety pipreqs"
    )
    if code != 0:
        print(f"❌ Failed to install tools: {output}")
        return 1

    # Auto-fix formatting
    print("🔧 Auto-fixing code formatting...")
    run_command("source venv/bin/activate && black --line-length=88 --exclude=venv .")
    run_command(
        "source venv/bin/activate && isort --profile=black --line-length=88 --skip=venv ."
    )
    print("✅ Code formatting applied")

    # Run static analysis
    print("🔍 Running static analysis...")

    # Pylint
    print("  → Running pylint...")
    run_command(
        "source venv/bin/activate && pylint --output-format=text --reports=yes --ignore=venv,build_release *.py agents/ core/ ui/ admin_panel/ plugins/ android_client/ IGED_Biometric_Auth/ > analysis_pylint.log 2>&1"
    )

    # Flake8
    print("  → Running flake8...")
    run_command(
        "source venv/bin/activate && flake8 --max-line-length=88 --extend-ignore=E203,W503 --exclude=venv,build_release *.py agents/ core/ ui/ admin_panel/ plugins/ android_client/ IGED_Biometric_Auth/ > analysis_flake8.log 2>&1"
    )

    # MyPy
    print("  → Running mypy...")
    run_command(
        "source venv/bin/activate && mypy --ignore-missing-imports --show-error-codes --exclude=venv --exclude=build_release . > analysis_typing.log 2>&1"
    )

    # Security analysis
    print("  → Running security analysis...")
    run_command(
        "source venv/bin/activate && bandit -r . -x venv,build_release -f txt > analysis_security.log 2>&1"
    )

    # Dependency check
    print("  → Checking dependencies...")
    run_command(
        "source venv/bin/activate && pip check > analysis_dependencies.log 2>&1"
    )
    run_command(
        "source venv/bin/activate && safety check > analysis_vulnerabilities.log 2>&1"
    )

    # Generate clean requirements
    print("  → Generating clean requirements...")
    run_command(
        "source venv/bin/activate && pipreqs . --force --encoding=utf-8 --savepath=requirements_clean.txt"
    )

    # Summary
    print("\n📊 Analysis Complete!")
    print("=" * 30)

    # Quick stats
    flake8_log = Path("analysis_flake8.log")
    if flake8_log.exists():
        violations = len(flake8_log.read_text().strip().split("\n"))
        print(f"  Flake8 violations: {violations}")

    pylint_log = Path("analysis_pylint.log")
    if pylint_log.exists():
        content = pylint_log.read_text()
        if "Your code has been rated at" in content:
            score_line = [
                line
                for line in content.split("\n")
                if "Your code has been rated at" in line
            ]
            if score_line:
                print(f"  Pylint score: {score_line[0].split('at ')[1]}")

    print("\n📄 Reports generated:")
    print("  • analysis_pylint.log - Code quality issues")
    print("  • analysis_flake8.log - Style violations")
    print("  • analysis_typing.log - Type checking")
    print("  • analysis_security.log - Security analysis")
    print("  • requirements_clean.txt - Clean dependencies")
    print("\n📖 See STATIC_ANALYSIS_REPORT.md for detailed analysis")

    return 0


if __name__ == "__main__":
    sys.exit(main())
