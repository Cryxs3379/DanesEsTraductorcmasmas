@echo off
echo ===============================================
echo   VERIFICACION DE INSTALACION
echo ===============================================
echo.

echo Verificando ejecutables...
if exist "cpp\build\bin\Release\traductor_gui.exe" (
    echo [OK] traductor_gui.exe encontrado
) else (
    echo [ERROR] traductor_gui.exe NO encontrado
)

if exist "cpp\build\bin\Release\traductor_cli.exe" (
    echo [OK] traductor_cli.exe encontrado
) else (
    echo [ERROR] traductor_cli.exe NO encontrado
)

if exist "cpp\build\bin\Release\traductor_rest.exe" (
    echo [OK] traductor_rest.exe encontrado
) else (
    echo [ERROR] traductor_rest.exe NO encontrado
)

echo.
echo Verificando plugins Qt...
if exist "cpp\build\bin\Release\platforms\qwindows.dll" (
    echo [OK] qwindows.dll encontrado
) else (
    echo [ERROR] qwindows.dll NO encontrado - La GUI no funcionara
)

echo.
echo Verificando modelos...
if exist "cpp\build\bin\Release\models\nllb-600m\sentencepiece.bpe.model" (
    echo [OK] sentencepiece.bpe.model encontrado
) else (
    echo [ERROR] sentencepiece.bpe.model NO encontrado
)

if exist "cpp\build\bin\Release\models\nllb-600m-ct2-int8\model.bin" (
    echo [OK] model.bin encontrado
) else (
    echo [ERROR] model.bin NO encontrado
)

echo.
echo ===============================================
echo   VERIFICACION COMPLETADA
echo ===============================================
echo.
pause

