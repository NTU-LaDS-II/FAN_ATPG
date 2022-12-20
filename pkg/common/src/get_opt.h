// **************************************************************************
// File       [ get_opt.h ]
// Author     [ littleshamoo ]
// Synopsis   [ parse options and arguments ]
// Date       [ Ver 1.0 started 2010/03/30 ]
// **************************************************************************

#ifndef _COMMON_GET_OPT_H_
#define _COMMON_GET_OPT_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace CommonNs
{

	typedef std::set<std::string> FlagSet;

	struct Arg
	{
		enum Type
		{
			REQ = 0,
			OPT,
			REQ_INF,
			OPT_INF
		};

		Arg(const Type &type,
				const std::string &des,
				const std::string &meta);
		~Arg();

		Type type_;
		std::string des_;
		std::string meta_;
	};

	struct Opt
	{
		enum Type
		{
			BOOL = 0,
			STR_REQ,
			STR_OPT
		};

		Opt(const Type &type,
				const std::string &des,
				const std::string &meta);
		~Opt();

		void addFlag(const std::string &f);

		Type type_;
		std::string des_;
		std::string meta_;
		FlagSet flags_;
	};

	class OptMgr
	{
	public:
		enum Error
		{
			E_EXIST = 0,
			E_NOT_REG,
			E_REQ
		};

		OptMgr();
		~OptMgr();

		std::string getName() const;
		void setName(const std::string &name);
		void setKeepFirstArg(const bool &keep);
		void setShortDes(const std::string &shortDes);
		void setDes(const std::string &des);
		Error getError() const;

		size_t getNParsedArg() const;
		std::string getParsedArg(const size_t &i) const;
		size_t getNFlag() const;
		std::string getFlag(const size_t &i) const;
		bool isFlagSet(const std::string &f) const;
		std::string getFlagVar(const std::string &f) const;

		bool regArg(Arg *const arg);
		bool regOpt(Opt *const opt);
		bool parse(int argc, char **argv);
		bool parse(std::vector<std::string> args);

		void usage(std::ostream &out = std::cout);

	private:
		std::vector<std::string> splitString(const std::string &input,
																				 const size_t &lineWidth,
																				 const bool &fitLine = true,
																				 const std::string &sep = "- ");
		std::string fitLine(const std::string &input,
												const size_t &lineWidth);
		bool setOpt(Opt *const opt,
								const bool &isLong,
								const bool &isConnected,
								size_t &i,
								std::vector<std::string> &args);

		static const size_t wsCol_ = 78;
		static const size_t tabSize_ = 8;

		bool keepFirstArg_;
		std::string name_;
		std::string shortDes_;
		std::string des_;
		Error error_;

		std::vector<std::string> flags_;
		std::vector<Arg *> args_;
		std::map<std::string, Opt *> flagToOpt_;
		std::map<Opt *, bool> optToSet_;
		std::map<Opt *, std::string> optToVar_;
		std::vector<std::string> parsedArgs_;
	};

	// inline methods
	inline Arg::Arg(const Type &type,
									const std::string &des,
									const std::string &meta)
	{
		type_ = type;
		des_ = des;
		meta_ = meta;
	}

	inline Arg::~Arg() {}

	inline Opt::Opt(const Type &type,
									const std::string &des,
									const std::string &meta)
	{
		type_ = type;
		des_ = des;
		meta_ = meta;
	}

	inline Opt::~Opt() {}

	inline void Opt::addFlag(const std::string &f)
	{
		flags_.insert(f.substr(0, f.find_first_of(' ')));
	}

	inline OptMgr::OptMgr()
	{
		keepFirstArg_ = false;
		name_ = "PROG";
		des_ = "NONE";
		shortDes_ = "NONE";
		error_ = E_EXIST;
	}

	inline OptMgr::~OptMgr() {}

	inline std::string OptMgr::getName() const
	{
		return name_;
	}

	inline void OptMgr::setName(const std::string &name)
	{
		name_ = name;
	}

	inline void OptMgr::setKeepFirstArg(const bool &keep)
	{
		keepFirstArg_ = keep;
	}

	inline void OptMgr::setShortDes(const std::string &shortDes)
	{
		shortDes_ = shortDes;
	}

	inline void OptMgr::setDes(const std::string &des)
	{
		des_ = des;
	}

	inline OptMgr::Error OptMgr::getError() const
	{
		return error_;
	}

	inline size_t OptMgr::getNParsedArg() const
	{
		return parsedArgs_.size();
	}

	inline std::string OptMgr::getParsedArg(const size_t &i) const
	{
		return parsedArgs_[i];
	}

	inline size_t OptMgr::getNFlag() const
	{
		return flags_.size();
	}

	inline std::string OptMgr::getFlag(const size_t &i) const
	{
		return flags_[i];
	}

	inline bool OptMgr::isFlagSet(const std::string &f) const
	{
		std::map<std::string, Opt *>::const_iterator fIt = flagToOpt_.find(f);
		if (fIt == flagToOpt_.end())
		{
			return false;
		}
		std::map<Opt *, bool>::const_iterator optIt;
		optIt = optToSet_.find(fIt->second);
		if (optIt == optToSet_.end())
		{
			return false;
		}
		else
		{
			return optIt->second;
		}
	}

	inline std::string OptMgr::getFlagVar(const std::string &f) const
	{
		std::map<std::string, Opt *>::const_iterator fIt = flagToOpt_.find(f);
		if (fIt == flagToOpt_.end())
		{
			return "";
		}
		std::map<Opt *, std::string>::const_iterator optIt;
		optIt = optToVar_.find(fIt->second);
		if (optIt == optToVar_.end())
		{
			return "";
		}
		else
		{
			return optIt->second;
		}
	}
};

#endif