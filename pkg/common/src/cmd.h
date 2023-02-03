// **************************************************************************
// File       [ cmd.h ]
// Author     [ littleshamoo ]
// Synopsis   [ header file for commands ]
// Date       [ Ver. 2.0 started 2010/04/09 ]
// **************************************************************************

#ifndef _COMMON_CMD_H_
#define _COMMON_CMD_H_

#include "ascii.h"
#include "get_opt.h"

#include <cstdio>

namespace CommonNs
{

	// type defines
	class Cmd;
	typedef std::map<std::string, std::set<std::string>> CatMap;
	typedef std::map<std::string, std::string> VarMap;
	typedef std::map<std::string, Cmd *> CmdMap;
	typedef CatMap::iterator CatMapIter;
	typedef VarMap::iterator VarMapIter;
	typedef CmdMap::iterator CmdMapIter;

	class Cmd
	{
	public:
		Cmd(const std::string &name);
		virtual ~Cmd();
		virtual bool exec(const std::vector<std::string> &argv) = 0;
		std::string getName() const;
		OptMgr optMgr_;
	};

	class CmdMgr
	{
	public:
		enum Result
		{
			EXIT = -1,
			SUCCESS,
			FAIL,
			NOT_EXIST,
			NOP
		};
		enum Error
		{
			E_EXIST = 0,
			E_NOT_REG,
			E_INVALID_VAR
		};
		enum Color
		{
			BLACK = 0,
			RED,
			GREEN,
			YELLOW,
			BLUE,
			PURPLE,
			CYAN,
			WHITE
		};

		CmdMgr();
		~CmdMgr();

		void regCmd(const std::string &cat, Cmd *const cmd);
		Cmd *getCmd(const std::string &name) const;
		Result read();
		Result exec(const std::string &cmdStr);
		void setComment(const char &ch);
		void setPrompt(const std::string &str);
		void setColor(const Color &color);
		void setExit(const bool &exit);
		bool addVar(const std::string &var, const std::string &str);
		VarMap getVars() const;
		Error getError() const;
		std::string getErrorStr() const;

		void usage(std::ostream &out = std::cout);

	private:
		// setting variables
		std::string expandVar(const std::string &var) const;
		std::string expandHome(const std::string &path) const;
		bool isLegalVar(const std::string &var) const;
		bool isLegalVarChar(const char &ch) const;

		// reading from standard input
		void setStdin() const;
		void resetStdin() const;

		// redirect standard output
		bool setStdoutRedir(const std::string &fname,
												const std::string &mode,
												int &stdoutFd,
												fpos_t &stdoutPos) const;
		void resetStdout(const int &stdoutFd,
										 const fpos_t &stdoutPos) const;

		// user interface printing
		void refresh();
		void autoComplete();
		int getTermCol() const;
		std::vector<std::string> getDirCts(const std::string &dirStr) const;
		std::vector<std::string> parse(const std::string &cmdStr) const;
		size_t cmnPrefix(const std::string &s1,
										 const std::string &s2) const;

		std::string prompt_;	 // command line prompt
		char comment_;				 // comment character
		Color color_;					 // prompt color
		std::string errorStr_; // error related string
		Error error_;					 // error type
		bool exit_;						 // exit flag

		CatMap catMap_;										// category map
		VarMap varMap_;										// variable map
		CmdMap cmdMap_;										// command map
		std::vector<std::string> cmdHis_; // command history
		size_t cmdHisPtr_;								// command history pointer
		std::string cmdStr_;							// user input command string
		size_t csrPos_;										// cursor position
		size_t maxPos_;										// maximum command string position
	};

	// Cmd inline methods
	inline Cmd::Cmd(const std::string &name)
	{
		optMgr_.setName(name);
	}

	inline Cmd::~Cmd() {}

	inline std::string Cmd::getName() const
	{
		return optMgr_.getName();
	}
	// class CmdMgr
	inline CmdMgr::CmdMgr()
	{
		prompt_ = "cmd> ";
		comment_ = '#';
		color_ = WHITE;
		error_ = E_EXIST;
		exit_ = false;
	}

	inline CmdMgr::~CmdMgr() {}

	inline void CmdMgr::setComment(const char &ch)
	{
		comment_ = ch;
	}

	inline void CmdMgr::setPrompt(const std::string &s)
	{
		prompt_ = s;
	}

	inline void CmdMgr::setColor(const Color &color)
	{
		color_ = color;
	}

	inline void CmdMgr::setExit(const bool &exit)
	{
		exit_ = exit;
	}

	inline bool CmdMgr::addVar(const std::string &var, const std::string &str)
	{
		if (!isLegalVar(var))
		{
			errorStr_ = var;
			error_ = E_INVALID_VAR;
			return false;
		}
		varMap_[var] = expandVar(str);
		return true;
	}

	inline bool CmdMgr::isLegalVar(const std::string &var) const
	{
		for (size_t i = 0; i < var.size(); ++i)
		{
			if (var[i] >= ASCII_0 && var[i] <= ASCII_9)
			{
				continue;
			}
			if (var[i] >= ASCII_A && var[i] <= ASCII_Z)
			{
				continue;
			}
			if (var[i] >= ASCII_a && var[i] <= ASCII_z)
			{
				continue;
			}
			if (var[i] == ASCII_UNDER)
			{
				continue;
			}
			return false;
		}
		return true;
	}

	inline bool CmdMgr::isLegalVarChar(const char &ch) const
	{
		if (ch >= ASCII_0 && ch <= ASCII_9)
		{
			return true;
		}
		if (ch >= ASCII_A && ch <= ASCII_Z)
		{
			return true;
		}
		if (ch >= ASCII_a && ch <= ASCII_z)
		{
			return true;
		}
		if (ch == ASCII_UNDER)
		{
			return true;
		}
		return false;
	}

	inline VarMap CmdMgr::getVars() const
	{
		return varMap_;
	}

	inline CmdMgr::Error CmdMgr::getError() const
	{
		return error_;
	}

	inline std::string CmdMgr::getErrorStr() const
	{
		return errorStr_;
	}

};

#endif