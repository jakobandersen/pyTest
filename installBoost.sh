#!/bin/bash

wget https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.gz
tar -xf boost_1_74_0.tar.gz
cd boost_1_74_0

sed -i".orig" "s/using python/#using python/" bootstrap.sh

pyver=$(python3 -c 'import sys; print(str(sys.version_info[0])+"."+str(sys.version_info[1]), end="")')
pyprefix=$(python3 -c 'import sys; print(sys.prefix, end="")')

echo "pyver=$pyver"
echo "pyprefix=$pyprefix"

rm -f user-config.jam
echo "using python : $pyver" >> user-config.jam
echo "             : python3" >> user-config.jam
echo "             : $pyprefix/include/python$pyver" >> user-config.jam
echo "             : $pyprefix/lib ;" >> user-config.jam

./bootstrap.sh --with-python=python3 --prefix=/opt/boost

echo "project-config.jam::::::::"
cat project-config.jam
echo "user-config.jam:::::::::::"
cat user-config.jam

sudo ./b2 -j 2 --with-python python=$pyver install
