#pragma once
#include<string>
class DebugView	{
	public:
		std::string error[3] = { "Can't load WSA library!", "User Can't be connected!", "Can't find important files!" };
		std::string result[2] = { "Client connection succesful!", "Listening socket created, waiting for clients..." };
		std::string msgs[1] = { "Welcome to the club buddy!" };
};