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

using namespace std;
using namespace CommonNs;

//{{{ class SysBashCmd   method
SysBashCmd::SysBashCmd(const string &name) : Cmd(name)
{
	optMgr_.setShortDes("opens a new bash shell environment");
	optMgr_.setDes("opens a new bash shell environment");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

SysBashCmd::~SysBashCmd() {}

bool SysBashCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	int res = system("bash --login");
	if (res != 0)
		return false;
	return true;
}
//}}}
//{{{ class SysListCmd   method
SysListCmd::SysListCmd(const string &name) : Cmd(name)
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

bool SysListCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	string cmdStr = "";
	string arg = "";
	for (size_t i = 0; i < argv.size(); ++i)
		arg += argv[i] + " ";
	cmdStr = arg + " --color=always -F 2> /dev/null";
	if (system(cmdStr.c_str()) != 0)
	{
		cerr << "**ERROR SysListCmd::exec(): list directory failed" << endl;
		return false;
	}
	return true;
}
//}}}
//{{{ class SysCatCmd    method
SysCatCmd::SysCatCmd(const string &name) : Cmd(name)
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

bool SysCatCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (argv.size() < 2)
	{
		cerr << "**ERROR SysCatCmd::exec(): file needed" << endl;
		return false;
	}

	string cmdStr = "";
	string arg = "";
	for (size_t i = 0; i < argv.size(); ++i)
		arg += argv[i] + " ";
	cmdStr = arg + " 2> /dev/null";
	if (system(cmdStr.c_str()) != 0)
	{
		cerr << "**ERROR SysCatCmd::exec(): cat files failed" << endl;
		return false;
	}
	return true;
}
//}}}
//{{{class SysCdCmd     method
SysCdCmd::SysCdCmd(const string &name) : Cmd(name)
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

bool SysCdCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (argv.size() == 1)
	{
		string home = string(getenv("HOME"));
		if (chdir(home.c_str()) != 0)
		{
			cerr << "**ERROR SysCdCmd::exec(): ";
			cerr << "cannot change to target directory" << endl;
			return false;
		}
		return true;
	}
	if (argv.size() > 1)
	{
		string dest = argv[1];
		if (chdir(dest.c_str()) != 0)
		{
			cerr << "**ERROR SysCdCmd::exec(): ";
			cerr << "cannot change to target directory" << endl;
			return false;
		}
	}
	return true;
}
//}}}
//{{{ class SysPwdCmd   method
SysPwdCmd::SysPwdCmd(const string &name) : Cmd(name)
{
	optMgr_.setShortDes("print name of current directory");
	optMgr_.setDes("prints the full filename of the current working directory");
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

SysPwdCmd::~SysPwdCmd() {}

bool SysPwdCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	int res = system("pwd");
	if (res != 0)
		return false;
	return true;
}
//}}}
//{{{ class SysSetCmd    method
SysSetCmd::SysSetCmd(const string &name, CmdMgr *cmdMgr) : Cmd(name)
{
	cmdMgr_ = cmdMgr;
	optMgr_.setShortDes("set variables");
	optMgr_.setDes("set VAR to STRING");
	optMgr_.regArg(new Arg(Arg::REQ, "variable name", "VAR"));
	optMgr_.regArg(new Arg(Arg::REQ, "string to be substituted", "STRING"));
	Opt *opt = new Opt(Opt::BOOL, "print usage", "");
	opt->addFlag("h");
	opt->addFlag("help");
	optMgr_.regOpt(opt);
}

SysSetCmd::~SysSetCmd() {}

bool SysSetCmd::exec(const vector<string> &argv)
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
		cout << "#  set variables" << endl;
		for (VarMapIter it = vars.begin(); it != vars.end(); ++it)
			cout << "#    " << it->first << " = " << it->second << endl;
		return true;
	}

	if (argv.size() < 3)
	{
		cerr << "**ERROR SysSetCmd::exec(): variable and string needed";
		cerr << endl;
		return false;
	}

	if (!cmdMgr_->addVar(argv[1], argv[2]))
	{
		cerr << "**ERROR SysSetCmd::exec(): variable `" << argv[1] << "' ";
		cerr << "set failed" << endl;
		return false;
	}

	return true;
}
//}}}
//{{{ class SysExitCmd   method
SysExitCmd::SysExitCmd(const string &name, CmdMgr *cmdMgr) : Cmd(name)
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

bool SysExitCmd::exec(const vector<string> &argv)
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
//}}}
//{{{ class SysSourceCmd method
SysSourceCmd::SysSourceCmd(const string &name, CmdMgr *cmdMgr) : Cmd(name)
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

bool SysSourceCmd::exec(const vector<string> &argv)
{
	optMgr_.parse(argv);
	if (optMgr_.isFlagSet("h"))
	{
		optMgr_.usage();
		return true;
	}

	if (argv.size() < 2)
	{
		cerr << "**ERROR SysSourceCmd::exec(): ";
		cerr << "please specify source file" << endl;
		return false;
	}

	ifstream fstr;
	fstr.open(argv[1].c_str(), ifstream::in);
	if (!fstr)
	{
		cerr << "**ERROR SysSourceCmd::exec(): ";
		cerr << "file `" << argv[1] << "' cannot be opened" << endl;
		return false;
	}

	size_t pos = argv[1].find_last_of('/');
	string baseName = pos == argv[1].npos ? argv[1] : argv[1].substr(pos);

	int count = 0;
	string cmdStr;
	CmdMgr::Result res;
	while (fstr.good())
	{
		getline(fstr, cmdStr);
		count++;
		if (cmdStr == "")
			continue;

		cout << baseName << " " << count << "> ";
		cout << cmdStr << endl;
		res = cmdMgr_->exec(cmdStr);
		if (res == CmdMgr::NOT_EXIST)
		{
			cerr << "**ERROR SysSourceCmd::exec(): ";
			cerr << "command does not exist" << endl;
		}
		if (res == CmdMgr::EXIT)
			break;
	}
	fstr.close();

	return true;
}
//}}}
//{{{ class SysHelpCmd   method
SysHelpCmd::SysHelpCmd(const string &name, CmdMgr *cmdMgr) : Cmd(name)
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

bool SysHelpCmd::exec(const vector<string> &argv)
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
		string cmdStr = argv[1];
		Cmd *cmd = cmdMgr_->getCmd(cmdStr);
		if (!cmd)
		{
			cerr << "**ERROR SysHelpCmd::exec(): command `" << cmdStr << "' ";
			cerr << "does not exist" << endl;
			return false;
		}
		else
			cmd->optMgr_.usage();
	}
	return true;
}
//}}}
