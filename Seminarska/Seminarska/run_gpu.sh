#!/bin/bash
gcc -fopenmp gpu.c -o gpu -framework OpenCL -O3

for run in {1..10}
do
  echo "Poganjanje z: odstrani 10 šivov na sliki velikosti 1428 × 968"
  ./gpu 10 image.pgm
  echo "Poganjanje z: odstrani 50 šivov na sliki velikosti 1428 × 968"
  ./gpu 50 image.pgm
  echo "Poganjanje z: odstrani 100 šivov na sliki velikosti 1428 × 968"
  ./gpu 100 image.pgm
  echo "Poganjanje z: odstrani 200 šivov na sliki velikosti 1428 × 968"
  ./gpu 200 image.pgm
  echo "Poganjanje z: odstrani 300 šivov na sliki velikosti 1428 × 968"
  ./gpu 300 image.pgm

  echo "Poganjanje z: odstrani 10 šivov na sliki velikosti 3922 × 2616"
  ./gpu 10 alstrom.pgm
  echo "Poganjanje z: odstrani 50 šivov na sliki velikosti 3922 × 2616"
  ./gpu 50 alstrom.pgm
  echo "Poganjanje z: odstrani 100 šivov na sliki velikosti 3922 × 2616"
  ./gpu 100 alstrom.pgm
  echo "Poganjanje z: odstrani 200 šivov na sliki velikosti 3922 × 2616"
  ./gpu 200 alstrom.pgm
  echo "Poganjanje z: odstrani 300 šivov na sliki velikosti 3922 × 2616"
  ./gpu 300 alstrom.pgm

  echo "Poganjanje z: odstrani 10 šivov na sliki velikosti 1920 × 1080"
  ./gpu 10 apostoles.pgm
  echo "Poganjanje z: odstrani 50 šivov na sliki velikosti 1920 × 1080"
  ./gpu 50 apostoles.pgm
  echo "Poganjanje z: odstrani 100 šivov na sliki velikosti 1920 × 1080"
  ./gpu 100 apostoles.pgm
  echo "Poganjanje z: odstrani 200 šivov na sliki velikosti 1920 × 1080"
  ./gpu 200 apostoles.pgm
  echo "Poganjanje z: odstrani 300 šivov na sliki velikosti 1920 × 1080"
  ./gpu 300 apostoles.pgm

  echo "Poganjanje z: odstrani 10 šivov na sliki velikosti 5177 × 3456"
  ./gpu 10 eidsvatnet.pgm
  echo "Poganjanje z: odstrani 50 šivov na sliki velikosti 5177 × 3456"
  ./gpu 50 eidsvatnet.pgm
  echo "Poganjanje z: odstrani 100 šivov na sliki velikosti 5177 × 3456"
  ./gpu 100 eidsvatnet.pgm
  echo "Poganjanje z: odstrani 200 šivov na sliki velikosti 5177 × 3456"
  ./gpu 200 eidsvatnet.pgm
  echo "Poganjanje z: odstrani 300 šivov na sliki velikosti 5177 × 3456"
  ./gpu 300 eidsvatnet.pgm

  echo "Poganjanje z: odstrani 10 šivov na sliki velikosti 3276 × 4096"
  ./gpu 10 hawaii.pgm
  echo "Poganjanje z: odstrani 50 šivov na sliki velikosti 3276 × 4096"
  ./gpu 50 hawaii.pgm
  echo "Poganjanje z: odstrani 100 šivov na sliki velikosti 3276 × 4096"
  ./gpu 100 hawaii.pgm
  echo "Poganjanje z: odstrani 200 šivov na sliki velikosti 3276 × 4096"
  ./gpu 200 hawaii.pgm
  echo "Poganjanje z: odstrani 300 šivov na sliki velikosti 3276 × 4096"
  ./gpu 300 hawaii.pgm
done
