#include "ClientMessageU.h"
#include "Messages.h"
#include "Responses.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

using std::cout;
using std::cin;
using std::endl;

size_t ClientMessageU::srv_send(std::vector<uint8_t> data)
{
	size_t ret = 0;
	boost::system::error_code error;
	ret = boost::asio::write(this->socket, boost::asio::buffer(data), error);
	if (error) {
		throw std::runtime_error("Failed to send data to server");
	}
	return ret;
}

std::vector<uint8_t> ClientMessageU::srv_rcv(size_t size)
{
	boost::system::error_code error;
	boost::asio::streambuf receive_buffer;
	
	boost::asio::read(socket, receive_buffer, boost::asio::transfer_exactly(size), error);
	if (error && error != boost::asio::error::eof) {
		throw std::runtime_error("Failed to recv data from server");
	}
	std::vector<uint8_t> ret(receive_buffer.size());
	boost::asio::buffer_copy(boost::asio::buffer(ret), receive_buffer.data());
	return ret;
}


void print_hex(const uint8_t * buffer, unsigned int length)
{
	std::ios::fmtflags f(std::cout.flags());
	std::cout << std::hex;
	for (size_t i = 0; i < length; i++)
		std::cout << std::setfill('0') << std::setw(2) << (0xFF & buffer[i]) << (((i + 1) % 16 == 0) ? "\n" : " ");
	std::cout << std::endl;
	std::cout.flags(f);
}

void ClientMessageU::do_register()
{
	if (this->is_registered) {
		cout << "Already registered! invalid command..." << endl;
		return;
	}

	std::string username;
	cout << "New username: ";
	cin >> username;

	// TODO: generate pubkey

	cout << "Sending register request..." << endl;
	MessageRegister msg(username, this->pubkey);
	this->srv_send(msg.build());
	

	ResponseRegister res(*this);
	cout << "Registered! Got id: " << endl;	
	print_hex(res.parsed.id, sizeof(res.parsed.id));

	std::copy(std::begin(res.parsed.id), std::end(res.parsed.id), this->id.begin());
	this->is_registered = true;

	// TODO: me.info
}

void ClientMessageU::do_list_clients()
{
	MessageListClients msg(this->id);
	this->srv_send(msg.build());

	ResponseListClients res(*this);
	cout << "Client list:" << endl;
	for (auto& it : res.entries) {
		cout << it.name << endl;
	}
}

void ClientMessageU::do_get_pubkey()
{
}

void ClientMessageU::do_recv_messages()
{
}

void ClientMessageU::do_send_message()
{
}

void ClientMessageU::do_request_symkey()
{
}

void ClientMessageU::do_send_symkey()
{
}

ClientMessageU::ClientMessageU(tcp::endpoint server)
	: io_service(),
	socket(io_service),
	pubkey(),
	is_registered(false)
{
	this->socket.connect(server);
	// TODO: me.info
	// TODO: pubkey
	
}

bool ClientMessageU::execute(int cmd)
{
	switch (cmd) {
	case 10:
		this->do_register();
		break;
	case 20:
		this->do_list_clients();
		break;
	case 30:
		this->do_get_pubkey();
		break;
	case 40:
		this->do_recv_messages();
		break;
	case 50:
		this->do_send_message();
		break;
	case 51:
		this->do_request_symkey();
		break;
	case 52:
		this->do_send_symkey();
		break;
	case 0:
		return false;
	default:
		cout << "Invalid command number: " << cmd << endl;
		break;
	}
	return true;
}
