#pragma once
#include <string>
#include <vector>
#include <array>
#include <cstdint>

#include "configuration.h"
#include "protocol.h"

class MessageBase
{
	struct MessageHeader header;
protected:
	virtual std::vector<uint8_t> buildPayload() = 0;
public:
	MessageBase(std::array<uint8_t, 16> id, uint16_t code);
	virtual std::vector<uint8_t> build();
};

class MessageRegister: public MessageBase
{
	struct RegistrationMessagePayload payload;
protected:
	virtual std::vector<uint8_t> buildPayload();
public:
	MessageRegister(std::string name, std::array<uint8_t, 160> pubkey);
};

class MessageListClients : public MessageBase
{
protected:
	virtual std::vector<uint8_t> buildPayload();
public:
	MessageListClients(std::array<uint8_t, 16> id);
};


class MessageGetPubkey : public MessageBase
{
	GetPubkeyMessagePayload payload;
protected:
	virtual std::vector<uint8_t> buildPayload();
public:
	MessageGetPubkey(std::array<uint8_t, 16> id, std::array<uint8_t, 16> requested_id);
};


class MessageSendMessage : public MessageBase
{
	SendMessagePayloadHeader payload_header;
	std::string content;
protected:
	virtual std::vector<uint8_t> buildPayload();
public:
	MessageSendMessage(std::array<uint8_t, 16> id, std::array<uint8_t, 16> destination_id, uint8_t message_type, std::string content);
};

class MessageGetMessages : public MessageBase
{
protected:
	virtual std::vector<uint8_t> buildPayload();
public:
	MessageGetMessages(std::array<uint8_t, 16> id);
};