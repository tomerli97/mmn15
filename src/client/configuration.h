#pragma once

#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

#define SERVER_VERSION (1)
tcp::endpoint get_server_address();

bool load_me_info(std::string& name, std::array<uint8_t, 16>& id, std::string& privkey);
void save_me_info(std::string& name, std::array<uint8_t, 16>& id, std::string& privkey);
