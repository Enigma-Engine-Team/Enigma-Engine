#include "network/client.h"
#include "debug/log.h"
#include "network/frame.h"

int Client::Create(std::string ip, int port)
{
	if (enet_initialize() != 0)
	{
		Debug::LogError("ENet initilization failed");
		return 1;
	}

	client = enet_host_create(nullptr, 1, 2, 0, 0);

	if (client == nullptr)
	{
		Debug::LogError("Enet failed to create client");
		return 1;
	}

	ENetAddress address;
	enet_address_set_host(&address, ip.c_str());
	address.port = port;

	peer = enet_host_connect(client, &address, 2, 0);
	if (peer == nullptr)
	{
		Debug::LogError("ENet failed to connect client to server");
		return 1;
	}

	ENetEvent eventConnect;
	if (enet_host_service(client, &eventConnect, 1000) > 0 && eventConnect.type == ENET_EVENT_TYPE_CONNECT)
	{
		Debug::LogSuccess("Client connected");
	}
	else
	{
		enet_peer_reset(peer);
		Debug::LogWarning("Client not connected");
		return 1;
	}

	ENetEvent eventReceive;
	if (enet_host_service(client, &eventReceive, 100) > 0 && eventReceive.type == ENET_EVENT_TYPE_RECEIVE)
	{
		struct ClientDatas { int index; int update; };
		ClientDatas clientDatas;
		memcpy(&clientDatas, eventReceive.packet->data, sizeof(ClientDatas));
		id = clientDatas.index;
		timeUpdate = clientDatas.update;
	}
	else
	{
		enet_peer_reset(peer);
		Debug::LogWarning("Client does not have id");
		return 1;
	}

	ReportStatus(PRESENT);
	start = std::chrono::high_resolution_clock::now();

	return 0;
}

void Client::Destroy()
{
	if (client)
	{
		ReportStatus(ABSENT);

		enet_host_destroy(client);
	}
}

void Client::ReportStatus(EStatus status)
{
	Frame frame;
	frame.WriteUInt8_t(id);
	frame.WriteUInt8_t(status);
	Send(frame, RELIABLE);
}

void Client::AddMessage(Message& message)
{
	if (message.reliability == RELIABLE)
	{
		reliableMessages.push_back(message);
	}
	else
	{
		unreliableMessages.push_back(message);
	}
}

void Client::Send(Frame& frame, EReliability reliability)
{
	ENetPacket* packet = enet_packet_create(frame.GetData(), frame.GetSize(), reliability);

	if (enet_peer_send(peer, reliability, packet) < 0)
	{
		Debug::LogError("Failed to send packet");
		enet_packet_destroy(packet);
	}

	enet_host_flush(client);
}

bool Client::Receive(Frame& frame)
{
	ENetEvent event;
	if (enet_host_service(client, &event, 0) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE)
	{
		frame.LoadFromPacket(event.packet->data, event.packet->dataLength);
		enet_packet_destroy(event.packet);
		return true;
	}
	return false;
}

void Client::SendAllMessages()
{
	if (reliableMessages.size() > 0)
	{
		Frame frame;
		frame.WriteUInt8_t(id);

		for (Message& message : reliableMessages)
		{
			AddMessageToFrame(frame, message.type, message.data);
		}
		Send(frame, EReliability::RELIABLE);
	}

	if (unreliableMessages.size() > 0)
	{
		Frame frame;
		frame.WriteUInt8_t(id);

		for (Message& message : unreliableMessages)
		{
			AddMessageToFrame(frame, message.type, message.data);
		}
		Send(frame, EReliability::UNRELIABLE);
	}
}

void Client::AddMessageToFrame(Frame& frame, uint8_t type, std::variant<int*, float*, Math::Vector3D*, Math::Quaternion*> data)
{
	frame.WriteUInt8_t(type);

	if (std::holds_alternative<int*>(data))
	{
		frame.WriteInt(*std::get<int*>(data));
	}
	else if (std::holds_alternative<float*>(data))
	{
		frame.WriteFloat(*std::get<float*>(data));
	}
	else if (std::holds_alternative<Math::Vector3D*>(data))
	{
		Math::Vector3D* vec3 = std::get<Math::Vector3D*>(data);
		frame.WriteVector3(vec3->x, vec3->y, vec3->z);
	}
	else if (std::holds_alternative<Math::Quaternion*>(data))
	{
		Math::Quaternion* quat = std::get<Math::Quaternion*>(data);
		frame.WriteQuaternion(quat->x, quat->y, quat->z, quat->w);
	}
}

void Client::RemoveMessage(Message& message)
{
	if (message.reliability == RELIABLE)
	{
		auto it = reliableMessages.begin();
		for (Message& m : reliableMessages)
		{
			if (m.type == message.type)
			{
				reliableMessages.erase(it);
				break;
			}
			++it;
		}
	}
	else if (message.reliability == UNRELIABLE)
	{
		auto it = unreliableMessages.begin();
		for (Message& m : unreliableMessages)
		{
			if (m.type == message.type)
			{
				unreliableMessages.erase(it);
				break;
			}
			++it;
		}
	}
}

void Client::SendUniqueMessage(Message& message)
{
	Frame frame;
	frame.WriteUInt8_t(id);
	AddMessageToFrame(frame, message.type, message.data);
	Send(frame, message.reliability);
}

int Client::GetID()
{
	return id;
}

void Client::SendToServer()
{
	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	if (duration.count() > timeUpdate)
	{
		start = std::chrono::high_resolution_clock::now();
		SendAllMessages();
	}
}

void Client::SetNickname(std::string _nickname)
{
	nickname = _nickname;
}

std::string Client::GetNickname()
{
	return nickname;
}