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
 * Call `bap_init` before calling any function.

### Threading
You have two basic options:
 1. Make all calls to `libbap` from the thread in which `bap_init` was initially called.
 2. Be aware that calls into `libbap` all block each other. This is due to a limitation in the OCaml runtime.
   * The call to `bap_init` also acts as a call to `bap_thread_register` followed by `bap_acquire`
   * Every thread which calls into `libbap` *must* call `bap_thread_register` first.
   * Any calls into `libbap` *must* be bracketed by `bap_acquire` and `bap_release` calls.
   * The lock *must not* be held by a thread calling `bap_thread_unregister` or `bap_thread_register`
   * Any thread which registered with `libbap` via `bap_thread_register` *must* call `bap_thread_unregister` before termination.

## Warning
This project is currently extremely nascent, and likely does not have any functionality that you want yet.
This warning will be removed at first release or when enough functionality is present to be useful.
