    Core package                                          last commented Jan 2023

                                                                    Wei-Shen Wang

    I. Content

        This package contains the core algorithm of the whole FAN ATPG project.

        The following is currently supported:
            1. FAN ATPG algorithm
            2. Static Test Compression
            3. Dynamic Test Compression
            4. Multiple Fault Orderings
            5. Parallel Fault Fault Simulation
            6. Parallel Pattern Fault Simulation
            7. Fault collapsing
            8. Netlist to Circuit Conversion

        Contents of this package are listed here
            .
            |-- Makefile                # the makefile for this package
            |-- README                  # this README
            |-- bin/                    # the folder to store the executable
            |-- lib/                    # the folder to store the library
            `-- src/                    # the source code
                |-- atpg.cpp            # The main ATPG algorithm including
                |-- atpg.h              # STC, DTC, MFO
                |
                |-- circuit.cpp         # Transform the Netlist parsed from
                |-- circuit.h           # Verilog file into a Circuit
                |
                |-- decision_tree.h     # The data structure for backtracking
                |                       # in the FAN algorithm
                |
                |-- fault.cpp           # Extract all necessary Faults for
                |-- fault.h             # ATPG
                |
                |-- gate.h              # Define class Gate with attributes
                |                       # for ATPG and good/fault simulation
                |
                |-- logic.cpp           # Set, get, print, the logic values
                |-- logic.h
                |
                |-- pattern_rw.cpp      # Define pattern reader and writer
                |-- pattern_rw.h
                |
                |-- pattern.h           # Define PatternProcessor as a container
                |                       # for ATPG patterns
                |
                |-- simulator.cpp       # Contains the algorithm for good/fault
                `-- simulator.h         # simulation

    II. Usage

        Circuit:
            # cir is a Circuit object, the following function will build it
            # with a Netlist object parsed from the verilog file
            e.g. cir->buildCircuit(fanMgr_->nl, nframe);

        FaultListExtract:
            # fListExtract is a FaultListExtract object, the following function
            # will extract the necessary faults from the Circuit of FaultListExtract
            e.g. fListExtract->extractFaultFromCircuit();

        PatternProcessor:
        Atpg:
            # atpg is a Atpg object, the following function will generate a
            # test pattern set for according to the fListExtract and store 
            # them into pcoll, the last parameter is true indicating that
            # Multiple Fault Orderings is turned ON
            e.g. atpg->generatePatternSet(fanMgr_->pcoll, fanMgr_->fListExtract, true);

        Simulator:
            # sim is a Simulator object, the following function will do 
            # parallel pattern fault simulation with all the patterns in 
            # pcoll on all the faults in fListExtract
            e.g. sim->parallelPatternFaultSimWithAllPattern(fanMgr_->pcoll, fanMgr_->fListExtract);


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