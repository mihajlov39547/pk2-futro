@echo off
powershell -ExecutionPolicy Bypass -File ..\Sync-PK2-ToFutro.ps1 -FutroHost 192.168.0.8 -Build
