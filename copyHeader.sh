#! /bin/bash

mkdir -p include/interface
mkdir -p include/common
mkdir -p include/core
mkdir -p include/fan

for header in pkg/common/src/*.h ;
do
cp ${header} include/common/;
done
for header in pkg/core/src/*.h ;
do
cp ${header} include/core/;
done
for header in pkg/fan/src/*.h ;
do
cp ${header} include/fan/;
done
for header in pkg/interface/src/*.h ;
do
cp ${header} include/interface/;
done