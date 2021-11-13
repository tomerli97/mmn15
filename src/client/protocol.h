#pragma once
#include <cstdint>
#include <vector>
#pragma pack(push, 1)

// Requests
struct MessageHeader {
	uint8_t id[16];
	uint8_t version;
	uint16_t code;
	uint32_t payload_size;
};

struct RegistrationMessagePayload {
	char name[255];
	uint8_t pubkey[160];
};

struct GetPubkeyMessagePayload {
	uint8_t id[16];
};

struct SendMessagePayloadHeader {
	uint8_t id[16];
	uint8_t type;
	uint32_t size;
};

// Responses
struct ResponseHeader {
	uint8_t version;
	uint16_t code;
	uint32_t payload_size;
};

struct RegisterSuccessPayload {
	uint8_t id[16];
};

struct GetUsersPayloadEntry {
	uint8_t id[16];
	char name[255];
};

struct GetPubkeyResponsePayload {
	uint8_t id[16];
	uint8_t pubkey[160];
};

struct MessageConfirmationResponsePayload {
	uint8_t id[16];
	uint32_t message_id;
};

struct GetMessagesPayloadEntryHeader {
	uint8_t id[16];
	uint32_t message_id;
	uint8_t type;
	uint32_t payload_size;
};
#pragma pack(pop)

struct GetMessagesPayloadEntry {
	struct GetMessagesPayloadEntryHeader header;
	std::vector<uint8_t> data;
};
