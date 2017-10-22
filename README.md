Tensor Fill Estimation: a comparison
======================

Organization
--------

oski-1.0.1h is the entire sparse kernel library. The tensor fill estimation
algorithm is from oski-1.0.1h/src/heur/estfill.c.

taco-oopsla2017 is the Tensor Algebra Compiler. It can be compiled by running
taco.sh from the base of the repository.

gsl-2.4 is the gnu standard library. It can be compiled by running
gsl.sh from the base of the repository.

important environment variables can be set by running env.sh from the base
of the repository.

Compilation
--------

Compilation is done from within the src/ directory. Just run
```
$ make
```
to get the executables. Inputs are provided in matrix/. To run on a particular matrix, use
```
$ ./asx matrix/[name]
```
