#!/usr/bin/env python3
"""
build.py - Robust helper to build the pybind11 extension on Windows.

This version writes a temporary .bat file that calls vcvarsall and then runs
the venv python to install build deps and run setup.py. Using a temp batch
avoids quoting/escaping issues when invoking cmd.exe from PowerShell.
"""
from __future__ import annotations
import os
import shutil
import subprocess
import sys
import tempfile
from typing import Optional

COMMON_VCVARS_PATHS = [
    r"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat",
    r"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat",
    r"C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat",
    r"C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat",
    r"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat",
]

def find_vswhere() -> Optional[str]:
    vswhere = shutil.which("vswhere.exe") or shutil.which("vswhere")
    if vswhere:
        return vswhere
    candidate = os.path.join(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"),
                             "Microsoft Visual Studio", "Installer", "vswhere.exe")
    if os.path.exists(candidate):
        return candidate
    return None

def find_vcvars() -> Optional[str]:
    env_path = os.environ.get("VCVARS_PATH") or os.environ.get("VCVARSALL_PATH")
    if env_path and os.path.exists(env_path):
        return env_path

    vswhere = find_vswhere()
    if vswhere:
        try:
            inst = subprocess.check_output(
                [vswhere, "-latest", "-products", "*", "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"],
                text=True
            ).strip()
            if inst:
                candidate = os.path.join(inst, "VC", "Auxiliary", "Build", "vcvarsall.bat")
                if os.path.exists(candidate):
                    return candidate
        except subprocess.CalledProcessError:
            pass

    for p in COMMON_VCVARS_PATHS:
        if os.path.exists(p):
            return p
    return None

def run_cmd_build_with_tempbat(vcvars_path: Optional[str], python_exe: str) -> int:
    if not vcvars_path:
        print("vcvarsall.bat not found. Attempting direct venv build (may fail).")
        try:
            subprocess.check_call([python_exe, "-m", "pip", "install", "--upgrade", "pip", "pybind11", "setuptools", "wheel"])
            subprocess.check_call([python_exe, "setup.py", "build_ext", "--inplace", "-v"])
            return 0
        except subprocess.CalledProcessError as e:
            print("Build failed:", e)
            return e.returncode if hasattr(e, "returncode") else 1

    # Compose batch content; wrap paths in double quotes
    bat_lines = [
        f'@echo off',
        f'call "{vcvars_path}" x64',
        f'"{python_exe}" -m pip install --upgrade pip pybind11 setuptools wheel',
        f'"{python_exe}" setup.py build_ext --inplace -v',
    ]
    bat_content = " && ".join(bat_lines)

    # Write a temporary .bat file and execute it via cmd.exe /C
    fd, bat_path = tempfile.mkstemp(suffix=".bat", text=True)
    os.close(fd)
    with open(bat_path, "w", newline="\r\n") as f:
        f.write(bat_content + "\r\n")

    print("Running build via temporary batch:", bat_path)
    try:
        proc = subprocess.run(["cmd.exe", "/C", bat_path], check=False)
        return proc.returncode
    finally:
        try:
            os.remove(bat_path)
        except OSError:
            pass

def main() -> None:
    python_exe = sys.executable
    print("Starting build.py for pybind11 extension on Windows.")
    print("Reminder: Try vscode task after working")
    print("Python used:", python_exe)

    vcvars = find_vcvars()
    if vcvars:
        print("Found vcvars:", vcvars)
    else:
        print("vcvars not found by heuristics (set VCVARS_PATH env var to override).")

    rc = run_cmd_build_with_tempbat(vcvars, python_exe)
    if rc == 0:
        print("Build finished successfully.")
    else:
        print(f"Build exited with code {rc}.")

if __name__ == "__main__":
    main()