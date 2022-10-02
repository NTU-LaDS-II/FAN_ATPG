#!/bin/bash

#############################################################################
# File       [ add_package.sh ]
# Author     [ littleshamoo ]
# Synopsis   [ add a package ]
# Date       [ 2010/10/07 created v3.0 ]
#############################################################################


shopt -s -o nounset

# global declaration

declare -rx SCRIPT=${0##*/}    # SCRIPT is the name of this script


# sanity checks

if [ -z "${1-}" ] ; then
  printf "$SCRIPT:$LINENO: please provide package name\n" >&2
  exit 192
fi

if [ -e "$1" ] ; then
  printf "$SCRIPT:$LINENO: the package already exist\n" >&2
  exit 192
fi


# generate package directory and Makefile
mkdir $1
cd $1
mkdir src

### generate Makefile for the package
echo "#############################################################################" >> Makefile
echo "# File       [ Makefile ]" >> Makefile
echo "# Author     [ littleshamoo ]" >> Makefile
echo "# Synopsis   [ package makefile ]" >> Makefile
echo "# Date       [ 2010/10/07 created ]" >> Makefile
echo "#############################################################################" >> Makefile
echo "" >> Makefile
echo "include ../../common.mk" >> Makefile
echo "include ../../info.mk" >> Makefile
echo "include ../../rule.mk" >> Makefile
echo "" >> Makefile

cd ..

# cleanup

exit 0

