/*****************************************************************************
@file     server.h
@brief    
@author   Grupo 7
******************************************************************************/

#ifndef _SERVER_H_
#define _SERVER_H_

#include <boost/asio.hpp>
#include <string>

#define HOST (std::string) "127.0.0.1"
#define PATH (std::string) "page"
#define FILENAME (std::string) "page.html" 


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
	

	std::string msg;

	boost::asio::io_context& context_;
	boost::asio::ip::tcp::socket socket_;
	boost::asio::ip::tcp::acceptor acceptor_;

};


#endif