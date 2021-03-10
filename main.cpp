#include <iostream>
#include "memory.h"
#include "parser.h"

int main(int argc, char *argv[]) {
	Parser parser(argc, argv);
	if (not parser.allOk())
		return 1;

	Memory memory;
	memory.load(parser);

}
