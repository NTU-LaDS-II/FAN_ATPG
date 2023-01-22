// **************************************************************************
// File       [ misc_cmd.h ]
// Author     [ littleshamoo ]
// Synopsis   [ ]
// Date       [ 2011/09/28 created ]
// **************************************************************************

#ifndef _FAN_MISC_CMD_H_
#define _FAN_MISC_CMD_H_

#include "common/cmd.h"

namespace FanNs {

// MISC commands
class ReportPatFormatCmd : public CommonNs::Cmd {
public:
         ReportPatFormatCmd(const std::string &name);
         ~ReportPatFormatCmd();

    bool exec(const std::vector<std::string> &argv);

};

class ReportMemUsgCmd : public CommonNs::Cmd {
public:
         ReportMemUsgCmd(const std::string &name);
         ~ReportMemUsgCmd();

    bool exec(const std::vector<std::string> &argv);

};

};

#endif