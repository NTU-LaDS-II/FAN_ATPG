// **************************************************************************
// File       [ setup_cmd.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/08/30 created ]
// **************************************************************************

#include "interface/netlist_builder.h"
#include "interface/techlib_builder.h"

#include "setup_cmd.h"

using namespace std;
using namespace CommonNs;
using namespace IntfNs;
using namespace CoreNs;
using namespace FanNs;


//{{{ ReadLibCmd::ReadLibCmd()
ReadLibCmd::ReadLibCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name) {
    fanMgr_ = fanMgr;
    optMgr_.setName(name);
    optMgr_.setShortDes("read mentor technology library");
    optMgr_.setDes("read mentor technology library");
    optMgr_.regArg(new Arg(Arg::REQ, "mentor technology library file",
                           "lib_file"));
    Opt *opt = new Opt(Opt::BOOL, "print usage", "");
    opt->addFlag("h");
    opt->addFlag("help");
    optMgr_.regOpt(opt);
    opt = new Opt(Opt::BOOL, "verbose on. Default is off", "");
    opt->addFlag("v");
    opt->addFlag("verbose");
    optMgr_.regOpt(opt);
}

ReadLibCmd::~ReadLibCmd() {}
//}}}
//{{{ bool ReadLibCmd::exec()
bool ReadLibCmd::exec(const vector<string> &argv) {
    optMgr_.parse(argv);

    if (optMgr_.isFlagSet("h")) {
        optMgr_.usage();
        return true;
    }

    if (optMgr_.getNParsedArg() < 1) {
        cerr << "**ERROR ReadLibCmd::exec(): library file needed";
        cerr << endl;
        return false;
    }


    // create library and library builder
    delete fanMgr_->lib;
    fanMgr_->lib = new Techlib;
    MdtFile *libBlder = new MdtLibBuilder(fanMgr_->lib);

    // read library
    fanMgr_->tmusg.periodStart();
    cout << "#  Reading technology library ..." << endl;
    bool verbose = optMgr_.isFlagSet("v");
    if (!libBlder->read(optMgr_.getParsedArg(0).c_str(), verbose)) {
        cerr << "**ERROR ReadLibCmd::exec(): MDT lib builder error" << endl;
        delete libBlder;
        delete fanMgr_->lib;
        fanMgr_->lib = NULL;
        return false;
    }

    // check library
    if (!fanMgr_->lib->check(verbose)) {
        cerr << "**ERROR ReadLibCmd::exec(): MDT lib error" << endl;
        delete libBlder;
        delete fanMgr_->lib;
        fanMgr_->lib = NULL;
        return false;
    }

    TmStat stat;
    if (!fanMgr_->tmusg.getPeriodUsage(stat))
        cout << "fishy ..." << endl;
    cout << "#  Finished reading library `" << optMgr_.getParsedArg(0) << "'";
    cout << "    " << (double)stat.rTime / 1000000.0 << " s";
    cout << "    " << (double)stat.vmSize / 1024.0   << " MB" << endl;

    delete libBlder;
    return true;
}
//}}}

//{{{ ReadNlCmd::ReadNlCmd()
ReadNlCmd::ReadNlCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name) {
    fanMgr_ = fanMgr;
    optMgr_.setName(name);
    optMgr_.setShortDes("read verilog gate level netlist");
    optMgr_.setDes("read verilog gate level netlist");
    optMgr_.regArg(new Arg(Arg::REQ, "verilog gate level netlist file",
                           "netlist_file"));
    Opt *opt = new Opt(Opt::BOOL, "print usage", "");
    opt->addFlag("h");
    opt->addFlag("help");
    optMgr_.regOpt(opt);
    opt = new Opt(Opt::BOOL, "verbose on. Default is off", "");
    opt->addFlag("v");
    opt->addFlag("verbose");
    optMgr_.regOpt(opt);
}
ReadNlCmd::~ReadNlCmd() {}
//}}}
//{{{ bool ReadNlCmd::exec()
bool ReadNlCmd::exec(const vector<string> &argv) {
    optMgr_.parse(argv);

    if (optMgr_.isFlagSet("h")) {
        optMgr_.usage();
        return true;
    }

    if (!fanMgr_->lib) {
        cerr << "**ERROR ReadNlCmd::exec(): technology library needed";
        cerr << endl;
        return false;
    }

    if (optMgr_.getNParsedArg() < 1) {
        cerr << "**ERROR ReadNlCmd::exec(): netlist file needed";
        cerr << endl;
        return false;
    }

    // create netlist and netlist builder
    delete fanMgr_->nl;
    fanMgr_->nl = new Netlist;
    fanMgr_->nl->setTechlib(fanMgr_->lib);
    VlogFile *nlBlder = new VlogNlBuilder(fanMgr_->nl);

    // read netlist
    fanMgr_->tmusg.periodStart();
    cout << "#  Reading netlist ..." << endl;
    bool verbose = optMgr_.isFlagSet("v");
    if (!nlBlder->read(optMgr_.getParsedArg(0).c_str(), verbose)) {
        cerr << "**ERROR ReadNlCmd()::exec(): verilog builder error" << endl;
        delete fanMgr_->nl;
        delete nlBlder;
        fanMgr_->nl = NULL;
        return false;
    }

    // check netlist
    if (!fanMgr_->nl->check(verbose)) {
        cerr << "**ERROR ReadNlCmd()::exec(): netlist error" << endl;
        delete fanMgr_->nl;
        delete nlBlder;
        fanMgr_->nl = NULL;
        return false;
    }

    TmStat stat;
    fanMgr_->tmusg.getPeriodUsage(stat);
    cout << "#  Finished reading netlist `" << optMgr_.getParsedArg(0) << "'";
    cout << "    " << (double)stat.rTime / 1000000.0 << " s";
    cout << "    " << (double)stat.vmSize / 1024.0   << " MB" << endl;

    delete nlBlder;
    return true;
} //}}}

//{{{ SetFaultTypeCmd::SetFaultTypeCmd()
SetFaultTypeCmd::SetFaultTypeCmd(const std::string &name, FanMgr *fanMgr) :
  Cmd(name) {
    fanMgr_ = fanMgr;
    optMgr_.setName(name);
    optMgr_.setShortDes("set fault type");
    optMgr_.setDes("set fault type. Currently supports stuck-at fault and transition delay fault");
    optMgr_.regArg(new Arg(Arg::REQ, "either saf or tdf",
                           "fault_type"));
    Opt *opt = new Opt(Opt::BOOL, "print usage", "");
    opt->addFlag("h");
    opt->addFlag("help");
    optMgr_.regOpt(opt);
}
SetFaultTypeCmd::~SetFaultTypeCmd() {}
//}}}
//{{{ bool SetFaultTypeCmd::exec()
bool SetFaultTypeCmd::exec(const vector<string> &argv) {
    optMgr_.parse(argv);

    if (optMgr_.isFlagSet("h")) {
        optMgr_.usage();
        return true;
    }

    if (optMgr_.getNParsedArg() < 1) {
        cerr << "**ERROR SetFaultTypeCmd::exec(): fault type needed";
        cerr << endl;
        return false;
    }

    if (!fanMgr_->fListExtract)
        fanMgr_->fListExtract = new FaultListExtract;

    if (optMgr_.getParsedArg(0) == "saf") {
        cout << "#  fault type set to stuck-at fault" << endl;
        fanMgr_->fListExtract->type_ = FaultListExtract::SAF;
    }
    else if (optMgr_.getParsedArg(0) == "tdf") {
        cout << "#  fault type set to transition delay fault" << endl;
        fanMgr_->fListExtract->type_ = FaultListExtract::TDF;
    }
    else {
        cerr << "**ERROR SetFaultTypeCmd::exec(): unknown fault type `";
        cerr << optMgr_.getParsedArg(0) << "'" << endl;
        return false;
    }

    return true;
} //}}}

//{{{ ReportNetlistCmd::ReportNetlistCmd()
ReportNetlistCmd::ReportNetlistCmd(const std::string &name, FanMgr *fanMgr) :
  Cmd(name) {
    fanMgr_ = fanMgr;
    optMgr_.setName(name);
    optMgr_.setShortDes("report netlist");
    optMgr_.setDes("report netlist information");
    Opt *opt = new Opt(Opt::BOOL, "print usage", "");
    opt->addFlag("h");
    opt->addFlag("help");
    optMgr_.regOpt(opt);
    opt = new Opt(Opt::BOOL, "print more detailed information", "");
    opt->addFlag("more");
    optMgr_.regOpt(opt);
}
ReportNetlistCmd::~ReportNetlistCmd() {}
//}}}
//{{{ bool ReportNetlistCmd::exec()
bool ReportNetlistCmd::exec(const vector<string> &argv) {
    optMgr_.parse(argv);

    if (optMgr_.isFlagSet("h")) {
        optMgr_.usage();
        return true;
    }

    if (!fanMgr_->nl) {
        cerr << "**ERROR ReportNetlistCmd::exec(): netlist needed" << endl;
        return false;
    }

    cout << "#  netlist information" << endl;
    cout << "#    number of modules: " << fanMgr_->nl->getNModule() << endl;
    cout << "#    modules:          ";
    for (size_t i = 0; i < fanMgr_->nl->getNModule(); ++i)
        cout << " " << fanMgr_->nl->getModule(i)->name_;
    cout << endl;
    Cell *top = fanMgr_->nl->getTop();
    cout << "#    current module:    " << top->name_ << endl;
    cout << "#    number of ports:   " << top->getNPort() << endl;
    cout << "#    number of cells:   " << top->getNCell() << endl;
    cout << "#    number of nets:    " << top->getNNet() << endl;
    if (!optMgr_.isFlagSet("more"))
        return true;
    cout << "#    ports:            ";
    for (size_t i = 0; i < top->getNPort(); ++i)
        cout << " " << top->getPort(i)->name_;
    cout << endl;
    cout << "#    cells:            ";
    for (size_t i = 0; i < top->getNCell(); ++i)
        cout << " " << top->getCell(i)->name_;
    cout << endl;
    cout << "#    nets:             ";
    for (size_t i = 0; i < top->getNNet(); ++i)
        cout << " " << top->getNet(i)->name_;
    cout << endl;

    return true;
} //}}}

//{{{ ReportCellCmd::ReportCellCmd()
ReportCellCmd::ReportCellCmd(const std::string &name, FanMgr *fanMgr) :
  Cmd(name) {
    fanMgr_ = fanMgr;
    optMgr_.setName(name);
    optMgr_.setShortDes("report cell");
    optMgr_.setDes("reports CELL information in the netlist");
    Arg *arg = new Arg(Arg::OPT_INF,
                       "if no name is specified, all cells will be reported",
                       "CELL");
    optMgr_.regArg(arg);
    Opt *opt = new Opt(Opt::BOOL, "print usage", "");
    opt->addFlag("h");
    opt->addFlag("help");
    optMgr_.regOpt(opt);
}
ReportCellCmd::~ReportCellCmd() {}
//}}}
//{{{ bool ReportCellCmd::exec()
bool ReportCellCmd::exec(const vector<string> &argv) {
    optMgr_.parse(argv);

    if (optMgr_.isFlagSet("h")) {
        optMgr_.usage();
        return true;
    }

    if (!fanMgr_->nl) {
        cerr << "**ERROR ReportCellCmd::exec(): netlist needed" << endl;
        return false;
    }

    if (optMgr_.getNParsedArg() == 0)
        for (size_t i = 0; i < fanMgr_->nl->getTop()->getNCell(); ++i)
            reportCell(i);
    else {
        for (size_t i = 0; i < optMgr_.getNParsedArg(); ++i) {
            string name = optMgr_.getParsedArg(i);
            Cell *c = fanMgr_->nl->getTop()->getCell(name.c_str());
            if (!c) {
                cerr << "**ERROR ReportCellCmd::exec(): cell `";
                cerr << name << "' does not exist" << endl;
                continue;
            }
            reportCell(c->id_);
        }
    }

    return true;
} //}}}
//{{{ void ReportCellCmd::reportCell()
void ReportCellCmd::reportCell(const size_t &i) const {
    Cell *c = fanMgr_->nl->getTop()->getCell(i);
    cout << "#  Cell " << c->name_ << " " << c->typeName_ << endl;
    CellSet fis  = fanMgr_->nl->getTop()->getFanin(i);
    cout << "#    fi[" << fis.size() << "]";
    for (CellSet::iterator it = fis.begin(); it != fis.end(); ++it) {
        cout << " " << (*it)->name_;
    }
    cout << endl;
    CellSet fos  = fanMgr_->nl->getTop()->getFanout(i);
    cout << "#    fo[" << fos.size() << "]";
    for (CellSet::iterator it = fos.begin(); it != fos.end(); ++it) {
        cout << " " << (*it)->name_;
    }
    cout << endl << endl;
} //}}}

//{{{ ReportLibCmd::ReportLibCmd()
ReportLibCmd::ReportLibCmd(const std::string &name, FanMgr *fanMgr) :
  Cmd(name) {
    fanMgr_ = fanMgr;
    optMgr_.setName(name);
    optMgr_.setShortDes("report library");
    optMgr_.setDes("report library information");
    Opt *opt = new Opt(Opt::BOOL, "print usage", "");
    opt->addFlag("h");
    opt->addFlag("help");
    optMgr_.regOpt(opt);
}
ReportLibCmd::~ReportLibCmd() {}
//}}}
//{{{ bool ReportLibCmd::exec()
bool ReportLibCmd::exec(const vector<string> &argv) {
    optMgr_.parse(argv);

    if (optMgr_.isFlagSet("h")) {
        optMgr_.usage();
        return true;
    }

    if (!fanMgr_->lib) {
        cerr << "**ERROR ReportLibraryCmd::exec(): library needed" << endl;
        return false;
    }

    cout << "#  library information" << endl;
    cout << "#    number of models: " << fanMgr_->lib->getNCell() << endl;
    cout << "#    models:          ";
    for (size_t i = 0; i < fanMgr_->lib->getNCell(); ++i)
        cout << " " << fanMgr_->lib->getCell(i)->name_;
    cout << endl;

    return true;
} //}}}

//{{{ BuildCircuitCmd::BuildCircuitCmd()
BuildCircuitCmd::BuildCircuitCmd(const std::string &name, FanMgr *fanMgr) :
  Cmd(name) {
    fanMgr_ = fanMgr;
    optMgr_.setName(name);
    optMgr_.setShortDes("build circuit");
    optMgr_.setDes("build netlist into internal circuit data structure");
    Opt *opt = new Opt(Opt::BOOL, "print usage", "");
    opt->addFlag("h");
    opt->addFlag("help");
    optMgr_.regOpt(opt);
    opt = new Opt(Opt::STR_REQ, "number of frames", "NUM");
    opt->addFlag("f");
    opt->addFlag("frame");
    optMgr_.regOpt(opt);
}
BuildCircuitCmd::~BuildCircuitCmd() {}
//}}}
//{{{ bool BuildCircuitCmd::exec()
bool BuildCircuitCmd::exec(const vector<string> &argv) {
    optMgr_.parse(argv);

    if (optMgr_.isFlagSet("h")) {
        optMgr_.usage();
        return true;
    }

    if (!fanMgr_->nl) {
        cerr << "**ERROR BuildCirucitCmd::exec(): netlist needed" << endl;
        return false;
    }

    int nframe = 1;
    if (optMgr_.isFlagSet("f")) {
        nframe = atoi(optMgr_.getFlagVar("f").c_str());
        nframe = nframe < 1 ? 1 : nframe;
    }

    fanMgr_->cir = new Circuit;
    // build circuit
    fanMgr_->tmusg.periodStart();
    cout << "#  Building circuit ..." << endl;
	if(fanMgr_->pcoll && fanMgr_->pcoll->type_ == PatternProcessor::LAUNCH_SHIFT) // launch on shift pattern
		fanMgr_->cir->build(fanMgr_->nl, nframe, Circuit::SHIFT);
	else
		fanMgr_->cir->build(fanMgr_->nl, nframe);

    TmStat stat;
    fanMgr_->tmusg.getPeriodUsage(stat);
    cout << "#  Finished building circuit";
    cout << "    " << (double)stat.rTime / 1000000.0 << " s";
    cout << "    " << (double)stat.vmSize / 1024.0   << " MB" << endl;

    return true;
} //}}}

//{{{ SetPatternTypeCmd::SetPatternTypeCmd()
SetPatternTypeCmd::SetPatternTypeCmd(const std::string &name, FanMgr *fanMgr) :
  Cmd(name) {
    fanMgr_ = fanMgr;
    optMgr_.setName(name);
    optMgr_.setShortDes("set pattern type");
    optMgr_.setDes("set pattern type. Currently supports basic scan, launch on shift and launch on capture");
    optMgr_.regArg(new Arg(Arg::REQ, "either BASIC, LOC or LOS",
                           "pattern_type"));
    Opt *opt = new Opt(Opt::BOOL, "print usage", "");
    opt->addFlag("h");
    opt->addFlag("help");
    optMgr_.regOpt(opt);
}
SetPatternTypeCmd::~SetPatternTypeCmd() {}
//}}}
//{{{ bool SetPatternTypeCmd::exec()
bool SetPatternTypeCmd::exec(const vector<string> &argv) {
    optMgr_.parse(argv);

    if (optMgr_.isFlagSet("h")) {
        optMgr_.usage();
        return true;
    }

    if (optMgr_.getNParsedArg() < 1) {
        cerr << "**ERROR SetPatternTypeCmd::exec(): pattern type needed";
        cerr << endl;
        return false;
    }

    if (!fanMgr_->pcoll)
        fanMgr_->pcoll = new PatternProcessor;

    if (optMgr_.getParsedArg(0) == "BASIC") {
        cout << "#  pattern type set to basic scan" << endl;
        fanMgr_->pcoll->type_ = PatternProcessor::BASIC_SCAN;
    }
    else if (optMgr_.getParsedArg(0) == "LOC") {
        cout << "#  pattern type set to launch on capture" << endl;
        fanMgr_->pcoll->type_ = PatternProcessor::LAUNCH_CAPTURE;
    }
    else if (optMgr_.getParsedArg(0) == "LOS") {
        cout << "#  pattern type set to launch on shift" << endl;
        fanMgr_->pcoll->type_ = PatternProcessor::LAUNCH_SHIFT;
		fanMgr_->pcoll->nsi_ = 1;
    }
    else {
        cerr << "**ERROR SetPatternTypeCmd::exec(): unknown pattern type `";
        cerr << optMgr_.getParsedArg(0) << "'" << endl;
        return false;
    }

    return true;
} //}}}

//{{{ SetStaticCompressionCmd::SetStaticCompressionCmd()
SetStaticCompressionCmd::SetStaticCompressionCmd(const std::string &name, FanMgr *fanMgr) :
  Cmd(name) {
    fanMgr_ = fanMgr;
    optMgr_.setName(name);
    optMgr_.setShortDes("set static compression");
    optMgr_.setDes("set static compression on/off");
    optMgr_.regArg(new Arg(Arg::REQ, "either on or off",
                           "on/off"));
    Opt *opt = new Opt(Opt::BOOL, "print usage", "");
    opt->addFlag("h");
    opt->addFlag("help");
    optMgr_.regOpt(opt);
}
SetStaticCompressionCmd::~SetStaticCompressionCmd() {}
//}}}
//{{{ bool SetStaticCompressionCmd::exec()
bool SetStaticCompressionCmd::exec(const vector<string> &argv) {
    optMgr_.parse(argv);

    if (optMgr_.isFlagSet("h")) {
        optMgr_.usage();
        return true;
    }

    if (optMgr_.getNParsedArg() < 1) {
        cerr << "**ERROR SetStaticCompressionCmd::exec(): on/off needed";
        cerr << endl;
        return false;
    }

	if (!fanMgr_->pcoll)
        fanMgr_->pcoll = new PatternProcessor;
	
	
    if (optMgr_.getParsedArg(0) == "on") {
        cout << "#  static compression set to on" << endl;
		
		fanMgr_->pcoll->staticCompression_ = PatternProcessor::ON;
	}
	else if (optMgr_.getParsedArg(0) == "off") {
        cout << "#  static compression set to off" << endl;
		
		fanMgr_->pcoll->staticCompression_ = PatternProcessor::OFF;
	}
    else {
        cerr << "**ERROR SetStaticCompressionCmd::exec(): unknown argument `";
        cerr << optMgr_.getParsedArg(0) << "'" << endl;
        return false;
    }

    return true;
} //}}}

//{{{ SetDynamicCompressionCmd::SetDynamicCompressionCmd()
SetDynamicCompressionCmd::SetDynamicCompressionCmd(const std::string &name, FanMgr *fanMgr) :
  Cmd(name) {
    fanMgr_ = fanMgr;
    optMgr_.setName(name);
    optMgr_.setShortDes("set dynamic compression");
    optMgr_.setDes("set dynamic compression on/off");
    optMgr_.regArg(new Arg(Arg::REQ, "either on or off",
                           "on/off"));
    Opt *opt = new Opt(Opt::BOOL, "print usage", "");
    opt->addFlag("h");
    opt->addFlag("help");
    optMgr_.regOpt(opt);
}
SetDynamicCompressionCmd::~SetDynamicCompressionCmd() {}
//}}}
//{{{ bool SetDynamicCompressionCmd::exec()
bool SetDynamicCompressionCmd::exec(const vector<string> &argv) {
    optMgr_.parse(argv);

    if (optMgr_.isFlagSet("h")) {
        optMgr_.usage();
        return true;
    }

    if (optMgr_.getNParsedArg() < 1) {
        cerr << "**ERROR SetDynamicCompressionCmd::exec(): on/off needed";
        cerr << endl;
        return false;
    }
	
	if (!fanMgr_->pcoll)
        fanMgr_->pcoll = new PatternProcessor;

	
    if (optMgr_.getParsedArg(0) == "on") {
        cout << "#  dynamic compression set to on" << endl;
		
		fanMgr_->pcoll->dynamicCompression_ = PatternProcessor::ON;
	}
	else if (optMgr_.getParsedArg(0) == "off") {
        cout << "#  dynamic compression set to off" << endl;
		
		fanMgr_->pcoll->dynamicCompression_ = PatternProcessor::OFF;
	}
    else {
        cerr << "**ERROR SetDynamicCompressionCmd::exec(): unknown argument `";
        cerr << optMgr_.getParsedArg(0) << "'" << endl;
        return false;
    }

    return true;
} //}}}

//{{{ SetXFillCmd::SetXFillCmd()
SetXFillCmd::SetXFillCmd(const std::string &name, FanMgr *fanMgr) :
  Cmd(name) {
    fanMgr_ = fanMgr;
    optMgr_.setName(name);
    optMgr_.setShortDes("set X-Fill");
    optMgr_.setDes("set X-Fill on/off");
    optMgr_.regArg(new Arg(Arg::REQ, "either on or off",
                           "on/off"));
    Opt *opt = new Opt(Opt::BOOL, "print usage", "");
    opt->addFlag("h");
    opt->addFlag("help");
    optMgr_.regOpt(opt);
}
SetXFillCmd::~SetXFillCmd() {}
//}}}
//{{{ bool SetXFillCmd::exec()
bool SetXFillCmd::exec(const vector<string> &argv) {
    optMgr_.parse(argv);

    if (optMgr_.isFlagSet("h")) {
        optMgr_.usage();
        return true;
    }

    if (optMgr_.getNParsedArg() < 1) {
        cerr << "**ERROR SetXFillCmd::exec(): on/off needed";
        cerr << endl;
        return false;
    }

	if (!fanMgr_->pcoll)
        fanMgr_->pcoll = new PatternProcessor;
	
	
    if (optMgr_.getParsedArg(0) == "on") {
        cout << "#  X-Fill set to on" << endl;
		
		fanMgr_->pcoll->XFill_ = PatternProcessor::ON;
	}
	else if (optMgr_.getParsedArg(0) == "off") {
        cout << "#  X-Fill set to off" << endl;
		
		fanMgr_->pcoll->XFill_ = PatternProcessor::OFF;
	}
    else {
        cerr << "**ERROR SetXFillCmd::exec(): unknown argument `";
        cerr << optMgr_.getParsedArg(0) << "'" << endl;
        return false;
    }

    return true;
} //}}}
//Ne

