    Common package                                                    Aug 2011

                                                                  Kuan-Yu Liao

    I. Content

        This package contains common tools that are frequently used. 
        The following is currently supported:
            1. resource usage (CPU runtime and memory)
            2. option parser
            3. command line interface

        Contents of this package are listed here
            .
            |-- Makefile                # the makefile for this package
            |-- README                  # this README
            |-- bin/                    # the folder to store the executable
            |-- lib/                    # the folder to store the library
            `-- src/                    # the source code
                |-- ascii.h
                |
                |-- cmd.cpp
                |-- cmd.h
                |
                |-- get_opt.cpp         # option parser
                |-- get_opt.h
                |
                |-- sys_cmd.cpp
                |-- sys_cmd.h
                |
                |-- tm_usage.cpp        # resource usage
                |-- tm_usage.h
                |
                |-- vt100.h             # command line interface
                |
                `-- common_test.cpp     # test binary for this package

    II. Usage

        1. resource usage
            Include `tm_usage.h'. Call totalStart() or periodStart() to start
            a total or period resource calculator respectively. Call
            getTotalUsage() to get total usage, getPeriodUsage() to get
            period usage.

            e.g.
                #include "tm_usage.h"
                TmUsage usg;
                usg.totalStart();
                TmStat stat;
                usg.getTotalUsage(stat);

        2. option parser
            Include `get_opt.h'. Register options or arguments using
            regOpt() or regArg() respectively. Call parse() to parse the
            command line. Call getParsedArgs() to get parsed arguments and
            call isFlagSet() and getFlagVar() to check options.

            e.g.
                #include "get_opt.h"
                OptMgr mgr;
                Option opt(Opt::BOOL, "netlist", "file");
                opt.addFlag("f");
                opt.addFlag("file");
                mgr.regOpt(&opt);
                mgr.parse();

        3. command line interface
            Include `cmd.h'. Call regCmd() to register commands. Call read()
            to read commands from stdin. Call exec() to execute a command
            string.

            e.g.
                #include "cmd.h"
                CmdMgr mgr;
                Cmd cmd("read_file");
                mgr.regCmd("I/O", &cmd);
                mgr.read();

        `common_test.cpp' is an example that shows how to use this package.

    III. Build

        Type `make' to install the package.

            $ make


        Type `make clean' to cleanup the installed files.

            $ make clean


        There are two modes to compile the source code. One is the optimized
        mode which comes with the `-O3' flag and the other one is the debug
        mode which can be used with debuggers. To choose between the two
        modes, additional flag `MODE' is needed when making the package.

            $ make MODE=opt         # compile the code using optimize flags
            $ make MODE=dbg         # compile the code using debug flags
            $ make clean MODE=opt   # clean the optimized version
            $ make clean MODE=dbg   # clean the debug version


    IV. Contact

        Please direct your questions to Kuan-Yu Liao (f97943076@ntu.edu.tw)
        or Wei-Shen Wang (b08901051@ntu.edu.tw)