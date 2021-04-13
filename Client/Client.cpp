#include "Client.h"
#include <iostream>
#include <curl/easy.h>
#include <fstream>
#include "ErrorManagement.h"

const int fileNameSize = 10;

//Callback for when message is received.
size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userData);

//Constructor de cliente, inicializa CURL en modo easy, y llama a la configuracion del cliente.
Client::Client(std::string host_, std::string path_, int port_) : host(host_), path(path_), port(port_)
{
	handler = nullptr;

	error = curl_global_init(CURL_GLOBAL_ALL);

	if (error == CURLE_OK) 
	{
		handler = curl_easy_init();
		if (handler)
		{
			configureClient();
		}
		else
			throw (Error("Failed to initialize handler.\n"));
	}
	else
		throw(Error("Failed to set Curl.\n"));
}

//Destructor cliente
Client::~Client() 
{
	curl_easy_cleanup(handler);
	curl_global_cleanup();
	message.close();
}

//Luego del setup inciial, se inicia el handler
void Client::startConnection(void) 
{
	error = curl_easy_perform(handler);
	if (error != CURLE_OK)
	{
		throw(Error("Bad connection.\n"));
	}
}

//Cofigura el cliente para mandar una GET request al server y escribir la data en userdata.

void Client::configureClient(void) 
{
	curl_easy_setopt(handler, CURLOPT_URL, (host + '/' + path).c_str());
	curl_easy_setopt(handler, CURLOPT_PORT, port);
	curl_easy_setopt(handler, CURLOPT_PROTOCOLS, CURLPROTO_HTTP);
	curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, &writeCallback);
	curl_easy_setopt(handler, CURLOPT_WRITEDATA, this);
}

//Callback para escribir, hace un append a this->message.
size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userData) 
{
	Client* userDataPtr = (Client*)userData;

	std::fstream& output = userDataPtr->getBuffer();

	if (!output.is_open())
	{
		userDataPtr->openFile();
	}

	output.write(ptr, nmemb);

	return size * nmemb;
}

//Getter de mensaje
std::fstream& Client::getBuffer(void)
{ 
	return message; 
}

void Client::openFile(void) 
{
	std::string filename;
	std::string ctnt;
	int pos = path.find_last_of('.');

	auto removeBar = [](std::string& name) {std::string temp; for (auto x : name) { if (x != '/') temp += x; } return temp; };

	error = curl_easy_getinfo(handler, CURLINFO_CONTENT_TYPE, &contentType);

	if (error == CURLE_OK) 
	{
		ctnt = contentType;

		if (pos == std::string::npos || pos == path.length())
			filename = removeBar(path);

		else 
		{
			filename = path.substr(0, pos);
			filename = removeBar(filename);
		}

		if (filename.length() > fileNameSize)
			filename = filename.substr(filename.length() - fileNameSize, fileNameSize);

		pos = ctnt.find('/');
		ctnt = ctnt.substr(pos + 1, ctnt.length() - pos);

		message.open((filename + '.' + ctnt).c_str(), std::ios::out | std::ios::binary);
		if (!message.is_open()) 
		{
			message.close();
			throw(Error("Failed to create output file.\n"));
		}
	}
	else
		throw(Error("Failed to get content type.\n"));
}