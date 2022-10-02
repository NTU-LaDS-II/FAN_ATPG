// **************************************************************************
// File       [ tm_usage.cpp ]
// Author     [ littleshamoo ]
// Synopsis   [ functions to calculate CPU time and memory usage ]
// Date       [ Ver 3.0 started 2010/12/20 ]
// **************************************************************************

#include <sys/resource.h> // for getrusage()
#include <sys/time.h>     // for gettimeofday()
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "tm_usage.h"

using namespace std;
using namespace CommonNs;

TmUsage::TmUsage() {
    tStart_.uTime  = 0;
    tStart_.sTime  = 0;
    tStart_.rTime  = 0;
    tStart_.vmSize = 0;
    tStart_.vmPeak = 0;
    tStart_.vmDiff = 0;
    pStart_.uTime  = 0;
    pStart_.sTime  = 0;
    pStart_.rTime  = 0;
    pStart_.vmSize = 0;
    pStart_.vmPeak = 0;
    pStart_.vmDiff = 0;
}

TmUsage::~TmUsage() {}


bool TmUsage::totalStart() {
    return checkUsage(tStart_);
}

bool TmUsage::periodStart() {
    return checkUsage(pStart_);
}

bool TmUsage::getTotalUsage(TmStat &st) const {
    if (!checkUsage(st))
        return false;
    st.uTime  -= tStart_.uTime;
    st.sTime  -= tStart_.sTime;
    st.rTime  -= tStart_.rTime;
    st.vmDiff = st.vmSize - tStart_.vmSize;
    st.vmPeak = st.vmPeak > tStart_.vmPeak ? st.vmPeak : tStart_.vmPeak;
    return true;
}

bool TmUsage::getPeriodUsage(TmStat &st) const {
    if (!checkUsage(st))
        return false;
    st.uTime  -= pStart_.uTime;
    st.sTime  -= pStart_.sTime;
    st.rTime  -= pStart_.rTime;
    st.vmDiff = st.vmSize - pStart_.vmSize;
    st.vmPeak = st.vmPeak > pStart_.vmPeak ? st.vmPeak : pStart_.vmPeak;
    return true;
}


// **************************************************************************
// Function   [ checkUsage(TmStat &) ]
// Author     [ littleshamoo ]
// Synopsis   [ get user time and system time using getrusage() function and
//              get real time using gettimeofday() function and read
//              "/proc/self/status" to get memory usage ]
// **************************************************************************
// {{{ bool checkUsage(TmStat &) const
bool TmUsage::checkUsage(TmStat &st) const {
    // check user time and system time
    rusage tUsg;
    timeval tReal;
    getrusage(RUSAGE_SELF, &tUsg);
    gettimeofday(&tReal, NULL);
    st.uTime = tUsg.ru_utime.tv_sec * 1000000 + tUsg.ru_utime.tv_usec;
    st.sTime = tUsg.ru_stime.tv_sec * 1000000 + tUsg.ru_stime.tv_usec;
    st.rTime = tReal.tv_sec * 1000000 + tReal.tv_usec;

    // check current memory and peak memory
    FILE *fmem = fopen("/proc/self/status", "r");
    if (!fmem) {
        fprintf(stderr,
                "**ERROR TmUsage::checkUsage(): cannot get memory usage\n");
        st.vmSize = 0;
        st.vmPeak = 0;
        return false;
    }
    char membuf[128];
    while (fgets(membuf, 128, fmem)) {
        char *ch;
        if ((ch = strstr(membuf, "VmPeak:"))) {
            st.vmPeak = atol(ch + 7);
            continue;
        }
        if ((ch = strstr(membuf, "VmSize:"))) {
            st.vmSize = atol(ch + 7);
            break;
        }
    }
    fclose(fmem);
    return true;
}
//}}}

