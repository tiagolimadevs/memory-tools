CXX=g++
CC=gcc
FILE_OFFSET = -D_FILE_OFFSET_BITS=64
CXX_O=$(CXX) -o
CXX_C=$(CXX) -c



all: main


main: main.cpp parser.o memory.o info.o
	$(CXX_O) build/main $^ 

parser.o: parser.cpp
	$(CXX_C) $^

memory.o: memory.cpp
	$(CXX_C) $^

info.o: info.cpp
	$(CXX_C) $^

clean:
	rm *\.o main
