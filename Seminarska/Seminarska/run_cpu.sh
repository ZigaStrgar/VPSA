#!/bin/bash
gcc -fopenmp cpu.c -O3 -o cpu

for run in {1..10}
do
  echo "Poganjanje z: odstrani 10 šivov na sliki velikosti 1428 × 968"
  ./cpu 10 image.pgm
  echo "Poganjanje z: odstrani 50 šivov na sliki velikosti 1428 × 968"
  ./cpu 50 image.pgm
  echo "Poganjanje z: odstrani 100 šivov na sliki velikosti 1428 × 968"
  ./cpu 100 image.pgm
  echo "Poganjanje z: odstrani 200 šivov na sliki velikosti 1428 × 968"
  ./cpu 200 image.pgm
  echo "Poganjanje z: odstrani 300 šivov na sliki velikosti 1428 × 968"
  ./cpu 300 image.pgm

  echo "Poganjanje z: odstrani 10 šivov na sliki velikosti 3922 × 2616"
  ./cpu 10 alstrom.pgm
  echo "Poganjanje z: odstrani 50 šivov na sliki velikosti 3922 × 2616"
  ./cpu 50 alstrom.pgm
  echo "Poganjanje z: odstrani 100 šivov na sliki velikosti 3922 × 2616"
  ./cpu 100 alstrom.pgm
  echo "Poganjanje z: odstrani 200 šivov na sliki velikosti 3922 × 2616"
  ./cpu 200 alstrom.pgm
  echo "Poganjanje z: odstrani 300 šivov na sliki velikosti 3922 × 2616"
  ./cpu 300 alstrom.pgm

  echo "Poganjanje z: odstrani 10 šivov na sliki velikosti 1920 × 1080"
  ./cpu 10 apostoles.pgm
  echo "Poganjanje z: odstrani 50 šivov na sliki velikosti 1920 × 1080"
  ./cpu 50 apostoles.pgm
  echo "Poganjanje z: odstrani 100 šivov na sliki velikosti 1920 × 1080"
  ./cpu 100 apostoles.pgm
  echo "Poganjanje z: odstrani 200 šivov na sliki velikosti 1920 × 1080"
  ./cpu 200 apostoles.pgm
  echo "Poganjanje z: odstrani 300 šivov na sliki velikosti 1920 × 1080"
  ./cpu 300 apostoles.pgm

  echo "Poganjanje z: odstrani 10 šivov na sliki velikosti 5177 × 3456"
  ./cpu 10 eidsvatnet.pgm
  echo "Poganjanje z: odstrani 50 šivov na sliki velikosti 5177 × 3456"
  ./cpu 50 eidsvatnet.pgm
  echo "Poganjanje z: odstrani 100 šivov na sliki velikosti 5177 × 3456"
  ./cpu 100 eidsvatnet.pgm
  echo "Poganjanje z: odstrani 200 šivov na sliki velikosti 5177 × 3456"
  ./cpu 200 eidsvatnet.pgm
  echo "Poganjanje z: odstrani 300 šivov na sliki velikosti 5177 × 3456"
  ./cpu 300 eidsvatnet.pgm

  echo "Poganjanje z: odstrani 10 šivov na sliki velikosti 3276 × 4096"
  ./cpu 10 hawaii.pgm
  echo "Poganjanje z: odstrani 50 šivov na sliki velikosti 3276 × 4096"
  ./cpu 50 hawaii.pgm
  echo "Poganjanje z: odstrani 100 šivov na sliki velikosti 3276 × 4096"
  ./cpu 100 hawaii.pgm
  echo "Poganjanje z: odstrani 200 šivov na sliki velikosti 3276 × 4096"
  ./cpu 200 hawaii.pgm
  echo "Poganjanje z: odstrani 300 šivov na sliki velikosti 3276 × 4096"
  ./cpu 300 hawaii.pgm
done
