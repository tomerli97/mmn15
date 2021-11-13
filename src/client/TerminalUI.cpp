#include "TerminalUI.h"
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

void TerminalUI::generate_option_list()
{	
	cout << this->initial_message << endl;
	for (std::string line : this->options) {
		cout << line << endl;
	}
	cout << "> ";
}

TerminalUI::TerminalUI(std::string initial_message, std::list<std::string> options)
	: initial_message(initial_message),
	options(options)
{

}

int TerminalUI::get_choice()
{
	int choice = 0;
	this->generate_option_list();
	cin >> choice;
	return choice;
}
