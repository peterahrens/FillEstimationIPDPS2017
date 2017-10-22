#!/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`/gsl/lib:`pwd`/taco/lib
export TACO_TMPDIR=`pwd`/taco/tmp
export TACO_CFLAGS="-O3 -ffast-math -std=c99 -fopenmp -funroll-loops"
