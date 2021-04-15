#include "Server.h"
#include <iostream>
#include <string>
#include <boost\bind.hpp>

//#define NOT_FOUND (x) ("HTTP/1.1 404 Not Found\nDate:"+(x)+"\nCache-Control: public, max-age=30\nExpires: Date + 30s (Ej: Tue, 04 Sep 2018 18:21:49 GMT)\nContent-Length: 0\nContent-Type: text/html; charset=iso-8859-1")


using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& io_context)

	:context_(io_context),
	acceptor_(io_context,tcp::endpoint(tcp::v4(), 80)),
	socket_(io_context)
{
	if (socket_.is_open()) //TODO si hay errores de inciilizacion es por esto
	{
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
	answer.clear();
} 


void Server::start_answering(bool isOk)
{
	std::cout << "start_answering()" << std::endl;
	
	//Abro archivo
	std::fstream pag("Page/" + FILENAME, std::ios::in | std::ios::binary);

	/*Checks if file was correctly open.*/
	if (!pag.is_open()) 
	{
		std::cout << "Failed to open file\n";
		return;
	}

	pag.seekg(0, pag.end);
	int size = pag.tellg();
	pag.seekg(0, pag.beg);

	this->file_size = size;

	std::cout << "Size of the file is" << " " << file_size << " " << "bytes" << std::endl;

	this->answer = generateAnswer(isOk);

	if (isOk)
	{
		std::ostringstream ss;
		ss << pag.rdbuf();
		this->answer += ss.str();
	}
	this->answer += "\r\n\r\n";


	boost::asio::async_write(
		socket_,
		boost::asio::buffer(this->answer),
		boost::bind(
			&Server::response_sent_cb,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	socket_.close(); //fijarse bien si hay que limpiar algun string proveniente del buffer
	pag.close();
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


void Server::response_sent_cb(const boost::system::error_code& error, size_t bytes_sent)
{
	std::cout << "response_sent_cb()" << std::endl;
	if (!error)
	{
		std::cout << "Response sent correctly! " << bytes_sent << "bytes" << std::endl;
	}
	else
	{
		std::cout << "Failed to respond...\n" << std::endl;
	}

	//Ahora que ya envie la respuesta, libero acceptor para un nueva conexion:
	start_waiting_connection();
}

void Server::message_received_cb(const boost::system::error_code& error, size_t bytes)
{
	if (!error)
	{
		//Se obtiene mensaje en formate de string, guardado message
		

		std::string message((std::istreambuf_iterator<char>(&buffer_)), std::istreambuf_iterator<char>());

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


std::string Server::generateAnswer(bool isOk)
{
	std::string date = makeDateString(false);
	
	std::string dateLater = makeDateString(true);
	
	std::string response;
	
	/*time_t now = time(0);
	std::string date_time = ctime(&now); //This method returns a pointer to a string that holds the date and time in the form of dayday monthmonth yearyear hours:minutes:seconds
	date = date_time;*/ 


	if (isOk)
	{
		response =
			"HTTP/1.1 200 OK\r\nDate:" + date + "Location: " + HOST + '/' + PATH + '/' + FILENAME + "\r\nCache-Control:  max-age=30\r\nExpires:"
			+ dateLater + "Content-Length:" + std::to_string(file_size) + "\r\nContent-Type: "
			+ TYPE + "; charset=iso-8859-1\r\n\r\n";
	}
	else
	{
		response = "HTTP/1.1 404 Not Found\r\nDate:" + date + "Location: " + HOST + '/' + PATH + '/' + FILENAME +
			"\r\nCache-Control: max-age=30\r\nExpires:" + dateLater + "Content-Length: 0" +
			"\r\nContemt_Type: " + TYPE + "; charset=iso-8859-1\r\n\r\n";
	}

	return response;
}


std::string Server::makeDateString(bool param)
{
	using namespace std::chrono;
	system_clock::time_point theTime = system_clock::now();

	if (param)
	{
		theTime += seconds(30);
	}

	time_t now = system_clock::to_time_t(theTime);
	return ctime(&now);
}