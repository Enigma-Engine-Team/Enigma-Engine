#pragma once

#include <enet/enet.h>
#include <string>
#include <variant>
#include <chrono>
#include "emath/vector.h"
#include "emath/quaternion.h"
#include "../utilities/macro.h"

class Frame;

class ENIGMA_API Client
{
public:
	enum EReliability : int
	{
		UNRELIABLE,
		RELIABLE
	};

	enum EStatus : int
	{
		PRESENT,
		ABSENT
	};

	struct Message
	{
		uint8_t type;
		std::variant<int*, float*, Math::Vector3D*, Math::Quaternion*> data;
		EReliability reliability;
	};

	Client() = default;
	~Client() = default;

	int Create(std::string ip, int port);
	void Destroy();

	void ReportStatus(EStatus status);
	void AddMessage(Message& message);
	bool Receive(Frame& frame);
	void SendUniqueMessage(Message& message);
	void RemoveMessage(Message& message);
	int GetID();
	void SetNickname(std::string nickname);
	std::string GetNickname();
	void SendToServer();

private:
	ENetHost* client = nullptr;
	ENetPeer* peer = nullptr;
	int id = 0;
	int timeUpdate = 0;
	std::chrono::steady_clock::time_point start;
	std::vector<Message> reliableMessages;
	std::vector<Message> unreliableMessages;
	std::string nickname;

	void AddMessageToFrame(Frame& frame, uint8_t type, std::variant<int*, float*, Math::Vector3D*, Math::Quaternion*> data);
	void Send(Frame& frame, EReliability reliability);
	void SendAllMessages();
};