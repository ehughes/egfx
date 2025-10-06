#!/usr/bin/env pwsh
# Build and run test project with Ninja

$ErrorActionPreference = "Stop"

Write-Host "Cleaning build folder..." -ForegroundColor Cyan
if (Test-Path "build") {
    Remove-Item -Recurse -Force "build"
}
New-Item -ItemType Directory -Path "build" | Out-Null

Write-Host "Running CMake with Ninja generator..." -ForegroundColor Cyan
Set-Location "build"
cmake .. -G "Ninja"

Write-Host "Building with Ninja..." -ForegroundColor Cyan
ninja

Write-Host "Running executable..." -ForegroundColor Green
$exe = "egfx_test_transpose_172_320_16BPP_RGB565.exe"
if (Test-Path $exe) {
    & ".\$exe"
} else {
    Write-Host "Error: Executable not found: $exe" -ForegroundColor Red
    Set-Location ..
    exit 1
}

Set-Location ..
