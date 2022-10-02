// **************************************************************************
// File       [ cmd.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ methods for command manager ]
// Date       [ Ver. 2.0 started 2010/04/09 ]
// **************************************************************************

#include <sys/ioctl.h>  // getting window size
#include <sys/stat.h>   // directory testing
#include <sys/types.h>  // file redirection
#include <pwd.h>        // user home path
#include <unistd.h>     // current user ID
#include <termios.h>    // setting stdin
#include <dirent.h>     // getting directory contents
#include <cstdlib>      // getting environment variable
#include <cstring>
#include <iomanip>

#include "ascii.h"
#include "vt100.h"
#include "cmd.h"

using namespace std;
using namespace CommonNs;

string CmdMgr::expandVar(const string &str) const {
    string exp = str;
    for (size_t i = 0; i < exp.size(); ++i) {
        if (exp[i] == ASCII_DOLLR) {
            size_t j = i + 1;
            if (j < exp.size() && exp[j] == ASCII_L_SCO)
                j++;
            string var = "";
            while (isLegalVarChar(exp[j])) {
                var += exp[j];
                j++;
            }
            map<string, string>::const_iterator it = varMap_.find(var);
            string rep = it == varMap_.end() ? "" : it->second;
            if (rep.size() == 0)
                if (getenv(var.c_str()))
                    rep = getenv(var.c_str());
            if (j < exp.size() && exp[j] == ASCII_R_SCO)
                j++;
            exp.replace(i, j - i, rep);
            i += rep.size();
        }
    }
    return exp;
}

string CmdMgr::expandHome(const string &path) const {
    string exp = path;
    for (size_t i = 0; i < exp.size(); ++i) {
        if (exp[i] == '~') {
            size_t j = i + 1;
            while (j < exp.size() && isLegalVarChar(exp[j]))
                j++;
            string uname = exp.substr(i, j - i);
            struct passwd *profile = NULL;
            if (uname.size() == 1)
                profile = getpwuid(getuid());
            else
                profile = getpwnam(uname.substr(1).c_str());
            if (profile) {
                exp.replace(i, j - i, profile->pw_dir);
                i += strlen(profile->pw_dir);
            }
            else
                i = j;
        }
    }
    return exp;
}

int CmdMgr::getTermCol() const {
    int fd = fileno(stdout);
    winsize ts;
    ioctl(fd, TIOCGWINSZ, &ts);
    return ts.ws_col;
}

size_t CmdMgr::cmnPrefix(const string &s1, const string &s2) const {
    size_t i = 0;
    for ( ; i < s1.size() && i < s2.size() && s1[i] == s2[i]; ++i)
        ;
    return i;
}

Cmd *CmdMgr::getCmd(const string &name) const {
    if (cmdMap_.find(name) != cmdMap_.end())
        return cmdMap_.find(name)->second;
    else
        return NULL;
}

void CmdMgr::usage(ostream &out) {
    for (CatMapIter cIt = catMap_.begin(); cIt != catMap_.end(); ++cIt) {
        out << cIt->first << " commands" << endl;
        size_t maxLen = 0;
        set<string>::iterator setIt = cIt->second.begin();
        for ( ; setIt != cIt->second.end(); ++setIt)
            if (maxLen < (*setIt).size())
                maxLen = (*setIt).size();
        setIt = cIt->second.begin();
        size_t cmdsPerLine = (size_t)getTermCol() / (maxLen + 2);
        size_t i = 0;
        for ( ; setIt != cIt->second.end(); ++setIt, ++i) {
            out << setw(maxLen + 2) << left << (*setIt);
            if ((i + 1) % cmdsPerLine == 0)
                out << endl;
        }
        if (cIt->second.size() % cmdsPerLine != 0)
            out << endl;
        out << endl;
    }
}


// **************************************************************************
// Function   [ void CmdMgr::regCmd(string, Cmd *) ]
// Author     [ littleshamoo ]
// Synopsis   [ registers commands to command manager ]
// **************************************************************************
void CmdMgr::regCmd(const string &cat, Cmd * const cmd) {
    string name = cmd->getName();
    if (cmdMap_.find(name) == cmdMap_.end()) {
        cmdMap_[name] = cmd;
        if (catMap_.find(cat) == catMap_.end()) { // new category
            set<string> cmdStrSet;
            cmdStrSet.insert(name);
            catMap_[cat] = cmdStrSet;
        }
        else // category already exist
            catMap_.find(cat)->second.insert(name);
    }
    else {
        cerr << "**ERROR CmdMgr::regCmd(): The command `" << name;
        cerr << "' has already been registered" << endl;
        error_ = E_EXIST;
    }
}


// **************************************************************************
// Function   [ void CmdMgr::setStdin() const ]
// Author     [ littleshamoo ]
// Synopsis   [ sets the stdin to non-canonical and non-echo ]
// **************************************************************************
void CmdMgr::setStdin() const {
    int fd = fileno(stdin);
    termios tcflags;
    tcgetattr(fd, &tcflags);
    tcflags.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(fd, TCSANOW, &tcflags);
}


// **************************************************************************
// Function   [ void CmdMgr::resetStdin() const ]
// Author     [ littleshamoo ]
// Synopsis   [ sets the stdin back to canonical and echo ]
// **************************************************************************
void CmdMgr::resetStdin() const {
    int fd = fileno(stdin);
    termios tcflags;
    tcgetattr(fd, &tcflags);
    tcflags.c_lflag |= ICANON | ECHO;
    tcsetattr(fd, TCSANOW, &tcflags);
}


// **************************************************************************
// Function   [ vector<string> CmdMgr::getDirCts(string) const ]
// Author     [ littleshamoo ]
// Synopsis   [ return contents in the directory ]
// **************************************************************************
vector<string> CmdMgr::getDirCts(const string &dirStr) const {
    vector<string> cts;
    DIR            *dir;
    dirent         *dirCts;
    if ((dir = opendir(dirStr.c_str())) != NULL) {
        while ((dirCts = readdir(dir)) != NULL) {
            string item = string(dirCts->d_name);
            if (item != "." && item != "..")
                cts.push_back(string(dirCts->d_name));
        }
        closedir(dir);
    }
    return cts;
}


// **************************************************************************
// Function   [ bool CmdMgr::read() ]
// Author     [ littleshamoo ]
// Synopsis   [ reads command from stdin and sets result ]
// **************************************************************************
//{{{ bool CmdMgr::read()
CmdMgr::Result CmdMgr::read() {
    setStdin();

    // initialize VT100 and save cursor position
    cout << VT100_ARM_ON << VT100_CSRS << flush;

    // perform actions on every keystroke
    int    ch         = -1;
           cmdStr_    = "";
           csrPos_    = 0;
           maxPos_    = prompt_.size();
           cmdHisPtr_ = cmdHis_.size();
    string cmdStrBak  = "";
    size_t csrPosBak  = 0;
    refresh();
    while (ch != ASCII_LF) {
        ch = getchar();
        //{{{ printable char
        if (ch >= ASCII_MIN_PR && ch <= ASCII_MAX_PR) {
            cmdStr_.insert(cmdStr_.begin() + csrPos_, ch);
            csrPos_++;
            refresh();
        } //}}}
        //{{{ nonprintable char
        else if (ch < ASCII_MIN_PR || ch > ASCII_MAX_PR) {
            switch(ch) {
                // ******************************************************
                // Special key combinations:
                //   ARROW_UP:    "\033[A"
                //   ARROW_DOWN:  "\033[B"
                //   ARROW_RIGHT: "\033[C"
                //   ARROW_LEFT:  "\033[D"
                //   HOME:        "\033[1" or "\033OH"
                //   INSERT:      "\033[2"
                //   DELETE:      "\033[3"
                //   END:         "\033[4" or "\033OF"
                //   PGUP:        "\033[5"
                //   PGDN:        "\033[6"
                // ******************************************************
                case ASCII_DEL: // {{{
                    if (csrPos_ > 0) {
                        csrPos_--;
                        cmdStr_.erase(cmdStr_.begin() + csrPos_);
                        refresh();
                    }
                    break;
                //}}}
                case ASCII_BS: // {{{
                    if (csrPos_ > 0) {
                        csrPos_--;
                        cmdStr_.erase(cmdStr_.begin() + csrPos_);
                        refresh();
                    }
                    break;
                //}}}
                case ASCII_HT: // {{{
                    autoCmplt();
                    refresh();
                    break;
                //}}}
                case ASCII_ESC: // {{{
                    int ch1, ch2;
                    switch (ch1 = getchar()) {
                        case ASCII_L_SQU: // {{{
                            switch (ch2 = getchar()) {
                                //{{{ ARROWS
                                case ASCII_A: // UP
                                    if (cmdHisPtr_ == cmdHis_.size()) {
                                        cmdStrBak = cmdStr_;
                                        csrPosBak = csrPos_;
                                    }
                                    if (cmdHisPtr_ > 0) {
                                        cmdHisPtr_--;
                                        cmdStr_ = cmdHis_[cmdHisPtr_];
                                        csrPos_ = cmdHis_[cmdHisPtr_].size();
                                    }
                                    refresh();
                                    break;
                                case ASCII_B: // DOWN
                                    if (cmdHisPtr_  + 1 < cmdHis_.size()) {
                                        cmdHisPtr_++;
                                        cmdStr_ = cmdHis_[cmdHisPtr_];
                                        csrPos_ = cmdHis_[cmdHisPtr_].size();
                                    }
                                    else {
                                        cmdHisPtr_ = cmdHis_.size();
                                        cmdStr_ = cmdStrBak;
                                        csrPos_ = csrPosBak;
                                    }
                                    refresh();
                                    break;
                                case ASCII_C: // RIGHT
                                    if (csrPos_ < cmdStr_.size()) {
                                        csrPos_++;
                                        refresh();
                                    }
                                    break;
                                case ASCII_D: // LEFT
                                    if (csrPos_ > 0) {
                                        csrPos_--;
                                        refresh();
                                    }
                                    break;
                                //}}}
                                //{{{ HOME, END, INS, DEL, PGUP, PGDN
                                case ASCII_1: // HOME
                                    getchar(); // eat "~"
                                    csrPos_ = 0;
                                    refresh();
                                    break;
                                case ASCII_3: // DEL
                                    getchar(); // eat "~"
                                    if (csrPos_ < cmdStr_.size()) {
                                        cmdStr_.erase(cmdStr_.begin()
                                                      + csrPos_);
                                        refresh();
                                    }
                                    break;
                                case ASCII_4: // END
                                    getchar(); // eat "~"
                                    csrPos_ = cmdStr_.size();
                                    refresh();
                                    break;
                                case ASCII_2: // INS
                                case ASCII_5: // PGUP
                                case ASCII_6: // PGDN
                                    getchar(); // eat "~"
                                    break;
                                //}}}
                                default:
                                    #ifdef DEBUG_MODE
                                    cout << endl << ch1  << " ";
                                    cout << ch2 << endl;
                                    #endif
                                    break;
                              }
                              break;
                            //}}}
                        case ASCII_O: // {{{
                            // HOME and END
                            if ((ch2 = getchar()) == ASCII_H)
                                csrPos_ = 0;
                            else if (ch2 == ASCII_F)
                                csrPos_ = cmdStr_.size();
                            refresh();
                            break;
                        //}}}
                        default:
                            #ifdef DEBUG_MODE
                            cout << endl << ch1 << endl;
                            #endif
                            break;
                    }
                //}}}
                default:
                  break;
            }
        } //}}}
        if (ch != ASCII_LF) // mask out all other not supported keys
            ch = -1;
    }
    cout << endl;

    resetStdin();

    return exec(cmdStr_);
}
//}}}


// **************************************************************************
// Function   [ void CmdMgr::refresh() ]
// Author     [ littleshamoo ]
// Synopsis   [ refresh the screen ]
// **************************************************************************
//{{{ void CmdMgr::refresh()
void CmdMgr::refresh() {
    // scroll screen on boundary condition
    if (prompt_.size() + cmdStr_.size() > maxPos_) {
        size_t nLinePrev = maxPos_ / (size_t)getTermCol();
        maxPos_ = prompt_.size() + cmdStr_.size();
        size_t nLineCur  = maxPos_ / (size_t)getTermCol();
        for (size_t i = 0; i < nLineCur - nLinePrev; ++i)
            cout << VT100_SCRU;
    }

    // clear current text
    size_t nRow = maxPos_ / (size_t)getTermCol();
    cout << VT100_CSRR;
    for (size_t i = 0; i < nRow; ++i)
        cout << VT100_CSRU;
    cout << VT100_ERSD << flush;

    // reprint prompt and cmd string
    switch (color_) {
        case BLACK:
            cout << ANSI_BLACK;
            break;
        case RED:
            cout << ANSI_RED;
            break;
        case GREEN:
            cout << ANSI_GREEN;
            break;
        case YELLOW:
            cout << ANSI_YELLOW;
            break;
        case BLUE:
            cout << ANSI_BLUE;
            break;
        case PURPLE:
            cout << ANSI_PURPLE;
            break;
        case CYAN:
            cout << ANSI_CYAN;
            break;
        case WHITE:
            cout << ANSI_WHITE;
            break;
        default:
            cout << ANSI_WHITE;
            break;
    }
    cout << prompt_ << ANSI_RESET << cmdStr_;

    // move cursor back to cursor position
    size_t csrLen = prompt_.size() + csrPos_;
    size_t row    = nRow - csrLen / (size_t)getTermCol();
    size_t col    = csrLen % (size_t)getTermCol();
    cout << VT100_CSRR;
    for (size_t i = 0; i < row; ++i)
        cout << VT100_CSRU;
    for (size_t i = 0; i < col; ++i)
        cout << VT100_CSRF;

    cout << flush;
}
//}}}


// **************************************************************************
// Function   [ vector<string> CmdMgr::parse(string) ]
// Author     [ littleshamoo ]
// Synopsis   [ parse command string ]
// **************************************************************************
//{{{ vector<string> CmdMgr::parse(string)
vector<string> CmdMgr::parse(const string &cmdStr) const {
	char           ch;
	size_t         i      = 0;
	string         str    = "";
    bool           inQuot = false;
    vector<string> argv;
    while (i < cmdStr.size()) {
        ch = cmdStr[i];

        // discard those after comment
        if (ch == comment_) {
            if (str != "")
                argv.push_back(str);
            return argv;
        }

        // '"' characters inside quotation marks are treated as one arg
        if (ch == ASCII_QUOT)
            inQuot = inQuot ? false : true;

        // '>' and ">>" for redirection
        if (ch == ASCII_R_ANG && !inQuot) {
            if (str != "") {
                argv.push_back(str);
                str = "";
            }
            if (i + 1 < cmdStr.size() && cmdStr[i + 1] == ASCII_R_ANG) {
                argv.push_back(">>");
                i += 2;
            }
            else {
                argv.push_back(">");
                i++;
            }
            continue;
        }

        // delimiter handling
        if ((ch == ASCII_SPACE || ch == ASCII_HT || ch == ASCII_LF
             || ch == ASCII_QUOT) && !inQuot) {
            if (str != "") {
                argv.push_back(str);
                str = "";
            }
            i++;
            continue;
        }

        // append other character to string
        if (ch != ASCII_QUOT)
            str += ch;

        i++;
    }

    if (str != "")
        argv.push_back(str);

    return argv;
}
//}}}


// **************************************************************************
// Function   [ void CmdMgr::autoCmplt() ]
// Author     [ littleshamoo ]
// Synopsis   [ performs command and file auto completion and show possible
//              candidates ]
// **************************************************************************
//{{{ void CmdMgr::autoCmplt()
void CmdMgr::autoCmplt() {
    vector<string> argv = parse(cmdStr_.substr(0, csrPos_));

    bool isCmd  = argv.size() == 0
                  || (argv.size() == 1 && cmdStr_[csrPos_ - 1] != ' ');
    bool isOpt  = argv.size() > 1 && cmdStr_[csrPos_ - 1] != ' '
                  && argv[argv.size() - 1][0] == '-'
                  && cmdMap_.count(argv[0]);
    bool isVar  = argv.size() > 1 && cmdStr_[csrPos_ - 1] != ' '
                  && argv[argv.size() - 1][0] == '$'
                  && argv[argv.size() - 1].find('}') == string::npos
                  && argv[argv.size() - 1].find('/') == string::npos;
    bool isFile = false;

    vector<string> cddts;
    string         pre;
    bool           isLong;
    if (isCmd) {
        pre = argv.size() == 0 ? "" : argv[0];

        // find out commands candidates and their common prefix
        for (CmdMapIter it = cmdMap_.begin(); it != cmdMap_.end(); ++it) {
            string cmd = it->first;
            if (pre == "" || pre.compare(cmd.substr(0, pre.size())) == 0)
                cddts.push_back(cmd);
        }
    }
    else if (isOpt) {
        Cmd *cmd = cmdMap_.find(argv[0])->second;
        isLong = argv[argv.size() - 1].size() > 1
                 && argv[argv.size() - 1][1] == '-';
        if (isLong)
            pre = argv[argv.size() - 1].substr(2);
        else
            pre = argv[argv.size() - 1].substr(1);
        for (size_t i = 0; i < cmd->optMgr_.getNFlag(); ++i) {
            string flag = cmd->optMgr_.getFlag(i);
            if (pre == "" || pre.compare(flag.substr(0, pre.size())) == 0) {
                if (flag.size() == 1 && !isLong)
                    cddts.push_back(flag);
                else if (flag.size() > 1 && isLong)
                    cddts.push_back(flag);
            }
        }
    }
    else if (isVar) {
        isLong = argv[argv.size() - 1].size() > 1
                 && argv[argv.size() - 1][1] == '{';
        if (isLong)
            pre = argv[argv.size() - 1].substr(2);
        else
            pre = argv[argv.size() - 1].substr(1);
        for (VarMapIter it = varMap_.begin(); it != varMap_.end(); ++it) {
            string var = it->first;
            if (pre == "" || pre.compare(var.substr(0, pre.size())) == 0)
                cddts.push_back(var);
        }
    }
    else { // files or directories
        // find directory string and prefix
        string dirStr;
        if (cmdStr_[csrPos_ - 1] != ' ') {
            string path = argv[argv.size() - 1];
            if (path.find_last_of('/') == path.npos) {
                dirStr = "./";
                pre = path;
            }
            else {
                dirStr = path.substr(0, path.find_last_of('/'));
                dirStr = expandVar(dirStr);
                dirStr = expandHome(dirStr) + '/';
                pre = path.substr(path.find_last_of('/') + 1);

            }
        }
        else {
            dirStr = "./";
            pre = "";
        }

        // find out content candidates and their common prefix
        vector<string> cts = getDirCts(dirStr);
        for (size_t i = 0; i < cts.size(); ++i) {
            string file = cts[i];
            if (pre == "" || pre.compare(file.substr(0, pre.size())) == 0) {
                struct stat fStat;
                string path = dirStr + "/" + file;
                if (stat(path.c_str(), &fStat) == 0
                    && S_ISDIR(fStat.st_mode) != 0)
                    file += "/";
                else
                    isFile = true;
                cddts.push_back(file);
            }
        }
    }

    // append common prefix to current command string
    size_t nAddedChar = 0;
    size_t cmnPos     = 0;
    string cmn        = cddts.size() > 0 ? cddts[0] : "";
    size_t maxCddtLen = cddts.size() > 0 ? cddts[0].size() : 0;
    for (size_t i = 1; i < cddts.size(); ++i) {
        if (cddts[i].size() > maxCddtLen)
            maxCddtLen = cddts[i].size();
        cmnPos = cmnPrefix(cmn, cddts[i]);
        cmn = cmn.substr(0, cmnPos);
    }
    if (cmn.size() != 0) {
        cmn = cmn.substr(pre.size());
        cmdStr_.insert(csrPos_, cmn);
        csrPos_ += cmn.size();
        nAddedChar += cmn.size();
    }

    // if only one candidate, append ending character
    if (cddts.size() == 1) {
        if (isCmd || isOpt || isVar || isFile) {
            cmdStr_.insert(csrPos_, " ");
            csrPos_++;
            nAddedChar++;
        }
    }

    // show possible candidates
    else if (cddts.size() > 1) {
        cout << VT100_CSRR << VT100_SCRU << flush;
        size_t filesPerLine = (size_t)getTermCol() / (maxCddtLen + 2);
        for (size_t i = 0; i < cddts.size(); ++i) {
            cout << setw(maxCddtLen + 2) << left << cddts[i];
            if ((i + 1) % filesPerLine == 0)
                cout << endl;
        }
        if (cddts.size() % filesPerLine != 0)
            cout << endl;
        size_t nLinePrev = maxPos_ / (size_t)getTermCol();
        size_t nLineCur  = (maxPos_ +  nAddedChar) / (size_t)getTermCol();
        for (size_t i = 0; i < nLinePrev - (nLineCur - nLinePrev); ++i)
            cout << VT100_SCRU << flush;
        cout << VT100_CSRS << flush;
    }
}
//}}}


// **************************************************************************
// Function   [ bool CmdMgr::exec(const string) ]
// Author     [ littleshamoo ]
// Synopsis   [ parses command line and executes command and stores result ]
// **************************************************************************
//{{{ bool CmdMgr::exec(const string)
CmdMgr::Result CmdMgr::exec(const string &cmdStr) {
    vector<string> argv = parse(cmdStr);

    Result res = NOP;

    // variable substitution
    for (size_t i = 1; i < argv.size(); ++i)
        argv[i] = expandVar(argv[i]);

    // home extension
    for (size_t i = 1; i < argv.size(); ++i)
        argv[i] = expandHome(argv[i]);

    // set redirection
    bool   isRedirected = false;
    int    stdoutFd;
    fpos_t stdoutPos;
    if (argv.size() >= 2
        && (argv[argv.size() - 2] == ">" || argv[argv.size() - 2] == ">>")) {
        string mode = argv[argv.size() - 2] == ">" ? "w" : "a";
        if (setStdoutRedir(argv[argv.size() - 1], mode, stdoutFd, stdoutPos))
            isRedirected = true;
        argv.erase(argv.end() - 2, argv.end());
    }

    // execute command
    if (argv.size() > 0) {
        cmdHis_.push_back(cmdStr);

        CmdMapIter it = cmdMap_.find(argv[0]);
        if (it == cmdMap_.end()) {
            errorStr_ = argv[0];
            error_ = E_NOT_REG;
            res = NOT_EXIST;
        }
        else
            res = it->second->exec(argv) ? (exit_ ? EXIT : SUCCESS) : FAIL;
    }

    // reset stdout
    if (isRedirected)
        resetStdout(stdoutFd, stdoutPos);

    return res;
}
//}}}


bool CmdMgr::setStdoutRedir(const string &fname, const string &mode,
                            int &stdoutFd, fpos_t &stdoutPos) const {
    stdoutFd = dup(fileno(stdout));
    fgetpos(stdout, &stdoutPos);
    FILE *fptr = fopen(fname.c_str(), mode.c_str());
    if (fptr) {
        fclose(fptr);
        if (!freopen(fname.c_str(), mode.c_str(), stdout)) {
            cerr << "**WARN CmdMgr::setStdoutRedir(): file `" << fname;
            cerr << "' cannot be written" << endl;
            return false;
        }
    }
    else {
        cerr << "**WARN CmdMgr::setStdoutRedir(): file `" << fname << "' ";
        cerr << "cannot be written" << endl;
        return false;
    }
    return true;
}

void CmdMgr::resetStdout(const int &stdoutFd, const fpos_t &stdoutPos) const {
        fflush(stdout);
        dup2(stdoutFd, fileno(stdout));
        close(stdoutFd);
        clearerr(stdout);
        fsetpos(stdout, &stdoutPos);
}

