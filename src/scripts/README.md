# Script and files

Various scripts and files used to build and maintain Friction.

## easing

Various JS easing scripts. Used in the expression editor in Friction.

## build_ci.sh

Our main CI build script. This script build and package Friction on Ubuntu 22.04.

Can also be used to easily build and package Friction on other Ubuntu systems.

## run_docker.sh / build_docker.sh

Scripts used to build and package Friction for each supported Ubuntu release using docker.

## build_macos.sh

Old macOS build script. Currently not supported.

## run_vfxplatform.sh / build_vfxplatform*.sh

Scripts used to build the universal Linux binaries. Using CentOS7 running through docker.

## make_hicolor.sh

Script used to update the Friction icon theme. Must be run after new icons are added.

**NOTE: `inkscape` must be in `PATH`**

## make_ico.sh

Generates a win32 application icon for Friction.

## mkicns.sh

Generates a macOS application icon for Friction.
