#!/bin/bash

file=$1
field=$2

head $file -n 1 | sed 's/,/\n/g' | grep -n $field
