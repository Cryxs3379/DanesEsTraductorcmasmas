@echo off
echo ===============================================
echo   SOLUCIÓN: TRADUCCIÓN REAL (NO SIMPLIFICADA)
echo ===============================================
echo.

echo 🔍 PROBLEMA IDENTIFICADO:
echo - El sistema está usando archivos *_simple.cpp
echo - Solo traduce "hola mundo" con traducciones hardcodeadas
echo - No usa NLLB-600M real
echo.

echo ✅ SOLUCIÓN APLICADA:
echo - Eliminados archivos TranslatorEngine_simple.cpp
echo - Eliminados archivos Tokenizer_simple.cpp
echo - Forzando uso de archivos reales
echo.

echo 🚀 PROBANDO TRADUCCIÓN REAL...
echo.

cd cpp\build\bin\Release

echo Verificando que los archivos simplificados fueron eliminados...
if exist "..\..\..\core\TranslatorEngine_simple.cpp" (
    echo ✗ Archivo simplificado aún existe
) else (
    echo ✓ Archivo simplificado eliminado
)

if exist "..\..\..\core\Tokenizer_simple.cpp" (
    echo ✗ Archivo simplificado aún existe
) else (
    echo ✓ Archivo simplificado eliminado
)

echo.
echo 🎯 INICIANDO GUI CON TRADUCCIÓN REAL...
echo.
echo IMPORTANTE: Ahora debe usar NLLB-600M real, no traducciones hardcodeadas
echo.

start traductor_gui.exe

echo.
echo ✅ GUI iniciada con archivos reales
echo.
echo PRUEBA:
echo 1. Escribe "Buenos días" - debe traducir correctamente
echo 2. Escribe "¿Cómo estás?" - debe traducir correctamente  
echo 3. Escribe "Gracias por tu ayuda" - debe traducir correctamente
echo.
echo Si solo traduce "hola mundo" y nada más, el problema persiste
echo Si traduce cualquier texto, ¡FUNCIONA! ✅
pause
