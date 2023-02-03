# Fan Package                                           
<small><i>Wang Wei-Shen Last modified in Jan 2023</i></small>

## I. Content

This package contains the command line instruction for access of the algorithm in the Core Package and the Entry Point ```main()``` of this whole project.

Contents of this package are listed here:

    .
    |-- Makefile                # the Makefile for this package
    |-- README.md               # this README.md
    |-- bin/                    # store the executable
    |-- lib/                    # store the library
    `-- src/                    # source code
        |
        |-- atpg_cmd.cpp        
        |-- atpg_cmd.h          # define classes for command designed for this project, e.g. ATPG, Fault Simulation, read/write pattern
        |
        |-- fan_mgr.h           # define the class FanMgr (Fan Manager) for storing all the main object for ATPG and Fault Simulation
        |
        |-- main.cpp            # Entry Point of this command line tool
        |
        |-- misc_cmd.cpp
        |-- misc_cmd.h          # define classes for miscellaneous command, e.g. report pattern format, report memory usage
        |
        |-- setup_cmd.cpp
        `-- setup_cmd.h         # define classes for initial setup command before ATPG or Fault Simulation, e.g. read Verilog    

## II. Usage
TODO

## III. Build

Type `make` to install the package.

```sh
make
```

Type `make clean` to cleanup the installed files.

```sh
make clean
```


There are two modes to compile the source code. One is the optimized
mode which comes with the `-O3` flag and the other one is the debug
mode which can be used with debuggers. To choose between the two
modes, additional flag `MODE` is needed when making the package.

```make
make MODE=opt         # compile the code using optimize flags
make MODE=dbg         # compile the code using debug flags
make clean MODE=opt   # clean the optimized version
make clean MODE=dbg   # clean the debug version
```

## IV. Contact
Wang Wei-Shen - b08901051@ntu.edu.tw