@echo off
echo ===============================================
echo   TRADUCTOR DANES-ESPANOL - CLI
echo ===============================================
echo.
echo Uso: Introduce texto en espanol y presiona Enter
echo      Presiona Ctrl+C para salir
echo.
cd cpp\build\bin\Release
traductor_cli.exe --direction es-da --metrics

