#!/bin/bash
diff --recursive --suppress-common-lines --side-by-side $1 $2 > $1_VS_$2.txt