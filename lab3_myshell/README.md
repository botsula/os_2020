# Lab 6 : Myshell + Sockets
Implementation of Shell with sockets option.

> Server supports only one client

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

## Create or not create server ?

#### To start **myshell** locally :
```bash
$ ./myshell
```

#### To start **myshell server** :
```bash
$ ./myshell --server
```
As default option, it will start on **PORT = 8080**.
But if you want to mention your special port, just start **myshell** as :
```bash
$ ./myshell --server --port [PORT]
```

As a connection tool (when server is already started) you can use **telnet** utility as following:
```bash
$ telnet 127.0.0.1 8080
```

Now you can do everything you need using socket!
Interface is the same, it would be described in **Usage** part.


Also all the information is logged in the server terminal and looks as following:
```bash
[myshell] SERVER IS MENTIONED
[myshell] SOCKET PORT: 8080
[myshell] SOCKET ADDRESS: 127.0.0.1
[myshell] ++ls++        from 127.0.0.1 : Mon Dec 14 12:55:32 2020
[myshell] ++ls++        from 127.0.0.1 : Mon Dec 14 12:55:34 2020
[myshell] ++mpwd++      from 127.0.0.1 : Mon Dec 14 12:55:35 2020
[myshell] ++mpwd++      from 127.0.0.1 : Mon Dec 14 12:55:42 2020
[myshell] SHUTDOWN OK: 0
```
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
