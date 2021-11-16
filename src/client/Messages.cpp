#include "Messages.h"

// Utils
std::vector<uint8_t> vector_from_data(uint8_t* data, size_t size)
{
	std::vector<uint8_t> vec;
	for (int i = 0; i < size; i++) {
		vec.push_back(data[i]);
	}
	return vec;
}

// MessageBase
MessageBase::MessageBase(std::array<uint8_t, 16> id, uint16_t code)
	: header()
{
	this->header.code = code;
	// TODO if server error raise error and add to server don't allow two users with same name
	for (int i = 0; i < id.size(); i++) {
		this->header.id[i] = id[i];
	}
	this->header.version = SERVER_VERSION;
}

std::vector<uint8_t> MessageBase::build()
{
	std::vector<uint8_t> payload = this->buildPayload();
	this->header.payload_size = payload.size();
	std::vector<uint8_t> built_message = vector_from_data((uint8_t*)&this->header, sizeof(this->header));
	built_message.insert(built_message.end(), payload.begin(), payload.end());
	return built_message;
}


// MessageRegister
std::vector<uint8_t> MessageRegister::buildPayload()
{
	return vector_from_data((uint8_t*)&this->payload, sizeof(this->payload));
}

MessageRegister::MessageRegister(std::string name, std::array<uint8_t, 160> pubkey)
	: MessageBase{{0,}, 1000}
{
	memset(&this->payload, 0, sizeof(this->payload));
	for (int i = 0; i < name.size(); i++) {
		this->payload.name[i] = name[i];
	}
	memcpy(&this->payload.pubkey, pubkey.data(), sizeof(this->payload.pubkey));
}

std::vector<uint8_t> MessageListClients::buildPayload()
{
	return std::vector<uint8_t>(); // No payload for this message
}

MessageListClients::MessageListClients(std::array<uint8_t, 16> id)
	: MessageBase{ id, 1001 }
{
}

std::vector<uint8_t> MessageGetPubkey::buildPayload()
{
	return vector_from_data((uint8_t*)&this->payload, sizeof(this->payload));
}

MessageGetPubkey::MessageGetPubkey(std::array<uint8_t, 16> id, std::array<uint8_t, 16> requested_id)
	: MessageBase{ id, 1002 }
{
	memset(&this->payload, 0, sizeof(this->payload));
	memcpy(&this->payload.id, requested_id.data(), sizeof(this->payload.id));
}

std::vector<uint8_t> MessageSendMessage::buildPayload()
{
	std::vector<uint8_t> payload = vector_from_data((uint8_t*)&this->payload_header, sizeof(this->payload_header));
	payload.insert(payload.end(), this->content.begin(), this->content.end());
	return payload;
}

MessageSendMessage::MessageSendMessage(std::array<uint8_t, 16> id, std::array<uint8_t, 16> destination_id, MessageType message_type, std::string content)
	:MessageBase{ id, 1003 },
	content(content)
{
	memset(&this->payload_header, 0, sizeof(this->payload_header));
	memcpy(&this->payload_header.id, destination_id.data(), sizeof(this->payload_header.id));
	this->payload_header.type = (uint8_t)message_type;
	this->payload_header.size = content.size();
}

std::vector<uint8_t> MessageGetMessages::buildPayload()
{
	return std::vector<uint8_t>(); // No payload for this message
}

MessageGetMessages::MessageGetMessages(std::array<uint8_t, 16> id)
	: MessageBase{ id, 1004 }
{
}
