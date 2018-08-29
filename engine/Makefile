CC = g++
CFLAGS = -g -Ddspp -I../include/
CFLAGS += -std=c++11

LDFLAGS =  -lgurobi_g++5.2 -lgurobi_c++ -lgurobi70

fp: SOURCES = src/model.cpp
fp: BIN = bin/fp_run
fp: build


all:  
		@echo "please choose target qp or qd"

build:
	$(CC) -o $(BIN) $(SOURCES) $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean: 
	rm -f bin/*
