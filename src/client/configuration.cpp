#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <boost/asio.hpp>
#include "configuration.h"

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::ifstream;
using std::cout;
using std::endl;


string read_file(const string& path) {
	ifstream f(path);
	if (f.is_open()) {
		return string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	}
	throw std::runtime_error("Failed to open requested file");
}


tcp::endpoint get_server_address() {
	std::cout << "Reading server configuration file..." << std::endl;
	string info = read_file("server.info");
	std::stringstream info_stream(info);
	string ip, port;
	std::getline(info_stream, ip, ':');
	std::getline(info_stream, port);
	std::cout << "The ip is: " << ip << " And the port is: " << port << std::endl;
	return tcp::endpoint(boost::asio::ip::address::from_string(ip), std::stoi(port));
}

