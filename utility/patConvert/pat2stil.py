#!/ibmnas/427/r101/walts850221/local/bin/python3
#############################################################################
# File       [ pat2stil.py ]
# Author     [ littleshamoo ]
# Synopsis   [ Converts 2012 PAT format to STIL ]
# Date       [ 2013/08/26 created ]
#############################################################################

import optparse
import re
import sys
import os
import os.path

def main(): #{{{
    (opts, args) = parseArg()
    ipat = args[0]
    if not os.path.isfile(ipat):
        print("**ERROR file `" + ipat + "' does not exit", file=sys.stderr)
        exit(0)

    ifile = open(ipat, "r")

    pis   = []
    pos   = []
    dffs  = []

    #{{{ find keyword __PI_ORDER__
    piOrderFound = False
    while True:
        # parse line and remove empty elements
        line = ifile.readline()
        if not line:
            break
        linelist = re.split(re.compile("[ \\\\\n\"=,():]+"), line)
        while "" in linelist:
            linelist.remove("")
        if not linelist:
            continue

        # find keyword '__PI_ORDER__'
        if len(linelist) > 0 and linelist[0] == "__PI_ORDER__":
            piOrderFound = True
            pis = linelist[1:]
            # generate test ports
            if opts.genTestPorts:
                pis.insert(0, opts.se)
                pis.insert(0, opts.si)
                pis.insert(0, opts.clk)
            break

    # check setup
    if not piOrderFound:
        print("**ERROR wrong format `__PI_ORDER__' not found"
            , file=sys.stderr)
        exit(0)
    #}}}
    #{{{ find keyword __PO_ORDER__
    poOrderFound = False
    while True:
        # parse line and remove empty elements
        line = ifile.readline()
        if not line:
            break
        linelist = re.split(re.compile("[ \\\\\n\"=,():]+"), line)
        while "" in linelist:
            linelist.remove("")
        if not linelist:
            continue

        # find keyword '__PO_ORDER__'
        if len(linelist) > 0 and linelist[0] == "__PO_ORDER__":
            poOrderFound = True
            pos = linelist[1:]
            break

    # check setup
    if not poOrderFound:
        print("**ERROR wrong format `__PO_ORDER__' not found"
            , file=sys.stderr)
        exit(0)
    #}}}
    #{{{ find keyword __SCAN_ORDER__
    scanOrderFound = False
    while True:
        # parse line and remove empty elements
        line = ifile.readline()
        if not line:
            break
        linelist = re.split(re.compile("[ \\\\\n\"=,():]+"), line)
        while "" in linelist:
            linelist.remove("")
        if not linelist:
            continue

        # find keyword '__SCAN_ORDER__'
        if len(linelist) > 0 and linelist[0] == "__SCAN_ORDER__":
            scanOrderFound = True
            dffs = linelist[1:]
            break

    # check setup
    if not scanOrderFound:
        print("**ERROR wrong format `__SCAN_ORDER__' not found"
            , file=sys.stderr)
        exit(0)
    #}}}

    #{{{ print Version
    print("STIL 1.0;")
    print()
    #}}}
    #{{{ print Signals
    print("Signals {")
    for pi in pis:
        if pi == opts.si and len(dffs) > 0:
            print("    \"" + pi + "\" In { ScanIn; }")
        else:
            print("    \"" + pi + "\" In;")
    for po in pos:
        if po == opts.so and len(dffs) > 0:
            print("    \"" + po + "\" Out { ScanOut; }")
        else:
            print("    \"" + po + "\" Out;")
    print("}")
    print()
    #}}}
    #{{{ print SignalGroups
    print("SignalGroups {")
    # group '_pi'
    print("    \"_pi\" = '", end="")
    for i in range(0, len(pis)):
        print("\"" + pis[i] + "\"", end="")
        if i + 1 != len(pis):
            print(" + ", end="")
    print("';")

    # group '_in'
    print("    \"_in\" = '", end="")
    for i in range(0, len(pis)):
        print("\"" + pis[i] + "\"", end="")
        if i + 1 != len(pis):
            print(" + ", end="")
    print("';")

    # group '_si'
    if len(dffs) > 0:
        print("    \"_si\" = '\"" + opts.si + "\"' { ScanIn; }")

    # group '_po'
    print("    \"_po\" = '", end="")
    for i in range(0, len(pos)):
        print("\"" + pos[i] + "\"", end="")
        if i + 1 != len(pos):
            print(" + ", end="")
    print("';")

    # group '_out'
    print("    \"_out\" = '", end="")
    for i in range(0, len(pos)):
        print("\"" + pos[i] + "\"", end="")
        if i + 1 != len(pos):
            print(" + ", end="")
    print("';")

    # group '_so'
    if len(dffs) > 0:
        print("    \"_so\" = '\"" + opts.so + "\"' { ScanOut; }")

    # group '_default_In_Timing_'
    print("    \"_default_In_Timing_\" = '", end="")
    for i in range(0, len(pis)):
        if pis[i] != opts.clk:
            print("\"" + pis[i] + "\"", end="")
            if i + 1 != len(pis):
                print(" + ", end="")
    print("';")

    # group '_default_Out_Timing_'
    print("    \"_default_Out_Timing_\" = '", end="")
    for i in range(0, len(pos)):
        print("\"" + pos[i] + "\"", end="")
        if i + 1 != len(pos):
            print(" + ", end="")
    print("';")

    print("}")
    print()
    #}}}
    #{{{ print Timing
    cycle = int(opts.cycle)
    print("Timing {")
    print("    WaveformTable \"_default_WFT_\" {")
    print("        Period '", cycle, "ns';", sep="")
    print("        Waveforms {")
    print("            \"" + opts.clk + "\" { 0 { '0ns' D; } }")
    print("            \"" + opts.clk + "\" { P { '0ns' D;", end="")
    print(" '", cycle // 2, "ns' U;", sep="", end="")
    print(" '", cycle // 2 + cycle // 4, "ns' D;", sep="", end="")
    print(" } }")
    print("            \"" + opts.clk + "\" { 1 { '0ns' U; } }")
    print("            \"" + opts.clk + "\" { Z { '0ns' Z; } }")
    print("            \"_default_In_Timing_\" { 0 { '0ns' D; } }")
    print("            \"_default_In_Timing_\" { 1 { '0ns' U; } }")
    print("            \"_default_In_Timing_\" { Z { '0ns' Z; } }")
    print("            \"_default_In_Timing_\" { N { '0ns' N; } }")
    print("            \"_default_Out_Timing_\" { X { '0ns' X; } }")
    print("            \"_default_Out_Timing_\" { H { '0ns' X;", end="")
    print(" '", cycle - cycle // 10, "ns' H; } }")
    print("            \"_default_Out_Timing_\" { T { '0ns' X;", end="")
    print(" '", cycle - cycle // 10, "ns' T; } }")
    print("            \"_default_Out_Timing_\" { L { '0ns' X;", end="")
    print(" '", cycle - cycle // 10, "ns' L; } }")
    print("        }")
    print("    }")
    print("}")
    print()
    #}}}
    #{{{ print ScanStructures
    if len(dffs) > 0:
        print("ScanStructures {")
        print("    ScanChain \"chain1\" {")
        print("       ScanLength ", len(dffs), ";", sep="")
        print("       ScanIn \"" + opts.si + "\";")
        print("       ScanOut \"" + opts.so + "\";")
        print("       ScanInversion 0;")
        print("       ScanCells",end = '')
        for dff in dffs:
            print(" \"", opts.top, ".", dff, ".SI\"", sep="",end = '')
        print(";")
        print("       ScanMasterClock \"", opts.clk, "\" ;", sep="")
        print("    }")
        print("}")
        print()
    #}}}
    #{{{ print PatternBurst
    print("PatternBurst \"_burst_\" {")
    print("    PatList { \"_pattern_\" { } }")
    print("}")
    print()
    #}}}
    #{{{ print PatternExec
    print("PatternExec {")
    print("    PatternBurst \"_burst_\";")
    print("}")
    print()
    #}}}
    #{{{ print Procedures
    print("Procedures {")

    # load_unload
    print("    \"load_unload\" {")
    print("        W \"_default_WFT_\";")
    print("        C {", end="")
    print(" \"", opts.si, "\"=0;", sep="", end="")
    print(" \"", opts.clk, "\"=0;", sep="", end="")
    print(" \"", opts.se, "\"=1;", sep="", end="")
    print(" }")
    print("        V { \"_so\"=#; }")
    print("        Shift {")
    print("            W \"_default_WFT_\";")
    print("            V { \"_si\"=#; \"_so\"=#;", end="")
    print(" \"", opts.clk, "\"=P; }", sep="")
    print("        }")
    print("    }")

    # capture_CK
    print("    \"capture_CK\" {")
    print("        W \"_default_WFT_\";")
    print("        F { \"", opts.se, "\"=0; }", sep="")
    print("        C { \"_po\"=\\r", len(pos), " X ; }", sep="")
    print("        \"forcePI\": V {", end="")
    print(" \"_pi\"=\\r", len(pis), " # ;", sep="", end="")
    print(" }")
    print("        \"measurePO\": V {", end="")
    print(" \"_po\"=\\r", len(pos), " # ;", sep="", end="")
    print(" }")
    print("        C { \"_po\"=\\r", len(pos), " X ; }", sep="")
    print("        \"pulse\": V { \"", opts.clk, "\"=P; }", sep="")
    print("    }")

    # capture
    print("    \"capture\" {")
    print("        W \"_default_WFT_\";")
    print("        F { \"", opts.se, "\"=0; }", sep="")
    print("        C { \"_po\"=\\r", len(pos), " X ; }", sep="")
    print("        \"forcePI\": V {", end="")
    print(" \"_pi\"=\\r", len(pis), " # ;", sep="", end="")
    print(" }")
    print("        \"measurePO\": V {", end="")
    print(" \"_po\"=\\r", len(pos), " # ;", sep="", end="")
    print(" }")
    print("    }")

    print("}")
    print()
    #}}}
    #{{{ print MacroDefs
    print("MacroDefs {")
    print("    \"test_setup\" {")
    print("        W \"_default_WFT_\";")
    print("        V { \"", opts.se, "\"=0; \"", opts.clk, "\"=0; }", sep="")
    print("    }")
    print("}")
    print()
    #}}}
    #{{{ print Pattern
    print("Pattern \"_pattern_\" {")
    print("    W \"_default_WFT_\";")
    print("    \"precondition all Signals\": C {", end="")
    print(" \"_pi\"=\\r", len(pis), " 0 ;", sep="", end="")
    print(" \"_po\"=\\r", len(pos), " X ;", sep="", end="")
    print(" }")
    print("    Macro \"test_setup\";")
    line = ifile.readline()
    prev = []
    prevHasScan = False
    prevPpiIdx = 0
    patNo = 0
    while line:
        line = line.rstrip()
        if not line:
            line = ifile.readline()
            continue

        # check pattern format
        linelist = re.split(re.compile("[ ]+"), line)
        if len(linelist) < 3:
            print("**ERROR pattern", patNo, "wrong format", file=sys.stderr)
            break
        depth = int(linelist[2])
        if len(linelist) < depth * 3 + 3:
            print("**ERROR pattern", patNo, "wrong format", file=sys.stderr)
            break
        hasScan = False
        ppiIdx = depth * 3 + 3
        if len(linelist) == depth * 3 + 5:
            hasScan = True

        # print pattern
        print("    \"pattern ", patNo, "\":", sep="")

        # print shift in/out
        if hasScan:
            print("        Call \"load_unload\" {")
            if prevHasScan:
                print("            \"test_so\"=", end="")
                printOut(prevlist[prevPpiIdx + 1][::-1])
            print("            \"test_si\"=", end="")
            printIn(linelist[ppiIdx][::-1])
            print("        }")

        # print PI/PO
        for i in range(0, depth):
            piIdx = i * 3 + 3
            if linelist[piIdx + 2] == "__HOLD__":
                print("        Call \"capture\" {")
            else:
                print("        Call \"capture_CK\" {")
            print("            \"_pi\"=", end="")
            if opts.genTestPorts:
                linelist[piIdx] = "000" + linelist[piIdx]
            printIn(linelist[piIdx])
            if i + 1 == depth:
                print("            \"_po\"=", end="")
                printOut(linelist[piIdx + 1])
            print("        }")

        # next pattern
        patNo += 1
        prevlist = list(linelist)
        prevHasScan = hasScan
        prevPpiIdx = ppiIdx
        line = ifile.readline()

    # print last shift out
    if prevHasScan:
        print("    \"end", patNo - 1, "unload\":")
        print("        Call \"load_unload\" {")
        print("            \"test_so\"=", end="")
        printOut(prevlist[prevPpiIdx + 1][::-1])
        print("        }")

    print("}")
    #}}}

    ifile.close()
#}}}

def printIn(values): #{{{
    for i in range(0, len(values)):
        if values[i] == '0':
            print('0', end="")
        elif values[i] == '1':
            print('1', end="")
        else:
            print('N', end="")
    print(";")
#}}}
def printOut(values): #{{{
    for i in range(0, len(values)):
        if values[i] == '0':
            print('L', end="")
        elif values[i] == '1':
            print('H', end="")
        else:
            print('X', end="")
    print(";")
#}}}
def parseArg(): #{{{
    usage = "Usage: %prog [options] <input pattern>"
    version = "%prog 1.0"
    prog = "pat2stil.py"
    parser = optparse.OptionParser(
        usage=usage,
        version=version,
        prog=prog
    )
    parser.add_option(
        "--clk",
        dest="clk",
        action="store",
        metavar="CLK",
        help="clock port name"
    )
    parser.add_option(
        "--cycle",
        dest="cycle",
        action="store",
        metavar="CYCLE",
        help="clock cycle in nano second"
    )
    parser.add_option(
        "--si",
        dest="si",
        action="store",
        metavar="SI",
        help="scan in port name"
    )
    parser.add_option(
        "--se",
        dest="se",
        action="store",
        metavar="SE",
        help="scan enable port name"
    )
    parser.add_option(
        "--so",
        dest="so",
        action="store",
        metavar="SO",
        help="scan out port name"
    )
    parser.add_option(
        "--gen-test-ports",
        dest="genTestPorts",
        action="store_true",
        metavar="",
        help="generation clock, scan in, and scan enable test ports"
    )
    parser.add_option(
        "--top",
        dest="top",
        action="store",
        metavar="TOP",
        help="top module name"
    )
    parser.set_defaults(
        clk="CK",
        cycle="100",
        si="test_si",
        se="test_se",
        so="test_so",
        genTestPorts=False,
        top="TOP",
    )
    (opts, args) = parser.parse_args()
    if len(args) != 1:
        print("**ERROR Please specify input file", file=sys.stderr)
        exit(0)
    return (opts, args)
#}}}

if __name__ == "__main__":
    main()

