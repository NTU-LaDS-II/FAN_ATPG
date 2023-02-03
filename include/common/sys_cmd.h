// **************************************************************************
// File       [ sys_cmd.h ]
// Author     [ littleshamoo ]
// Synopsis   [ some system and misc commands ]
// Date       [ Ver. 2.0 started 2010/04/09 ]
// **************************************************************************

#ifndef _COMMON_SYS_CMD_H_
#define _COMMON_SYS_CMD_H_

#include "cmd.h"

namespace CommonNs
{

	class SysBashCmd : public Cmd
	{
	public:
		SysBashCmd(const std::string &name);
		~SysBashCmd();

		bool exec(const std::vector<std::string> &argv);
	};

	class SysListCmd : public Cmd
	{
	public:
		SysListCmd(const std::string &name);
		~SysListCmd();

		bool exec(const std::vector<std::string> &argv);
	};

	class SysCatCmd : public Cmd
	{
	public:
		SysCatCmd(const std::string &name);
		~SysCatCmd();

		bool exec(const std::vector<std::string> &argv);
	};

	class SysCdCmd : public Cmd
	{
	public:
		SysCdCmd(const std::string &name);
		~SysCdCmd();

		bool exec(const std::vector<std::string> &argv);
	};

	class SysPwdCmd : public Cmd
	{
	public:
		SysPwdCmd(const std::string &name);
		~SysPwdCmd();

		bool exec(const std::vector<std::string> &argv);
	};

	class SysSetCmd : public Cmd
	{
	public:
		SysSetCmd(const std::string &name, CmdMgr *mgr);
		~SysSetCmd();

		bool exec(const std::vector<std::string> &argv);

	private:
		CmdMgr *cmdMgr_;
	};

	class SysExitCmd : public Cmd
	{
	public:
		SysExitCmd(const std::string &name, CmdMgr *mgr);
		~SysExitCmd();

		bool exec(const std::vector<std::string> &argv);

	private:
		CmdMgr *cmdMgr_;
	};

	class SysSourceCmd : public Cmd
	{
	public:
		SysSourceCmd(const std::string &name, CmdMgr *mgr);
		~SysSourceCmd();

		bool exec(const std::vector<std::string> &argv);

	private:
		CmdMgr *cmdMgr_;
	};

	class SysHelpCmd : public Cmd
	{
	public:
		SysHelpCmd(const std::string &name, CmdMgr *mgr);
		~SysHelpCmd();

		bool exec(const std::vector<std::string> &argv);

	private:
		CmdMgr *cmdMgr_;
	};

};

#endif