# Script and files

Various scripts and files used to build and maintain Friction.

**NOTE: Several scripts assume that the host OS is Ubuntu 22.04.**

## build_ci.sh

Our main CI script. This script build and package Friction on Ubuntu 22.04.

Can also be used to easily build and package Friction on other Ubuntu systems.

## run_docker.sh

Scripts used to build and package Friction for each supported Ubuntu release using docker.

## run_vfxplatform.sh

Script used to build the universal Linux binaries using docker.

## make_hicolor.sh

Script used to update the Friction icon theme. Must be run after new icons are added.

**NOTE: `inkscape` must be in `PATH`**
