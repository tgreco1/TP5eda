#include <iostream>
#include "Client.h"
#include "DataInput.h"
#include "ErrorManagement.h"

int main(int argC, char** argV) {
	Location userData;
	int out = -1;

	if (getData(argV, argC, &userData)) {
		try 
		{
			//Se crea objeto cliente
			Client myNewClient(userData.host, userData.path, 80);

			//Comienza conexion al host
			myNewClient.startConnection();

			std::cout << "Successfully connected with server.\n";

			out = 0;
		}
		catch (Error& e) 
		{
			std::cout << e.what() << std::endl;
		}
	}
	else
		std::cout << "Failed to reach server. Wrong syntax.\n";

	return out;
}