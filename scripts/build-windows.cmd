@echo off
rem Builds CSDL on Windows. Arguments are passed on, e.g. "build-windows.cmd -Config Debug -Run".
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0build-windows.ps1" %*
