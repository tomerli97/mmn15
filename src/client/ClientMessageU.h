#pragma once
#include <boost/asio.hpp>
#include <vector>
#include <array>
#include <string>

#include "cryptopp_wrapper/Base64Wrapper.h"
#include "cryptopp_wrapper/RSAWrapper.h"
#include "cryptopp_wrapper/AESWrapper.h"


using namespace boost::asio;
using ip::tcp;

class ClientMessageU
{
	boost::asio::io_service io_service;
	tcp::socket socket;
	RSAPrivateWrapper keypair;
	std::array<uint8_t, 16> id;
	bool is_registered;

	std::map<std::string, std::array<uint8_t, 16>> users_names;
	std::map<std::array<uint8_t, 16>, std::array<uint8_t, 160>> users_pubkeys;
	std::map<std::array<uint8_t, 16>, std::string> users_session_keys; // TODO size of key

	std::array<uint8_t, 16> interactive_input_user();

	void do_register();
	void do_list_clients();
	void do_get_pubkey();
	void do_recv_messages();
	void do_send_message();
	void do_request_symkey();
	void do_send_symkey();

	void handle_message(struct GetMessagesPayloadEntry &message);
public:
	size_t srv_send(std::vector<uint8_t> data);
	std::vector<uint8_t> srv_rcv(size_t size);

	ClientMessageU(tcp::endpoint server);
	bool execute(int cmd);
	std::array<uint8_t, 160> get_pubkey();

};


class UserInputErrorException : public std::runtime_error {
public:
	UserInputErrorException() throw();
};

