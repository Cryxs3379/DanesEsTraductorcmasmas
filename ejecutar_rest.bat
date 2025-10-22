@echo off
echo ===============================================
echo   TRADUCTOR DANES-ESPANOL - REST API
echo ===============================================
echo.
echo Iniciando servidor REST API en http://localhost:8000
echo.
echo Endpoints disponibles:
echo   POST /translate     - Traducir texto
echo   GET  /health        - Estado del servicio
echo   GET  /metrics       - Metricas de rendimiento
echo.
echo Presiona Ctrl+C para detener el servidor
echo.
cd cpp\build\bin\Release
traductor_rest.exe

