#include "network/network.h"
#include "multithreading/async.h"
#include <iostream>
#include <fstream>

Network& Network::GetInstance()
{
	static Network network;
	return network;
}

void Network::Create(std::string path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		return;
	}

	std::string type, ip, port, nickname;
	std::getline(file, type);
	std::getline(file, ip);
	std::getline(file, port);
	std::getline(file, nickname);

	if (type == "server")
	{
		InitServer(std::stoi(port));
	}

	InitClient(ip, std::stoi(port), nickname);

	file.close();
}

void Network::Destroy()
{
	client->Destroy();

	if (server)
	{
		server->Close();
		Async::GetInstance().WaitTask(serverTask);
		server->Destroy();
	}
}

void Network::InitServer(int port)
{
	server = new Server();
	server->Create(port, 144);

	auto UpdateServer = [this]()
		{
			server->Update();
		};

	serverTask = Async::GetInstance().CreateTask(UpdateServer);
}

void Network::InitClient(std::string ip, int port, std::string nickname)
{
	client = new Client();
	client->Create(ip, port);
	client->SetNickname(nickname);
}

void Network::Send()
{
	client->SendToServer();
}

Frame Network::Receive()
{
	Frame frame;
	client->Receive(frame);
	return frame;
}

void Network::UpdateStatus(Client::EStatus status)
{
	client->ReportStatus(status);
}

void Network::Update()
{
	Send();
	currentFrame = Receive();
}

Frame Network::GetFrame()
{
	return currentFrame;
}

Client* Network::GetClient()
{
	return client;
}