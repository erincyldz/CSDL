<#
.SYNOPSIS
    Builds CSDL on Windows from any terminal, without opening Visual Studio.

.DESCRIPTION
    Finds the newest Visual Studio or Build Tools install that has the C++ tools, loads its
    64-bit compiler environment and runs the CMake preset. Usually started through
    build-windows.cmd, which also works when PowerShell scripts are blocked.

.EXAMPLE
    scripts\build-windows.cmd                  # Release build -> bin\engine.exe
    scripts\build-windows.cmd -Config Debug    # Debug build   -> build-windows-debug\bin\engine.exe
    scripts\build-windows.cmd -Run             # build, then start the app
#>
param(
    [ValidateSet('Release', 'Debug')]
    [string]$Config = 'Release',
    [switch]$Run
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot

if ($Config -eq 'Debug') {
    $preset = 'windows-debug'
    $buildDir = Join-Path $root 'build-windows-debug'
    $exe = Join-Path $buildDir 'bin\engine.exe'
} else {
    $preset = 'windows'
    $buildDir = Join-Path $root 'build-windows'
    $exe = Join-Path $root 'bin\engine.exe'
}

# vswhere ships with every Visual Studio / Build Tools install, always at this path.
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path $vswhere)) {
    throw "Visual Studio 2022 or newer (or Build Tools) with the 'Desktop development with C++' workload is required."
}
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) {
    throw "No Visual Studio install with the C++ tools was found. Add the 'Desktop development with C++' workload in the Visual Studio Installer."
}
Write-Host "Using $vs"

# Load the 64-bit compiler environment into this PowerShell session.
Import-Module (Join-Path $vs 'Common7\Tools\Microsoft.VisualStudio.DevShell.dll')
Enter-VsDevShell -VsInstallPath $vs -SkipAutomaticLocation -DevCmdArguments '-arch=x64 -host_arch=x64' | Out-Null
if (-not $env:VCPKG_ROOT) {
    throw "vcpkg was not found. Add the 'vcpkg package manager' component in the Visual Studio Installer, or set VCPKG_ROOT."
}

# A build folder configured from a 32-bit prompt or with another generator cannot be reused.
$cache = Join-Path $buildDir 'CMakeCache.txt'
if (Test-Path $cache) {
    $text = Get-Content $cache -Raw
    if ($text -notmatch 'CMAKE_GENERATOR:INTERNAL=Ninja' -or $text -match '/x86/cl\.exe') {
        Write-Host "Removing $buildDir (it was configured for a different compiler or generator)"
        Remove-Item -Recurse -Force $buildDir
    }
}

Push-Location $root
try {
    cmake --preset $preset
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    cmake --build --preset $preset
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
} finally {
    Pop-Location
}

Write-Host "Built $exe"
if ($Run) { & $exe }
