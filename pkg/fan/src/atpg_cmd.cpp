// **************************************************************************
// File       [ atpg_cmd.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/09/28 created ]
// **************************************************************************

#include <cstdlib>
#include <iomanip>
#include <fstream>

#include "atpg_cmd.h"
#include "core/pattern_rw.h"
#include "core/simulator.h"

using namespace std;
using namespace CommonNs;
using namespace IntfNs;
using namespace CoreNs;
using namespace FanNs;

double rtime;

//{{{ ReadPatCmd::ReadPatCmd()
ReadPatCmd::ReadPatCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("read patterns");
	optMgr_.setDes("read pattern form FILE");
	optMgr_.regArg(new Arg(Arg::REQ, "pattern file", "FILE"));
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
	opt = new Opt(Opt::BOOL, "verbose on. Default is off", "");
	opt->addFlag("v");
	opt->addFlag("verbose");
	optMgr_.regOpt(opt);
}
ReadPatCmd::~ReadPatCmd() {}
//}}}
//{{{ bool ReadPatCmd::exec()
bool ReadPatCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR ReadPatCmd::exec(): circuit needed";
		cerr << endl;
		return false;
	}

	if (optMgr_.getNParsedArg() < 1)
	{
		cerr << "**ERROR ReadPatCmd::exec(): pattern file needed";
		cerr << endl;
		return false;
	}

	// create pattern and pattern builder
	delete fanMgr_->pcoll;
	fanMgr_->pcoll = new PatternProcessor;
	PatFile *patBlder = new PatternReader(fanMgr_->pcoll, fanMgr_->cir);

	// read pattern
	fanMgr_->tmusg.periodStart();
	cout << "#  Reading pattern ..." << endl;
	bool verbose = optMgr_.isFlagSet("v");
	if (!patBlder->read(optMgr_.getParsedArg(0).c_str(), verbose))
	{
		cerr << "**ERROR ReadPatCmd()::exec(): pattern builder error" << endl;
		delete fanMgr_->pcoll;
		delete patBlder;
		fanMgr_->pcoll = NULL;
		return false;
	}

	TmStat stat;
	fanMgr_->tmusg.getPeriodUsage(stat);
	cout << "#  Finished reading pattern `" << optMgr_.getParsedArg(0) << "'";
	cout << "    " << (double)stat.rTime / 1000000.0 << " s";
	cout << "    " << (double)stat.vmSize / 1024.0 << " MB" << endl;

	delete patBlder;
	return true;
} //}}}

//{{{ ReportPatCmd::ReportPatCmd()
ReportPatCmd::ReportPatCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("report pattern");
	optMgr_.setDes("report pattern information");
	Arg *arg = new Arg(Arg::REQ_INF, "pattern number to be reported", "NUM");
	optMgr_.regArg(arg);
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
	opt = new Opt(Opt::BOOL, "don't print order information", "");
	opt->addFlag("disable-order");
	optMgr_.regOpt(opt);
}
ReportPatCmd::~ReportPatCmd() {}
//}}}
//{{{ bool ReportPatCmd::exec()
bool ReportPatCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->pcoll)
	{
		cerr << "**ERROR ReportPatCmd::exec(): pattern needed" << endl;
		return false;
	}

	cout << "#  pattern information" << endl;
	cout << "#    number of pattern: " << fanMgr_->pcoll->pats_.size() << endl;
	if (!optMgr_.isFlagSet("disable-order"))
	{
		cout << "#    pi order: ";
		for (int i = 0; i < fanMgr_->pcoll->npi_; ++i)
			cout << " " << fanMgr_->pcoll->piOrder_[i];
		cout << endl;
		cout << "#    ppi order:";
		for (int i = 0; i < fanMgr_->pcoll->nppi_; ++i)
			cout << " " << fanMgr_->pcoll->ppiOrder_[i];
		cout << endl;
		cout << "#    po order: ";
		for (int i = 0; i < fanMgr_->pcoll->npo_; ++i)
			cout << " " << fanMgr_->pcoll->poOrder_[i];
		cout << endl;
	}
	cout << "#" << endl;

	for (int i = 0; i < (int)fanMgr_->pcoll->pats_.size(); ++i)
	{
		cout << "#    pattern " << i << endl;
		cout << "#      pi1: ";
		if (fanMgr_->pcoll->pats_[i]->pi1_)
			for (int j = 0; j < fanMgr_->pcoll->npi_; ++j)
				printValue(fanMgr_->pcoll->pats_[i]->pi1_[j]);
		cout << endl;
		cout << "#      pi2: ";
		if (fanMgr_->pcoll->pats_[i]->pi2_)
			for (int j = 0; j < fanMgr_->pcoll->npi_; ++j)
				printValue(fanMgr_->pcoll->pats_[i]->pi2_[j]);
		cout << endl;
		cout << "#      ppi: ";
		if (fanMgr_->pcoll->pats_[i]->ppi_)
			for (int j = 0; j < fanMgr_->pcoll->nppi_; ++j)
				printValue(fanMgr_->pcoll->pats_[i]->ppi_[j]);
		cout << endl;
		cout << "#      po1: ";
		if (fanMgr_->pcoll->pats_[i]->po1_)
			for (int j = 0; j < fanMgr_->pcoll->npo_; ++j)
				printValue(fanMgr_->pcoll->pats_[i]->po1_[j]);
		cout << endl;
		cout << "#      po2: ";
		if (fanMgr_->pcoll->pats_[i]->po2_)
			for (int j = 0; j < fanMgr_->pcoll->npo_; ++j)
				printValue(fanMgr_->pcoll->pats_[i]->po2_[j]);
		cout << endl;
		cout << "#      ppo: ";
		if (fanMgr_->pcoll->pats_[i]->ppo_)
			for (int j = 0; j < fanMgr_->pcoll->nppi_; ++j)
				printValue(fanMgr_->pcoll->pats_[i]->ppo_[j]);
		cout << endl
				 << "#" << endl;
	}

	return true;
} //}}}

//{{{ AddFaultCmd::AddFaultCmd()
AddFaultCmd::AddFaultCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("add faults");
	optMgr_.setDes("adds faults either by extract from circuit or from file");
	Arg *arg = new Arg(Arg::OPT, "fault type. Can be SA0, SA1, STR, STF",
										 "TYPE");
	optMgr_.regArg(arg);
	arg = new Arg(Arg::OPT, "pin location. Hierachy separated by '/'",
								"PIN");
	optMgr_.regArg(arg);
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
	opt = new Opt(Opt::STR_REQ, "custom fault file", "FILE");
	opt->addFlag("f");
	opt->addFlag("file");
	optMgr_.regOpt(opt);
	opt = new Opt(Opt::BOOL, "add all faults", "");
	opt->addFlag("a");
	opt->addFlag("all");
	optMgr_.regOpt(opt);
}
AddFaultCmd::~AddFaultCmd() {}
//}}}
//{{{ bool AddFaultCmd::exec()
bool AddFaultCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR AddFaultCmd::exec(): circuit needed";
		cerr << endl;
		return false;
	}

	if (!fanMgr_->fListExtract)
		fanMgr_->fListExtract = new FaultListExtract;

	fanMgr_->fListExtract->extract(fanMgr_->cir);

	// add all faults
	if (optMgr_.isFlagSet("a"))
		addAllFault();
	else
	{ // add specific faults
		if (fanMgr_->fListExtract->type_ == FaultListExtract::SAF || fanMgr_->fListExtract->type_ == FaultListExtract::TDF)
		{
			if (optMgr_.getNParsedArg() < 2)
			{
				cerr << "**ERROR AddFaultCmd::exec(): need fault type ";
				cerr << "and pin location" << endl;
				return false;
			}
			string type = optMgr_.getParsedArg(0);
			if (fanMgr_->fListExtract->type_ == FaultListExtract::SAF &&
					(type != "SA0" && type != "SA1"))
			{
				cerr << "**ERROR AddFaultCmd::exec(): stuck-at fault only ";
				cerr << "supports SA0 and SA1" << endl;
				return false;
			}
			if (fanMgr_->fListExtract->type_ == FaultListExtract::TDF &&
					(type != "STR" && type != "STF"))
			{
				cerr << "**ERROR AddFaultCmd::exec(): transition delay ";
				cerr << "fault only supports STR and STF" << endl;
				return false;
			}
			string pinloc = optMgr_.getParsedArg(1);
			string cell = pinloc.substr(0, pinloc.find_first_of('/'));
			string pin = pinloc.substr(cell.size());
			if (pin.size() > 0)
				pin = pin.substr(1);
			if (pin.size() == 0)
			{
				if (!addPinFault(type, cell))
					return false;
			}
			else
			{
				if (!addCellFault(type, cell, pin))
					return false;
			}
		}
	}

	return true;
} //}}}
//{{{ void AddFaultCmd::addAllFault()
void AddFaultCmd::addAllFault()
{
	cout << "#  Building fault list ..." << endl;
	fanMgr_->tmusg.periodStart();

	fanMgr_->fListExtract->current_.resize(fanMgr_->fListExtract->faults_.size());
	FaultListIter it = fanMgr_->fListExtract->current_.begin();
	for (size_t i = 0; i < fanMgr_->fListExtract->faults_.size(); ++i, ++it)
		(*it) = fanMgr_->fListExtract->faults_[i];

	TmStat stat;
	fanMgr_->tmusg.getPeriodUsage(stat);
	cout << "#  Finished building fault list";
	cout << "    " << (double)stat.rTime / 1000000.0 << " s";
	cout << "    " << (double)stat.vmSize / 1024.0 << " MB" << endl;
} //}}}
//{{{ void AddFaultCmd::addPinFault(const string &, const string &)
bool AddFaultCmd::addPinFault(const string &type, const string &pin)
{
	Port *p = fanMgr_->nl->getTop()->getPort(pin.c_str());
	if (!p)
	{
		cerr << "**ERROR AddFaultCmd::exec(): port `";
		cerr << pin << "' does not exist" << endl;
		return false;
	}
	int gid = fanMgr_->cir->portToGate_[p->id_];
	int offset = (type == "SA0" || type == "STR") ? 0 : 1;
	int fid = fanMgr_->fListExtract->gateToFault_[gid] + offset;
	Fault *f = fanMgr_->fListExtract->faults_[fid];
	fanMgr_->fListExtract->current_.push_back(f);
	return true;
} //}}}
//{{{ void AddFaultCmd::addCellFault(const string &, const string &, const string &)
bool AddFaultCmd::addCellFault(const string &type, const string &cell,
															 const string &pin)
{
	Cell *c = fanMgr_->nl->getTop()->getCell(cell.c_str());
	if (!c)
	{
		cerr << "**ERROR AddFaultCmd::exec(): cell `";
		cerr << cell << "' does not exist" << endl;
		return false;
	}
	Cell *libc = c->libc_;
	Port *p = libc->getPort(pin.c_str());
	if (!p)
	{
		cerr << "**ERROR AddFaultCmd::exec(): port `";
		cerr << cell << "/" << pin << "' does not exist" << endl;
		return false;
	}
	PortSet pset = libc->getNetPorts(p->inNet_->id_);
	PortSet::iterator it = pset.begin();
	for (; it != pset.end(); ++it)
	{
		if ((*it)->top_ == libc)
			continue;
		Cell *pmt = (*it)->top_;
		int pmtid = pmt->id_;
		int gid = fanMgr_->cir->cellToGate_[c->id_] + pmtid;
		int pid = 0;
		if ((*it)->type_ == Port::INPUT)
		{
			int nOutput = 0;
			for (int i = 0; i < (*it)->id_; ++i)
				if (pmt->getPort(i)->type_ == Port::OUTPUT)
					nOutput++;
			pid = (*it)->id_ - nOutput + 1;
		}
		int offset = (type == "SA0" || type == "STR") ? 0 : 1;
		int fid = fanMgr_->fListExtract->gateToFault_[gid] + 2 * pid + offset;
		Fault *f = fanMgr_->fListExtract->faults_[fid];
		fanMgr_->fListExtract->current_.push_back(f);
	}
	return true;
} //}}}

//{{{ ReportFaultCmd::ReportFaultCmd()
ReportFaultCmd::ReportFaultCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("report fault");
	optMgr_.setDes("report fault information");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
	opt = new Opt(Opt::STR_REQ, "print only faults with state STATE",
								"STATE");
	opt->addFlag("s");
	opt->addFlag("state");
	optMgr_.regOpt(opt);
}
ReportFaultCmd::~ReportFaultCmd() {}
//}}}
//{{{ bool ReportFaultCmd::exec()
bool ReportFaultCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->fListExtract)
	{
		cerr << "**ERROR ReportFaultCmd::exec(): fault list needed" << endl;
		return false;
	}

	bool stateSet = false;
	Fault::State state = Fault::UD;
	if (optMgr_.isFlagSet("s"))
	{
		stateSet = true;
		string stateStr = optMgr_.getFlagVar("s");
		if (stateStr == "ud" || stateStr == "UD")
			state = Fault::UD;
		else if (stateStr == "dt" || stateStr == "DT")
			state = Fault::DT;
		else if (stateStr == "au" || stateStr == "AU")
			state = Fault::AU;
		else if (stateStr == "ti" || stateStr == "TI")
			state = Fault::TI;
		else if (stateStr == "re" || stateStr == "RE")
			state = Fault::RE;
		else if (stateStr == "ab" || stateStr == "AB")
			state = Fault::AB;
		else
		{
			stateSet = true;
			cerr << "**WARN ReportFaultCmd::exec(): fault state `";
			cerr << stateStr << "' is not supported" << endl;
		}
	}

	cout << "#  fault information" << endl;
	cout << "#    fault type:       ";
	switch (fanMgr_->fListExtract->type_)
	{
		case FaultListExtract::SAF:
			cout << "stuck-at fault" << endl;
			break;
		case FaultListExtract::TDF:
			cout << "transition delay fault" << endl;
			break;
		case FaultListExtract::BRF:
			cout << "bridging fault" << endl;
			break;
		default:
			cout << endl;
			break;
	}
	cout << "#    number of faults: " << fanMgr_->fListExtract->current_.size();
	cout << endl;
	cout << "#    type    code    pin (cell)" << endl;
	cout << "#    ----    ----    ----------------------------------" << endl;
	FaultListIter it = fanMgr_->fListExtract->current_.begin();
	for (; it != fanMgr_->fListExtract->current_.end(); ++it)
	{
		if (!stateSet || (*it)->state_ != state)
			continue;
		cout << "#    ";
		switch ((*it)->type_)
		{
			case Fault::SA0:
				cout << "SA0     ";
				break;
			case Fault::SA1:
				cout << "SA1     ";
				break;
			case Fault::STR:
				cout << "STR     ";
				break;
			case Fault::STF:
				cout << "STF     ";
				break;
			case Fault::BR:
				cout << "BR      ";
				break;
		}
		switch ((*it)->state_)
		{
			case Fault::UD:
				cout << " UD     ";
				break;
			case Fault::DT:
				cout << " DT     ";
				break;
			case Fault::PT:
				cout << " PT     ";
				break;
			case Fault::AU:
				cout << " AU     ";
				break;
			case Fault::TI:
				cout << " TI     ";
				break;
			case Fault::RE:
				cout << " RE     ";
				break;
			case Fault::AB:
				cout << " AB     ";
				break;
		}
		int cid = fanMgr_->cir->gates_[(*it)->gate_].cid_;
		int pid = (*it)->line_;
		int pmtid = fanMgr_->cir->gates_[(*it)->gate_].pmtid_;
		if ((*it)->gate_ == -1)
		{ // CK
			cout << "CK";
		}
		else if ((*it)->gate_ == -2)
		{ // test_si
			cout << "test_si";
		}
		else if ((*it)->gate_ == -3)
		{ // test_so
			cout << "test_so";
		}
		else if ((*it)->gate_ == -4)
		{ // test_se
			cout << "test_se";
		}
		else if (fanMgr_->cir->gates_[(*it)->gate_].type_ == Gate::PI)
		{
			cout << fanMgr_->nl->getTop()->getPort(cid)->name_ << " ";
			cout << "(primary input)";
		}
		else if (fanMgr_->cir->gates_[(*it)->gate_].type_ == Gate::PO)
		{
			cout << fanMgr_->nl->getTop()->getPort(cid)->name_ << " ";
			cout << "(primary output)";
		}
		else
		{
			Cell *c = fanMgr_->nl->getTop()->getCell(cid);
			// cout << "test" << c->name_<< " " << cid << endl;
			Cell *libc = c->libc_;
			Cell *pmt = libc->getCell(pmtid);
			Port *p = NULL;
			if (pid < 0)
			{ // must be CK,SE,SI pins on FF
				if (pid == -1)
					cout << c->name_ << "/CK ";
				else if (pid == -2)
					cout << c->name_ << "/SE ";
				else if (pid == -3)
					cout << c->name_ << "/SI ";
				else
					cout << c->name_ << "/QN ";
			}
			else if (pid == 0)
			{ // output
				if (!strcmp(libc->name_, "SDFFXL"))
					cout << c->name_ << "/"
							 << "Q"
							 << " ";
				else
				{
					for (size_t i = 0; i < pmt->getNPort(); ++i)
					{
						if (pmt->getPort(i)->type_ != Port::OUTPUT)
							continue;
						Net *n = pmt->getPort(i)->exNet_;
						if (!n)
							continue;
						PortSet pset = libc->getNetPorts(n->id_);
						PortSet::iterator pit = pset.begin();
						for (; pit != pset.end(); ++pit)
						{
							if ((*pit)->top_ != libc)
								continue;
							p = (*pit);
							break;
						}
						if (p)
							break;
					}
				}
			}
			else
			{ // input
				int inCount = 0;
				for (size_t i = 0; i < pmt->getNPort(); ++i)
				{
					if (pmt->getPort(i)->type_ == Port::INPUT)
						inCount++;
					if (inCount != pid)
						continue;
					Net *n = pmt->getPort(i)->exNet_;
					if (!n)
						continue;
					PortSet pset = libc->getNetPorts(n->id_);
					PortSet::iterator pit = pset.begin();
					for (; pit != pset.end(); ++pit)
					{
						if ((*pit)->top_ != libc)
							continue;
						p = (*pit);
						break;
					}
					if (p)
						break;
				}
			}
			if (p)
				cout << c->name_ << "/" << p->name_ << " ";
			cout << "(" << libc->name_ << ")";
		}
		cout << endl;
	}
	cout << endl;

	return true;
} //}}}

//{{{ ReportCircuitCmd::ReportCircuitCmd()
ReportCircuitCmd::ReportCircuitCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("report circuit");
	optMgr_.setDes("report circuit information");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
ReportCircuitCmd::~ReportCircuitCmd() {}
//}}}
//{{{ bool ReportCircuitCmd::exec()
bool ReportCircuitCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR ReportCircuitCmd::exec(): circuit needed" << endl;
		return false;
	}

	Cell *top = fanMgr_->cir->nl_->getTop();
	cout << "#  circuit information" << endl;
	cout << "#    netlist:         " << top->name_ << endl;
	cout << "#    number of PIs:   " << fanMgr_->cir->npi_ << endl;
	cout << "#    number of PPIs:  " << fanMgr_->cir->nppi_ << endl;
	cout << "#    number of POs:   " << fanMgr_->cir->npo_ << endl;
	cout << "#    number of combs: " << fanMgr_->cir->ncomb_ << endl;
	cout << "#    number of gates: " << fanMgr_->cir->ngate_ << endl;
	cout << "#    number of nets:  " << fanMgr_->cir->nnet_ << endl;

	return true;
} //}}}

//{{{ ReportGateCmd::ReportGateCmd()
ReportGateCmd::ReportGateCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("report gate information");
	optMgr_.setDes("reports GATE information in the circuit");
	Arg *arg = new Arg(Arg::OPT_INF,
										 "if no name is specified, all gates will be reported",
										 "GATE");
	optMgr_.regArg(arg);
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
ReportGateCmd::~ReportGateCmd() {}
//}}}
//{{{ bool ReportGateCmd::exec()
bool ReportGateCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR ReportGateCmd::exec(): circuit needed" << endl;
		return false;
	}

	if (optMgr_.getNParsedArg() == 0)
	{
		for (int i = 0; i < fanMgr_->cir->ngate_ * fanMgr_->cir->nframe_; ++i)
			reportGate(i);
	}
	else
	{
		for (size_t i = 0; i < optMgr_.getNParsedArg(); ++i)
		{
			string name = optMgr_.getParsedArg(i);
			Cell *c = fanMgr_->nl->getTop()->getCell(name.c_str());
			if (c)
				for (size_t j = 0; j < c->libc_->getNCell(); ++j)
					reportGate(fanMgr_->cir->cellToGate_[c->id_] + j);
			Port *p = fanMgr_->nl->getTop()->getPort(name.c_str());
			if (p)
				reportGate(fanMgr_->cir->portToGate_[p->id_]);
			if (!c && !p)
			{
				cerr << "**ERROR ReportGateCmd::exec(): cell or port `";
				cerr << name << "' does not exist" << endl;
			}
		}
	}

	return true;
} //}}}
//{{{ void ReportGateCmd::reportGate()
void ReportGateCmd::reportGate(const int &i) const
{
	Gate *g = &fanMgr_->cir->gates_[i];
	cout << "#  ";
	if (g->type_ == Gate::PI || g->type_ == Gate::PO)
		cout << fanMgr_->nl->getTop()->getPort((size_t)g->cid_)->name_;
	else
		cout << fanMgr_->nl->getTop()->getCell((size_t)g->cid_)->name_;
	cout << " id(" << i << ") ";
	cout << "lvl(" << g->lvl_ << ") ";
	cout << "type(" << g->type_ << ") ";
	cout << "frame(" << g->frame_ << ")";
	cout << endl;
	cout << "#    fi[" << g->nfi_ << "]";
	for (int j = 0; j < g->nfi_; ++j)
		cout << " " << g->fis_[j];
	cout << endl;
	cout << "#    fo[" << g->nfo_ << "]";
	for (int j = 0; j < g->nfo_; ++j)
		cout << " " << g->fos_[j];
	cout << endl
			 << endl;
} //}}}

//{{{ ReportValueCmd::ReportValueCmd()
ReportValueCmd::ReportValueCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("report gate values");
	optMgr_.setDes("reports GATE values in the circuit");
	Arg *arg = new Arg(Arg::OPT_INF,
										 "if no name is specified, all gates will be reported",
										 "GATE");
	optMgr_.regArg(arg);
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
ReportValueCmd::~ReportValueCmd() {}
//}}}
//{{{ bool ReportValueCmd::exec()
bool ReportValueCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR ReportValueCmd::exec(): circuit needed" << endl;
		return false;
	}

	if (optMgr_.getNParsedArg() == 0)
	{
		for (int i = 0; i < fanMgr_->cir->tgate_; ++i)
			reportValue(i);
	}
	else
	{
		for (size_t i = 0; i < optMgr_.getNParsedArg(); ++i)
		{
			string name = optMgr_.getParsedArg(i);
			Cell *c = fanMgr_->nl->getTop()->getCell(name.c_str());
			if (c)
				for (size_t j = 0; j < c->libc_->getNCell(); ++j)
					reportValue(fanMgr_->cir->cellToGate_[c->id_] + j);
			Port *p = fanMgr_->nl->getTop()->getPort(name.c_str());
			if (p)
				reportValue(fanMgr_->cir->portToGate_[p->id_]);
			if (!c && !p)
			{
				cerr << "**ERROR ReportValueCmd::exec(): cell or port `";
				cerr << name << "' does not exist" << endl;
			}
		}
	}

	return true;
} //}}}
//{{{ void ReportValueCmd::reportGate()
void ReportValueCmd::reportValue(const int &i) const
{
	Gate *g = &fanMgr_->cir->gates_[i];
	cout << "#  ";
	if (g->type_ == Gate::PI || g->type_ == Gate::PO)
		cout << fanMgr_->nl->getTop()->getPort((size_t)g->cid_)->name_;
	else
		cout << fanMgr_->nl->getTop()->getCell((size_t)g->cid_)->name_;
	cout << " id(" << i << ") ";
	cout << "lvl(" << g->lvl_ << ") ";
	cout << "type(" << g->type_ << ") ";
	cout << "frame(" << g->frame_ << ")";
	cout << endl;
	cout << "#    good:   ";
	printValue(g->gl_, g->gh_);
	cout << endl;
	cout << "#    faulty: ";
	printValue(g->fl_, g->fh_);
	cout << endl
			 << endl;
} //}}}

//{{{ ReportStatsCmd::ReportStatsCmd()
ReportStatsCmd::ReportStatsCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("report statistics");
	optMgr_.setDes("reports statistics on fault coverage");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
ReportStatsCmd::~ReportStatsCmd() {}
//}}}
//{{{ bool ReportStatsCmd::exec()
bool ReportStatsCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->fListExtract || fanMgr_->fListExtract->current_.size() == 0)
	{
		cerr << "**WARN ReportStatsCmd::exec(): no statistics" << endl;
		return false;
	}

	// determine fault model
	string ftype = "";
	switch (fanMgr_->fListExtract->type_)
	{
		case FaultListExtract::SAF:
			ftype = "SAF";
			break;
		case FaultListExtract::TDF:
			ftype = "TDF";
			break;
		case FaultListExtract::BRF:
			ftype = "BRF";
			break;
	}

	// determine pattern type
	string ptype = "";
	switch (fanMgr_->pcoll->type_)
	{
		case PatternProcessor::BASIC_SCAN:
			ptype = "BASIC";
			break;
		case PatternProcessor::LAUNCH_CAPTURE:
			ptype = "LOC";
			break;
		case PatternProcessor::LAUNCH_SHIFT:
			ptype = "LOS";
			break;
	}

	// determine atpg runtime
	// float rtime = (double)fanMgr_->atpgStat.rTime / 1000000.0;

	size_t npat = 0;

	if (fanMgr_->pcoll)
		npat = fanMgr_->pcoll->pats_.size();

	size_t fu = fanMgr_->fListExtract->current_.size();
	size_t ud = 0;
	size_t dt = 0;
	size_t pt = 0;
	size_t au = 0;
	size_t ti = 0;
	size_t re = 0;
	size_t ab = 0;

	FaultListIter it = fanMgr_->fListExtract->current_.begin();
	for (; it != fanMgr_->fListExtract->current_.end(); ++it)
	{
		switch ((*it)->state_)
		{
			case Fault::UD:
				// cout << (*it)->gate_ << ' ' << (*it)->line_ << ' ' << (*it)->type_ << ' ' << (((*it)->line_==0)?(0):(fanMgr_->cir->gates_[(*it)->gate_].fis_[(*it)->line_-1])) << endl;
				ud++;
				break;
			case Fault::DT:
				dt++;
				break;
			case Fault::PT:
				pt++;
				break;
			case Fault::AU:
				// cout << (*it)->gate_ << ' ' << (*it)->line_ << ' ' << (*it)->type_ << ' ' << (((*it)->line_==0)?(0):(fanMgr_->cir->gates_[(*it)->gate_].fis_[(*it)->line_-1])) << endl;
				au++;
				break;
			case Fault::TI:
				ti++;
				break;
			case Fault::RE:
				re++;
				break;
			case Fault::AB:
				ab++;
				break;
		}
	}

	float fc = (float)dt / (float)fu * 100;
	float tc = (float)dt / (float)(ud + dt + pt + ab) * 100;
	float ae = (float)(dt + au + ti + re) / (float)fu * 100;

	cout << right;
	cout << setprecision(4);
	cout << "#                 Statistics Report" << endl;
	cout << "#  Circuit name                  " << setw(19);
	cout << fanMgr_->nl->getTop()->name_ << endl;
	cout << "#  Fault model                   " << setw(19) << ftype << endl;
	cout << "#  Pattern type                  " << setw(19) << ptype << endl;
	cout << "#  -------------------------------------------------" << endl;
	cout << "#  Fault classes                             #faults" << endl;
	cout << "#  ----------------------------  -------------------" << endl;
	cout << "#    FU (full)                   " << setw(19) << fu << endl;
	cout << "#    --------------------------  -------------------" << endl;
	cout << "#    UD (undetected)             " << setw(19) << ud << endl;
	cout << "#    PT (possibly testable)      " << setw(19) << pt << endl;
	cout << "#    AU (atpg untestable)        " << setw(19) << au << endl;
	cout << "#    RE (redundant)              " << setw(19) << re << endl;
	cout << "#    AB (atpg abort)             " << setw(19) << ab << endl;
	cout << "#    TI (tied)                   " << setw(19) << ti << endl;
	cout << "#    --------------------------  -------------------" << endl;
	cout << "#    DT (detected)               " << setw(19) << dt << endl;
	cout << "#  -------------------------------------------------" << endl;
	cout << "#  Coverage                               percentage" << endl;
	cout << "#    --------------------------  -------------------" << endl;
	cout << "#    test coverage                            ";
	cout << setw(5) << tc << "%" << endl;
	cout << "#    fault coverage                           ";
	cout << setw(5) << fc << "%" << endl;
	cout << "#    atpg effectiveness                       ";
	cout << setw(5) << ae << "%" << endl;
	cout << "#  -------------------------------------------------" << endl;
	cout << "#  #Patterns                     " << setw(19) << npat << endl;
	cout << "#  -------------------------------------------------" << endl;
	cout << "#  ATPG runtime                  " << setw(17) << rtime;
	cout << " s" << endl;
	cout << "#  -------------------------------------------------" << endl;

	return true;
} //}}}

//{{{ AddPinConsCmd::AddnPinConsCmd()
AddPinConsCmd::AddPinConsCmd(const std::string &name,
														 FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("add pin constraint");
	optMgr_.setDes("constraint PI(s) to 0 or 1");
	Arg *arg = new Arg(Arg::REQ_INF, "PIs to be constrainted", "PI");
	optMgr_.regArg(arg);
	arg = new Arg(Arg::REQ, "constraint value, 0 or 1", "<0|1>");
	optMgr_.regArg(arg);
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
AddPinConsCmd::~AddPinConsCmd() {}
//}}}
//{{{ bool AddPinConsCmd::exec()
bool AddPinConsCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR AddPinConsCmd::exec(): circuit needed";
		cerr << endl;
		return false;
	}

	if (optMgr_.getNParsedArg() < 2)
	{
		cerr << "**ERROR AddPinConsCmd::exec(): need at least one PI ";
		cerr << "and the constraint value" << endl;
		return false;
	}

	size_t npi = optMgr_.getNParsedArg() - 1;
	int cons = atoi(optMgr_.getParsedArg(npi).c_str());
	for (size_t i = 0; i < npi; ++i)
	{
		string piname = optMgr_.getParsedArg(i);
		Port *p = fanMgr_->nl->getTop()->getPort(piname.c_str());
		if (!p)
		{
			cerr << "**ERROR AddPinConsCmd::exec(): Port `" << piname;
			cerr << "' not found" << endl;
			continue;
		}
		int gid = fanMgr_->cir->portToGate_[p->id_];
		if (fanMgr_->cir->gates_[gid].type_ != Gate::PI)
		{
			cerr << "**ERROR AddPinConsCmd::exec(): Port `" << piname;
			cerr << "' is not PI" << endl;
			continue;
		}
		fanMgr_->cir->gates_[gid].hasCons_ = true;
		if (cons)
			fanMgr_->cir->gates_[gid].cons_ = PARA_H;
		else
			fanMgr_->cir->gates_[gid].cons_ = PARA_L;
	}

	return true;
} //}}}

//{{{ RunLogicSimCmd::RunLogicSimCmd()
RunLogicSimCmd::RunLogicSimCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("run logic simulation");
	optMgr_.setDes("run logic simulation on the given pattern");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
RunLogicSimCmd::~RunLogicSimCmd() {}
//}}}
//{{{ bool RunLogicSimCmd::exec()
bool RunLogicSimCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR RunLogicSimCmd::exec(): circuit needed" << endl;
		return false;
	}

	if (!fanMgr_->pcoll)
	{
		cerr << "**ERROR RunLogicSimCmd::exec(): pattern needed" << endl;
		return false;
	}

	fanMgr_->tmusg.periodStart();
	cout << "#  Performing logic simulation ..." << endl;

	Simulator sim(fanMgr_->cir);
	sim.ppGoodSim(fanMgr_->pcoll);

	TmStat stat;
	fanMgr_->tmusg.getPeriodUsage(stat);
	cout << "#  Finished logic simulation";
	cout << "    " << (double)stat.rTime / 1000000.0 << " s";
	cout << "    " << (double)stat.vmSize / 1024.0 << " MB" << endl;

	return true;
} //}}}

//{{{ RunFaultSimCmd::RunFaultSimCmd()
RunFaultSimCmd::RunFaultSimCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("run fault simulation");
	optMgr_.setDes("run fault simulation on the given pattern");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
	opt = new Opt(Opt::STR_REQ, "simulation METHOD. Choose either pp (parallel pattern) or pf (parallel fault)", "METHOD");
	opt->addFlag("m");
	opt->addFlag("method");
	optMgr_.regOpt(opt);
}
RunFaultSimCmd::~RunFaultSimCmd() {}
//}}}
//{{{ bool RunFaultSimCmd::exec()
bool RunFaultSimCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR RunFaultSimCmd::exec(): circuit needed" << endl;
		return false;
	}

	if (!fanMgr_->pcoll)
	{
		cerr << "**ERROR RunFaultSimCmd::exec(): pattern needed" << endl;
		return false;
	}

	if (!fanMgr_->fListExtract || fanMgr_->fListExtract->faults_.size() == 0)
	{
		cerr << "**ERROR RunFaultSimCmd::exec(): fault list needed" << endl;
		return false;
	}

	if (!fanMgr_->sim)
		fanMgr_->sim = new Simulator(fanMgr_->cir);

	cout << "#  Performing fault simulation ..." << endl;
	fanMgr_->tmusg.periodStart();

	if (optMgr_.isFlagSet("m") && optMgr_.getFlagVar("m") == "pf")
		fanMgr_->sim->pfFaultSim(fanMgr_->pcoll, fanMgr_->fListExtract);
	else
		fanMgr_->sim->ppFaultSim(fanMgr_->pcoll, fanMgr_->fListExtract);

	TmStat stat;
	fanMgr_->tmusg.getPeriodUsage(stat);
	cout << "#  Finished fault simulation";
	cout << "    " << (double)stat.rTime / 1000000.0 << " s";
	cout << "    " << (double)stat.vmSize / 1024.0 << " MB" << endl;
	rtime = (double)stat.rTime / 1000000.0;

	return true;
} //}}}

//{{{ RunAtpgCmd::RunAtpgCmd()
RunAtpgCmd::RunAtpgCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("run automatic test pattern generation");
	optMgr_.setDes("run automatic test pattern generation");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
RunAtpgCmd::~RunAtpgCmd() {}
//}}}
//{{{ bool RunAtpgCmd::exec()
bool RunAtpgCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR RunFaultSimCmd::exec(): circuit needed" << endl;
		return false;
	}

	if (!fanMgr_->pcoll)
	{
		fanMgr_->pcoll = new PatternProcessor;
	}

	if (!fanMgr_->fListExtract)
	{
		fanMgr_->fListExtract = new FaultListExtract;
		fanMgr_->fListExtract->extract(fanMgr_->cir);
	}

	if (!fanMgr_->sim)
		fanMgr_->sim = new Simulator(fanMgr_->cir);

	delete fanMgr_->atpg;
	fanMgr_->atpg = new Atpg(fanMgr_->cir, fanMgr_->sim);

	cout << "#  Performing pattern generation ..." << endl;
	fanMgr_->tmusg.periodStart();

	fanMgr_->atpg->generation(fanMgr_->pcoll, fanMgr_->fListExtract);

	fanMgr_->tmusg.getPeriodUsage(fanMgr_->atpgStat);
	cout << "#  Finished pattern generation";
	cout << "    " << (double)fanMgr_->atpgStat.rTime / 1000000.0 << " s";
	cout << "    " << (double)fanMgr_->atpgStat.vmSize / 1024.0 << " MB";
	cout << endl;
	rtime = (double)fanMgr_->atpgStat.rTime / 1000000.0;

	return true;
} //}}}

//{{{ WritePatCmd::WritePatCmd()
WritePatCmd::WritePatCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("write patterns");
	optMgr_.setDes("writes patterns to FILE");
	Arg *arg = new Arg(Arg::REQ, "output pattern file", "FILE");
	optMgr_.regArg(arg);
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
	opt = new Opt(Opt::STR_REQ, "pattern format. Currently supports `pat' and `ascii'", "FORMAT");
	opt->addFlag("f");
	opt->addFlag("format");
	optMgr_.regOpt(opt);
}
WritePatCmd::~WritePatCmd() {}
//}}}
//{{{ bool WritePatCmd::exec()
bool WritePatCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (optMgr_.getNParsedArg() < 1)
	{
		cerr << "**ERROR WritePatternCmd::exec(): output file needed";
		cerr << endl;
		return false;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR WritePatternCmd::exec(): circuit needed" << endl;
		return false;
	}

	if (!fanMgr_->pcoll)
	{
		cerr << "**ERROR WritePatternCmd::exec(): pattern needed" << endl;
		return false;
	}

	cout << "#  Writing pattern to `" << optMgr_.getParsedArg(0) << "' ...";
	cout << endl;
	PatternWriter writer(fanMgr_->pcoll, fanMgr_->cir);
	if (optMgr_.getFlagVar("f") == "lht")
	{
		if (!writer.writeLht(optMgr_.getParsedArg(0).c_str()))
		{
			cerr << "**ERROR WritePatternCmd::exec(): writer failed" << endl;
			return false;
		}
	}
	else if (optMgr_.getFlagVar("f") == "ascii")
	{
		if (!writer.writeAscii(optMgr_.getParsedArg(0).c_str()))
		{
			cerr << "**ERROR WritePatternCmd::exec(): writer failed" << endl;
			return false;
		}
	}
	else if (optMgr_.getFlagVar("f") == "pat" || optMgr_.getFlagVar("f") == "")
	{
		if (!writer.writePat(optMgr_.getParsedArg(0).c_str()))
		{
			cerr << "**ERROR WritePatternCmd::exec(): writer failed" << endl;
			return false;
		}
	}
	else
		cerr << "**ERROR WritePatternCmd::exec(): undefined parameter in -f" << endl;

	return true;
} //}}}

// Ne
//{{{ AddScanChainsCmd::AddScanChainsCmd()
AddScanChainsCmd::AddScanChainsCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("Add Scan Chains");
	optMgr_.setDes("Add Scan Chains to FFs");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
AddScanChainsCmd::~AddScanChainsCmd() {}
//}}}
//{{{ bool AddScanChainsCmd::exec()
bool AddScanChainsCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR AddScanChainsCmd::exec(): circuit needed" << endl;
		return false;
	}

	cout << "#  Add Scan Chains " << endl;

	return true;
} //}}}
// Ne

//{{{ WriteProcCmd::WriteProcCmd()
WriteProcCmd::WriteProcCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("write test procedure file");
	optMgr_.setDes("create test procedure file ");
	Arg *arg = new Arg(Arg::REQ, "output test procedure file", "FILE");
	optMgr_.regArg(arg);
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
WriteProcCmd::~WriteProcCmd() {}
//}}}
//{{{ bool WriteProcCmd::exec()
bool WriteProcCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (optMgr_.getNParsedArg() < 1)
	{
		cerr << "**ERROR WritePatternCmd::exec(): output file needed";
		cerr << endl;
		return false;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR WritePatternCmd::exec(): circuit needed" << endl;
		return false;
	}

	cout << "#  Writing test procedure file ...";
	cout << endl;
	ProcedureWriter writer(fanMgr_->cir);
	if (!writer.writeProc(optMgr_.getParsedArg(0).c_str()))
	{
		cerr << "**ERROR WritePatternCmd::exec(): writer failed" << endl;
		return false;
	}

	return true;
} //}}}

//{{{ WriteStilCmd::WriteStilCmd()
WriteStilCmd::WriteStilCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("write to STIL");
	optMgr_.setDes("writes to STIL");
	Arg *arg = new Arg(Arg::REQ, "output pattern file", "FILE");
	optMgr_.regArg(arg);
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
	opt = new Opt(Opt::STR_REQ, "pattern format. Currently supports `pat'", "FORMAT");
	opt->addFlag("f");
	opt->addFlag("format");
	optMgr_.regOpt(opt);
}
WriteStilCmd::~WriteStilCmd() {}
//}}}
//{{{ bool WriteStilCmd::exec()
bool WriteStilCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (optMgr_.getNParsedArg() < 1)
	{
		cerr << "**ERROR WriteSTILCmd::exec(): output file needed";
		cerr << endl;
		return false;
	}

	if (!fanMgr_->cir)
	{
		cerr << "**ERROR WriteSTILCmd::exec(): circuit needed" << endl;
		return false;
	}

	if (!fanMgr_->pcoll)
	{
		cerr << "**ERROR WriteSTILCmd::exec(): pattern needed" << endl;
		return false;
	}

	cout << "#  Writing pattern to STIL...";
	cout << endl;
	PatternWriter writer(fanMgr_->pcoll, fanMgr_->cir);

	if (!writer.writeSTIL(optMgr_.getParsedArg(0).c_str()))
	{
		cerr << "**ERROR WriteSTILCmd::exec(): writer failed" << endl;
		return false;
	}

	return true;
} //}}}
