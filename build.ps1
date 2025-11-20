param(
    [string]$Python = "python"
)

Set-StrictMode -Version Latest

# Ensure pip and pybind11/setuptools are available in the venv
& $Python -m pip install --upgrade pip
& $Python -m pip install pybind11 setuptools wheel

# Build the extension in-place
& $Python setup.py build_ext --inplace

Write-Host "Built extension (if build succeeded, see cpphello.*.pyd)"