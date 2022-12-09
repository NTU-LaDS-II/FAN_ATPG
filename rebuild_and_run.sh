#!/bin/bash
make clean
make &> makefile_output.txt
bash script/autoRunScripts/runFAN.sh script/fanScripts