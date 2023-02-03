CXX=mpicxx
CXXFLAGES= -o3 -lm -Wall
TARGETS=hw1

.PHONY: all
all: $(TARGETS)

.PHONY:clear
clear:
	rm -f $(TARGETS)