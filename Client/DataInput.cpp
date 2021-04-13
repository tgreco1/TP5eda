#include <iostream>
#include "DataInput.h"

//Recibe dos parametros de la linea de comando, el nonbre del ejecutable y host/path/filename.
#define PARAMS 2

//Separa en host y path/filename, Retorna true si es valido, sino falso.
bool getData(char** argV, int argC, void* userData) {
	Location* userDataPtr = (Location*)userData;

	bool result = false;
	std::string data;
	int pos;

	if (argC == PARAMS) 
	{
		data = argV[1];
		result = true;

		//Busco '/'
		pos = data.find('/');

		//Si no se encuentra, todo el input es el host, entonces el path esta vacio
		if (pos == std::string::npos) 
		{
			pos = data.length();
			data.append(" ");
		}
		 
		//seteo clase userdata
		userDataPtr->host = data.substr(0, pos);
		userDataPtr->path = data.substr(pos + 1, data.length() - pos + 1);
	}

	return result;
}