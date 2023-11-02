# Proyecto de Raycaster estilo Doom en C++ 🚀

Este proyecto es un Raycaster en C++ que simula un entorno estilo Doom. Utiliza las librerías SDL y SDL_Image para cargar imágenes, música y archivos de texto desde la carpeta "assets". El código fuente se encuentra en la carpeta "script" e incluye los archivos: `main.cpp`, `raycaster.h`, `color.h`, `imageLoader.h` y `print.h`. Además, hay una carpeta "SDL2" que contiene las librerías SDL y SDL_Image necesarias para la ejecución.

## Requisitos del Proyecto ⚡

El objetivo de este proyecto es desarrollar un Ray Caster simple que renderice un nivel jugable. Los puntos cubiertos fueron:

1. Controlar al jugador sin atravesar las paredes ni causar fallos. ✅
2. Mantener al menos 30 fps constantemente (los fps se deben mostrar). ❌
3. Implementar una cámara con movimiento hacia adelante y hacia atrás, así como rotación. ✅
4. Rotación horizontal con el mouse. ✅
5. Incluir un minimapa que muestre la posición del jugador en una esquina del mundo. ✅
6. Agregar música de fondo para una experiencia más inmersiva.✅
7. Incorporar efectos de sonido. ✅
8. Agregar al menos una animación a alguna sprite en la pantalla. ✅
9. Incluir una pantalla de bienvenida. ✅
10. Seleccionar entre múltiples niveles. ✅
11. Pantalla de éxito cuando se cumpla una condición en el nivel. ✅

## Ejecución del Proyecto

Este proyecto fue desarrollado en CLion para compilar y ejecutar. Asegúrate de tener instalado SDL y SDL_Image en tu entorno de desarrollo.

## Juego 🎮

![Visualizacion del juego](/juego.png)

## Video de Demostración

[Enlace al Video de Demostración](https://youtu.be/x-zyTPmRmkc)

## Recursos Utilizados 🔩

- Librerías SDL y SDL_Image: Se encuentran en la carpeta "SDL2".

## Clases y Estructuras Principales 🌊
Se muestran detalles del archivo principal del proyecto `Raycaster`.

### Clase Raycaster

La clase `Raycaster` es el núcleo de tu proyecto y se encarga de renderizar el mundo en estilo Doom. Aquí hay una descripción de sus principales atributos y métodos:

- **Atributos:**
    - `player`: Una estructura que representa al jugador con información sobre su posición, orientación y campo de visión.
    - `map`: Un vector que almacena el mapa del mundo.
    - `renderer`: El renderizador de SDL para dibujar en la pantalla.
    - `scale`: Un factor de escala para ajustar la altura de las paredes.
    - `textSize`: El tamaño de textura utilizado para texturizar las paredes.

- **Métodos:**
    - `load_map(const string& filename)`: Carga el mapa desde un archivo de texto.
    - `render()`: Renderiza el mundo y realiza la proyección de rayos.
    - `has_won()`: Verifica si el jugador ha ganado al llegar a una posición específica en el mapa.
    - `draw_victory_screen()`: Dibuja la pantalla de victoria cuando el jugador gana.

### Clase Fog

La clase `Fog` se encarga de dibujar un efecto de niebla en la pantalla para mejorar la estética del juego. Sus atributos principales son:

- `renderer`: El renderizador de SDL para dibujar el efecto de niebla.
- `opacity`: La opacidad de la niebla.
- `color`: El color de la niebla.

El método `draw()` se utiliza para dibujar el efecto de niebla en la pantalla.

### Estructuras Player e Impact

Estas estructuras almacenan información sobre el jugador y los impactos de rayos en el mundo, respectivamente. Son utilizadas por la clase `Raycaster` para realizar cálculos y renderizar el juego.

### Funciones y Métodos Adicionales 🛟

El código también incluye diversas funciones y métodos adicionales para realizar tareas específicas, como proyección de rayos, dibujo de elementos en la pantalla y detección de colisiones.

Asegúrate de explorar el código fuente para obtener una comprensión completa de cómo se implementa el proyecto. Si tienes alguna pregunta o necesitas más información sobre una parte específica del código, no dudes en preguntar.

## Contribuir ❓
Si deseas contribuir a este proyecto, ¡te damos la bienvenida! Puedes abrir problemas (issues) y enviar solicitudes de extracción (pull requests) para mejorar el código y la funcionalidad.

## Licencia 📰

Este proyecto se distribuye bajo la [Licencia MIT](LICENSE).

¡Disfruta del juego! 😎
