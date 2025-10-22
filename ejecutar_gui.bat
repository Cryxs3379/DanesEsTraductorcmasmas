@echo off
echo ===============================================
echo   TRADUCTOR DANÉS-ESPAÑOL - INTERFAZ GRÁFICA
echo ===============================================
echo.
echo Iniciando interfaz gráfica...
echo.
echo IMPORTANTE: La GUI debe ejecutarse desde el directorio Release
echo para que pueda encontrar los modelos de traducción.
echo.

cd /d "%~dp0cpp\build\bin\Release"
echo Directorio de trabajo: %CD%
echo.
echo Verificando modelos...
if exist "models\nllb-600m\sentencepiece.bpe.model" (
    echo ✓ Tokenizer encontrado
) else (
    echo ✗ Tokenizer no encontrado
    pause
    exit /b 1
)

if exist "models\nllb-600m-ct2-int8\model.bin" (
    echo ✓ Modelo CTranslate2 encontrado
) else (
    echo ✗ Modelo CTranslate2 no encontrado
    pause
    exit /b 1
)

echo.
echo Iniciando GUI...
echo.
echo Si aparece "Modelo: ✓ Listo" en verde, todo está funcionando correctamente.
echo Si aparece "Modelo: ✗ Error" en rojo, hay un problema con los modelos.
echo.
start traductor_gui.exe
