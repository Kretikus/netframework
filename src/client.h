#pragma once

#include <string>

class Client
{
public:
	Client();
	~Client();

	bool connect(const std::string & srvname, unsigned int port);

	void sendData();

private:
	class Impl;
	Impl * _d;
};
