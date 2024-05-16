# Friction

Friction is a powerful and versatile motion graphics application that allows you to create stunning vector and raster animations for web and video platforms with ease. See [friction.graphics](https://friction.graphics) for more information.

## Contribute

We accept any contributions (in form of a PR). Before submitting a PR it's recommended that you communicate with the developers first (in `Issues` or `Discussions`). Someone might already be working on the same feature/issue, or for some reason the feature is not wanted.

It's always preferred to submit PR's against the `main` branch. If your feature is experimental, a new branch could be made available for further development before entering `main`.

## Branches and versions

Friction uses `X.Y.Z` version numbers and `X.Y` branches.

* `X` = Major
* `Y` = Minor
* `Z` = Patch

Branch `main` is always the current branch for the next `X` or `Y` release. The `main` branch should not be considered stable, but usable.

A new stable branch is cut from `main` on each `X` or `Y` release and is maintained until a new stable branch is created.

The next `Z` release usually comes from the latest stable branch (`vX.Y`).

Breaking features under development should be kept in it's own branch until ready to be merged with `main`.

## Build

Generic build instructions.

### Requirements

* pkg-config *(Linux)*
* ninja
* python3
* cmake *(3.12+)*
* clang *(7+)*
* MSVC 2017 *(Windows)*
* Qt *(5.15)*
    * Gui
    * Widgets
    * OpenGL
    * Multimedia
    * Qml
    * Xml
* qscintilla
* ffmpeg *(4.2)*
    * libavformat
    * libavcodec
    * libavutil
    * libswscale
    * libswresample
* libunwind *(Linux)*
* expat
* harfbuzz
* freetype
* fontconfig
* libjpeg-turbo
* libpng
* libwebp
* zlib
* icu

### Get

```
git clone --recurse-submodules https://github.com/friction2d/friction
```

or download a release tarball *(friction-VERSION.tar.bz2)*.

### Build

```
cd friction
mkdir build
cd build
```

```
cmake -G Ninja \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=/usr \
-DCMAKE_CXX_COMPILER=clang++ \
-DCMAKE_C_COMPILER=clang \
..
```
Note that on some configurations you may need to specify `qscintilla` paths:
```
-DQSCINTILLA_INCLUDE_DIRS=<PATH_TO_QSCINTILLA_INCLUDE_DIR> \
-DQSCINTILLA_LIBRARIES_DIRS=<PATH_TO_LIBS> \
-DQSCINTILLA_LIBRARIES=<QSCINTILLA_LIBRARY_NAME>
```

Now build:

```
cmake --build .
```

## License

Copyright &copy; Friction [contributors](https://github.com/friction2d/friction/graphs/contributors).

Friction is a fork of [enve](https://github.com/MaurycyLiebner/enve). enve is copyright &copy; [Maurycy Liebner](https://github.com/MaurycyLiebner).

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

**This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the [GNU General Public License](LICENSE.md) for more details.**
