// **************************************************************************
// File       [ get_opt.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ parse options and arguments ]
// Date       [ Ver 1.0 started 2010/03/30 ]
// **************************************************************************

#include <sys/ioctl.h>

#include "get_opt.h"

using namespace CommonNs;

bool OptMgr::regArg(Arg *const arg)
{
	args_.push_back(arg);
	return true;
}

bool OptMgr::regOpt(Opt *const opt)
{
	if (opt->flags_.size() == 0)
	{
		std::cerr << "**ERROR OptMgr::regOpt(): No option flags found\n";
		error_ = E_REQ;
		return false;
	}

	// checking for existing flags
	std::set<std::string>::const_iterator sIt = opt->flags_.begin();
	for (; sIt != opt->flags_.end(); ++sIt)
	{
		if (flagToOpt_.count(*sIt) > 0)
		{
			std::cerr << "**ERROR OptMgr::regOpt(): The flag `" << *sIt << "' ";
			std::cerr << "has already been registered\n";
			error_ = E_EXIST;
			return false;
		}
	}

	// add flags
	sIt = opt->flags_.begin();
	for (; sIt != opt->flags_.end(); ++sIt)
	{
		flags_.push_back(*sIt);
		flagToOpt_[*sIt] = opt;
	}
	optToSet_[opt] = false;
	optToVar_[opt] = "";

	return true;
}

bool OptMgr::parse(int argc, char **argv)
{
	std::vector<std::string> args;
	for (int i = 0; i < argc; ++i)
	{
		args.push_back(std::string(argv[i]));
	}
	return parse(args);
}

bool OptMgr::parse(std::vector<std::string> args)
{
	// clear parsing infomation
	parsedArgs_.clear();
	std::map<Opt *, bool>::iterator bIt = optToSet_.begin();
	for (; bIt != optToSet_.end(); ++bIt)
	{
		bIt->second = false;
	}
	std::map<Opt *, std::string>::iterator sIt = optToVar_.begin();
	for (; sIt != optToVar_.end(); ++sIt)
	{
		sIt->second = "";
	}

	// keep first arg if needed
	if (keepFirstArg_ && args.size() >= 1)
	{
		parsedArgs_.push_back(args[0]);
	}

	for (size_t i = 1; i < args.size(); ++i)
	{
		std::string arg = args[i];
		std::map<std::string, Opt *>::iterator iter;
		bool isLong = false;
		bool isConnected = false;
		if (arg.size() >= 2 && arg[0] == '-')
		{
			if (arg[1] != '-')
			{ // one dash '-'
				iter = flagToOpt_.find(arg.substr(1, 1));
				isLong = false;
				if (arg.size() >= 3)
				{
					isConnected = true;
					if (iter != flagToOpt_.end())
					{
						Opt::Type type = iter->second->type_;
						if (type == Opt::BOOL)
						{
							std::string nextArg = "-" + arg.substr(2);
							args.insert(args.begin() + i + 1, nextArg);
							arg = arg.substr(0, 2);
							args[i] = arg;
						}
					}
				}
			}
			else
			{ // two dashes '--'
				size_t pos = arg.find_first_of('=');
				if (pos != arg.npos)
				{
					isConnected = true;
				}
				iter = flagToOpt_.find(arg.substr(2, pos - 2));
				isLong = true;
			}
			if (iter != flagToOpt_.end())
			{
				Opt *opt = iter->second;
				if (!setOpt(opt, isLong, isConnected, i, args))
				{
					return false;
				}
			}
			else
			{ // not in option map
				std::cerr << "**ERROR: option `" << arg;
				std::cerr << "' not registered\n";
				error_ = E_NOT_REG;
				return false;
			}
		}
		else // not an option, an argument
		{
			parsedArgs_.push_back(arg);
		}
	} // iteration over all arguments

	return true;
}

bool OptMgr::setOpt(Opt *const opt, const bool &isLong,
										const bool &isConnected, size_t &i,
										std::vector<std::string> &args)
{
	std::string arg = args[i];
	switch (opt->type_)
	{
		case Opt::BOOL:
			break;
		case Opt::STR_REQ:
			if (!isConnected)
			{
				++i;
				if (i >= args.size())
				{
					std::cerr << "**ERROR: option `" << arg;
					std::cerr << "' requires a value\n";
					error_ = E_REQ;
					return false;
				}
				optToVar_[opt] = args[i];
			}
			else
			{
				std::string value = "";
				if (isLong)
				{
					value = arg.substr(arg.find_first_of('='));
					if (value.size() == 1)
					{
						std::cerr << "**ERROR: option `" << arg;
						std::cerr << "' requires a value\n";
						error_ = E_REQ;
						return false;
					}
					value = value.substr(1);
				}
				else
				{
					value = arg.substr(2);
				}
				optToVar_[opt] = value;
			}
			break;
		case Opt::STR_OPT:
			if (!isConnected)
			{
				++i;
				if (i < args.size() && args[i][0] != '-')
				{
					optToVar_[opt] = args[i];
				}
				else
				{
					--i;
				}
			}
			else
			{
				std::string value = "";
				if (isLong)
				{
					value = arg.substr(arg.find_first_of('='));
					if (value.size() > 1)
					{
						optToVar_[opt] = value.substr(1);
					}
				}
				else
				{
					optToVar_[opt] = arg.substr(2);
				}
			}
			break;
		default:
			break;
	}
	optToSet_[opt] = true;

	return true;
}

// **************************************************************************
// Function   [ OptMgr::usage(ostream &) ]
// Author     [ littleshamoo ]
// Synopsis   [ print the usage of the program in the following format
//              (similar to the MAN page in linux system)
//
//              NAME
//                      <program name> - <program short description>
//
//              SYNOPSIS
//                      <program name> [options] <arguments>
//
//              DESCRIPTION
//                      <program description>
//
//              ARGUMENTS
//                      <arg1>  <arg1 description>
//                      <arg2>  <arg2 description>
//                      <arg3>  <arg3 description>
//                      ...
//
//              OPTIONS
//                      <opt1> <opt1 description>
//                      <opt2> <opt2 description>
//                      <opt3> <opt3 description>
//                      ...
//            ]
// **************************************************************************
void OptMgr::usage(std::ostream &out)
{
	size_t wsCol = wsCol_;
	size_t tabSize = tabSize_;

	// determine window size
	struct winsize ws;
	wsCol = ioctl(0, TIOCGWINSZ, &ws) == 0 ? ws.ws_col - 2 : wsCol;

	// minimum line width is three tabs
	wsCol = wsCol < 3 * tabSize ? 3 * tabSize : wsCol;

	out << "\n";

	// print NAME section
	out << "NAME\n";
	std::string nameSec = name_ + " - " + shortDes_;
	std::vector<std::string> nameSecVec = splitString(nameSec, wsCol - tabSize);
	for (size_t i = 0; i < nameSecVec.size(); ++i)
	{
		out << std::string(tabSize, ' ') << nameSecVec[i] << "\n";
	}
	out << "\n"; // NAME section end
	// print SYNOPSIS section
	out << "SYNOPSIS\n";
	std::string synSec = name_ + " ";
	std::string singleCharSyn = "";
	std::string multiCharSyn = "";
	std::map<Opt *, std::string>::iterator optIt = optToVar_.begin();
	for (; optIt != optToVar_.end(); ++optIt)
	{
		Opt *opt = optIt->first;
		std::set<std::string>::iterator fIt = opt->flags_.begin();
		for (; fIt != opt->flags_.end(); ++fIt)
		{
			std::string flag = *fIt;
			if (flag.size() == 1 && opt->meta_.size() == 0)
			{
				singleCharSyn += flag;
			}
			else
			{
				multiCharSyn += " ";
				if (flag.size() == 1)
				{
					multiCharSyn += "[-";
				}
				else
				{
					multiCharSyn += "[--";
				}
				multiCharSyn += flag;
				if (opt->meta_.size() == 0)
				{
					multiCharSyn += "]";
				}
				else
				{
					if (opt->type_ == Opt::STR_OPT)
					{
						multiCharSyn += " [" + opt->meta_ + "]]";
					}
					else
					{
						multiCharSyn += " " + opt->meta_ + "]";
					}
				}
			}
		}
	}
	if (singleCharSyn.size() != 0)
	{
		singleCharSyn = "[-" + singleCharSyn + "]";
	}
	synSec += singleCharSyn + multiCharSyn;
	std::string argMetaSyn = "";
	for (size_t i = 0; i < args_.size(); ++i)
	{
		Arg *arg = args_[i];
		if (arg->meta_.size() == 0)
		{
			continue;
		}
		argMetaSyn += " ";
		if (arg->type_ == Arg::REQ)
		{
			argMetaSyn += arg->meta_;
		}
		else if (arg->type_ == Arg::OPT)
		{
			argMetaSyn += "[" + arg->meta_ + "]";
		}
		else if (arg->type_ == Arg::REQ_INF)
		{
			argMetaSyn += arg->meta_ + "...";
		}
		else if (arg->type_ == Arg::OPT_INF)
		{
			argMetaSyn += "[" + arg->meta_ + "]...";
		}
	}
	synSec += argMetaSyn;
	std::vector<std::string> synSecVec = splitString(synSec, wsCol - tabSize, true,
																									 " ");
	for (size_t i = 0; i < synSecVec.size(); ++i)
	{
		out << std::string(tabSize, ' ') << synSecVec[i] << "\n";
	}
	out << "\n"; // end SYNOPSIS section
	// print DESCRIPTION section
	out << "DESCRIPTION\n";
	std::vector<std::string> desSecVec = splitString(des_, wsCol - tabSize);
	for (size_t i = 0; i < desSecVec.size(); ++i)
	{
		out << std::string(tabSize, ' ') << desSecVec[i] << "\n";
	}
	out << "\n"; // end DESCRIPTION section
	// print ARGUMENTS section
	if (args_.size() != 0)
	{
		out << "ARGUMENTS\n";
	}
	for (size_t i = 0; i < args_.size(); ++i)
	{
		Arg *arg = args_[i];
		out << std::string(tabSize, ' ') << arg->meta_;
		if (arg->meta_.size() + 2 >= tabSize)
		{
			out << "\n";
		}
		else
		{
			for (size_t j = arg->meta_.size(); j < tabSize; ++j)
			{
				out << " ";
			}
		}
		std::vector<std::string> argDesVec = splitString(arg->des_,
																										 wsCol - 2 * tabSize);
		for (size_t i = 0; i < argDesVec.size(); ++i)
		{
			if (i != 0 || arg->meta_.size() + 2 >= tabSize)
			{
				out << std::string(2 * tabSize, ' ');
			}
			out << argDesVec[i] << "\n";
		}
		out << "\n";
	}
	// end ARGUMENTS section
	// print OPTIONS section
	if (optToVar_.size() != 0)
	{
		out << "OPTIONS\n";
	}

	for (optIt = optToVar_.begin(); optIt != optToVar_.end(); ++optIt)
	{
		Opt *opt = optIt->first;
		std::set<std::string>::iterator fIt = opt->flags_.begin();
		std::string flagStr = "";
		for (size_t i = 0; fIt != opt->flags_.end(); ++fIt, ++i)
		{
			if ((*fIt).size() == 1)
			{
				flagStr += "-" + (*fIt);
			}
			else
			{
				flagStr += "--" + (*fIt);
			}

			if (i + 1 != opt->flags_.size())
			{
				flagStr += ",";
			}
		}

		if (opt->meta_.size() != 0)
		{
			flagStr += "  " + opt->meta_;
		}
		out << std::string(tabSize, ' ') << flagStr;

		if (flagStr.size() + 2 >= tabSize)
		{
			out << "\n";
		}
		else
		{
			for (size_t i = flagStr.size(); i < tabSize; ++i)
			{
				out << " ";
			}
		}
		std::vector<std::string> optDesVec = splitString(opt->des_,
																										 wsCol - 2 * tabSize);
		for (size_t i = 0; i < optDesVec.size(); ++i)
		{
			if (i != 0 || flagStr.size() + 2 >= tabSize)
			{
				out << std::string(2 * tabSize, ' ');
			}
			out << optDesVec[i] << "\n";
		}
		out << "\n";
	}
	// end OPTIONS section
}

// **************************************************************************
// Function   [ vector<string> OptMgr::splitString(string, size_t, bool,
//                                                 string)
//            ]
// Author     [ littleshamoo ]
// Synopsis   [ split the string at "sep" characters into lineWidth length.
//              If fitLine is set, the string will be stretched by adding
//              additional white spaces to fit the lineWidth.
//            ]
// Date       [ 2010/12/07 ]
// **************************************************************************
std::vector<std::string> OptMgr::splitString(const std::string &input,
																						 const size_t &lineWidth,
																						 const bool &fitLine,
																						 const std::string &sep)
{
	std::vector<std::string> splitStr;
	if (lineWidth < 1)
	{
		splitStr.push_back(input);
		return splitStr;
	}

	size_t startPos = 0;
	size_t inputPos = lineWidth;
	while (inputPos < input.size())
	{
		std::string lineSeg = input.substr(startPos, lineWidth);
		if (input[inputPos] == ' ')
		{
			if (fitLine)
			{
				splitStr.push_back(this->fitLine(lineSeg, lineWidth));
			}
			else
			{
				splitStr.push_back(lineSeg);
			}
			startPos = inputPos + 1;
			inputPos += lineWidth + 1;
		}
		else
		{
			size_t cutPos = lineSeg.find_last_of(sep);
			if (cutPos == lineSeg.npos)
			{
				splitStr.push_back(input.substr(startPos, lineWidth));
			}
			else
			{
				++cutPos;
				lineSeg = input.substr(startPos, cutPos);
				if (fitLine)
				{
					splitStr.push_back(this->fitLine(lineSeg, lineWidth));
				}
				else
				{
					splitStr.push_back(lineSeg);
				}
				inputPos = startPos + cutPos;
			}
			startPos = inputPos;
			inputPos += lineWidth;
		}
	}
	splitStr.push_back(input.substr(startPos));
	return splitStr;
}

// **************************************************************************
// Function   [ string OptMgr::fitLine(string, size_t) ]
// Author     [ littleshamoo ]
// Synopsis   [ stretch the input string by adding addtional white spaces
//              to fit lineWidth
//            ]
// Date       [ 2010/12/08 ]
// **************************************************************************
std::string OptMgr::fitLine(const std::string &input, const size_t &lineWidth)
{
	// trim leading white spaces
	size_t leadWht = 0;
	for (; leadWht < input.size() && input[leadWht] == ' '; ++leadWht)
		;
	std::string output = input.substr(leadWht);
	// trim trailing white spaces
	size_t trailWht = output.size();
	for (; trailWht > 0 && output[trailWht - 1] == ' '; --trailWht)
		;
	output = output.substr(0, trailWht);

	size_t nWht = 0;
	for (size_t i = 0; i < output.size(); ++i)
	{
		if (output[i] == ' ')
		{
			++nWht;
		}
	}

	if (lineWidth <= output.size())
	{
		return output;
	}

	size_t nFillWht = lineWidth - output.size();
	size_t nAddPerWht = nFillWht / nWht;
	size_t nLeftWht = nFillWht % nWht;
	size_t whtNo = 0;
	for (size_t i = 0; i < output.size(); ++i)
	{
		if (output[i] == ' ')
		{
			size_t nAddWht = nAddPerWht;
			nAddWht += whtNo < nLeftWht ? 1 : 0;
			std::string extraWht = "";
			for (size_t j = 0; j < nAddWht; ++j)
			{
				extraWht += " ";
			}
			output = output.substr(0, i) + extraWht + output.substr(i);
			i += nAddWht;
			++whtNo;
		}
	}
	return output;
}