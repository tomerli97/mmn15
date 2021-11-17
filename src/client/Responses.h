#pragma once

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <array>
#include <cstdint>

#include "configuration.h"
#include "protocol.h"
#include "ClientMessageU.h"

using namespace boost::asio;

class ServerErrorMessageException : public std::runtime_error {
public:
	ServerErrorMessageException() throw();
};

class ResponseBase
{
public:
	std::vector<uint8_t> data;
	std::vector<uint8_t> payload;
	struct ResponseHeader header;
	ResponseBase(ClientMessageU &cmu);
};


class ResponseRegister: public ResponseBase
{
public:
	struct RegisterSuccessPayload parsed;
	ResponseRegister(ClientMessageU& cmu);
};

class ResponseListClients : public ResponseBase
{
public:
	std::vector<struct GetUsersPayloadEntry> entries;
	ResponseListClients (ClientMessageU& cmu);
};

class ResponseGetPubkey : public ResponseBase
{
public:
	struct GetPubkeyResponsePayload parsed;
	ResponseGetPubkey(ClientMessageU& cmu);
};

class ResponseMessageAccepted : public ResponseBase
{
public:
	struct MessageConfirmationResponsePayload parsed;
	ResponseMessageAccepted(ClientMessageU& cmu);
};

class ResponseGetMessages : public ResponseBase
{
public:
	std::vector<struct GetMessagesPayloadEntry> entries;
	ResponseGetMessages(ClientMessageU& cmu);
};