#!/usr/bin/env python3
"""
IGED - Final Encryption Key Generator (Enterprise-Hardened)
"""

import argparse
import base64
import stat
import subprocess
import sys
from datetime import datetime

LOG_PATH = Path("logs/encryption.log")


def log(msg) -> None:
    """Log function."""
    print(msg)
    try:
        LOG_PATH.parent.mkdir(exist_ok=True)
        with open(LOG_PATH, "a", encoding="utf-8") as log_file:
            log_file.write(f"[{datetime.now().isoformat()}] {msg}\n")
    except Exception:
        pass


def create_encryption_key(force=False) -> None:
    """Create encryption key with security and fallback mechanisms"""

    log("🔑 Starting IGED Encryption Key Generation")

    try:

        log("✅ Cryptography module loaded")
    except ImportError as e:
        log(f"❌ Cryptography not installed: {e}")
        log("💡 Install it with: pip install cryptography")
        return False

    key = Fernet.generate_key()
    key_b64 = base64.b64encode(key).decode("utf-8")

    current_dir = Path.cwd()
    config_dir = current_dir / "config"
    key_file = config_dir / "secret.key"

    log(f"📁 Working directory: {current_dir}")
    log(f"📁 Config directory: {config_dir}")
    log(f"🔑 Key file path: {key_file}")

    config_dir.mkdir(exist_ok=True)

    if key_file.exists() and not force:
        log("⚠️ Encryption key already exists. Use --force to regenerate.")
        return True

    try:
        with NamedTemporaryFile("wb", delete=False, dir=config_dir) as tmp:
            tmp.write(key)
            tmp_path = Path(tmp.name)
        tmp_path.replace(key_file)

        # Lock file permission (Unix + Windows)
        try:
            key_file.chmod(stat.S_IRUSR | stat.S_IWUSR)  # rw-------
        except Exception:
            pass

        log("✅ Key written securely using atomic write")

    except Exception as e:
        log(f"⚠️ Atomic write failed: {e}")
        try:
            subprocess.run(
                ["cmd", "/c", f'echo {key_b64} > "{key_file}"'], check=True, shell=True
            )
            log("✅ Key written via subprocess fallback")
        except Exception as e2:
            log(f"❌ Subprocess fallback failed: {e2}")
            log("\n📋 MANUAL KEY CREATION REQUIRED:")
            log("=" * 50)
            log(f"Key (base64): {key_b64}")
            log("=" * 50)
            log("1. Create file: config/secret.key")
            log("2. Paste the base64 key into the file")
            log("3. Save and rerun IGED")
            return False

    if key_file.exists():
        log(f"🎉 Key file successfully created!")
        log(f"📁 Location: {key_file.resolve()}")
        log(f"📦 Size: {key_file.stat().st_size} bytes")
        return True
    else:
        log("❌ Final check failed: key file does not exist.")
        return False


def main() -> None:
    """Main function."""
    parser = argparse.ArgumentParser(description="IGED Encryption Key Generator")
    parser.add_argument(
        "--force",
        action="store_true",
        help="Force key regeneration if it already exists",
    )
    args = parser.parse_args()

    print("🚀 IGED Hardened Encryption Key Generator")
    print("=" * 50)

    success = create_encryption_key(force=args.force)

    if success:
        print("\n✅ Encryption key created successfully!")
        print("🚀 IGED is ready to run.")
        print("\nNext steps:")
        print("1. Run: python test_installation.py")
        print("2. Launch: python launcher.py")
        print("3. Web admin: http://localhost:8080")
    else:
        print("\n❌ Key creation failed.")
        print("💡 Retry or manually place key at: config/secret.key")
        sys.exit(1)


if __name__ == "__main__":
    main()
