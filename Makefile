all:
	g++ *.cpp tests/test-datablock.cpp -o test-datablock -I./ -ggdb
	g++ *.cpp tests/test-stl-alloc.cpp -o test-stl-alloc -I./ -ggdb