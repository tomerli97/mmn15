#pragma once

#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;

tcp::endpoint get_server_address();
