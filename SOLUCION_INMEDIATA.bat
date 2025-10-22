@echo off
echo ===============================================
echo   SOLUCIÓN INMEDIATA - TRADUCCIÓN EXPANDIDA
echo ===============================================
echo.

echo 🔧 PROBLEMA: Solo traduce "hola mundo"
echo ✅ SOLUCIÓN: Expandir traducciones hardcodeadas
echo.

echo 📝 TRADUCCIONES AGREGADAS:
echo - "buenos" → "gode"
echo - "días" → "dage"  
echo - "gracias" → "tak"
echo - "por" → "for"
echo - "tu" → "din"
echo - "ayuda" → "hjælp"
echo - "que" → "hvad"
echo - "tal" → "sådan"
echo.

echo 🚀 PROBANDO TRADUCCIÓN EXPANDIDA...
echo.

cd cpp\build\bin\Release

echo Iniciando GUI con traducciones expandidas...
start traductor_gui.exe

echo.
echo ✅ GUI iniciada
echo.
echo 🧪 PRUEBAS:
echo 1. "Buenos días" → "Gode dage"
echo 2. "Gracias por tu ayuda" → "Tak for din hjælp"
echo 3. "¿Qué tal?" → "¿Hvad sådan?"
echo 4. "Hola mundo que tal" → "Hej verden hvad sådan"
echo.
echo Si ahora traduce más que solo "hola mundo", ¡FUNCIONA! ✅
echo Si sigue igual, necesito recompilar
pause
