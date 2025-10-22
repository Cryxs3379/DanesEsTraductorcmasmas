# 🚀 Cómo Usar el Traductor Danés-Español

## ✅ El problema de Qt está SOLUCIONADO

Los plugins de Qt se han copiado correctamente. Ahora puedes usar el traductor.

---

## 🖥️ Opción 1: Interfaz Gráfica (GUI)

### Ejecutar:
```bash
# Doble clic en:
ejecutar_gui.bat

# O desde PowerShell:
cd cpp\build\bin\Release
.\traductor_gui.exe
```

### Uso:
1. Se abrirá una ventana con la interfaz gráfica
2. Verifica que la barra inferior muestre **"Modelo: ✓ Listo"** (verde)
3. Escribe texto en español en el cuadro superior
4. Haz clic en **"Traducir"**
5. La traducción aparecerá en el cuadro inferior

### Pruebas recomendadas:
| Texto español | Traducción danesa esperada |
|--------------|---------------------------|
| Hola mundo | Hej verden |
| Buenos días | gode dage |
| Gracias por tu ayuda | tak for din hjælp |
| ¿Qué tal? | ¿Hvad sådan? |

---

## 💻 Opción 2: Línea de Comandos (CLI)

### Ejecutar:
```bash
# Doble clic en:
ejecutar_cli.bat

# O desde PowerShell:
cd cpp\build\bin\Release
echo "Hola mundo" | .\traductor_cli.exe --direction es-da
```

### Ejemplos:
```bash
# Traducción simple
echo "Hola mundo" | .\traductor_cli.exe --direction es-da

# Con métricas de rendimiento
echo "Buenos días" | .\traductor_cli.exe --direction es-da --metrics

# Traducción inversa (danés a español)
echo "Hej verden" | .\traductor_cli.exe --direction da-es

# Modo formal
echo "Hola" | .\traductor_cli.exe --direction es-da --formal
```

---

## 🌐 Opción 3: API REST

### Ejecutar servidor:
```bash
# Doble clic en:
ejecutar_rest.bat

# O desde PowerShell:
cd cpp\build\bin\Release
.\traductor_rest.exe
```

### Usar desde otra terminal:
```bash
# Traducción simple
curl -X POST http://localhost:8000/translate `
  -H "Content-Type: application/json" `
  -d '{\"text\":\"Hola mundo\",\"direction\":\"es-da\"}'

# Estado del servicio
curl http://localhost:8000/health

# Métricas
curl http://localhost:8000/metrics
```

---

## 📁 Estructura de Archivos

```
DanesEsTraductorcmasmas/
├── ejecutar_gui.bat          ← Ejecuta la GUI
├── ejecutar_cli.bat          ← Ejecuta el CLI
├── ejecutar_rest.bat         ← Ejecuta el REST API
└── cpp/build/bin/Release/
    ├── traductor_gui.exe     ← Ejecutable GUI
    ├── traductor_cli.exe     ← Ejecutable CLI
    ├── traductor_rest.exe    ← Ejecutable REST
    ├── platforms/
    │   └── qwindows.dll      ✅ Plugin Qt (COPIADO)
    └── models/
        ├── nllb-600m/
        │   └── sentencepiece.bpe.model
        └── nllb-600m-ct2-int8/
            └── model.bin
```

---

## ⚠️ IMPORTANTE: Modo Simplificado

El traductor está compilado en **modo simplificado** porque CTranslate2 no está disponible en vcpkg.

### Traducciones disponibles:
- ✅ "Hola" → "Hej"
- ✅ "mundo" → "verden"
- ✅ "Buenos días" → "gode dage"
- ✅ "Gracias por tu ayuda" → "tak for din hjælp"
- ✅ "¿Qué tal?" → "¿Hvad sådan?"
- ✅ Y más combinaciones de estas palabras

### Limitaciones:
- ❌ No usa el modelo NLLB-600M real
- ❌ Solo traduce palabras/frases programadas
- ❌ No puede traducir texto arbitrario

---

## 🔧 Solución de Problemas

### Si la GUI no abre:
```bash
# Ver errores detallados
cd cpp\build\bin\Release
.\traductor_gui.exe 2>&1
```

### Si aparece "Modelo: ✗ Error":
1. Pasa el mouse sobre el texto rojo para ver el error
2. Verifica que existan:
   - `models/nllb-600m/sentencepiece.bpe.model`
   - `models/nllb-600m-ct2-int8/model.bin`

### Si falta qwindows.dll:
```powershell
# Ya está solucionado, pero si vuelve a fallar:
Copy-Item -Path "C:\vcpkg\installed\x64-windows\Qt6\plugins\platforms\*" `
          -Destination "cpp\build\bin\Release\platforms\" -Force
```

---

## 📊 Estado del Sistema

- ✅ **Build completado**: Todos los ejecutables compilados
- ✅ **Plugins Qt**: Copiados correctamente
- ✅ **Modelos**: Disponibles en Release
- ✅ **CLI**: Funcional
- ✅ **GUI**: Funcional (problema de Qt solucionado)
- ✅ **REST API**: Compilado
- ⚠️ **CTranslate2**: No disponible (modo simplificado)

---

## 🎯 Próximos Pasos

Para usar el modelo NLLB-600M real (no simplificado):
1. Compilar CTranslate2 desde código fuente
2. Actualizar CMake para encontrar CTranslate2
3. Recompilar el proyecto

**Por ahora, el modo simplificado funciona para pruebas básicas.**

