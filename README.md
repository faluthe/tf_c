# tf_c
Work in Progress TF2 GNU Linux Dynamic Library.

### Dependencies:
- `gdb`
- `gcc`
- `libx11-dev`
- `libglew-dev` (2.1, [see below](#runtime-environment))
- `libsdl2-dev`
- `ProggySquare.tff` (Download [here](https://github.com/bluescan/proggyfonts/blob/master/ProggyOriginal/ProggySquare.ttf))

### Runtime Environment
The Steam Runtime Environment containerizes Team Fortress for portability and compatibility between distros. The runtime uses Linux namespaces to build a predictable environment. You can read more [here](https://gitlab.steamos.cloud/steamrt/steam-runtime-tools/-/blob/main/docs/container-runtime.md#steam-linux-runtime-30-sniper). What this means for us is that we must match the versions and paths of the libraries we link with against the versions and paths present in the container's namespace.

For most users all this means is that you should install GLEW **version 2.1**, instead of the most recent version. `tf_c` will try to link with version 2.1, and the runtime will use version 2.1.

I recommend installing the `libglew-dev` package and then installing `libglew2.1` and `libglew-dev2.1`. You'll probably find these are not available on your package manager. Ubuntu users can find the them [here](http://archive.ubuntu.com/ubuntu/pool/universe/g/glew/) and install them using `sudo dpkg -i`.

### Contributors
- faluthe
- [yoshisaac](https://github.com/yoshisaac)