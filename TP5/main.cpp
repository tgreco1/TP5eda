#include <iostream>
#include <boost/asio.hpp>
#include "server.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
	try {
		boost::asio::io_context io_context;
		Server server(io_context);
		server.start();
		io_context.run();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}