#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h> // atoi()
#include <cstdlib> // exit, break, continue
#include <sys/socket.h> // socket();
#include <sys/types.h> // bind() - asociar direcciones al socket
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_addr() - convert ip string to binary
#include <unistd.h> // close()
#include <poll.h> // poll()

#define MAX_CLIENTS 5  // Número máximo de conexiones en espera

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Bad arguments: ./ircserv <port> <password>" << std::endl;
		return(1);
	}
	int port = atoi(argv[1]);
	if (port < 0 || port > 65536)
	{
		std::cout << "Error: invalid port" << std::endl;
		return (1);
	}

	//SOCKET
	int server_fd;
    struct sockaddr_in address = {};
    socklen_t addrlen = sizeof(address);

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		std::cout << "Error al crear el Socket" << std::endl;
		return(1);
	}

	//SOCKETS ADDRESS
	//struct sockaddr_in address = {};

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY; // todas las direcciones <----- set IP
	// address.sin_addr.s_addr = inet_addr("172.0.0.1");

	// ASOCIAR PARAMETROS AL SOCKET
	int bind_ret;

	bind_ret = bind(server_fd, (struct sockaddr *) &address, sizeof(address));
	if (bind_ret < 0)
	{
		std::cout << "Error al assignar direccion al socket con bind()" << std::endl;
		return(1);
	}

	
	// ESCUCHANDO CONEXIONES ENTRANTES
	if (listen(server_fd, MAX_CLIENTS) < 0)
	{
		std::cout << "Error: listen function crash" << std::endl;
		return 1;
	}
	
	std::cout << "Socket escuchando en el puerto " << port << "..." << std::endl;
	
	std::vector<pollfd> poll_fds;
	pollfd server_poll;

	server_poll.fd = server_fd;
	server_poll.events = POLLIN;
	poll_fds.push_back(server_poll);
	
	while (1)
	{
		int activity = poll(poll_fds.data(), poll_fds.size(), -1);
		if (activity < 0)
		{
			std::cout << "Error: poll function crash" << std::endl;
			break;
		}

		if (poll_fds[0].revents && POLLIN)
		{
			int new_socket = accept(server_fd, (struct sockaddr*) &address, &addrlen);
			if (new_socket < 0)
			{
				std::cerr << "Error en accept()\n";
				continue;
			}

			std::cout << "Nuevo cliente conectado\n";

			pollfd client_poll;
			client_poll.fd = new_socket;
			client_poll.events = POLLIN;
			poll_fds.push_back(client_poll);

			std::string welcome_msg = "Bienvenido al servidor IRC!\n";
			send(new_socket, welcome_msg.c_str(), welcome_msg.length(), 0);
		}

		for(size_t i = 1; i < poll_fds.size(); i++)
		{
			if (poll_fds[i].revents && POLLIN)
			{
				char buffer[1024] = {0};
				int valread = recv(poll_fds[i].fd, buffer, sizeof(buffer), 0);

				if (valread <= 0)
				{
					std::cout << "Cliente desconectado\n";
					close (poll_fds[i].fd);
					poll_fds.erase(poll_fds.begin() + i);
					i--;
				}
				else
				{
					std::cout << "Mensaje recibido: " << buffer;
					send(poll_fds[i].fd, "Mensaje enviado correctamente\n", 30, 0);
				}
			}
		}

		sleep(1);
	}

	close(server_fd);

    return 0;
}
