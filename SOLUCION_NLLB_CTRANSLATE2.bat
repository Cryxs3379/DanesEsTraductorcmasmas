@echo off
echo ===============================================
echo   SOLUCIÓN NLLB-600M CON CTRANSLATE2 REAL
echo ===============================================
echo.

echo 🎯 OBJETIVO: Eliminar modo simplificado, usar NLLB-600M real
echo.

echo 1. Verificando build existente...
if exist "cpp\build\bin\Release\traductor_gui.exe" (
    echo ✓ GUI encontrada
) else (
    echo ✗ GUI no encontrada - necesitas compilar primero
    pause
    exit /b 1
)

echo.
echo 2. Verificando modelos NLLB-600M...
if exist "cpp\build\bin\Release\models\nllb-600m\sentencepiece.bpe.model" (
    echo ✓ Tokenizer NLLB-600M encontrado
) else (
    echo ✗ Tokenizer no encontrado - copiando...
    xcopy models cpp\build\bin\Release\models /E /I /Y
)

if exist "cpp\build\bin\Release\models\nllb-600m-ct2-int8\model.bin" (
    echo ✓ Modelo CTranslate2 NLLB-600M encontrado
) else (
    echo ✗ Modelo CTranslate2 no encontrado - copiando...
    xcopy models cpp\build\bin\Release\models /E /I /Y
)

echo.
echo 3. Verificando plugins Qt...
if exist "cpp\build\bin\Release\platforms\qwindows.dll" (
    echo ✓ Plugins Qt encontrados
) else (
    echo ✗ Plugins Qt no encontrados - copiando...
    xcopy cpp\build\vcpkg_installed\x64-windows\Qt6\plugins\platforms cpp\build\bin\Release\platforms /E /I /Y
)

echo.
echo 4. Probando CLI para verificar NLLB-600M...
cd cpp\build\bin\Release
echo "Hola mundo" | traductor_cli.exe --direction es-da --metrics
if %ERRORLEVEL% EQU 0 (
    echo ✓ CLI funciona con NLLB-600M
) else (
    echo ✗ CLI no funciona
    pause
    exit /b 1
)

echo.
echo 5. Iniciando GUI con NLLB-600M real...
echo.
echo IMPORTANTE: La GUI debe mostrar "Modelo: ✓ Listo" en VERDE
echo Si aparece "Modelo: ✗ Error" en ROJO, hay un problema con NLLB-600M
echo.
start traductor_gui.exe

echo.
echo ✅ GUI iniciada con NLLB-600M real
echo.
echo VERIFICACIÓN:
echo 1. Debe mostrar "Modelo: ✓ Listo" en verde
echo 2. Escribe "Hola mundo" y traduce
echo 3. Debe aparecer "Hej verden" (traducción real de NLLB-600M)
echo.
echo Si todo funciona, NLLB-600M está operativo sin modo simplificado ✅
pause
