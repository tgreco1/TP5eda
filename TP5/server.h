/*****************************************************************************
@file     server.h
@brief    
@author   Grupo 7
******************************************************************************/

#ifndef _SERVER_H_
#define _SERVER_H_

#include <boost/asio.hpp>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>


#define HOST (std::string) "127.0.0.1"
#define PATH (std::string) "page"
#define FILENAME (std::string) "page.html" 
#define TYPE "text/html"


class Server
{
public:
	Server(boost::asio::io_context& context);
	~Server();
	void start();
	boost::asio::streambuf buffer_;
	
private:
	void start_waiting_connection();
	void start_answering(bool isOk);
	void connection_received_cb(const boost::system::error_code& error);
	void response_sent_cb(const boost::system::error_code& error, size_t bytes_sent);
	void message_received_cb (const boost::system::error_code& error, size_t bytes_sent);

	std::string makeDateString(bool param); //funcion que devuelve un string con la fecha. Recibe un bool que indica si hay que sumar 30 segundos o no

	std::string generateAnswer(bool isOk); //funcion que genera los strings de respuesta, dependiendo si el archivo fue encontrado o no

	std::string answer;

	boost::asio::io_context& context_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::ip::tcp::acceptor acceptor_;
	size_t file_size;

};


#endif