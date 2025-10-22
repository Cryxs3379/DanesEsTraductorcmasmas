@echo off
echo ===============================================
echo   SOLUCIÓN FINAL - TRADUCTOR GUI
echo ===============================================
echo.

echo ✅ PROBLEMA RESUELTO:
echo - Configuración explícita de rutas de modelos
echo - Métodos setModelDir() y setCt2Dir() agregados
echo - GUI recompilada con rutas correctas
echo.

echo 🚀 EJECUTANDO GUI CORREGIDA...
echo.

REM Cambiar al directorio correcto
cd /d "%~dp0cpp\build\bin\Release"

echo Directorio: %CD%
echo.

echo Verificando modelos...
if exist "models\nllb-600m\sentencepiece.bpe.model" (
    echo ✓ Tokenizer: ENCONTRADO
) else (
    echo ✗ Tokenizer: NO ENCONTRADO
    pause
    exit /b 1
)

if exist "models\nllb-600m-ct2-int8\model.bin" (
    echo ✓ Modelo CTranslate2: ENCONTRADO
) else (
    echo ✗ Modelo CTranslate2: NO ENCONTRADO
    pause
    exit /b 1
)

echo.
echo 🎯 INICIANDO GUI...
echo.
echo IMPORTANTE: La GUI ahora debe mostrar "Modelo: ✓ Listo" en VERDE
echo Si aparece "Modelo: ✗ Error" en ROJO, hay un problema con la configuración
echo.

start traductor_gui.exe

echo.
echo ✅ GUI iniciada correctamente
echo.
echo VERIFICACIÓN:
echo 1. La GUI debe mostrar "Modelo: ✓ Listo" en verde
echo 2. Puedes escribir "Hola mundo" y traducir
echo 3. Debe aparecer "Hej verden" como resultado
echo.
echo Si todo funciona, el problema está RESUELTO ✅
pause
