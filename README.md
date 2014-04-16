[![Build Status](https://travis-ci.org/kazemakase/libgdf.svg?branch=master)](https://travis-ci.org/kazemakase/libgdf)

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
replace `$GDF_ROOT` with the (relative or absolute) path to the source
tree (e.g. ~/repositories/libgdf).

    mkdir build
    cd build
    cmake $GDF_ROOT -DBUILD_TESTING=yes
    make
    make test
