# A ground-up 3D renderer in pure C

Behold the limited power of a single CPU core performing an uncomfortable amount of
computational geometry!

![Animated gif of a 3D F22 fighter jet cycling through different render modes](assets/f22-small.gif "F22 render modes")

This exercise in the mathematical foundations of 3D rendering loads models from .obj files and
renders them with perspective-correct textures. It demonstrates:
  * Linear algebra for perspective projection of models and textures
  * Flat shading
  * Back-face culling
  * Ambient lighting
  * An .obj file parser
  * 8 rendering modes

What did I learn? Why the GPU was invented.

## Usage
Simply drop your model and its PNG texture into the assets folder, update the asset paths in main.c,
compile and run. Low-poly models are recommended unless you have a really good CPU fan.

Rendering mode keybindings:
  * 1: Vertex-only
  * 2: Vertex wireframe
  * 3: Wireframe only
  * 4: Flat-shaded
  * 5: Flat-shaded with wireframe
  * 6: Textured
  * 7: Textured with wireframe
  * c: Toggle back-face culling

## Future improvements
* Camera control
* Clipping of assets outside the rendering frustum
* Inline triangle and fgace vertex getters with macros
* Improved consistency in the function signatures of rendering methods
* Specify assets to render via the command line
* Further performance optimization

## Acknowledgements
This project is based on [a course by Gustavo
Pezzi](https://pikuma.com/courses/learn-3d-computer-graphics-programming), with significant
modifications to the source code by me to improve performance, readability and maintainability. It
is distributed under the same GNU General Public License as the original. Additional thanks go to
gustavopezzi for [the dynamic array code](https://github.com/gustavopezzi/dynamicarray), and to
elanthis for [the PNG decoder, upng](https://github.com/elanthis/upng). The F22 model and texture
were created by Artem Katrych.