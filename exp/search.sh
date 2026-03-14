#!/bin/bash

# continue on error
set +e

# Path to the compiled executable
EXECUTABLE=../build/exp
DIR_PATH=../dat
DAMP_DIR=../damp

PCT=0.1 # control the window set, which = $PCT * #Edge
COUNT=1000
FDBCC=0 # set this value to 1 to run HDT; 0 to disable HDT

# Datasets
DATA_FILE=DN

EXP=search_efficiency

PCT=0.2

$EXECUTABLE --experiment=$EXP --pct=$PCT --tsv=$DATA_FILE  --data_dir=$DIR_PATH  --damp_dir=$DAMP_DIR --stride-count=1 --search-count=$COUNT --fdbcc=$FDBCC

