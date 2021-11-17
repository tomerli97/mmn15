#include "cryptopp_wrapper/Base64Wrapper.h"
#include "cryptopp_wrapper/RSAWrapper.h"
#include "cryptopp_wrapper/AESWrapper.h"
#include "TerminalUI.h"
#include "Messages.h"
#include "ClientMessageU.h"
#include "configuration.h"

#include <iostream>
#include <iomanip>


#include <boost/chrono.hpp>
#include <boost/asio.hpp>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

int main()
{
	ClientMessageU cmu(get_server_address());
	int choice = 50;
	bool run = true;
	TerminalUI tui("MessageU client at your service", {
		"10) Register",
		"20) Request for clients list",
		"30) Request for public key",
		"40) Request for waiting messages",
		"50) Send a text message",
		"51) Send a request for symmetric key",
		"52) Send your symmetric key",
		" 0) Exit client",
		});
	do {
		choice = tui.get_choice();
		run = cmu.execute(choice);
	} while (run);
	
	return 0;

}

