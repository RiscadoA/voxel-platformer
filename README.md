# RiscadoA's Voxel Platformer

This is a simple platformer game I made from scratch in one week (from
27/02/2021 to 05/03/2021).
I reused a small amount of code from my previous voxel experiments (the meshing
algorithm and the Qubicle file parser).
I wrote an article about my experience making this game, you can read it
[here](https://riscadoa.com/game-dev/voxel-platformer/).

## Configuration

You can change the game configuration by changing the settings on the
configuration [file](vpg.cfg). If the game isn't working on your computer you
can try decreasing the setting `update_fps` to something lower (like 60). You
may also enable fullscreen by setting `window.fullscreen` to `true`.

## Installation

You can either build this project yourself using CMake or download an already
built release for your operating system. 

## Dependencies

For you to build this project you need:
- [GLEW](https://github.com/nigels-com/glew) (used to load OpenGL
extensions)
- [GLFW](https://github.com/glfw/glfw) (used for window and input
management)
- [GLM](https://github.com/g-truc/glm) (used for math)

## Authors

- Ricardo Antunes - [RiscadoA](https://github.com/RiscadoA)

## License

[MIT](LICENSE.txt)

