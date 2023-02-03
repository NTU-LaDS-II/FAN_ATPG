# Interface Package
<small><i>Wang Wei-Shen Last modified in Jan 2023</i></small>

## I. Content

This package contains the interface to scan and parse the input files, e.g. `.v`, `.mdt` . 
In addition of C++, this package use Lex&Yacc for scanning and parsing input files
and turning the inputs into data structure we defined in C++ codes.

Contents of this package are listed here:

    .
    |-- Makefile                # the Makefile for this package
    |-- README.md               # this README.md
    |-- bin/                    # store the executable
    |-- lib/                    # store the library
    `-- src/                    # source code
        |
        |-- global.h
        |
        |-- cell.cpp
        |-- cell.h
        |
        |-- primitive.cpp
        |-- primitive.h
        |
        |-- netlist.cpp
        |-- netlist.h
        |
        |-- netlist_builder.cpp
        |-- netlist_builder.h
        |
        |-- techlib.cpp
        |-- techlib.h
        |
        |-- techlib_builder.cpp
        |-- techlib_builder.h
        |
        |-- mdt_file.cpp
        |-- mdt_file.h
        |-- mdt_file.l
        |-- mdt_file.y
        |
        |-- vlog_file.cpp
        |-- vlog_file.h
        |-- vlog_file.l
        |-- vlog_file.y
        |
        |-- pat_file.cpp
        |-- pat_file.h
        |-- pat_file.l
        |-- pat_file.y
        |
        |-- lht_pat_file.cpp    # deprecated pattern format
        |-- lht_pat_file.h
        |-- lht_pat_file.l
        |-- lht_pat_file.y
        |
        |-- vlog2pmt.cpp        # generate executable for flattening verilog to primitives
        |
        `-- *_test.cpp          # software testing code

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