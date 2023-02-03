// **************************************************************************
// File       [ setup_cmd.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/08/30 created ]
// **************************************************************************

#ifndef _FAN_SETUP_CMD_H_
#define _FAN_SETUP_CMD_H_

#include "common/cmd.h"

#include "fan_mgr.h"

namespace FanNs {

// SETUP commands
class ReadLibCmd : public CommonNs::Cmd {
public:
         ReadLibCmd(const std::string &name, FanMgr *fanMgr);
         ~ReadLibCmd();

    bool exec(const std::vector<std::string> &argv);

private:
    FanMgr *fanMgr_;
};

class ReadNlCmd : public CommonNs::Cmd {
public:
         ReadNlCmd(const std::string &name, FanMgr *fanMgr);
         ~ReadNlCmd();

    bool exec(const std::vector<std::string> &argv);

private:
    FanMgr *fanMgr_;
};

class SetFaultTypeCmd : public CommonNs::Cmd {
public:
         SetFaultTypeCmd(const std::string &name, FanMgr *fanMgr);
         ~SetFaultTypeCmd();

    bool exec(const std::vector<std::string> &argv);

private:
    FanMgr *fanMgr_;
};

class ReportNetlistCmd : public CommonNs::Cmd {
public:
         ReportNetlistCmd(const std::string &name, FanMgr *fanMgr);
         ~ReportNetlistCmd();

    bool exec(const std::vector<std::string> &argv);

private:
    FanMgr *fanMgr_;
};

class ReportCellCmd : public CommonNs::Cmd {
public:
         ReportCellCmd(const std::string &name, FanMgr *fanMgr);
         ~ReportCellCmd();

    bool exec(const std::vector<std::string> &argv);

private:
    FanMgr *fanMgr_;
    void   reportCell(const size_t &i) const;
};

class ReportLibCmd : public CommonNs::Cmd {
public:
         ReportLibCmd(const std::string &name, FanMgr *fanMgr);
         ~ReportLibCmd();

    bool exec(const std::vector<std::string> &argv);

private:
    FanMgr *fanMgr_;
};

class BuildCircuitCmd : public CommonNs::Cmd {
public:
         BuildCircuitCmd(const std::string &name, FanMgr *fanMgr);
         ~BuildCircuitCmd();

    bool exec(const std::vector<std::string> &argv);

private:
    FanMgr *fanMgr_;
};

//Ne
class SetPatternTypeCmd : public CommonNs::Cmd {
public:
         SetPatternTypeCmd(const std::string &name, FanMgr *fanMgr);
         ~SetPatternTypeCmd();

    bool exec(const std::vector<std::string> &argv);

private:
    FanMgr *fanMgr_;
};

class SetStaticCompressionCmd : public CommonNs::Cmd {
public:
         SetStaticCompressionCmd(const std::string &name, FanMgr *fanMgr);
         ~SetStaticCompressionCmd();

    bool exec(const std::vector<std::string> &argv);

private:
    FanMgr *fanMgr_;
};

class SetDynamicCompressionCmd : public CommonNs::Cmd {
public:
         SetDynamicCompressionCmd(const std::string &name, FanMgr *fanMgr);
         ~SetDynamicCompressionCmd();

    bool exec(const std::vector<std::string> &argv);

private:
    FanMgr *fanMgr_;
};

class SetXFillCmd : public CommonNs::Cmd {
public:
         SetXFillCmd(const std::string &name, FanMgr *fanMgr);
         ~SetXFillCmd();

    bool exec(const std::vector<std::string> &argv);

private:
    FanMgr *fanMgr_;
};
//Ne



};

#endif