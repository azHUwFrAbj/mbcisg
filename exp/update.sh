#!/bin/bash

# continue on error
set +e

# Path to the compiled executable
EXECUTABLE=../build/exp
DIR_PATH=../dat
DAMP_DIR=../damp


WINDOW=-1
STRIDE_COUNT=10000
PCT=0.2 # control the window set, which = $PCT * #Edge

FDBCC=1 # set this value to 1 to run HDT; 0 to disable HDT

# Datasets
DATA_FILE=DN

EXP_TYPE=update_efficiency

$EXECUTABLE --experiment=$EXP_TYPE --pct=$PCT --tsv=$DATA_FILE  --data_dir=$DIR_PATH  --damp_dir=$DAMP_DIR  --window=$WINDOW --stride=1 --stride-count=$STRIDE_COUNT --fdbcc=$FDBCC

