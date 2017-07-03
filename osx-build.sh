#!/bin/bash

python setup.py build
install_name_tool -change libvixAllProducts.dylib "/Applications/VMware Fusion.app/Contents/Public/libvixAllProducts.dylib" $(find . -name _vixpy.so)
sudo python setup.py install

sudo rm -rf build dist vixpy.egg-info
