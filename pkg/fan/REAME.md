    Fan package                                           last commented Jan 2023

                                                                    Wei-Shen Wang

    I. Content

        This package contains the command line instruction for access of the 
        algorithm in the Core package and the entry point (main()) of this
        whole project.

        The following is currently supported:

        Contents of this package are listed here
            .
            |-- Makefile                # the makefile for this package
            |-- README                  # this README
            |-- bin/                    # the folder to store the executable
            |-- lib/                    # the folder to store the library
            `-- src/                    # the source code
                |-- atpg_cmd.cpp        # define classes for command designed
                |-- atpg_cmd.h          # for this project, e.g. atpg, fault
                |                       # simulation, read/write pattern
                |
                |-- fan_mgr.h           # define the class FanMgr, Fan Manager
                |                       # for storing all the main object for 
                |                       # Circuit, ATPG, fault simulation as
                |                       # its attributes
                |
                |-- main.cpp            # contains the main function and main
                |                       # set up for this command line tool
                |
                |-- misc_cmd.cpp
                |-- misc_cmd.h
                |
                |-- setup_cmd.cpp
                `-- setup_cmd.h

    II. Usage



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

        Please direct your questions to Wei-Shen Wang (b08901051@ntu.edu.tw)