# A ground-up 3D renderer in pure C

![Animated gif of a 3D F22 fighter jet cycling through different render modes](assets/f22-small.gif "F22 render modes")

Behold the limited power of a single CPU core performing an uncomfortable amount of
computational geometry!

This exercise in the mathematical foundations of 3D rendering loads models from .obj files and
renders them with perspective-correct textures. Simply drop your model and its PNG texture into the
assets folder, update the asset paths in main.c, compile and run. Low-poly models are recommended
unless you have a really good CPU fan.

This project implements almost everything from scratch, from linear algebra to the .obj file parser.
What did I learn? Why the GPU was invented.

This project is based on [a course by Gustavo
Pezzi](https://pikuma.com/courses/learn-3d-computer-graphics-programming), with significant
modifications to the source code by me to improve performance, readability and maintainability. It
is distributed under the same GNU General Public License as the original. Additional thanks go to
gustavopezzi for [the dynamic array code](https://github.com/gustavopezzi/dynamicarray), and to
elanthis for [the PNG decoder, upng](https://github.com/elanthis/upng). The F22 model and texture
were created by Artem Katrych.