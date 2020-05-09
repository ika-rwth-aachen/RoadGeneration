#!/bin/bash

cmp --silent $1 $2 && exit 0 || exit 1