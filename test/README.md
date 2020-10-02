# Lab 3 : Myshell
First iteration.

## Team

 - [Anna Botsula](https://github.com/botsula)

## Prerequisites

 - **C++ compiler** - needs to support **C++17** standard
 - **CMake** 3.15+
 
The rest prerequisites (such as development libraries) can be found in the [packages file](./apt_packages.txt) in the form of the apt package manager package names.

## Installing


1. Build.
    ```bash
    cmake -G"Unix Makefiles" -Bbuild
    cmake --build build
    ```
   
2. Put internal programs
    Put programs from the `/bin/..` directory to the build directory. Now there is only `mycat`. 

## Usage

```bash
$ myshell mscr.msh # execute script in internall process
$ .mscr.msh        # execute script in internall process
$ ./mscr.msh       # execute script in externall process

$ merrno           # show the code of last error
$ mpwd             # show current working directory
$ mcd ./../[path]  # change directory
$ mexport VAR=VAL  # export variable to environment
$ mexit            # exit myshell

$ ls               # call /bin/ls
$ echo ...         # call /bin/echo
$ cat ...          # call /bin/cat
etc.

   
```
Now supporting `-h` command and `wildcards` are not available, but release will be upgraded soon.
