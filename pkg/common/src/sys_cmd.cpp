// **************************************************************************
// File       [ sys_cmd.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ some system and misc commands ]
// Date       [ Ver. 2.0 started 2010/07/01 ]
// **************************************************************************

#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include "sys_cmd.h"

using namespace CommonNs;

SysBashCmd::SysBashCmd(const std::string &name) : Cmd(name)
{
	optMgr_.setShortDes("opens a new bash shell environment");
	optMgr_.setDes("opens a new bash shell environment");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

SysBashCmd::~SysBashCmd() {}

bool SysBashCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	int res = system("bash --login");
	if (res != 0)
	{
		return false;
	}
	return true;
}

// SysListCmd methods
SysListCmd::SysListCmd(const std::string &name) : Cmd(name)
{
	optMgr_.setShortDes("list diectory contents");
	optMgr_.setDes("lists contents in DIRECTORY. If not specified, list current directory content.");
	optMgr_.regArg(new Arg(Arg::OPT, "target directories", "DIRECTORY"));
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

SysListCmd::~SysListCmd() {}

bool SysListCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	std::string cmdStr = "";
	std::string arg = "";
	for (size_t i = 0; i < argv.size(); ++i)
	{
		arg += argv[i] + " ";
	}
	cmdStr = arg + " --color=always -F 2> /dev/null";
	if (system(cmdStr.c_str()) != 0)
	{
		std::cerr << "**ERROR SysListCmd::exec(): list directory failed\n";
		return false;
	}
	return true;
}

SysCatCmd::SysCatCmd(const std::string &name) : Cmd(name)
{
	optMgr_.setShortDes("concatenate files and print on the standard output");
	optMgr_.setDes("Concatenate FILE(s), or standard input, to standard output");
	optMgr_.regArg(new Arg(Arg::REQ_INF, "files to be printed", "FILE"));
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

SysCatCmd::~SysCatCmd() {}

bool SysCatCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (argv.size() < 2)
	{
		std::cerr << "**ERROR SysCatCmd::exec(): file needed\n";
		return false;
	}

	std::string cmdStr = "";
	std::string arg = "";
	for (size_t i = 0; i < argv.size(); ++i)
	{
		arg += argv[i] + " ";
	}
	cmdStr = arg + " 2> /dev/null";
	if (system(cmdStr.c_str()) != 0)
	{
		std::cerr << "**ERROR SysCatCmd::exec(): cat files failed\n";
		return false;
	}
	return true;
}

// SysCdCmd methods
SysCdCmd::SysCdCmd(const std::string &name) : Cmd(name)
{
	optMgr_.setShortDes("change directory");
	optMgr_.setDes("changes working directory to DIRECTORY. If not specified, changes to home directory.");
	optMgr_.regArg(new Arg(Arg::OPT, "target directories", "DIRECTORY"));
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

SysCdCmd::~SysCdCmd() {}

bool SysCdCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (argv.size() == 1)
	{
		std::string home = std::string(getenv("HOME"));
		if (chdir(home.c_str()) != 0)
		{
			std::cerr << "**ERROR SysCdCmd::exec(): ";
			std::cerr << "cannot change to target directory\n";
			return false;
		}
		return true;
	}
	if (argv.size() > 1)
	{
		std::string dest = argv[1];
		if (chdir(dest.c_str()) != 0)
		{
			std::cerr << "**ERROR SysCdCmd::exec(): ";
			std::cerr << "cannot change to target directory\n";
			return false;
		}
	}
	return true;
}

// SysPwdCmd methods
SysPwdCmd::SysPwdCmd(const std::string &name) : Cmd(name)
{
	optMgr_.setShortDes("print name of current directory");
	optMgr_.setDes("prints the full filename of the current working directory");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

SysPwdCmd::~SysPwdCmd() {}

bool SysPwdCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	int res = system("pwd");
	if (res != 0)
	{
		return false;
	}
	return true;
}

// SysSetCmd methods
SysSetCmd::SysSetCmd(const std::string &name, CmdMgr *cmdMgr) : Cmd(name)
{
	cmdMgr_ = cmdMgr;
	optMgr_.setShortDes("set variables");
	optMgr_.setDes("set VAR to STRING");
	optMgr_.regArg(new Arg(Arg::REQ, "variable name", "VAR"));
	optMgr_.regArg(new Arg(Arg::REQ, "std::string to be substituted", "STRING"));
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

SysSetCmd::~SysSetCmd() {}

bool SysSetCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (argv.size() == 1)
	{
		VarMap vars = cmdMgr_->getVars();
		std::cout << "#  set variables\n";
		for (VarMapIter it = vars.begin(); it != vars.end(); ++it)
		{
			std::cout << "#    " << it->first << " = " << it->second << "\n";
		}
		return true;
	}

	if (argv.size() < 3)
	{
		std::cerr << "**ERROR SysSetCmd::exec(): variable and std::string needed";
		std::cerr << "\n";
		return false;
	}

	if (!cmdMgr_->addVar(argv[1], argv[2]))
	{
		std::cerr << "**ERROR SysSetCmd::exec(): variable `" << argv[1] << "' ";
		std::cerr << "set failed\n";
		return false;
	}

	return true;
}

// SysExitCmd methods
SysExitCmd::SysExitCmd(const std::string &name, CmdMgr *cmdMgr) : Cmd(name)
{
	cmdMgr_ = cmdMgr;
	optMgr_.setShortDes("exit the program");
	optMgr_.setDes("exits the program");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}
SysExitCmd::~SysExitCmd() {}

bool SysExitCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	cmdMgr_->setExit(true);
	return true;
}

SysSourceCmd::SysSourceCmd(const std::string &name, CmdMgr *cmdMgr) : Cmd(name)
{
	cmdMgr_ = cmdMgr;
	optMgr_.setShortDes("run commands from startup file");
	optMgr_.setDes("runs commands from FILE");
	optMgr_.regArg(new Arg(Arg::REQ, "target file with commands", "FILE"));
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

SysSourceCmd::~SysSourceCmd() {}

bool SysSourceCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (argv.size() < 2)
	{
		std::cerr << "**ERROR SysSourceCmd::exec(): ";
		std::cerr << "please specify source file\n";
		return false;
	}

	std::ifstream fstr;
	fstr.open(argv[1].c_str(), std::ifstream::in);
	if (!fstr)
	{
		std::cerr << "**ERROR SysSourceCmd::exec(): ";
		std::cerr << "file `" << argv[1] << "' cannot be opened\n";
		return false;
	}

	size_t pos = argv[1].find_last_of('/');
	std::string baseName = pos == argv[1].npos ? argv[1] : argv[1].substr(pos);

	int count = 0;
	std::string cmdStr;
	CmdMgr::Result res;
	while (fstr.good())
	{
		getline(fstr, cmdStr);
		++count;
		if (cmdStr == "")
		{
			continue;
		}

		std::cout << baseName << " " << count << "> ";
		std::cout << cmdStr << "\n";
		res = cmdMgr_->exec(cmdStr);
		if (res == CmdMgr::NOT_EXIST)
		{
			std::cerr << "**ERROR SysSourceCmd::exec(): ";
			std::cerr << "command does not exist\n";
		}
		if (res == CmdMgr::EXIT)
		{
			break;
		}
	}
	fstr.close();

	return true;
}

SysHelpCmd::SysHelpCmd(const std::string &name, CmdMgr *cmdMgr) : Cmd(name)
{
	cmdMgr_ = cmdMgr;
	optMgr_.setShortDes("print help messages");
	optMgr_.setDes("prints help for COMMAND. If no specitfied, prints the usage of the command manager.");
	optMgr_.regArg(new Arg(Arg::OPT, "target command", "COMMAND"));
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

SysHelpCmd::~SysHelpCmd() {}

bool SysHelpCmd::exec(const std::vector<std::string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (argv.size() == 1)
	{
		cmdMgr_->usage();
		return true;
	}
	if (argv.size() > 1)
	{
		std::string cmdStr = argv[1];
		Cmd *cmd = cmdMgr_->getCmd(cmdStr);
		if (!cmd)
		{
			std::cerr << "**ERROR SysHelpCmd::exec(): command `" << cmdStr << "' ";
			std::cerr << "does not exist\n";
			return false;
		}
		else
		{
			cmd->optMgr_.usage();
		}
	}
	return true;
}