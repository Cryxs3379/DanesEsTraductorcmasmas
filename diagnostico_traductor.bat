@echo off
echo ===============================================
echo   DIAGNÓSTICO DEL TRADUCTOR DANÉS-ESPAÑOL
echo ===============================================
echo.

echo 1. Verificando estructura de archivos...
echo.
echo Ejecutables:
dir cpp\build\bin\Release\*.exe
echo.
echo Modelos:
dir cpp\build\bin\Release\models
echo.
echo Plugins Qt:
dir cpp\build\bin\Release\platforms
echo.

echo 2. Probando CLI...
echo.
echo "Hola mundo" | cpp\build\bin\Release\traductor_cli.exe --direction es-da --metrics
echo.

echo 3. Verificando REST API...
echo.
echo Iniciando servidor REST en segundo plano...
start /B cpp\build\bin\Release\traductor_rest.exe
timeout /t 3 /nobreak >nul
echo.
echo Probando endpoint de salud...
curl -s http://localhost:8000/health
echo.
echo Probando traducción...
curl -s -X POST http://localhost:8000/translate -H "Content-Type: application/json" -d "{\"text\":\"Hola mundo\",\"direction\":\"es-da\"}"
echo.
echo.
echo 4. Verificando GUI...
echo.
echo Abriendo GUI (verificar que muestre "Modelo: ✓ Listo")...
start cpp\build\bin\Release\traductor_gui.exe
echo.
echo ===============================================
echo   DIAGNÓSTICO COMPLETADO
echo ===============================================
echo.
echo Si todo funciona correctamente:
echo - CLI: Debería mostrar "Hej verden"
echo - REST: Debería responder con JSON válido
echo - GUI: Debería mostrar "Modelo: ✓ Listo" en verde
echo.
pause
