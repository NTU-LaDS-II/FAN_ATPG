<h1>Core Package</h1>                     
<h6><i>Wei-Shen Wang Last modified in Jan 2023</i></h6>

## I. Content

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
        |-- pattern.h           # Define PatternProcessor as a 
        |                       # container for ATPG patterns
        |
        |-- simulator.cpp       # Contains the algorithm for 
        `-- simulator.h         # good/fault simulation

## II. Usage

- Circuit

    ```cpp
    #include "circuit.h"
    #include "interface/netlist.h"

    int nframe = 1;
    Netlist netlist;
    ...
    Circuit circuit;
    circuit.buildCircuit(&netlist, nframe);
    ```

- FaultListExtract

    ```cpp
    #include "circuit.h"
    #include "fault.h"

    Circuit circuit;
    ...
    FaultListExtract fListExtract;
    fListExtract.extractFaultFromCircuit(&circuit);
    ```

- Simulator, PatternProcessor

    ```cpp
    #include "circuit.h"
    #include "fault.h"
    #include "pattern.h"
    #include "simulator.h"

    Circuit circuit;
    FaultListExtract fListExtract;
    ...
    PatternProcessor patternCollector(&circuit);
    ...
    Simulator simulator(&circuit);
    simulator.parallelPatternFaultSimWithAllPattern(&patternCollector, &fListExtract);
    ```

- Atpg

    ```cpp
    #include "atpg.h"
    #include "circuit.h"
    #include "fault.h"
    #include "pattern.h"
    #include "simulator.h"

    Circuit circuit;
    FaultListExtract fListExtract;
    ...
    PatternProcessor patternCollector(&circuit);
    ...
    Simulator simulator(&circuit);
    ...
    Atpg atpg(&circuit, &simulator);
    atpg.generatePatternSet(&patternCollector, &fListExtract, true);
    ```

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
> Wei-Shen Wang - b08901051@ntu.edu.tw