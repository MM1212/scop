# Scop

Render a 3D model with Vulkan.

## Credits
[Vulkan (c++) Game Engine Tutorials by Brendan Galea](https://www.youtube.com/playlist?list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR)

## Dependencies
- [premake5](https://premake.github.io/download.html)
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)
- [GLFW](https://www.glfw.org/download.html)
- [GLM](https://glm.g-truc.net/0.9.8/index.html)
- [entt](https://github.com/skypjack/entt)

## Build
```bash
premake5 <target> # target: (linux)gmake2, (windows)vs2019
make
```
## Run
```bash
./bin/<target>-<os>/Scop/scop
```