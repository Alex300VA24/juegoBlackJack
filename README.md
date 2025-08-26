# JuegoBlackJack

Un juego de **Black Jack** implementado en **C++**, que utiliza **SFML** para la interfaz gráfica. Este proyecto está diseñado para funcionar exclusivamente en entornos **Linux**.

---

## Características

- Juego de Black Jack con interfaz gráfica basada en **SFML**
- Lógica del juego implementada en C++, con clases como `Carta`, `Jogo` y `CartaGrafica`
- Estructura modular para facilitar extensiones futuras
- Incluye recursos en la carpeta `assets`

---

## Requisitos

- **Sistema operativo**: Linux (probado en distribuciones como Ubuntu, Debian, Fedora)
- **Dependencias**:
  - **SFML** (Simple and Fast Multimedia Library), versión compatible con tu distribución
  - **CMake** para el sistema de compilación
  - Compilador **g++** compatible con C++ (se recomienda al menos C++11)

---

## Instalación

1. Instala `SFML` y otras dependencias en tu sistema:
   ```bash
   sudo apt-get update
   sudo apt-get install libsfml-dev cmake g++


## Estructura del proyecto

juegoBlackJack/
├── .vscode/             — Configuración del editor (opcional)
├── SFML/                — Archivos relacionados con SFML (si están incluidos)
├── assets/              — Recursos gráficos y multimedia
├── build/               — Directorio de compilación (generado por CMake)
├── Carta.h              — Clase para representar una carta
├── CartaGrafica.cpp     — Implementación gráfica de la carta
├── CartaGrafica.h       — Encabezado gráfico de carta
├── Jogo.h               — Lógica del juego
├── main.cpp             — Punto de entrada del programa
└── CMakeLists.txt       — Archivo de configuración para CMake

## Uso

Desde el directorio build, ejecuta:

```bash
    ./juegoBlackJack
