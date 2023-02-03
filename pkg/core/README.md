# Core Package                 
###### *Wang Wei-Shen Last modified in Jan 2023*

## I. Content

This package contains the core algorithm of the whole FAN ATPG project.

Currently support:
1. Fault Collapsing
2. Parallel Fault Fault Simulation
3. Parallel Pattern Fault Simulation
4. FAN ATPG algorithm
5. Static Test Compression
6. Dynamic Test Compression
7. Multiple Fault Orderings
8. Netlist to Circuit Conversion

Contents of this package are listed here:

    .
    |-- Makefile                # the Makefile for this package
    |-- README.md               # this README.md
    |-- lib/                    # store the library
    `-- src/                    # source code
        |
        |-- atpg.cpp
        |-- atpg.h              # The main ATPG algorithm including STC, DTC
        |
        |-- circuit.cpp
        |-- circuit.h           # Transform the Netlist parsed from Verilog file into a Circuit
        |
        |-- decision_tree.h     # The data structure for backtracking in the FAN algorithm
        |
        |-- fault.cpp           
        |-- fault.h             # Extract all necessary Faults for ATPG
        |
        |-- gate.h              # Define class Gate with attributes for ATPG and good/fault simulation
        |
        |-- logic.cpp
        |-- logic.h             # Set, get, print, the logic values
        |
        |-- pattern_rw.cpp
        |-- pattern_rw.h        # Define pattern reader and writer
        |
        |-- pattern.h           # Define PatternProcessor as a container for ATPG patterns
        |
        |-- simulator.cpp
        `-- simulator.h         # Contains the procedure for good/fault simulation

## II. Usage

* Circuit

    ```cpp
    #include "circuit.h"
    #include "interface/netlist.h"

    int nframe = 1;
    Netlist netlist;
    ...
    Circuit circuit;
    circuit.buildCircuit(&netlist, nframe);
    ```

* FaultListExtract

    ```cpp
    #include "circuit.h"
    #include "fault.h"

    Circuit circuit;
    ...
    FaultListExtract fListExtract;
    fListExtract.extractFaultFromCircuit(&circuit);
    ```

* Simulator, PatternProcessor

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

* Atpg

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
Wang Wei-Shen - b08901051@ntu.edu.tw