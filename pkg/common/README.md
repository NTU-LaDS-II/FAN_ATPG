# Common Package
###### *Kuan-Yu Liao Started in Aug 2011*
###### *Wang Wei-Shen Last Modified in Jan 2023*

## I. Content

This package contains common tools that are frequently used.

The following is currently supported:
1. resource usage (CPU runtime and memory)
2. option parser
3. command line interface

Contents of this package are listed here:

    .
    |-- Makefile                # the makefile for this package
    |-- README.md               # this README
    |-- bin/                    # the folder to store the executable
    |-- lib/                    # the folder to store the library
    `-- src/                    # the source code
        |
        |-- ascii.h             # define ascii code of some characters
        |
        |-- cmd.cpp
        |-- cmd.h               # define class CmdMgr (Command Manager)
        |
        |-- get_opt.cpp         
        |-- get_opt.h           # parse options and arguments from command line, e.g. -h
        |
        |-- sys_cmd.cpp
        |-- sys_cmd.h           # support basic command for shell, e.g. pwd
        |
        |-- tm_usage.cpp        # resource usage
        |-- tm_usage.h
        |
        |-- vt100.h             # command line interface interactions
        |
        `-- common_test.cpp     # software testing for this package

## II. Usage

1. resource usage

    Include `tm_usage.h`. Call `totalStart()` or `periodStart()` to start a total or period resource calculator respectively. Call `getTotalUsage()` to get total usage, `getPeriodUsage()` to get period usage.

    ```cpp
    #include "tm_usage.h"

    TmUsage usg;
    usg.totalStart();
    TmStat stat;
    usg.getTotalUsage(stat);
    ```

2. option parser

    Include `get_opt.h`. Register options or arguments using
    `regOpt()` or `regArg()` respectively. Call `parse()` to parse the
    command line. Call `getParsedArgs()` to get parsed arguments and
    call `isFlagSet()` and `getFlagVar()` to check options.

    ```cpp
    #include "get_opt.h"

    OptMgr mgr;
    Option opt(Opt::BOOL, "netlist", "file");
    opt.addFlag("f");
    opt.addFlag("file");
    mgr.regOpt(&opt);
    mgr.parse();
    ```

3. command line interface

    Include `cmd.h`. Call `regCmd()` to register commands. Call `read()` to read commands from `stdin`. Call `exec()` to execute a command string.

    ```cpp
    #include "cmd.h"

    CmdMgr mgr;
    Cmd cmd("read_file");
    mgr.regCmd("I/O", &cmd);
    mgr.read();
    ```

*`common_test.cpp` is an example that shows how to use this package.*

## III. Build

Install the package
```sh
make
```

Cleanup the installed files
```sh
make clean
```


There are two modes to compile the source code. One is the optimized
mode which comes with the `-O3` flag and the other one is the debug
mode which can be used with debuggers. To choose between the two
modes, additional flag `MODE` is needed when making the package.

```makefile
make MODE=opt         # compile the code using optimize flags
make MODE=dbg         # compile the code using debug flags
make clean MODE=opt   # clean the optimized version
make clean MODE=dbg   # clean the debug version
```

## IV. Contact

Liao Kuan-Yu - f97943076@ntu.edu.tw<br>
Wang Wei-Shen - b08901051@ntu.edu.tw