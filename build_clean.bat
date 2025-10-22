@echo off
echo ===============================================
echo   BUILD LIMPIO - TRADUCTOR CON CTRANSLATE2
echo ===============================================
echo.

echo 1. Limpiando build anterior...
rmdir /s /q cpp\build 2>nul
echo ✓ Build anterior eliminado

echo.
echo 2. Limpiando vcpkg...
vcpkg remove --recurse --outdated
echo ✓ vcpkg limpiado

echo.
echo 3. Instalando dependencias con CTranslate2...
vcpkg install ctranslate2[cpu]:x64-windows sentencepiece:x64-windows protobuf:x64-windows abseil:x64-windows nlohmann-json:x64-windows qtbase:x64-windows drogon:x64-windows
echo ✓ Dependencias instaladas

echo.
echo 4. Configurando CMake...
cmake -B cpp/build -S cpp -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DCMAKE_BUILD_TYPE=Release
echo ✓ CMake configurado

echo.
echo 5. Compilando proyecto...
cmake --build cpp/build --config Release --parallel
echo ✓ Compilación completada

echo.
echo 6. Verificando ejecutables...
if exist "cpp\build\bin\Release\traductor_gui.exe" (
    echo ✓ GUI compilada
) else (
    echo ✗ GUI no encontrada
)

if exist "cpp\build\bin\Release\traductor_cli.exe" (
    echo ✓ CLI compilada
) else (
    echo ✗ CLI no encontrada
)

if exist "cpp\build\bin\Release\traductor_rest.exe" (
    echo ✓ REST compilada
) else (
    echo ✗ REST no encontrada
)

echo.
echo 7. Verificando modelos...
if exist "cpp\build\bin\Release\models\nllb-600m\sentencepiece.bpe.model" (
    echo ✓ Tokenizer copiado
) else (
    echo ✗ Tokenizer no encontrado
)

if exist "cpp\build\bin\Release\models\nllb-600m-ct2-int8\model.bin" (
    echo ✓ Modelo CTranslate2 copiado
) else (
    echo ✗ Modelo CTranslate2 no encontrado
)

echo.
echo ===============================================
echo   BUILD COMPLETADO
echo ===============================================
echo.
echo Para probar la GUI:
echo   cd cpp\build\bin\Release
echo   traductor_gui.exe
echo.
echo Debe mostrar "Modelo: ✓ Listo" en verde
echo.
pause
