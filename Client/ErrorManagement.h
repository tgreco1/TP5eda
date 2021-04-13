#ifndef ERRORM_H
#define ERRORM_H

#include <iostream>

class Error : public std::exception 
{
public:
	Error(const char* message_);
	const char* what(void);
	~Error();
private:
	const char* message;
};




#endif
