#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <boost/asio.hpp>

#include "configuration.h"
#include "cryptopp_wrapper/Base64Wrapper.h"

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::ifstream;
using std::ofstream;


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


std::string hex2str(std::string& hexstr) {
	int len = hexstr.length();
	std::string str;
	for (int i = 0; i < len; i += 2)
	{
		int chr;
		std::istringstream(hexstr.substr(i, 2)) >> std::hex >> chr;
		str.push_back(chr);
	}
	return str;
}

std::string str2hex(std::string str) {
	int len = str.length();
	std::ostringstream hexstr;
	hexstr << std::hex;
	for (size_t i : str)
	{
		hexstr << std::setfill('0') << std::setw(2) << (i & 0xFF);
	}
	return hexstr.str();
}


//template <size_t size>
//std::string str2hex(std::array<uint8_t, size>& arr) {
//	int len = arr.size();
//	std::ostringstream hexstr;
//	for (int i : arr)
//	{
//		hexstr << std::hex << i;
//	}
//	return hexstr.str();
//}

bool load_me_info(std::string& name, std::array<uint8_t, 16>& id, std::string& privkey) {
	// Check file existance
	ifstream f("me.info");
	if (!f.is_open()) {
		std::cout << "No me.info file detected, new account must be registered." << std::endl;
		return false;
	}
	std::string conf_name, conf_id, conf_privkey, line;
	std::string conf_id_binary, conf_privkey_binary;

	std::getline(f, conf_name);
	std::getline(f, conf_id);
	while (!f.eof()) {
		std::getline(f, line);
		conf_privkey += line + "\n";
	}

	f.close();

	conf_id_binary = hex2str(conf_id);
	conf_privkey_binary = Base64Wrapper::decode(conf_privkey);

	// Output
	name = conf_name;
	std::copy(conf_id_binary.begin(), conf_id_binary.end(), id.begin());
	privkey.append(conf_privkey_binary.begin(), conf_privkey_binary.end());

	return true;
}

void save_me_info(std::string& name, std::array<uint8_t, 16>& id, std::string& privkey) {
	ofstream f("me.info");
	if (!f.is_open()) {
		std::cout << "No me.info file detected, new account must be registered." << std::endl;
		throw std::runtime_error("Failed to create me.info file!");
	}

	f << name << std::endl;
	f << str2hex(std::string(id.begin(), id.end())) << std::endl;
	f << Base64Wrapper::encode(std::string(privkey.begin(), privkey.end())) << std::endl;

	f.close();
}



