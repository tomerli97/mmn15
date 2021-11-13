#pragma once
#include <boost/asio.hpp>
#include <vector>
#include <array>


using namespace boost::asio;
using ip::tcp;

class ClientMessageU
{
	boost::asio::io_service io_service;
	tcp::socket socket;
	std::array<uint8_t, 160> pubkey;
	std::array<uint8_t, 16> id;
	bool is_registered;


	void do_register();
	void do_list_clients();
	void do_get_pubkey();
	void do_recv_messages();
	void do_send_message();
	void do_request_symkey();
	void do_send_symkey();
public:
	size_t srv_send(std::vector<uint8_t> data);
	std::vector<uint8_t> srv_rcv(size_t size);

	ClientMessageU(tcp::endpoint server);
	bool execute(int cmd);
};

