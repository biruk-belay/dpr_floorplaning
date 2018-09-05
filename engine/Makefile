CC = g++
CFLAGS = -g -Dfbdn -Ddspp -I../include/
CFLAGS += -std=c++11

LDFLAGS = -L/opt/gurobi702/linux64/lib/ -lgurobi_g++5.2 -lgurobi_c++ -lgurobi70

fp: SOURCES = model_working.cpp
fp: BIN = bin/fp_run
fp: build


all:  
		@echo "please choose target qp or qd"

build:
	$(CC) -o $(BIN) $(SOURCES) $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean: 
	rm -f bin/*
