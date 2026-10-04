# Fault reporting regression

After building FAN with the existing Makefile, run:

```sh
make -j2
python3 tests/report_fault_scan.py
```

This uses only the bundled Nangate45 library, modified netlists and patterns for
s27, s5378, s9234 and s15850. It checks complete collapsed SAF exports, unique
scan identities, positive equivalence multiplicities, all weighted state counts,
DT/UD filtering and the special control faults. It also checks the functional Q/D
sites of every scan FF in s27. It performs fault simulation, without ATPG.

To compare statistics with an unmodified build of the same source revision:

```sh
python3 tests/report_fault_scan.py --baseline /path/to/unmodified/fan
```

The baseline executes simulation and statistics only, avoiding its faulty reporter.
No new patterns, libraries or netlists are generated. A single small case can be
selected with `--designs s27`, and `--fan` can select another executable.

A minimal reporting probe, run from the repository root, is:

```text
read_lib techlib/mod_nangate45.mdt
read_netlist mod_netlist/s27.v
build_circuit --frame 1
set_fault_type saf
add_fault -a
report_fault
exit
```

Save it as `report.script` and run `bin/opt/fan -f report.script`. The unmodified
reporter declares 90 faults but prints zero rows; the repaired reporter prints
all 90 collapsed fault classes. Replacing the report command with
`report_fault -s UD` exercises the scan site names. Using `report_fault -s DT`
exercises the negative CK/test_si/test_so/test_se gate IDs; an instrumented
unmodified reporter reads outside the gate array before handling these IDs.

Each repaired fault row includes `[equivalent=N]`, where N is the existing
collapsed class multiplicity. This is an output-format addition; extraction,
collapsing and simulation remain unchanged.
