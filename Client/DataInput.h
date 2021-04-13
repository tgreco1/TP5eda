#ifndef DATAINPUT_H
#define DATAINPUT_H

#include <string>

typedef struct {
	std::string host;
	std::string path;
} Location;

bool getData(char**, int, void*);

#endif DATAINPUT_H