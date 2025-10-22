@echo off
echo ===============================================
echo   TRADUCTOR DANÉS-ESPAÑOL - GUI CORREGIDA
echo ===============================================
echo.

echo Solucionando problema de carga de modelos...
echo.

REM Cambiar al directorio correcto donde están los modelos
cd /d "%~dp0cpp\build\bin\Release"

echo Directorio de trabajo: %CD%
echo.

echo Verificando modelos...
if exist "models\nllb-600m\sentencepiece.bpe.model" (
    echo ✓ Tokenizer encontrado
) else (
    echo ✗ Tokenizer NO encontrado - copiando...
    cd /d "%~dp0"
    xcopy models cpp\build\bin\Release\models /E /I /Y
    cd /d "%~dp0cpp\build\bin\Release"
)

if exist "models\nllb-600m-ct2-int8\model.bin" (
    echo ✓ Modelo CTranslate2 encontrado
) else (
    echo ✗ Modelo CTranslate2 NO encontrado - copiando...
    cd /d "%~dp0"
    xcopy models cpp\build\bin\Release\models /E /I /Y
    cd /d "%~dp0cpp\build\bin\Release"
)

echo.
echo Verificando plugins Qt...
if exist "platforms\qwindows.dll" (
    echo ✓ Plugins Qt encontrados
) else (
    echo ✗ Plugins Qt NO encontrados - copiando...
    cd /d "%~dp0"
    xcopy cpp\build\vcpkg_installed\x64-windows\Qt6\plugins\platforms cpp\build\bin\Release\platforms /E /I /Y
    cd /d "%~dp0cpp\build\bin\Release"
)

echo.
echo Probando CLI para verificar que todo funciona...
echo "Hola mundo" | traductor_cli.exe --direction es-da > nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo ✓ CLI funciona correctamente
) else (
    echo ✗ CLI no funciona - hay un problema con los modelos
    pause
    exit /b 1
)

echo.
echo Iniciando GUI desde el directorio correcto...
echo.
echo IMPORTANTE: La GUI debe mostrar "Modelo: ✓ Listo" en VERDE
echo Si aparece "Modelo: ✗ Error" en ROJO, cierra la GUI y ejecuta este script de nuevo
echo.
start traductor_gui.exe

echo.
echo GUI iniciada. Verifica que muestre "Modelo: ✓ Listo" en verde.
echo Si no funciona, ejecuta este script de nuevo.
pause
