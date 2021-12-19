#pragma once
#include<string>
class DebugView {
public:
	std::string error[2] = { "Can't load WSA library!", "Server connection failed!" };
	std::string result[1] = { "Client connection succesful!" };
	std::string msgs[1] = { "Succesfuly connected to the server!" };
};