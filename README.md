# Linearly Transformed Cosines
This is a fork of the original [LTC code base by selfshadow](https://github.com/selfshadow/ltc_code/). It has various improvements which include:
* CMake build system
* Dependency management using [vcpkg](https://github.com/microsoft/vcpkg)
* Split fitting code into library + executable to make it easier to integrate in another project.
* Parallel fitting code for significantly improved fitting speed.
* Modernize code base in a few places (definitely more work to be done)
  * No more `new`/`delete` (replaced by `std::vector`)
  * Replace C math calls by their C++ equivalent (e.g. `std::sqrt` instead of `sqrtf`)
  * No more `using namespace`
  * Added all code to `ltc` namespace

## Overview
This repository contains an (evolving) reference implementation for the following publications:
* [Real-Time Polygonal-Light Shading with Linearly Transformed Cosines](https://eheitzresearch.wordpress.com/415-2/) (2016)
* [Real-Time Area Lighting: a Journey From Research to Production](https://blog.selfshadow.com/publications/s2016-advances/) (2016)
* [Linear-Light Shading with Linearly Transformed Cosines](https://blogs.unity3d.com/2017/04/17/linear-light-shading-with-linearly-transformed-cosines/) (2017)
* [Real-Time Line- and Disk-Light Shading](https://blog.selfshadow.com/publications/s2017-shading-course/) (2017)

## Notes:
* The Linearly Transformed Cosine (LTC) tables in this implementation differ in their parameterisation and storage compared to [the original paper](https://eheitzresearch.wordpress.com/415-2/). See fitting code and WebGL demos for details.

## WebGL Demos
[Quad lights](http://blog.selfshadow.com/ltc/webgl/ltc_quad.html)  
[Line lights](http://blog.selfshadow.com/ltc/webgl/ltc_line.html)  
[Disk lights](http://blog.selfshadow.com/ltc/webgl/ltc_disk.html)  
