#include "network/server.h"
#include "network/client.h"
#include "debug/log.h"
#include "network/frame.h"

void Server::Create(int port, int fps)
{
	if (enet_initialize() != 0)
	{
		Debug::LogError("ENet initilization failed");
		return;
	}

	address.host = ENET_HOST_ANY;
	address.port = port;

	server = enet_host_create(&address, 4, 2, 0, 0);

	if (server == nullptr)
	{
		Debug::LogError("Enet failed to create server");
		return;
	}
	Debug::LogSuccess("Server created");
	timeUpdate = 1000.f / fps;
}

void Server::Destroy()
{
	if (server)
	{
		enet_host_destroy(server);
	}

	enet_deinitialize();
}

void Server::Update()
{
	ENetEvent event;

	while (!shouldClose)
	{
		while (enet_host_service(server, &event, 0) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				Debug::LogSuccess("New client connected");
				struct ClientDatas { int index; int update; };
				ClientDatas clientDatas = { peers.size(), timeUpdate };
				ENetPacket* packetID = enet_packet_create(&clientDatas, sizeof(clientDatas), ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(event.peer, 0, packetID);
				peers.push_back(event.peer);
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				if (event.peer == nullptr)
				{
					enet_host_broadcast(server, 0, event.packet);
				}
				else
				{
					for (ENetPeer* peer : peers)
					{
						if (peer == event.peer)
						{
							continue;
						}

						if (enet_peer_send(peer, 0, event.packet) < 0)
						{
							printf("Error sending\n");
						}
					}
					enet_host_flush(server);
				}
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				printf("A client disconnected\n");
				break;
			}
			default:
				break;
			}
		}
	}
}

void Server::Close()
{
	shouldClose = true;
}