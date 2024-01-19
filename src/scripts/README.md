# Script and files

Various scripts and files used to build and maintain Friction.

**NOTES:**

* Some scripts may assume you are running Ubuntu 22.04
* Scripts in this folder must be run from the root Friction folder! (`./src/scripts/script.sh`)
* Never run `build_vfxplatform*.sh` or `build_docker.sh` outside of docker!

## build_ci.sh

Our main CI script. This script build and package Friction on Ubuntu 22.04.

Can also be used to easily build and package Friction on other Ubuntu systems.

## run_docker.sh

Script used to build and package Friction for supported Ubuntu releases using docker.

## run_vfxplatform.sh

Script used to build the universal Linux binaries using docker.

## make_hicolor.sh

Script used to update the Friction icon theme. Must be run after new icons are added.

**NOTE: `inkscape` must be in `PATH`**
