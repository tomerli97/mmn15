#include "ClientMessageU.h"
#include "Messages.h"
#include "Responses.h"
#include "protocol.h"
#include "configuration.h"

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

std::array<uint8_t, 16> ClientMessageU::interactive_input_user()
{
	std::string username;
	cout << "Requested username: ";
	cin >> username;

	if (this->users_names.count(username) == 0) {
		cout << "Couldn't find user named " << username << endl;
		cout << "Try to list users again to update contact list" << endl;
		throw UserInputErrorException();
	}
	return this->users_names[username];
}

void ClientMessageU::do_register()
{
	if (this->is_registered) {
		cout << "Already registered! invalid command..." << endl;
		return;
	}

	// Get username
	std::string username;
	cout << "New username: ";
	cin >> username;
	this->name = username;

	// Generate keypair
	RSAPrivateWrapper keypair;
	this->privkey = keypair.getPrivateKey();

	cout << "Sending register request..." << endl;
	MessageRegister msg(username, this->get_pubkey());
	this->srv_send(msg.build());
	

	ResponseRegister res(*this);
	cout << "Registered!" << endl;	

	std::copy(std::begin(res.parsed.id), std::end(res.parsed.id), this->id.begin());
	this->is_registered = true;
	
	// Save to me.info
	save_me_info(this->name, this->id, this->privkey);
}

void ClientMessageU::do_list_clients()
{
	MessageListClients msg(this->id);
	this->srv_send(msg.build());

	ResponseListClients res(*this);
	cout << "Client list:" << endl;
	for (auto& it : res.entries) {
		cout << it.name << endl;

		// Create key if non-existent
		this->users_names.try_emplace(it.name, std::array<uint8_t, 16>()); //TODO bug?
		// Add id to map
		std::copy(std::begin(it.id), std::end(it.id), this->users_names[it.name].begin());
	}
}

void ClientMessageU::do_get_pubkey()
{
	// TODO: state check for every action

	auto requested_id = this->interactive_input_user();

	// Send pubkey request
	MessageGetPubkey msg(this->id, requested_id);
	this->srv_send(msg.build());

	// Get and parse response
	ResponseGetPubkey res(*this);
	std::array<uint8_t, 160> requested_pubkey;
	std::copy(std::begin(res.parsed.pubkey), std::end(res.parsed.pubkey), requested_pubkey.begin());

	// Save pubkey
	this->users_pubkeys[requested_id] = requested_pubkey;
}


std::string aes_decrypt(std::string key, const void* data, size_t size) {
	AESWrapper aes(key.data(), key.size());
	return aes.decrypt(data, size);
}

std::string aes_decrypt(std::string key, std::string data) {
	return aes_decrypt(key, data.data(), data.size());
}

std::string aes_decrypt(std::string key, std::vector<uint8_t> data) {
	return aes_decrypt(key, data.data(), data.size());
}

std::string aes_encrypt(std::string key, const void* data, size_t size) {
	AESWrapper aes(key.data(), key.size());
	return aes.encrypt(data, size);
}

std::string aes_encrypt(std::string key, std::string data) {
	return aes_encrypt(key, data.data(), data.size());
}

std::string aes_encrypt(std::string key, std::vector<uint8_t> data) {
	return aes_encrypt(key, data.data(), data.size());
}

std::string rsa_decrypt(std::string privkey, const void* data, size_t size) {
	RSAPrivateWrapper rsapriv(privkey.data(), privkey.size());
	return rsapriv.decrypt(data, size);
}

std::string rsa_decrypt(std::string privkey, std::string data) {
	return rsa_decrypt(privkey, data.data(), data.size());
}

std::string rsa_decrypt(std::string privkey, std::vector<uint8_t> data) {
	return rsa_decrypt(privkey, data.data(), data.size());
}

std::string rsa_encrypt(std::array<uint8_t, 160> pubkey, const void* data, size_t size) {
	RSAPublicWrapper rsapub(pubkey.data(), pubkey.size());
	return rsapub.encrypt(data, size);
}

std::string rsa_encrypt(std::array<uint8_t, 160> pubkey, std::string data) {
	return rsa_encrypt(pubkey, data.data(), data.size());
}

std::string rsa_encrypt(std::array<uint8_t, 160> pubkey, std::vector<uint8_t> data) {
	return rsa_encrypt(pubkey, data.data(), data.size());
}

void ClientMessageU::handle_message(GetMessagesPayloadEntry& message)
{
	std::string text;

	std::array<uint8_t, 16> sender_id;
	std::copy(std::begin(message.header.id), std::end(message.header.id), sender_id.begin());

	// Find sender name
	std::string sender_name = "unknown";
	for (auto& it : this->users_names) {
		if (it.second == sender_id) {
			sender_name = it.first;
		}
	}

	// Print message header
	cout << "From: " << sender_name << endl;
	cout << "Content:" << endl;

	switch ((MessageType)message.header.type) {
	case MessageType::SYM_KEY_REQUEST:
		// Symmetric key request
		text = "Request for symmetric key";
		break;
	case MessageType::SYM_KEY_RESPONSE:
		// Symmetric key response
		this->users_session_keys[sender_id] = rsa_decrypt(this->privkey, message.data);
		text = "Symmetric key received";
		break;
	case MessageType::TEXT_MESSAGE:
		if (this->users_session_keys.count(sender_id) == 0) {
			// No session key
			text = "can't decrypt message";
		}
		else {
			// Session key exists
			text.append(aes_decrypt(this->users_session_keys[sender_id], message.data));
		}
		break;
	default:
		text = "Unsupported message type";
		break;
	}

	// Print message footer
	cout << text << endl;
	cout << "-----<EOM>-----" << endl << endl;
}

void ClientMessageU::do_recv_messages()
{
	// Request new messages
	MessageGetMessages msg(this->id);
	this->srv_send(msg.build());

	// Get and parse response
	ResponseGetMessages res(*this);
	
	// Display and handle messages
	for (auto& it : res.entries) {	
		this->handle_message(it);
	}
}

void ClientMessageU::do_send_message()
{
	auto requested_id = this->interactive_input_user();

	std::string text;
	cout << "Message: ";
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	if (!std::getline(cin, text)) {
		cout << "Failed to read!" << endl;
	}

	// Encrypt message
	if (this->users_session_keys.count(requested_id) == 0) {
		// No session key
		cout << "No symmetric key established with this user. Aborting." << endl;
		throw UserInputErrorException();
	}
	std::string encrypted = aes_encrypt(this->users_session_keys[requested_id], text);

	// Send message
	MessageSendMessage msg(this->id, requested_id, MessageType::TEXT_MESSAGE, encrypted);
	this->srv_send(msg.build());

	ResponseMessageAccepted res(*this); // Parse message to check for errors
}

void ClientMessageU::do_request_symkey()
{
	auto requested_id = this->interactive_input_user();

	MessageSendMessage msg(this->id, requested_id, MessageType::SYM_KEY_REQUEST, "");
	this->srv_send(msg.build());

	ResponseMessageAccepted res(*this); // Parse message to check for errors
}


void ClientMessageU::do_send_symkey()
{
	auto requested_id = this->interactive_input_user();

	// Check for user public key and session key
	if (this->users_pubkeys.count(requested_id) == 0) {
		cout << "You have not requested the public key for this user yet." << endl;
		throw UserInputErrorException();
	}
	if (this->users_session_keys.count(requested_id) != 0) {
		cout << "You already have a symmetric key with this user. Aborting." << endl;
		throw UserInputErrorException();
	}

	// Generate symmetric key
	unsigned char key_buffer[AESWrapper::DEFAULT_KEYLENGTH];
	AESWrapper::GenerateKey(key_buffer, AESWrapper::DEFAULT_KEYLENGTH);
	std::string key(key_buffer, &key_buffer[AESWrapper::DEFAULT_KEYLENGTH]);


	// Encrypt symmetric key with public key
	std::string enc_key = rsa_encrypt(this->users_pubkeys[requested_id], key);

	MessageSendMessage msg(this->id, requested_id, MessageType::SYM_KEY_RESPONSE, enc_key);
	this->srv_send(msg.build());


	ResponseMessageAccepted res(*this); // Parse message to check for errors

	// Save symmetric key as session key
	this->users_session_keys[requested_id] = key;
}

ClientMessageU::ClientMessageU(tcp::endpoint server)
	: io_service(),
	socket(io_service),
	privkey(),
	is_registered(false),
	users_names(),
	users_pubkeys(),
	users_session_keys(),
	id(),
	name()
{
	this->socket.connect(server);
	this->is_registered = load_me_info(this->name, this->id, this->privkey);
}

bool ClientMessageU::execute(int cmd)
{
	try {
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
	}
	catch (UserInputErrorException eu) {
		cout << "Invalid input, operation aborted." << endl;
	}
	catch (ServerErrorMessageException es) {
		cout << "server responded with an error" << endl;
	}
	return true;
}

std::array<uint8_t, 160> ClientMessageU::get_pubkey()
{
	std::array<uint8_t, 160> pubkey;
	RSAPrivateWrapper keypair(this->privkey.data(), this->privkey.size());
	keypair.getPublicKey((char*)pubkey.data(), pubkey.size());
	return pubkey;
}

UserInputErrorException::UserInputErrorException() throw()
	: std::runtime_error("")
{
}
