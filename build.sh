#!/bin/bash

gcc src/simquad.c -O2 -Wall -o bin/simquad -pthread  -lgsl -lmpfr 
