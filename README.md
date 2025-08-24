# satsusl

## Introduction

Satsu SL is a simple scripting language. This repository contains three tools,
libsatsu, csatsu, and gsatsu. The libsatsu library provides a C interface for
interpreting and managing Satsu SL programs. The csatsu tool provides a CLI for
doing the same, built upon libsatsu. The gstatsu tool provides a GUI for doing
the same, built upon libsatsu.

## Dependencies

* A C compiler supporting `-D_GNU_SOURCE`
* A Linux environment (program has not been tested under Windows)
* Make (for gsatsu asset build)
* xxd (for gsatsu asset build)
* SDL2 (for gsatsu)
* SDL2 TTF (for gsatsu)

## Management

* To build libsatsu, run `./libsatsu-build.sh`
* To install libsatsu to the system, run `./libsatsu-install.sh` as root
* To remove libsatsu from the system, run `./libsatsu-uninstall.sh` as root
* To build csatsu, run `./csatsu-build.sh`
* To install csatsu to the system, run `./csatsu-install.sh` as root
* To remove csatsu from the system, run `./csatsu-uninstall.sh` as root
* To build gsatsu, run `./gsatsu-build.sh`
* To install gsatsu to the system, run `./gsatsu-install.sh` as root
* To remove gsatsu from the system, run `./gsatsu-uninstall.sh` as root

Building csatsu and gsatsu requires first building libsatsu, as both programs
rely on the library.

## Usage

Satsu SL can either be used as a library in C programs with libsatsu, or you can
run csatsu / gsatsu for a simple interface to manage and run Satsu SL programs
with base settings (system functions, etc.).

## Contributing

Do not contribute to this repository unless previously agreed upon.
