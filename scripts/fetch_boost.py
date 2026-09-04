#!/usr/bin/env python3
"""
Downloads and extracts Boost headers into cpp/include/boost if not already present.
This ensures reproducible builds without committing heavy Boost headers to Git.
"""

import shutil
import subprocess
import tarfile
import urllib.request
from pathlib import Path

BOOST_VERSION = "1.85.0"
BOOST_UNDERSCORE = BOOST_VERSION.replace(".", "_")
BOOST_URL = f"https://archives.boost.io/release/{BOOST_VERSION}/source/boost_{BOOST_UNDERSCORE}.tar.gz"

PROJECT_ROOT = Path(__file__).resolve().parent.parent
TARGET_INCLUDE_DIR = PROJECT_ROOT / "cpp" / "include"
BOOST_DIR = TARGET_INCLUDE_DIR / "boost"
CONTRACT_HEADER = BOOST_DIR / "contract.hpp"


def is_installed() -> bool:
    return CONTRACT_HEADER.exists()


def install_boost() -> None:
    if is_installed():
        print(f"[Boost] Already installed at {BOOST_DIR}")
        return

    print(f"[Boost] boost/contract.hpp not found in {TARGET_INCLUDE_DIR}.")
    print(f"[Boost] Downloading Boost {BOOST_VERSION} headers from {BOOST_URL}...")

    TARGET_INCLUDE_DIR.mkdir(parents=True, exist_ok=True)

    # Try streaming extract via curl and tar first (fastest on Linux)
    curl_bin = shutil.which("curl")
    tar_bin = shutil.which("tar")
    if curl_bin and tar_bin:
        cmd = f"{curl_bin} -sSL {BOOST_URL} | {tar_bin} -xz -C {TARGET_INCLUDE_DIR} --strip-components=1 boost_{BOOST_UNDERSCORE}/boost"
        print("[Boost] Running streaming extraction via curl | tar...")
        ret = subprocess.run(cmd, shell=True, cwd=PROJECT_ROOT, check=False)
        if ret.returncode == 0 and is_installed():
            print(f"[Boost] Successfully installed Boost headers to {BOOST_DIR}")
            return
        print(
            "[Boost] Streaming extraction failed, falling back to python urllib/tarfile..."
        )

    # Fallback to pure Python streaming download & extract
    temp_archive = PROJECT_ROOT / f"boost_{BOOST_UNDERSCORE}.tar.gz"
    try:
        print(f"[Boost] Downloading {BOOST_URL} to {temp_archive}...")
        urllib.request.urlretrieve(BOOST_URL, temp_archive)
        print("[Boost] Extracting boost directory...")
        with tarfile.open(temp_archive, "r:gz") as tar:
            prefix = f"boost_{BOOST_UNDERSCORE}/boost/"
            members = [m for m in tar.getmembers() if m.name.startswith(prefix)]
            for m in members:
                # Strip leading prefix 'boost_X_Y_Z/' so it lands in cpp/include/boost/
                m.name = m.name[len(f"boost_{BOOST_UNDERSCORE}/") :]
            tar.extractall(path=TARGET_INCLUDE_DIR, members=members)
        print(f"[Boost] Successfully installed Boost headers to {BOOST_DIR}")
    finally:
        if temp_archive.exists():
            temp_archive.unlink()


if __name__ == "__main__":
    install_boost()
