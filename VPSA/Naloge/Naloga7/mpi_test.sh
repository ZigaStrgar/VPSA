#!/bin/sh

PROGRAM="mpi_test"
ARGUMENTI="$@" # Argumenti, ki jih dobimo iz xRSL (atribut "arguments")

# 1. Prevajanje
mpicc $PROGRAM.c -o $PROGRAM

# ------------------------------------
# Izpis spremenljivk, da vidimo, kaj predstavljajo

# $MPIRUN se izbere glede na katero MPI razlicico
# smo izbrali v xRSL runTimeEnvironment atributu
echo "MPIRUN:""$MPIRUN"

# $MPIARGS doloca stevilo procesov preko -np parametra,
# dolocen je na podlagi count atributa v xRSL
echo "MPIARGS:""$MPIARGS"
#-------------------------------------

# 2. Zagon programa
$MPIRUN $MPIARGS $PROGRAM $ARGUMENTI

# 3. Koncno stanje programa
exitcode=$?
echo Program je koncal s kodo $exitcode.

exit $exitcode
