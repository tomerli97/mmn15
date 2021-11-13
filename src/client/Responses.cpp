#include "Responses.h"
#include <iostream>

ResponseBase::ResponseBase(ClientMessageU &cmu)
	: data(cmu.srv_rcv(sizeof(this->header))),
	payload()
{
	// Copy header
	memcpy(&this->header, data.data(), sizeof(this->header));

	// Get payload
	if (this->header.payload_size != 0) {
		this->payload = cmu.srv_rcv(this->header.payload_size);
	}
}

ResponseRegister::ResponseRegister(ClientMessageU& cmu)
	: ResponseBase{ cmu },
	parsed()
{
	// Parse payload
	memcpy(&this->parsed, this->payload.data(), sizeof(this->parsed));
}

ResponseListClients::ResponseListClients(ClientMessageU& cmu)
	: ResponseBase{ cmu },
	entries()
{
	struct GetUsersPayloadEntry raw_entry;
	size_t num_entries = this->payload.size() / sizeof(struct GetUsersPayloadEntry);
	for (int i = 0; i < num_entries; i++) {
		memcpy(&raw_entry, &this->payload.data()[i * sizeof(raw_entry)], sizeof(raw_entry));
		this->entries.push_back(raw_entry);
	}
}

ResponseGetPubkey::ResponseGetPubkey(ClientMessageU& cmu)
	: ResponseBase{ cmu },
	parsed()
{
	// Parse payload
	memcpy(&this->parsed, this->payload.data(), sizeof(this->parsed));
}

ResponseMessageAccepted::ResponseMessageAccepted(ClientMessageU& cmu)
	: ResponseBase{ cmu },
	parsed()
{
	// Parse payload
	memcpy(&this->parsed, this->payload.data(), sizeof(this->parsed));
}

ResponseGetMessages::ResponseGetMessages(ClientMessageU& cmu)
	: ResponseBase{ cmu },
	entries()
{
	struct GetMessagesPayloadEntry raw_entry;
	size_t offset = 0;
	while (offset < this->payload.size()) {
		memcpy(&raw_entry.header, &this->payload.data()[offset], sizeof(raw_entry.header));
		offset += sizeof(raw_entry.header);
		raw_entry.data.clear();
		raw_entry.data.insert(raw_entry.data.end(), this->payload.begin() + offset, this->payload.begin() + offset + raw_entry.header.payload_size);
		offset += raw_entry.header.payload_size;
		this->entries.push_back(raw_entry);
	}
}
