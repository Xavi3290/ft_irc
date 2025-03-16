#include <iostream>
#include <string>
#include <stdlib.h> // atoi()
#include <sys/socket.h> // socket();
#include <sys/types.h> // bind() - asociar direcciones al socket
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_addr() - convert ip string to binary
#include <unistd.h> // close()


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
	int	server_fd;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		std::cout << "Error al crear el Socket" << std::endl;
		return(1);
	}

	//SOCKETS ADRESS
	struct sockaddr_in address = {};

	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY; // todas las direcciones <----- set IP
	// address.sin_addr.s_addr = inet_addr("172.0.0.1");

	// ASOCIAR PARAMETROS AL SOCKET
	int bind_ret;

	bind_ret = bind(server_fd, (struct sockaddr *) &address, sizeof(address));
	if (bind_ret < 0)
	{
		std::cout << "Error al assignar direccion al socke con bind()" << std::endl;
		return(1);
	}

	std::cout << "Socket creado y enlazado al puerto " << port << " correctamente." << std::endl;
	
	close(server_fd);

    return 0;
}
