#!/ibmnas/427/r101/walts850221/local/bin/python3
import optparse
import re
import sys
import os
import os.path

#{{{ main
def main():
    # check input
    (opts, args) = parseOpt()
    ipat = args[0]
    if not os.path.isfile(ipat):
        print("**ERROR file `" + ipat + "' does not exit", file=sys.stderr)
        exit(0)
    ifile = open(ipat, "r")

    #{{{ find keyword SignalGroups
    sigGrpsFound = False
    while True:
        # parse line and remove empty elements
        line = ifile.readline()
        if not line:
            break
        linelist = re.split(re.compile("[ ]+"), line)
        while "" in linelist:
            linelist.remove("")
        if not linelist:
            continue

        # find keyword 'SignalGroups'
        if len(linelist) > 0 and linelist[0] == "SignalGroups":
            sigGrpsFound = True
            break

    # check SignalGroups
    if not sigGrpsFound:
        print("**ERROR wrong format `SignalGroups' not found", file=sys.stderr)
        exit(0)
    #}}}
    #{{{ find PIs and POs
    piFound = False
    poFound = False
    piStart = False
    poStart = False
    pis = []
    pos = []
    clkIndex = -1
    siIndex = -1
    seIndex = -1
    while True:
        # parse line and remove empty elements
        line = ifile.readline()
        if not line:
            break
        linelist = re.split(re.compile("[ \'\"\n=+{}]+"), line)
        while "" in linelist:
            linelist.remove("")
        # remove comments
        if "//" in linelist:
            linelist = linelist[:linelist.index("//")]
        if not linelist:
            continue

        # find PI
        if piStart:
            pis.extend(linelist)
            if pis[-1] == ";":
                piStart = False
                pis = pis[:-1]
        if len(linelist) > 2 and linelist[0] == "_pi":
            piFound = True
            if linelist[-1] != ";":
                pis.extend(linelist[1:])
                piStart = True
            else:
                pis.extend(linelist[1:-1])

        # find PO
        if poStart:
            pos.extend(linelist)
            if pos[-1] == ";":
                poStart = False
                pos = pos[:-1]
        if len(linelist) > 2 and linelist[0] == "_po":
            poFound = True
            if linelist[-1] != ";":
                poStart = True
                pos.extend(linelist[1:])
            else:
                pos.extend(linelist[1:-1])

        if piFound and not piStart and poFound and not poStart:
            break

    # check pins
    if not piFound:
        print("**ERROR PI not found", file=sys.stderr)
        exit(0)
    if not poFound:
        print("**ERROR PO not found", file=sys.stderr)
        exit(0)
    if opts.si not in pis:
        print("**ERROR scan in pin not found", file=sys.stderr)
        exit(0)
    if opts.so not in pos:
        print("**ERROR scan out pin not found", file=sys.stderr)
        exit(0)
    if opts.clk in pis:
        clkIndex = pis.index(opts.clk)
    if opts.si in pis:
        siIndex = pis.index(opts.clk)
    if opts.se in pis:
        seIndex = pis.index(opts.clk)

    # debug
    # print("PIs", pis)
    # print("POs", pos)
    #}}}
    #{{{ find keyword ScanStructures
    scanStructFound = False
    while True:
        # parse line and remove empty elements
        line = ifile.readline()
        if not line:
            break
        linelist = re.split(re.compile("[ ]+"), line)
        while "" in linelist:
            linelist.remove("")
        if not linelist:
            continue

        # find keyword 'ScanStructures'
        if len(linelist) > 0 and linelist[0] == "ScanStructures":
            scanStructFound = True
            break

    # check ScanStructures
    if not scanStructFound:
        print("**ERROR wrong format `ScanStructures' not found", file=sys.stderr)
        exit(0)
    #}}}
    #{{{ find scan cells
    ffFound = False
    ffStart = False
    ffs = []
    while True:
        # parse line and remove empty elements
        line = ifile.readline()
        if not line:
            break
        linelist = re.split(re.compile("[ \'\"\n=+{}]+"), line)
        while "" in linelist:
            linelist.remove("")
        if not linelist:
            continue

        # find FF
        if ffStart:
            ffs.extend(linelist)
            if ffs[-1] == ";":
                ffStart = False
                ffs = ffs[:-1]
        if len(linelist) > 2 and linelist[0] == "ScanCells":
            ffFound = True
            if linelist[-1] != ";":
                ffs.extend(linelist[1:])
                ffStart = True
            else:
                ffs.extend(linelist[1:-1])

        if ffFound and not ffStart:
            break

    # remove hierarchy
    for i in range(0, len(ffs)):
        ffs[i] = ffs[i][ffs[i].index('.') + 1:]
        ffs[i] = ffs[i][:ffs[i].index('.')]

    # debug
    # print("FFs", ffs)
    #}}}

    #{{{ print header
    print("#  stil2pat.py")
    print("#")
    print("#  Pattern formats:")
    print("#      __PI_ORDER__    <PI>...")
    print("#      __PO_ORDER__    <PO>...")
    print("#      __SCAN_ORDER__  [SFF]...")
    print("#      __PATTERN__     <index> <depth>", end=" ")
    print("< <PIs> <POs> __HOLD__|__CAPT__ >...", end = " ")
    print("[PPIs] [PPOs]")
    print("#")
    #}}}
    #{{{ print PIs, POs, and SFFs
    print("__PI_ORDER__", end=" ")
    for pi in pis:
        if (pi == opts.clk or pi == opts.si or pi == opts.se) \
           and opts.noTestPorts:
            continue
        print(pi, end=" ")
    print()

    print("__PO_ORDER__", end=" ")
    for po in pos:
        print(po, end=" ")
    print()

    print("__SCAN_ORDER__", end=" ")
    for ff in ffs:
        print(ff, end=" ")
    print()
    #}}}
    #{{{ print patterns
    patFound = False
    piStart = [ False ]
    poStart = [ False ]
    siStart = [ False ]
    soStart = [ False ]
    piValue = []
    poValue = []
    siValue = []
    soValue = []
    clk = []
    nPat = 0
    while True:
        # parse line and remove empty elements
        line = ifile.readline()
        if not line:
            break
        linelist = re.split(re.compile("[ \'\"\n=+{}]+"), line)
        while "" in linelist:
            linelist.remove("")
        if not linelist:
            continue

        if linelist[0] == "pattern":
            patFound = True

        if not patFound:
            continue

        if "Call" in linelist and len(linelist) > linelist.index("Call"):
            if linelist[linelist.index("Call") + 1] == "capture_CK":
                clk.append("__CAPT__")
            elif linelist[linelist.index("Call") + 1] == "capture":
                clk.append("__HOLD__")
            if len(poValue) < len(piValue):
                poAppendX(poValue, len(pos))
        if linelist[0] == "V":
            clk.append("__HOLD__")
            if len(poValue) < len(piValue):
                poAppendX(poValue, len(pos))

        if piStart[0]:
            appendValue(linelist, piValue, piStart)

        if poStart[0]:
            appendValue(linelist, poValue, poStart)

        if soStart[0]:
            appendValue(linelist, soValue, soStart)
            if soStart[0] == False:
                if "__HOLD__" in clk and opts.noHold:
                    delValue(piValue, poValue, siValue, soValue, clk)
                    continue
                if opts.noTestPorts:
                    rmTestPorts(clkIndex, siIndex, seIndex, piValue)
                printPat(nPat, piValue, poValue, siValue, soValue, clk)
                nPat += 1

        if siStart[0]:
            appendValue(linelist, siValue, siStart)

        if "_pi" in linelist:
            startValue(linelist, "_pi", piValue, piStart)

        if "_po" in linelist:
            startValue(linelist, "_po", poValue, poStart)

        if opts.so in linelist:
            startValue(linelist, opts.so, soValue, soStart)
            if soStart[0] == False:
                if "__HOLD__" in clk and opts.noHold:
                    delValue(piValue, poValue, siValue, soValue, clk)
                    continue
                if opts.noTestPorts:
                    rmTestPorts(clkIndex, siIndex, seIndex, piValue)
                printPat(nPat, piValue, poValue, siValue, soValue, clk)
                nPat += 1

        if opts.si in linelist:
            startValue(linelist, opts.si, siValue, siStart)
    #}}}

    ifile.close()
#}}}

def startValue(linelist, name, value, flag): #{{{
    index = linelist.index(name)
    if len(linelist) > index + 1:
        value.append(linelist[index + 1])
    else:
        value.append("")
    if value and value[-1] != "" and value[-1][-1] == ';':
        value[-1] = value[-1][:-1]
    else:
        flag[0] = True
#}}}
def appendValue(linelist, value, flag): #{{{
    value[-1] += linelist[0]
    if value[-1][-1] == ';':
        value[-1] = value[-1][:-1]
        flag[0] = False
#}}}
def rmTestPorts(clkIndex, siIndex, seIndex, value): #{{{
    portIndex = [clkIndex, siIndex, seIndex]
    portIndex.sort()
    for i in range(0, len(value)):
        valueList = list(value[i])
        del valueList[portIndex[2]]
        del valueList[portIndex[1]]
        del valueList[portIndex[0]]
        value[i] = "".join(valueList)
#}}}
def delValue(piValue, poValue, siValue, soValue, clk): #{{{
    del piValue[0:len(piValue)]
    del poValue[0:len(poValue)]
    del siValue[0:len(siValue)]
    del soValue[0:len(soValue)]
    del clk[0:len(clk)]
#}}}
def printPat(nPat, piValue, poValue, siValue, soValue, clk): #{{{
    print("__PATTERN__", nPat, len(piValue), end=" ")
    for i in range(0, len(piValue)):
        print(piValue[i], end=" ")
        for value in poValue[i]:
            if value == 'L':
                print('0', end="")
            elif value == 'H':
                print('1', end="")
            else:
                print('X', end="")
        print("", end=" ")
        print(clk[i], end=" ")
    print(siValue[0][::-1], end=" ")
    for value in soValue[0][::-1]:
        if value == 'L':
            print('0', end="")
        elif value == 'H':
            print('1', end="")
        else:
            print('X', end="")
    print()
    delValue(piValue, poValue, siValue, soValue, clk)
#}}}
def poAppendX(poValue, nPos): #{{{
    value = ""
    for i in range(0, nPos):
        value += 'X'
    poValue.append(value)
#}}}
def parseOpt(): #{{{
    usage = "Usage: %prog <STIL file>"
    version = "%prog 1.0"
    prog = "stil2pat.py"
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
        "--no-test-ports",
        dest="noTestPorts",
        action="store_true",
        metavar="",
        help="remove clock, scan in, and scan enable ports"
    )
    parser.add_option(
        "--no-hold",
        dest="noHold",
        action="store_true",
        metavar="",
        help="skip pattern containing __HOLD__"
    )
    parser.set_defaults(
        clk="CK",
        si="test_si",
        se="test_se",
        so="test_so",
        noTestPorts=False,
        noHold=False,
    )
    (opts, args) = parser.parse_args()
    if len(args) < 1:
        print("**ERROR Please specify input file.", file=sys.stderr)
        exit(0)
    return (opts, args)
#}}}

if __name__ == "__main__":
    main()

