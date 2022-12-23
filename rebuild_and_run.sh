#!/bin/bash
make clean
make &> makefile_output.txt
bash script/autoRunScripts/runFAN.sh script/fanScripts
bash script/autoRunScripts/averageAllFC.sh rpt &> FC.txt
bash script/autoRunScripts/averageAllTL.sh rpt &> TL.txt
bash script/autoRunScripts/averageAllRT.sh rpt &> RT.txt