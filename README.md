# gpu-pm
[WIP] photon mapper for GPU using vulkan

this is currently incomplete, i advise against trying to use this for now. also, i don't have a deadline to finish this.

this is free software.

# requirements for building

- assimp (>= 5.0.0)

- glm (>= 0.9.9)

- vulkan compatible device (>= 1.1)

- glslc (for shader compiling. this should come with the latest LunarG vulkan SDK, just make sure it's in an executable path)

- meson (use a reasonably recent version)

- ninja (or some other meson backend, but i'm only using ninja)

- some C++ compiler

# how to build

- clone this repository and go to the directory;

- enter the command **meson setup build**;

- go to the created directory **cd build**;

- enter the command **ninja**. if everything works you'll have a **demo** executable in the build directory.

# usage

(i'll complete this in the future when this program is actually user-ready).

# contributing

i'm not accepting contributions right now, but maybe i will in the future.

have any suggestion, problem, question, or maybe you want to confess your love or something? open an issue. maybe i'll respond maybe not.

want me to work on this? give it a motivational star.
