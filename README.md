# Friction

[![Logo](https://friction.graphics/assets/logo-uni.svg)](https://friction.graphics)

Friction is an open-source, vector-based application designed for creating web animations and videos.

***This application is beta quality, expect missing and/or broken features.***

## Features

### High Performance

![Screenshot 1](https://friction.graphics/assets/screenshots/friction-screenshot.jpg)

Friction provides a scalable, high-performance GPU/CPU vector and raster pipeline developed in C++, using Skia and the Qt framework.

### Animations for the Web

![Screenshot 2](https://friction.graphics/assets/screenshots/friction-web-animations.jpg)

Create smooth and responsive animations for the web with Friction! Using Friction, you can create animated scalable vector graphics (SVG) using the synchronized multimedia integration language (SMIL), as recommended by the World Wide Web Consortium (W3C).

### Multiple Scenes and Timelines

![Screenshot 3](https://friction.graphics/assets/screenshots/friction-multiple-scenes.jpg)

Friction gives you the flexibility to work on multiple scenes at once, with multiple viewers, timelines, and graphs. Adjust it to fit your needs and screen real estate.

### Powerful Expression Editor

![Screenshot 4](https://friction.graphics/assets/screenshots/friction-expressions.jpg)

Power users are welcome! Friction provides support for expressions with ECMAScript in parameters and effects. Additionally, it includes a complete code editor with autocompletion, syntax highlighting, custom functions, and more.

### Text and Path Effects

![Screenshot 5](https://friction.graphics/assets/screenshots/friction-text-effect.jpg)

Friction includes a diverse range of effects for enhancing your vector graphics and text. Stay tuned for upcoming releases, as we continue to expand our collection of effects.

### Raster/Shader Effects

![Screenshot 6](https://friction.graphics/assets/screenshots/friction-shader-effects.jpg)

Friction offers support for user-customizable shader effects that can be added while the application is running. It includes a foundational set of core effects, such as motion blur.

## System Requirements

Most computers in use today should work just fine with Friction. The only strict requirements are:

* OpenGL 3.3 compatible graphics card
  * NVIDIA GeForce 8000 series or higher
  * AMD Radeon HD 2000 series or higher
  * Intel HD Graphics 4000 or higher
* AVX compatible CPU
  * Intel Sandy Bridge or higher
  * AMD Bulldozer or higher

In general, a dual-core laptop with 4GB of RAM should be usable for small projects. Larger projects and/or the use of raster graphics and effects will require more RAM and CPU/GPU power.

## Supported Platforms

Friction currently supports the following platforms:

* Windows 10 / 11
* Ubuntu 22.04 LTS / 23.04
* Fedora 38
* Enterprise Linux 9

Platform support may undergo changes in future releases without prior notice.

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

*During `v0.9` development minor and patch releases are combined. Meaning `v0.9.x` will contain both features and fixes. This is an exception until we reach `v1.0.0`.*

## License

Copyright &copy; Friction [contributors](https://github.com/friction2d/friction/graphs/contributors).

Friction is a fork of [enve](https://github.com/MaurycyLiebner/enve). Copyright &copy; [Maurycy Liebner](https://github.com/MaurycyLiebner).

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

**This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the [GNU General Public License](LICENSE.md) for more details.**

Friction includes several [icons](https://github.com/Shrinks99/blender-icons) from Blender. These icons are released under a [CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/) license.

## Contributors

### Friction

* Ole-André Rodlie *(@rodlie)*
* sanjuchopracool *(@sanjuchopracool)*

### enve

* Maurycy Liebner *(@MaurycyLiebner)*
* Alex Kiryanov *(@AlexKiryanov)*
* Poren Chiang *(@rschiang)*
* goyalyashpal *(@goyalyashpal)*
* eszlari *(@eszlari)*
* Alexandre Prokoudine *(@prokoudine)*
* nabbisen *(@nabbisen)*
* luzpaz *(@luzpaz)*
