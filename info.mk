#############################################################################
# File       [ info.mk ]
# Author     [ littleshamoo ]
# Synopsis   [ package type and dependency ]
# Date       [ 2010/10/07 created ]
#############################################################################

#############################################################################
# Package List
# 'STAPKGS' and 'DYNPKGS' are packages written by user.
# Following is the description of the two types of packages:
#
#   1. STAPKGS: static packages are those intended to be archived into .a
#               library
#   2. DYNPKGS: dyn packages are those intended to be built into .so library
#
#   i.e. to add an dynamic package `out' and two static packages `test1' and
#        `test2'
#
#        STAPKGS = test1 test2
#        DYNPKGS = out
#
#############################################################################
STAPKGS = common interface core fan
DYNPKGS =

OWNPKGS = $(STAPKGS) $(DYNPKGS)

# To specify dependent packages
#        test3_DEP = test1 test2          // test3 depends on test1 and test2

core_DEP = interface
fan_DEP = common interface core

# To specify sources containing main function
#        test1_EXE = hello.cpp route.cpp  // hello.cpp and route.cpp in test1
#                                         // contain main function

common_EXE = common_test.cpp

interface_EXE = mdt_test.cpp     \
				vlog_test.cpp    \
				sdf_test.cpp     \
				pat_test.cpp     \
				lht_pat_test.cpp \
				vlog2pmt.cpp

fan_EXE = main.cpp

# To specify binary names
# 		 hello.cpp_test1_BIN = Hello      // hello.cpp in test1 will be built
# 		                                  // into Hello

main.cpp_fan_BIN = fan

# To specify package additional C flags
#        test1_CFLAGS = -Wall             // test1 C flag is "-Wall"


# To specify package additional external libraries
#        test1_OPTEXT = oaCommon          // test1 opt external lib is oaCommon
#        test1_DBGEXT = oaCommonD         // test1 debug external lib is
#                                         // oaCommonD

