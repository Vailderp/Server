#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <functional>
#include <iostream>
#include <map>
#include <vector>
#include <WinSock2.h>
#include "Error.h"
#include <thread>
#pragma warning(disable: 4996)
namespace vl
{
	class Server
	{
	private:
		SOCKET* connections_;
		u_short n_connections_{ 0 };

		const u_short hostshort_;
		const std::string& inet_addr_;

		std::map<const char*, std::function<void(const char*, SOCKET connection)>> on_functions_;
		std::function<void(const char*, SOCKET connection)> on_any_function_ = [](const char* ch, SOCKET socket){};

	public:
		explicit Server(const int n_connections = 1 << 8, const u_short hostshort = 7070, const std::string& inet_address = "127.0.0.1") :
			connections_(new SOCKET[n_connections]),
			hostshort_(hostshort),
			inet_addr_(inet_address)
		{
		}

	public:

		void start()
		{
			WSAData wsa_data;
			const WORD dll_version = MAKEWORD(2, 1);
			if (WSAStartup(dll_version, &wsa_data) != 0)
			{
				vl::err<WORD>("WORD & WSAData");
			}

			SOCKADDR_IN sockaddr_in;
			sockaddr_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
			sockaddr_in.sin_port = htons(1111);
			sockaddr_in.sin_family = AF_INET;

			SOCKET socket_listen = socket(AF_INET, SOCK_STREAM, NULL);
			int sizeof_socket_listen = sizeof(sockaddr_in);
			bind(socket_listen, (SOCKADDR*)(&sockaddr_in), sizeof_socket_listen);
			listen(socket_listen, SOMAXCONN);

			SOCKET socket_connection;

			for (int l = 0; l < 1 << 8; l++)
			{
				socket_connection = accept(socket_listen, (SOCKADDR*)(&sockaddr_in), &sizeof_socket_listen);

				if (socket_connection == 0)
				{
					vl::err<SOCKET>("CONNECTED TO SOCKET NOT ACCEPTED");
				}
				else
				{
					vl::done<SOCKET>("CONNECTED TO SOCKET ACCEPTED");

					connections_[l] = socket_connection;
					n_connections_++;

					std::thread* thread = new std::thread(
						[&, l]()
						{
							int sizeof_packet = 0;
							while (true)
							{
								if (recv(connections_[l], (char*)&sizeof_packet, sizeof(int), NULL) != -1)
								{

									char* packet = new char[sizeof_packet + 1];
									if (recv(connections_[l], packet, sizeof_packet, NULL) != -1)
									{

										packet[sizeof_packet] = '\0';
										for (const auto &it : on_functions_)
										{
											if (it.first == packet)
											{
												it.second(packet, connections_[l]);
												break;
											}
										}

										on_any_function_(packet, connections_[l]);
										
										/*for (int i = 0; i < n_connections_; i++)
										{
											if (i == l)
											{
												continue;
											}

											send(connections_[i], (char*)&sizeof_packet, sizeof(int), NULL);
											send(connections_[i], packet, sizeof_packet, NULL);
										}*/
										delete[] packet;
									}
								}
							}
						});
					thread->detach();
				}
			}
			system("pause");
		}

		void on(const char* packet, const std::function<void(const char*, SOCKET socket)>& on_function)
		{
			on_functions_[packet] = on_function;
		}

		void onAny(const std::function<void(const char*, SOCKET socket)>& on_any_function)
		{
			on_any_function_ = on_any_function;
		}

		void emit(const char* packet) const
		{
			int sizeof_packet = static_cast<int>(strlen(packet));
			for (int i = 0; i < n_connections_; i++)
			{
				send(connections_[i], (char*)&sizeof_packet, sizeof(int), NULL);
				send(connections_[i], packet, sizeof_packet, NULL);
			}
		}

		void emit(const char* packet, const SOCKET exception_connection) const
		{
			int sizeof_packet = static_cast<int>(strlen(packet));
			for (int i = 0; i < n_connections_; i++)
			{
				if (connections_[i] == exception_connection)
				{
					continue;
				}
				send(connections_[i], (char*)&sizeof_packet, sizeof(int), NULL);
				send(connections_[i], packet, sizeof_packet, NULL);
			}
		}

		u_short getNConnections() const
		{
			return n_connections_;
		}
		
	};
}
