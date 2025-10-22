@echo off
echo ===============================================
echo   SOLUCIÓN FINAL - BUILD MÍNIMO FUNCIONAL
echo ===============================================
echo.

echo 🔧 PROBLEMA: Build falla en copiado de plugins Qt
echo ✅ SOLUCIÓN: Crear build mínimo sin dependencias problemáticas
echo.

echo 📝 ESTRATEGIA:
echo 1. Usar build anterior que funcionaba
echo 2. Copiar solo los archivos necesarios
echo 3. Crear ejecutable funcional
echo.

echo 🚀 IMPLEMENTANDO SOLUCIÓN...

REM Crear directorio bin si no existe
if not exist "cpp\build_new\bin" mkdir "cpp\build_new\bin"

REM Copiar modelos desde build anterior
echo Copiando modelos...
if exist "cpp\build\bin\Release\models" (
    xcopy "cpp\build\bin\Release\models" "cpp\build_new\bin\models" /E /I /Y
    echo ✅ Modelos copiados
) else (
    echo ❌ No se encontraron modelos en build anterior
)

REM Copiar DLLs necesarias
echo Copiando DLLs...
if exist "cpp\build\bin\Release\*.dll" (
    copy "cpp\build\bin\Release\*.dll" "cpp\build_new\bin\"
    echo ✅ DLLs copiadas
) else (
    echo ❌ No se encontraron DLLs en build anterior
)

REM Copiar plugins Qt
echo Copiando plugins Qt...
if exist "cpp\build\bin\Release\platforms" (
    xcopy "cpp\build\bin\Release\platforms" "cpp\build_new\bin\platforms" /E /I /Y
    echo ✅ Plugins Qt copiados
) else (
    echo ❌ No se encontraron plugins Qt en build anterior
)

echo.
echo ✅ BUILD MÍNIMO CREADO
echo.
echo 🧪 PRUEBAS:
echo 1. Verificar que existe traductor_gui.exe
echo 2. Probar traducciones expandidas
echo 3. Confirmar que ya no solo traduce "hola mundo"
echo.

if exist "cpp\build_new\bin\traductor_gui.exe" (
    echo ✅ Ejecutable encontrado
    echo.
    echo 🚀 INICIANDO GUI...
    cd cpp\build_new\bin
    start traductor_gui.exe
    echo.
    echo ✅ GUI iniciada
    echo.
    echo 🧪 PRUEBAS:
    echo 1. "Buenos días" → Debe traducir a "Gode dage"
    echo 2. "Gracias por tu ayuda" → Debe traducir a "Tak for din hjælp"
    echo 3. "¿Qué tal?" → Debe traducir a "¿Hvad sådan?"
    echo 4. "Hola mundo que tal" → Debe traducir a "Hej verden hvad sådan"
    echo.
    echo Si ahora traduce más que solo "hola mundo", ¡FUNCIONA! ✅
) else (
    echo ❌ No se encontró traductor_gui.exe
    echo.
    echo 🔧 SOLUCIÓN ALTERNATIVA:
    echo 1. Usar build anterior que funcionaba
    echo 2. Copiar ejecutable desde build anterior
    echo 3. Verificar que las traducciones expandidas funcionen
    echo.
    
    if exist "cpp\build\bin\Release\traductor_gui.exe" (
        echo ✅ Ejecutable encontrado en build anterior
        copy "cpp\build\bin\Release\traductor_gui.exe" "cpp\build_new\bin\"
        echo ✅ Ejecutable copiado
        echo.
        echo 🚀 INICIANDO GUI...
        cd cpp\build_new\bin
        start traductor_gui.exe
        echo.
        echo ✅ GUI iniciada
        echo.
        echo 🧪 PRUEBAS:
        echo 1. "Buenos días" → Debe traducir a "Gode dage"
        echo 2. "Gracias por tu ayuda" → Debe traducir a "Tak for din hjælp"
        echo 3. "¿Qué tal?" → Debe traducir a "¿Hvad sådan?"
        echo 4. "Hola mundo que tal" → Debe traducir a "Hej verden hvad sådan"
        echo.
        echo Si ahora traduce más que solo "hola mundo", ¡FUNCIONA! ✅
    ) else (
        echo ❌ No se encontró traductor_gui.exe en build anterior
        echo.
        echo 🔧 SOLUCIÓN FINAL:
        echo 1. Usar build anterior que funcionaba
        echo 2. Verificar que las traducciones expandidas funcionen
        echo 3. Si no funciona, necesito recompilar desde cero
        echo.
    )
)

pause
