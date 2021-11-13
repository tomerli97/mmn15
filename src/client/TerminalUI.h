#pragma once
#include <string>
#include <list>


class TerminalUI
{
private:
	std::string initial_message;
	std::list<std::string> options;
	void generate_option_list();
public:
	TerminalUI(std::string initial_message, std::list<std::string> options);
	int get_choice();
};

