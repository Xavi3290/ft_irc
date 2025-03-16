# ft_irc

#### **📌 Librerías necesarias**
```c++
#include <iostream>      // Para std::cout y std::cerr
#include <cstdlib>       // Para exit()
#include <cstring>       // Para memset()
#include <arpa/inet.h>   // Para sockaddr_in, inet_addr()
#include <sys/socket.h>  // Para socket(), setsockopt(), bind()
#include <unistd.h>      // Para close()
```

#### 📌 Dominios de comunicación

| PF_INET  | Procesos que se comunican<br>usando una red IPv4 |
| -------- | ------------------------------------------------ |
| PF_INET6 | Procesos que se comunican<br>usando una red IPv6 |
#### 📌 Tipos de sockets

En el dominio PF_INET se definen los siguientes tipos de sockets:
- Sockets Stream (TCP)
- Sockets Datagram (UDP)
- Sockets Raw (IP y Red)

#### 📌 Creación de sockets
Función para crear sockets:
```c++
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int socket(int domain, int type, int protocol);
```
- `sockfd`: Identificador de socket. Se utilizará para conectarse, recibir
conexiones, enviar y recibir datos, etc.
- `domain`: Dominio donde se realiza la conexión. Para esta práctica, el dominio
será siempre `PF_INET`.
- `type`: Se corresponde con el tipo de socket que se va a crear, y puede tomar los
siguientes valores (definidos como constantes en las librerías): `SOCK_STREAM,
`SOCK_DGRAM`
- `protocol`: Indica el protocolo que se va a utilizar. El valor 0 indica que
seleccione el protocolo más apropiado (TCP para SOCK_STREAM, UDP para
SOCK_DGRAM).

Creación del socket:
```c++
server_fd = socket(AF_INET, SOCK_STREAM, 0);
if (server_fd == -1) {
    std::cerr << "Error al crear el socket\n";
    return 1;
}
```

- `socket(AF_INET, SOCK_STREAM, 0)`:
    - `AF_INET`: Usa IPv4.
    - `SOCK_STREAM`: TCP (conexión orientada).
    - `0`: Usa el protocolo por defecto para TCP.
- Si `socket()` devuelve `-1`, significa que hubo un error.

#### 📌 DIRECCIÓN DE UN SOCKET

Estructuras a utilizar:
```c++
struct sockaddr
{
unsigned short sa_family; // AF_*
char sa_data[14]; // Dirección de protocolo.
};

struct sockaddr_in
{
short int sin_family; // AF_INET
unsigned short sin_port; // Numero de puerto.
struct in_addr sin_addr; // Dirección IP.
unsigned char sin_zero[8]; // Relleno.
};

struct in_addr
{
unsigned long s_addr; // 4 bytes.
};
```

- `sin_family = AF_INET`: Indica que usaremos IPv4.
- `sin_addr.s_addr = INADDR_ANY`: Escuchará en **todas** las interfaces de red.
- `sin_port = htons(PORT)`: Convierte el puerto a **big-endian** (formato de red).


#### 📌 ASOCIACIÓN DE UN SOCKET A UNOS PARÁMETROS DETERMINADOS

```c++
int bind(int local_s, const struct sockaddr *addr, int
addrlen);

int bind(int sockfd, struct sockaddr *my_addr, int
addrlen);
```
- **`sockfd`**: Identificador de socket devuelto por la función socket().
- **`my_addr`**: Es un puntero a una estructura **`sockaddr`** que contiene la IP de la
máquina local y el número de puerto que se va a asignar al socket (esta
estructura se detalla en la siguiente sección).
- **`addrlen`**: debe estar establecido al tamaño de la estructura anterior, utilizando
para ello la función **`sizeof()`**.

Es importante destacar que todas las funciones de sockets esperan recibir como
parámetro un puntero a una estructura **`sockaddr`**, por lo que es necesario realizar una
conversión de tipos (cast) a este tipo.

Devuelve -1 en caso de que se haya producido alguna situación de error.

```c++
/*struct sockaddr_in address = {};

address.sin_family = AF_INET;
address.sin_port = htons(PORT);
address.sin_addr.s_addr = INADDR_ANY;*/

int ret;
ret = bind (server_fd, (struct sockaddr *)&sin, sizeof (sin));

if(ret < 0)
	perror(“Error binding the socket”);
```
