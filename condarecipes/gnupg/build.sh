#!/bin/bash

export LDFLAGS=-ltinfo
./configure --prefix=$PREFIX && make && make check && make install
