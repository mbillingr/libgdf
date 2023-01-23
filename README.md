[![Build Status](https://travis-ci.org/mbillingr/libgdf.svg?branch=master)](https://travis-ci.com/mbillingr/libgdf)
[![CMake](https://github.com/mbillingr/libgdf/actions/workflows/cmake.yml/badge.svg)](https://github.com/mbillingr/libgdf/actions/workflows/cmake.yml)

This branch only exists to test if CI runs correctly on master, after all this time...

libGDF
======

C++ implementation of GDF - "a general dataformat for biosignals" version V2.20. 

Obtaining libGDF
----------------

Use the following command to fetch the sources:

    git clone https://github.com/kazemakase/libgdf.git libgdf
    
Dependencies
------------
Required: boost

Build Instructions
------------------

The preferred method is to perform an out-of-source build.
Replace `$GDF_ROOT` with the (relative or absolute) path to the source
tree (e.g. ~/repositories/libgdf).

    mkdir build
    cd build
    cmake $GDF_ROOT -DBUILD_TESTING=yes
    make
    make test
