#pragma once

#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

#define SERVER_VERSION (1)
tcp::endpoint get_server_address();
