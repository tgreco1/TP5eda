#include "Server.h"
#include <iostream>
#include <string>
#include <boost\bind.hpp>

//#define NOT_FOUND (x) ("HTTP/1.1 404 Not Found\nDate:"+(x)+"\nCache-Control: public, max-age=30\nExpires: Date + 30s (Ej: Tue, 04 Sep 2018 18:21:49 GMT)\nContent-Length: 0\nContent-Type: text/html; charset=iso-8859-1")


using boost::asio::ip::tcp;
std::string make_string(char * path);

Server::Server(boost::asio::io_context& io_context)

	:context_(io_context),
	acceptor_(io_context,tcp::endpoint(tcp::v4(), 80)),
	socket_(io_context)
{
	if (socket_.is_open()) {
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket_.close();
	}
}

Server::~Server()
{
	std::cout << "\nClosing server.\n";
	if (socket_.is_open()) 
	{
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket_.close();
	}

	if (acceptor_.is_open())
	{
		acceptor_.close();
	}

	std::cout << "Server is closed.\n";
}


void Server::start() 
{
	if (socket_.is_open()) 
	{
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
		socket_.close();
	}
	start_waiting_connection();
}

void Server::start_waiting_connection()
{
	std::cout << "start_waiting_connection()" << std::endl;
	if (socket_.is_open()) 
	{
		std::cout << "Error: can't accept new connection from an open socket" << std::endl;
		return;
	}
	
	if (acceptor_.is_open()) 
	{
		std::cout << "Waiting for connection." << std::endl;
		acceptor_.async_accept(socket_, boost::bind(&Server::connection_received_cb, this, boost::asio::placeholders::error));
	}
	msg.clear();
} 


void Server::start_answering(bool isOk)
{
	std::cout << "start_answering()" << std::endl;
	
	//Abro archivo
	std::fstream a(FILENAME, std::ios::in | std::ios::binary);

	/*Checks if file was correctly open.*/
	if (!a.is_open()) 
	{
		std::cout << "Failed to open file\n";
		return;
	}


	boost::asio::async_write(
		socket_,
		boost::asio::buffer(msg),
		boost::bind(
			&Server::response_sent_cb,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket_.close();
}


void Server::connection_received_cb(const boost::system::error_code& error) {
	std::cout << "connection_received_cb" << std::endl;

	if (!error) 
	{
		boost::asio::async_read_until(socket_, buffer_, "\r\n\r\n",    // Leemos hasta el terminador
			boost::bind(
				&Server::message_received_cb,              
				this,
				boost::asio::placeholders::error,              
				boost::asio::placeholders::bytes_transferred)  
		);
		//start_answering();
		//start_waiting_connection();
	}
	else {
		std::cout << error.message() << std::endl;
	}
}

/*void Server::message_received_cb(const boost::system::error_code& error, size_t bytes_sent)
{
	// averiguo si exite o no el path y llamado
	using namespace std;
	
    std::istream is(&buffer_);
    std::string buf;
    std::getline(is, buf);
	
	cout << buf << endl;
	
	const char* test = buf.c_str();
	FILE* filePointer;
	//errno_t err; 
	if ((fopen_s(&filePointer, test, "r")) != 0) //error al abrirlo
	{
		msg = "No encontro el archivo\n";
		start_answering(msg);
	}
	else
	{
		msg = "Encontro el archivo\n";
		start_answering(msg);
	}
}*/

void Server::response_sent_cb(const boost::system::error_code& error, size_t bytes_sent)
{
	std::cout << "response_sent_cb()" << std::endl;
	if (!error)
	{
		std::cout << "Response sent" << bytes_sent << "bytes" << std::endl;
	}
}

void Server::message_received_cb(const boost::system::error_code& error, size_t bytes)
{
	if (!error)
	{
		//Se obtiene mensaje en formate de string, guardado message
		std::istream is(&buffer_);
		std::string message;
		std::getline(is, message);

		std::string validFormat = "GET /" + PATH + '/' + FILENAME + " HTTP/1.1\r\nHost: " + HOST + "\r\n";
		bool isOk = false;

		int len = message.length();
		if (message.find(validFormat) == 0)
		{
			if (len > validFormat.length() && message[len - 2] == '\r' && message[len - 1] == '\n')
			{
				isOk = true;
			}
		}
		else
		{
			std::cout << "Wrong input sent." << std::endl;
		}

		start_answering(isOk);
	}
	else
	{
		std::cout << error.message() << std::endl;
	}


}
