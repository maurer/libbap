# libbap
C Bindings for [BAP](https://github.com/BinaryAnalysisPlatform/bap)

`libbap` is intended to provide static and shared library versions of BAP as a series of C functions, suitable to be called from other languages.

## Goals
* Export most (if not all) of BAP's capabilities to C.
* Require no OCaml tooling or libraries once installed.
* Be relatively efficient - avoid copies where possible, this is meant to be a thin layer around BAP

## Build/Install
```
oasis setup
./configure
make
make install
```

## Usage
 * Call `bap_init` before calling any function. This function should be idempotent.
 * Make all calls to `libbap` from the thread in which `bap_init` was initially called. The threading rules may be more lax than this, but I'm still trying to nail them down.

## Warning
This project is currently extremely nascent, and likely does not have any functionality that you want yet.
This warning will be removed at first release or when enough functionality is present to be useful.
