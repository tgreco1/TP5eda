#ifndef CLIENT_H
#define CLIENT_H

#include <curl/curl.h>
#include <string>
#include <fstream>


#define HOST (std::string)"127.0.0.1"
#define PATH (std::string) "img/page/page.html" //aca va link a pagina
#define PORT 80

class Client
{
public:
	Client(std::string host_ = HOST, std::string path_ = PATH, int port_ = PORT);

	void startConnection();

	std::fstream& getBuffer(void);

	~Client();

	void openFile(void);

private:
	void configureClient(void);
	std::string path, host;
	int port;

	char* contentType;

	std::fstream message;
	CURL* handler;
	CURLcode error;
};


#endif