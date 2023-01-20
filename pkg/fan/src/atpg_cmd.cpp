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

using namespace CommonNs;
using namespace IntfNs;
using namespace CoreNs;
using namespace FanNs;

double rtime;

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

bool ReadPatCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR ReadPatCmd::exec(): circuit needed\n";
		return false;
	}

	if (optMgr_.getNParsedArg() < 1)
	{
		std::cerr << "**ERROR ReadPatCmd::exec(): pattern file needed\n";
		return false;
	}

	// create pattern and pattern builder
	delete fanMgr_->pcoll;
	fanMgr_->pcoll = new PatternProcessor;
	PatFile *patBlder = new PatternReader(fanMgr_->pcoll, fanMgr_->cir);

	// read pattern
	fanMgr_->tmusg.periodStart();
	std::cout << "#  Reading pattern ...\n";
	bool verbose = optMgr_.isFlagSet("v");
	if (!patBlder->read(optMgr_.getParsedArg(0).c_str(), verbose))
	{
		std::cerr << "**ERROR ReadPatCmd()::exec(): pattern builder error\n";
		delete fanMgr_->pcoll;
		delete patBlder;
		fanMgr_->pcoll = NULL;
		patBlder = NULL;
		return false;
	}

	TmStat stat;
	fanMgr_->tmusg.getPeriodUsage(stat);
	std::cout << "#  Finished reading pattern `" << optMgr_.getParsedArg(0) << "'";
	std::cout << "    " << (double)stat.rTime / 1000000.0 << " s";
	std::cout << "    " << (double)stat.vmSize / 1024.0 << " MB\n";

	delete patBlder;
	patBlder = NULL;
	return true;
}

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

bool ReportPatCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->pcoll)
	{
		std::cerr << "**ERROR ReportPatCmd::exec(): pattern needed\n";
		return false;
	}

	std::cout << "#  pattern information\n";
	std::cout << "#    number of pattern: " << fanMgr_->pcoll->patternVector_.size() << "\n";
	if (!optMgr_.isFlagSet("disable-order"))
	{
		std::cout << "#    pi order: ";
		for (int i = 0; i < fanMgr_->pcoll->numPI_; ++i)
		{
			std::cout << " " << fanMgr_->pcoll->pPIorder_[i];
		}
		std::cout << "\n";
		std::cout << "#    ppi order:";
		for (int i = 0; i < fanMgr_->pcoll->numPPI_; ++i)
		{
			std::cout << " " << fanMgr_->pcoll->pPPIorder_[i];
		}
		std::cout << "\n";
		std::cout << "#    po order: ";
		for (int i = 0; i < fanMgr_->pcoll->numPO_; ++i)
		{
			std::cout << " " << fanMgr_->pcoll->pPOorder_[i];
		}
		std::cout << "\n";
	}
	std::cout << "#\n";

	for (int i = 0; i < (int)fanMgr_->pcoll->patternVector_.size(); ++i)
	{
		std::cout << "#    pattern " << i << "\n";
		std::cout << "#      pi1: ";
		if (!fanMgr_->pcoll->patternVector_[i].PI1_.empty())
		{
			for (int j = 0; j < fanMgr_->pcoll->numPI_; ++j)
			{
				printValue(fanMgr_->pcoll->patternVector_[i].PI1_[j]);
			}
		}
		std::cout << "\n";
		std::cout << "#      pi2: ";
		if (!fanMgr_->pcoll->patternVector_[i].PI2_.empty())
		{
			for (int j = 0; j < fanMgr_->pcoll->numPI_; ++j)
			{
				printValue(fanMgr_->pcoll->patternVector_[i].PI2_[j]);
			}
		}
		std::cout << "\n";
		std::cout << "#      ppi: ";
		if (!fanMgr_->pcoll->patternVector_[i].PPI_.empty())
		{
			for (int j = 0; j < fanMgr_->pcoll->numPPI_; ++j)
			{
				printValue(fanMgr_->pcoll->patternVector_[i].PPI_[j]);
			}
		}
		std::cout << "\n";
		std::cout << "#      po1: ";
		if (!fanMgr_->pcoll->patternVector_[i].PO1_.empty())
		{
			for (int j = 0; j < fanMgr_->pcoll->numPO_; ++j)
			{
				printValue(fanMgr_->pcoll->patternVector_[i].PO1_[j]);
			}
		}
		std::cout << "\n";
		std::cout << "#      po2: ";
		if (!fanMgr_->pcoll->patternVector_[i].PO2_.empty())
		{
			for (int j = 0; j < fanMgr_->pcoll->numPO_; ++j)
			{
				printValue(fanMgr_->pcoll->patternVector_[i].PO2_[j]);
			}
		}
		std::cout << "\n";
		std::cout << "#      ppo: ";
		if (!fanMgr_->pcoll->patternVector_[i].PPO_.empty())
		{
			for (int j = 0; j < fanMgr_->pcoll->numPPI_; ++j)
			{
				printValue(fanMgr_->pcoll->patternVector_[i].PPO_[j]);
			}
		}
		std::cout << "\n"
							<< "#\n";
	}
	return true;
}

AddFaultCmd::AddFaultCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("add faults");
	optMgr_.setDes("adds faults either by extract from circuit or from file");
	Arg *arg = new Arg(Arg::OPT, "fault type. Can be SA0, SA1, STR, STF", "TYPE");
	optMgr_.regArg(arg);
	arg = new Arg(Arg::OPT, "pin location. Hierarchy separated by '/'", "PIN");
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

bool AddFaultCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR AddFaultCmd::exec(): circuit needed\n";
		return false;
	}

	if (!fanMgr_->fListExtract)
	{
		fanMgr_->fListExtract = new FaultListExtract;
	}

	fanMgr_->fListExtract->extractFaultFromCircuit(fanMgr_->cir);

	// add all faults
	if (optMgr_.isFlagSet("a"))
	{
		addAllFault();
	}
	else
	{ // add specific faults
		if (fanMgr_->fListExtract->faultListType_ == FaultListExtract::SAF || fanMgr_->fListExtract->faultListType_ == FaultListExtract::TDF)
		{
			if (optMgr_.getNParsedArg() < 2)
			{
				std::cerr << "**ERROR AddFaultCmd::exec(): need fault type ";
				std::cerr << "and pin location\n";
				return false;
			}
			std::string type = optMgr_.getParsedArg(0);
			if (fanMgr_->fListExtract->faultListType_ == FaultListExtract::SAF &&
					(type != "SA0" && type != "SA1"))
			{
				std::cerr << "**ERROR AddFaultCmd::exec(): stuck-at fault only ";
				std::cerr << "supports SA0 and SA1\n";
				return false;
			}
			if (fanMgr_->fListExtract->faultListType_ == FaultListExtract::TDF &&
					(type != "STR" && type != "STF"))
			{
				std::cerr << "**ERROR AddFaultCmd::exec(): transition delay ";
				std::cerr << "fault only supports STR and STF\n";
				return false;
			}
			std::string pinloc = optMgr_.getParsedArg(1);
			std::string cell = pinloc.substr(0, pinloc.find_first_of('/'));
			std::string pin = pinloc.substr(cell.size());
			if (pin.size() > 0)
			{
				pin = pin.substr(1);
			}
			if (pin.size() == 0)
			{
				if (!addPinFault(type, cell))
				{
					return false;
				}
			}
			else
			{
				if (!addCellFault(type, cell, pin))
				{
					return false;
				}
			}
		}
	}
	return true;
}

void AddFaultCmd::addAllFault()
{
	std::cout << "#  Building fault list ...\n";
	fanMgr_->tmusg.periodStart();

	fanMgr_->fListExtract->faultsInCircuit_.resize(fanMgr_->fListExtract->extractedFaults_.size());
	FaultPtrListIter it = fanMgr_->fListExtract->faultsInCircuit_.begin();
	for (int i = 0; i < fanMgr_->fListExtract->extractedFaults_.size(); ++i, ++it)
	{
		(*it) = &fanMgr_->fListExtract->extractedFaults_[i];
	}

	TmStat stat;
	fanMgr_->tmusg.getPeriodUsage(stat);
	std::cout << "#  Finished building fault list";
	std::cout << "    " << (double)stat.rTime / 1000000.0 << " s";
	std::cout << "    " << (double)stat.vmSize / 1024.0 << " MB\n";
}

bool AddFaultCmd::addPinFault(const std::string &type, const std::string &pin)
{
	Port *p = fanMgr_->nl->getTop()->getPort(pin.c_str());
	if (!p)
	{
		std::cerr << "**ERROR AddFaultCmd::exec(): port `";
		std::cerr << pin << "' does not exist\n";
		return false;
	}
	int gid = fanMgr_->cir->portIndexToGateIndex_[p->id_];
	int offset = (type == "SA0" || type == "STR") ? 0 : 1;
	int fid = fanMgr_->fListExtract->gateIndexToFaultIndex_[gid] + offset;
	Fault *f = &fanMgr_->fListExtract->extractedFaults_[fid];
	fanMgr_->fListExtract->faultsInCircuit_.push_back(f);
	return true;
}

bool AddFaultCmd::addCellFault(const std::string &type, const std::string &cell,
															 const std::string &pin)
{
	Cell *c = fanMgr_->nl->getTop()->getCell(cell.c_str());
	if (!c)
	{
		std::cerr << "**ERROR AddFaultCmd::exec(): cell `";
		std::cerr << cell << "' does not exist\n";
		return false;
	}
	Cell *libc = c->libc_;
	Port *p = libc->getPort(pin.c_str());
	if (!p)
	{
		std::cerr << "**ERROR AddFaultCmd::exec(): port `";
		std::cerr << cell << "/" << pin << "' does not exist\n";
		return false;
	}
	PortSet pset = libc->getNetPorts(p->inNet_->id_);
	PortSet::iterator it = pset.begin();
	for (; it != pset.end(); ++it)
	{
		if ((*it)->top_ == libc)
		{
			continue;
		}
		Cell *pmt = (*it)->top_;
		int pmtid = pmt->id_;
		int gid = fanMgr_->cir->cellIndexToGateIndex_[c->id_] + pmtid;
		int pid = 0;
		if ((*it)->type_ == Port::INPUT)
		{
			int nOutput = 0;
			for (int i = 0; i < (*it)->id_; ++i)
			{
				if (pmt->getPort(i)->type_ == Port::OUTPUT)
				{
					++nOutput;
				}
			}
			pid = (*it)->id_ - nOutput + 1;
		}
		int offset = (type == "SA0" || type == "STR") ? 0 : 1;
		int fid = fanMgr_->fListExtract->gateIndexToFaultIndex_[gid] + 2 * pid + offset;
		Fault *f = &fanMgr_->fListExtract->extractedFaults_[fid];
		fanMgr_->fListExtract->faultsInCircuit_.push_back(f);
	}
	return true;
}

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
	opt = new Opt(Opt::STR_REQ, "print only faults with state STATE", "STATE");
	opt->addFlag("s");
	opt->addFlag("state");
	optMgr_.regOpt(opt);
}

ReportFaultCmd::~ReportFaultCmd() {}

bool ReportFaultCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->fListExtract)
	{
		std::cerr << "**ERROR ReportFaultCmd::exec(): fault list needed\n";
		return false;
	}

	bool stateSet = false;
	Fault::FAULT_STATE state = Fault::UD;
	if (optMgr_.isFlagSet("s"))
	{
		stateSet = true;
		std::string stateStr = optMgr_.getFlagVar("s");
		if (stateStr == "ud" || stateStr == "UD")
		{
			state = Fault::UD;
		}
		else if (stateStr == "dt" || stateStr == "DT")
		{
			state = Fault::DT;
		}
		else if (stateStr == "au" || stateStr == "AU")
		{
			state = Fault::AU;
		}
		else if (stateStr == "ti" || stateStr == "TI")
		{
			state = Fault::TI;
		}
		else if (stateStr == "re" || stateStr == "RE")
		{
			state = Fault::RE;
		}
		else if (stateStr == "ab" || stateStr == "AB")
		{
			state = Fault::AB;
		}
		else
		{
			stateSet = true;
			std::cerr << "**WARN ReportFaultCmd::exec(): fault state `";
			std::cerr << stateStr << "' is not supported\n";
		}
	}

	std::cout << "#  fault information\n";
	std::cout << "#    fault type:       ";
	switch (fanMgr_->fListExtract->faultListType_)
	{
		case FaultListExtract::SAF:
			std::cout << "stuck-at fault\n";
			break;
		case FaultListExtract::TDF:
			std::cout << "transition delay fault\n";
			break;
		case FaultListExtract::BRF:
			std::cout << "bridging fault\n";
			break;
		default:
			std::cout << "\n";
			break;
	}
	std::cout << "#    number of faults: " << fanMgr_->fListExtract->faultsInCircuit_.size();
	std::cout << "\n";
	std::cout << "#    type    code    pin (cell)\n";
	std::cout << "#    ----    ----    ----------------------------------\n";
	FaultPtrListIter it = fanMgr_->fListExtract->faultsInCircuit_.begin();
	for (; it != fanMgr_->fListExtract->faultsInCircuit_.end(); ++it)
	{
		if (!stateSet || (*it)->faultState_ != state)
		{
			continue;
		}
		std::cout << "#    ";
		switch ((*it)->faultType_)
		{
			case Fault::SA0:
				std::cout << "SA0     ";
				break;
			case Fault::SA1:
				std::cout << "SA1     ";
				break;
			case Fault::STR:
				std::cout << "STR     ";
				break;
			case Fault::STF:
				std::cout << "STF     ";
				break;
			case Fault::BR:
				std::cout << "BR      ";
				break;
		}
		switch ((*it)->faultState_)
		{
			case Fault::UD:
				std::cout << " UD     ";
				break;
			case Fault::DT:
				std::cout << " DT     ";
				break;
			case Fault::PT:
				std::cout << " PT     ";
				break;
			case Fault::AU:
				std::cout << " AU     ";
				break;
			case Fault::TI:
				std::cout << " TI     ";
				break;
			case Fault::RE:
				std::cout << " RE     ";
				break;
			case Fault::AB:
				std::cout << " AB     ";
				break;
		}
		int cid = fanMgr_->cir->circuitGates_[(*it)->gateID_].cellId_;
		int pid = (*it)->faultyLine_;
		int pmtid = fanMgr_->cir->circuitGates_[(*it)->gateID_].primitiveId_;
		if ((*it)->gateID_ == -1)
		{ // CK
			std::cout << "CK";
		}
		else if ((*it)->gateID_ == -2)
		{ // test_si
			std::cout << "test_si";
		}
		else if ((*it)->gateID_ == -3)
		{ // test_so
			std::cout << "test_so";
		}
		else if ((*it)->gateID_ == -4)
		{ // test_se
			std::cout << "test_se";
		}
		else if (fanMgr_->cir->circuitGates_[(*it)->gateID_].gateType_ == Gate::PI)
		{
			std::cout << fanMgr_->nl->getTop()->getPort(cid)->name_ << " ";
			std::cout << "(primary input)";
		}
		else if (fanMgr_->cir->circuitGates_[(*it)->gateID_].gateType_ == Gate::PO)
		{
			std::cout << fanMgr_->nl->getTop()->getPort(cid)->name_ << " ";
			std::cout << "(primary output)";
		}
		else
		{
			Cell *c = fanMgr_->nl->getTop()->getCell(cid);
			// std::cout << "test" << c->name_<< " " << cid << "\n";
			Cell *libc = c->libc_;
			Cell *pmt = libc->getCell(pmtid);
			Port *p = NULL;
			if (pid < 0)
			{ // must be CK,SE,SI pins on FF
				if (pid == -1)
				{
					std::cout << c->name_ << "/CK ";
				}
				else if (pid == -2)
				{
					std::cout << c->name_ << "/SE ";
				}
				else if (pid == -3)
				{
					std::cout << c->name_ << "/SI ";
				}
				else
				{
					std::cout << c->name_ << "/QN ";
				}
			}
			else if (pid == 0)
			{ // output
				if (!strcmp(libc->name_, "SDFFXL"))
				{
					std::cout << c->name_ << "/Q ";
				}
				else
				{
					for (int i = 0; i < pmt->getNPort(); ++i)
					{
						if (pmt->getPort(i)->type_ != Port::OUTPUT)
						{
							continue;
						}
						Net *n = pmt->getPort(i)->exNet_;
						if (!n)
						{
							continue;
						}
						PortSet pset = libc->getNetPorts(n->id_);
						PortSet::iterator pit = pset.begin();
						for (; pit != pset.end(); ++pit)
						{
							if ((*pit)->top_ != libc)
							{
								continue;
							}
							p = (*pit);
							break;
						}
						if (p)
						{
							break;
						}
					}
				}
			}
			else
			{ // input
				int inCount = 0;
				for (int i = 0; i < pmt->getNPort(); ++i)
				{
					if (pmt->getPort(i)->type_ == Port::INPUT)
					{
						++inCount;
					}
					if (inCount != pid)
					{
						continue;
					}
					Net *n = pmt->getPort(i)->exNet_;
					if (!n)
					{
						continue;
					}
					PortSet pset = libc->getNetPorts(n->id_);
					PortSet::iterator pit = pset.begin();
					for (; pit != pset.end(); ++pit)
					{
						if ((*pit)->top_ != libc)
						{
							continue;
						}
						p = (*pit);
						break;
					}
					if (p)
					{
						break;
					}
				}
			}
			if (p)
			{
				std::cout << c->name_ << "/" << p->name_ << " ";
			}
			std::cout << "(" << libc->name_ << ")";
		}
		std::cout << "\n";
	}
	std::cout << "\n";

	return true;
}

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

bool ReportCircuitCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR ReportCircuitCmd::exec(): circuit needed\n";
		return false;
	}

	Cell *top = fanMgr_->cir->pNetlist_->getTop();
	std::cout << "#  circuit information\n";
	std::cout << "#    netlist:         " << top->name_ << "\n";
	std::cout << "#    number of PIs:   " << fanMgr_->cir->numPI_ << "\n";
	std::cout << "#    number of PPIs:  " << fanMgr_->cir->numPPI_ << "\n";
	std::cout << "#    number of POs:   " << fanMgr_->cir->numPO_ << "\n";
	std::cout << "#    number of combs: " << fanMgr_->cir->numComb_ << "\n";
	std::cout << "#    number of gates: " << fanMgr_->cir->numGate_ << "\n";
	std::cout << "#    number of nets:  " << fanMgr_->cir->numNet_ << "\n";

	return true;
}

ReportGateCmd::ReportGateCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("report gate information");
	optMgr_.setDes("reports GATE information in the circuit");
	Arg *arg = new Arg(Arg::OPT_INF, "if no name is specified, all gates will be reported", "GATE");
	optMgr_.regArg(arg);
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
ReportGateCmd::~ReportGateCmd() {}

bool ReportGateCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR ReportGateCmd::exec(): circuit needed\n";
		return false;
	}

	if (optMgr_.getNParsedArg() == 0)
	{
		for (int i = 0; i < fanMgr_->cir->numGate_ * fanMgr_->cir->numFrame_; ++i)
		{
			reportGate(i);
		}
	}
	else
	{
		for (int i = 0; i < optMgr_.getNParsedArg(); ++i)
		{
			std::string name = optMgr_.getParsedArg(i);
			Cell *c = fanMgr_->nl->getTop()->getCell(name.c_str());
			if (c)
			{
				for (int j = 0; j < c->libc_->getNCell(); ++j)
				{
					reportGate(fanMgr_->cir->cellIndexToGateIndex_[c->id_] + j);
				}
			}
			Port *p = fanMgr_->nl->getTop()->getPort(name.c_str());
			if (p)
			{
				reportGate(fanMgr_->cir->portIndexToGateIndex_[p->id_]);
			}
			if (!c && !p)
			{
				std::cerr << "**ERROR ReportGateCmd::exec(): cell or port `";
				std::cerr << name << "' does not exist\n";
			}
		}
	}
	return true;
}

void ReportGateCmd::reportGate(const int &i) const
{
	Gate *g = &fanMgr_->cir->circuitGates_[i];
	std::cout << "#  ";
	if (g->gateType_ == Gate::PI || g->gateType_ == Gate::PO)
	{
		std::cout << fanMgr_->nl->getTop()->getPort((size_t)g->cellId_)->name_;
	}
	else
	{
		std::cout << fanMgr_->nl->getTop()->getCell((size_t)g->cellId_)->name_;
	}
	std::cout << " id(" << i << ") ";
	std::cout << "lvl(" << g->numLevel_ << ") ";
	std::cout << "type(" << g->gateType_ << ") ";
	std::cout << "frame(" << g->frame_ << ")";
	std::cout << "\n";
	std::cout << "#    fi[" << g->numFI_ << "]";
	for (int j = 0; j < g->numFI_; ++j)
	{
		std::cout << " " << g->faninVector_[j];
	}
	std::cout << "\n";
	std::cout << "#    fo[" << g->numFO_ << "]";
	for (int j = 0; j < g->numFO_; ++j)
	{
		std::cout << " " << g->fanoutVector_[j];
	}
	std::cout << "\n\n";
}

ReportValueCmd::ReportValueCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("report gate values");
	optMgr_.setDes("reports GATE values in the circuit");
	Arg *arg = new Arg(Arg::OPT_INF, "if no name is specified, all gates will be reported", "GATE");
	optMgr_.regArg(arg);
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
ReportValueCmd::~ReportValueCmd() {}

bool ReportValueCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR ReportValueCmd::exec(): circuit needed\n";
		return false;
	}

	if (optMgr_.getNParsedArg() == 0)
	{
		for (int i = 0; i < fanMgr_->cir->totalGate_; ++i)
		{
			reportValue(i);
		}
	}
	else
	{
		for (int i = 0; i < optMgr_.getNParsedArg(); ++i)
		{
			std::string name = optMgr_.getParsedArg(i);
			Cell *c = fanMgr_->nl->getTop()->getCell(name.c_str());
			if (c)
			{
				for (int j = 0; j < c->libc_->getNCell(); ++j)
				{
					reportValue(fanMgr_->cir->cellIndexToGateIndex_[c->id_] + j);
				}
			}
			Port *p = fanMgr_->nl->getTop()->getPort(name.c_str());
			if (p)
			{
				reportValue(fanMgr_->cir->portIndexToGateIndex_[p->id_]);
			}
			if (!c && !p)
			{
				std::cerr << "**ERROR ReportValueCmd::exec(): cell or port `";
				std::cerr << name << "' does not exist\n";
			}
		}
	}
	return true;
}

void ReportValueCmd::reportValue(const int &i) const
{
	Gate *g = &fanMgr_->cir->circuitGates_[i];
	std::cout << "#  ";
	if (g->gateType_ == Gate::PI || g->gateType_ == Gate::PO)
	{
		std::cout << fanMgr_->nl->getTop()->getPort((size_t)g->cellId_)->name_;
	}
	else
	{
		std::cout << fanMgr_->nl->getTop()->getCell((size_t)g->cellId_)->name_;
	}
	std::cout << " id(" << i << ") ";
	std::cout << "lvl(" << g->numLevel_ << ") ";
	std::cout << "type(" << g->gateType_ << ") ";
	std::cout << "frame(" << g->frame_ << ")";
	std::cout << "\n";
	std::cout << "#    good:   ";
	printSimulationValue(g->goodSimLow_, g->goodSimHigh_);
	std::cout << "\n";
	std::cout << "#    faulty: ";
	printSimulationValue(g->faultSimLow_, g->faultSimHigh_);
	std::cout << "\n\n";
}

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

bool ReportStatsCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->fListExtract || fanMgr_->fListExtract->faultsInCircuit_.size() == 0)
	{
		std::cerr << "**WARN ReportStatsCmd::exec(): no statistics\n";
		return false;
	}

	// determine fault model
	std::string ftype = "";
	switch (fanMgr_->fListExtract->faultListType_)
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
	std::string ptype = "";
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
	{
		npat = fanMgr_->pcoll->patternVector_.size();
	}

	size_t numCollapsedFaults = 0;
	size_t fu = 0;
	size_t ud = 0;
	size_t dt = 0;
	size_t pt = 0;
	size_t au = 0;
	size_t ti = 0;
	size_t re = 0;
	size_t ab = 0;

	FaultPtrListIter it = fanMgr_->fListExtract->faultsInCircuit_.begin();
	for (; it != fanMgr_->fListExtract->faultsInCircuit_.end(); ++it)
	{
		++numCollapsedFaults;
		int eq = (*it)->equivalent_;
		fu += eq;
		switch ((*it)->faultState_)
		{
			case Fault::UD:
				ud += eq;
				break;
			case Fault::DT:
				dt += eq;
				break;
			case Fault::PT:
				pt += eq;
				break;
			case Fault::AU:
				au += eq;
				break;
			case Fault::TI:
				ti += eq;
				break;
			case Fault::RE:
				re += eq;
				break;
			case Fault::AB:
				ab += eq;
				break;
		}
	}

	float fc = (float)dt / (float)fu * 100;
	float tc = (float)dt / (float)(ud + dt + pt + ab) * 100;
	float ae = (float)(dt + au + ti + re) / (float)fu * 100;

	std::cout << std::right;
	std::cout << std::setprecision(4);
	std::cout << "#                 Statistics Report\n";
	std::cout << "#  Circuit name                  " << std::setw(19);
	std::cout << fanMgr_->nl->getTop()->name_ << "\n";
	std::cout << "#  Fault model                   " << std::setw(19) << ftype << "\n";
	std::cout << "#  Pattern type                  " << std::setw(19) << ptype << "\n";
	std::cout << "#  -------------------------------------------------\n";
	std::cout << "#  Fault classes                             #faults\n";
	std::cout << "#  ----------------------------  -------------------\n";
	std::cout << "#    FU (full)                   " << std::setw(19) << fu << "\n";
	std::cout << "#    FU (collapsed)              " << std::setw(19) << numCollapsedFaults << "\n";
	std::cout << "#    --------------------------  -------------------\n";
	std::cout << "#    UD (undetected)             " << std::setw(19) << ud << "\n";
	std::cout << "#    PT (possibly testable)      " << std::setw(19) << pt << "\n";
	std::cout << "#    AU (atpg untestable)        " << std::setw(19) << au << "\n";
	std::cout << "#    RE (redundant)              " << std::setw(19) << re << "\n";
	std::cout << "#    AB (atpg abort)             " << std::setw(19) << ab << "\n";
	std::cout << "#    TI (tied)                   " << std::setw(19) << ti << "\n";
	std::cout << "#    --------------------------  -------------------\n";
	std::cout << "#    DT (detected)               " << std::setw(19) << dt << "\n";
	std::cout << "#  -------------------------------------------------\n";
	std::cout << "#  Coverage                               percentage\n";
	std::cout << "#    --------------------------  -------------------\n";
	std::cout << "#    test coverage                            ";
	std::cout << std::setw(5) << tc << "%\n";
	std::cout << "#    fault coverage                           ";
	std::cout << std::setw(5) << fc << "%\n";
	std::cout << "#    atpg effectiveness                       ";
	std::cout << std::setw(5) << ae << "%\n";
	std::cout << "#  -------------------------------------------------\n";
	std::cout << "#  #Patterns                     " << std::setw(19) << npat << "\n";
	std::cout << "#  -------------------------------------------------\n";
	std::cout << "#  ATPG runtime                  " << std::setw(17) << rtime;
	std::cout << " s\n";
	std::cout << "#  -------------------------------------------------\n";

	return true;
}

AddPinConsCmd::AddPinConsCmd(const std::string &name, FanMgr *fanMgr) : Cmd(name)
{
	fanMgr_ = fanMgr;
	optMgr_.setName(name);
	optMgr_.setShortDes("add pin constraint");
	optMgr_.setDes("constraint PI(s) to 0 or 1");
	Arg *arg = new Arg(Arg::REQ_INF, "PIs to be constrained", "PI");
	optMgr_.regArg(arg);
	arg = new Arg(Arg::REQ, "constraint value, 0 or 1", "<0|1>");
	optMgr_.regArg(arg);
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

AddPinConsCmd::~AddPinConsCmd() {}

bool AddPinConsCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR AddPinConsCmd::exec(): circuit needed";
		std::cerr << "\n";
		return false;
	}

	if (optMgr_.getNParsedArg() < 2)
	{
		std::cerr << "**ERROR AddPinConsCmd::exec(): need at least one PI ";
		std::cerr << "and the constraint value\n";
		return false;
	}

	size_t npi = optMgr_.getNParsedArg() - 1;
	int cons = atoi(optMgr_.getParsedArg(npi).c_str());
	for (int i = 0; i < npi; ++i)
	{
		std::string pinName = optMgr_.getParsedArg(i);
		Port *p = fanMgr_->nl->getTop()->getPort(pinName.c_str());
		if (!p)
		{
			std::cerr << "**ERROR AddPinConsCmd::exec(): Port `" << pinName;
			std::cerr << "' not found\n";
			continue;
		}
		int gid = fanMgr_->cir->portIndexToGateIndex_[p->id_];
		if (fanMgr_->cir->circuitGates_[gid].gateType_ != Gate::PI)
		{
			std::cerr << "**ERROR AddPinConsCmd::exec(): Port `" << pinName;
			std::cerr << "' is not PI\n";
			continue;
		}
		fanMgr_->cir->circuitGates_[gid].hasConstraint_ = true;
		if (cons)
		{
			fanMgr_->cir->circuitGates_[gid].constraint_ = PARA_H;
		}
		else
		{
			fanMgr_->cir->circuitGates_[gid].constraint_ = PARA_L;
		}
	}
	return true;
}

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

bool RunLogicSimCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR RunLogicSimCmd::exec(): circuit needed\n";
		return false;
	}

	if (!fanMgr_->pcoll)
	{
		std::cerr << "**ERROR RunLogicSimCmd::exec(): pattern needed\n";
		return false;
	}

	fanMgr_->tmusg.periodStart();
	std::cout << "#  Performing logic simulation ...\n";

	Simulator sim(fanMgr_->cir);
	sim.parallelPatternGoodSimWithAllPattern(fanMgr_->pcoll);

	TmStat stat;
	fanMgr_->tmusg.getPeriodUsage(stat);
	std::cout << "#  Finished logic simulation";
	std::cout << "    " << (double)stat.rTime / 1000000.0 << " s";
	std::cout << "    " << (double)stat.vmSize / 1024.0 << " MB\n";

	return true;
}

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

bool RunFaultSimCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR RunFaultSimCmd::exec(): circuit needed\n";
		return false;
	}

	if (!fanMgr_->pcoll)
	{
		std::cerr << "**ERROR RunFaultSimCmd::exec(): pattern needed\n";
		return false;
	}

	if (!fanMgr_->fListExtract || fanMgr_->fListExtract->faultsInCircuit_.size() == 0)
	{
		std::cerr << "**ERROR RunFaultSimCmd::exec(): fault list needed\n";
		return false;
	}

	if (!fanMgr_->sim)
	{
		fanMgr_->sim = new Simulator(fanMgr_->cir);
	}

	std::cout << "#  Performing fault simulation ...\n";
	fanMgr_->tmusg.periodStart();

	if (optMgr_.isFlagSet("m") && optMgr_.getFlagVar("m") == "pf")
	{
		fanMgr_->sim->parallelFaultFaultSimWithAllPattern(fanMgr_->pcoll, fanMgr_->fListExtract);
	}
	else
	{
		fanMgr_->sim->parallelPatternFaultSimWithAllPattern(fanMgr_->pcoll, fanMgr_->fListExtract);
	}

	TmStat stat;
	fanMgr_->tmusg.getPeriodUsage(stat);
	std::cout << "#  Finished fault simulation";
	std::cout << "    " << (double)stat.rTime / 1000000.0 << " s";
	std::cout << "    " << (double)stat.vmSize / 1024.0 << " MB\n";
	rtime = (double)stat.rTime / 1000000.0;

	return true;
}

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

bool RunAtpgCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR RunFaultSimCmd::exec(): circuit needed\n";
		return false;
	}

	if (!fanMgr_->pcoll)
	{
		fanMgr_->pcoll = new PatternProcessor;
	}

	if (!fanMgr_->fListExtract)
	{
		fanMgr_->fListExtract = new FaultListExtract;
		fanMgr_->fListExtract->extractFaultFromCircuit(fanMgr_->cir);
	}

	if (!fanMgr_->sim)
	{
		fanMgr_->sim = new Simulator(fanMgr_->cir);
	}

	delete fanMgr_->atpg;
	fanMgr_->atpg = new Atpg(fanMgr_->cir, fanMgr_->sim);

	std::cout << "#  Performing pattern generation ...\n";
	fanMgr_->tmusg.periodStart();

	fanMgr_->atpg->generatePatternSet(fanMgr_->pcoll, fanMgr_->fListExtract, true);

	fanMgr_->tmusg.getPeriodUsage(fanMgr_->atpgStat);
	std::cout << "#  Finished pattern generation";
	std::cout << "    " << (double)fanMgr_->atpgStat.rTime / 1000000.0 << " s";
	std::cout << "    " << (double)fanMgr_->atpgStat.vmSize / 1024.0 << " MB";
	std::cout << "\n";
	rtime = (double)fanMgr_->atpgStat.rTime / 1000000.0;

	return true;
}

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

bool WritePatCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (optMgr_.getNParsedArg() < 1)
	{
		std::cerr << "**ERROR WritePatternCmd::exec(): output file needed";
		std::cerr << "\n";
		return false;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR WritePatternCmd::exec(): circuit needed\n";
		return false;
	}

	if (!fanMgr_->pcoll)
	{
		std::cerr << "**ERROR WritePatternCmd::exec(): pattern needed\n";
		return false;
	}

	std::cout << "#  Writing pattern to `" << optMgr_.getParsedArg(0) << "' ...";
	std::cout << "\n";
	PatternWriter writer(fanMgr_->pcoll, fanMgr_->cir);
	if (optMgr_.getFlagVar("f") == "lht")
	{
		if (!writer.writeLht(optMgr_.getParsedArg(0).c_str()))
		{
			std::cerr << "**ERROR WritePatternCmd::exec(): writer failed\n";
			return false;
		}
	}
	else if (optMgr_.getFlagVar("f") == "ascii")
	{
		if (!writer.writeAscii(optMgr_.getParsedArg(0).c_str()))
		{
			std::cerr << "**ERROR WritePatternCmd::exec(): writer failed\n";
			return false;
		}
	}
	else if (optMgr_.getFlagVar("f") == "pat" || optMgr_.getFlagVar("f") == "")
	{
		if (!writer.writePattern(optMgr_.getParsedArg(0).c_str()))
		{
			std::cerr << "**ERROR WritePatternCmd::exec(): writer failed\n";
			return false;
		}
	}
	else
	{
		std::cerr << "**ERROR WritePatternCmd::exec(): undefined parameter in -f\n";
	}

	return true;
}

// Ne
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

bool AddScanChainsCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR AddScanChainsCmd::exec(): circuit needed\n";
		return false;
	}

	std::cout << "#  Add Scan Chains \n";

	return true;
}
// Ne

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

bool WriteProcCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (optMgr_.getNParsedArg() < 1)
	{
		std::cerr << "**ERROR WritePatternCmd::exec(): output file needed";
		std::cerr << "\n";
		return false;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR WritePatternCmd::exec(): circuit needed\n";
		return false;
	}

	std::cout << "#  Writing test procedure file ...\n";
	ProcedureWriter writer(fanMgr_->cir);
	if (!writer.writeProcedure(optMgr_.getParsedArg(0).c_str()))
	{
		std::cerr << "**ERROR WritePatternCmd::exec(): writer failed\n";
		return false;
	}

	return true;
}

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

bool WriteStilCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);

	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (optMgr_.getNParsedArg() < 1)
	{
		std::cerr << "**ERROR WriteSTILCmd::exec(): output file needed";
		std::cerr << "\n";
		return false;
	}

	if (!fanMgr_->cir)
	{
		std::cerr << "**ERROR WriteSTILCmd::exec(): circuit needed\n";
		return false;
	}

	if (!fanMgr_->pcoll)
	{
		std::cerr << "**ERROR WriteSTILCmd::exec(): pattern needed\n";
		return false;
	}

	std::cout << "#  Writing pattern to STIL...\n";
	PatternWriter writer(fanMgr_->pcoll, fanMgr_->cir);

	if (!writer.writeSTIL(optMgr_.getParsedArg(0).c_str()))
	{
		std::cerr << "**ERROR WriteSTILCmd::exec(): writer failed\n";
		return false;
	}

	return true;
}
