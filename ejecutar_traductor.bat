@echo off
REM Script para ejecutar el Traductor Danés-Español
echo ===============================================
echo   TRADUCTOR DANÉS-ESPAÑOL
echo   Versión 1.0.0 - C++ Native
echo ===============================================
echo.
echo Seleccione la interfaz:
echo.
echo 1. CLI (Línea de comandos)
echo 2. GUI (Interfaz gráfica)
echo 3. REST API (Servidor)
echo 4. Mostrar ayuda CLI
echo.
set /p opcion="Ingrese su opción (1-4): "

if "%opcion%"=="1" goto CLI
if "%opcion%"=="2" goto GUI
if "%opcion%"=="3" goto REST
if "%opcion%"=="4" goto HELP
echo Opción inválida
pause
exit /b

:CLI
echo.
echo Ejecutando CLI...
echo.
set /p texto="Ingrese texto a traducir: "
echo %texto% | cpp\build\bin\Release\traductor_cli.exe --direction es-da --glossary cpp\assets\glossary.txt --metrics
pause
exit /b

:GUI
echo.
echo Abriendo interfaz gráfica...
echo Nota: La GUI debe ejecutarse desde el directorio de Release para encontrar los modelos
cd cpp\build\bin\Release
start traductor_gui.exe
exit /b

:REST
echo.
echo Iniciando servidor REST API en http://localhost:8000
echo.
echo Endpoints disponibles:
echo   GET  /health
echo   POST /translate
echo   POST /translate/html
echo.
cpp\build\bin\Release\traductor_rest.exe
pause
exit /b

:HELP
cpp\build\bin\Release\traductor_cli.exe --help
pause
exit /b

