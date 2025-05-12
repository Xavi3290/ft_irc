#include "../../inc/NumericReplies.hpp"
#include <sstream>
#include <iomanip>

std::string NumericReplies::reply(int code, const std::string &nick, const std::string &params, const std::string &message)
{
    std::ostringstream oss;

    // Prefijo del servidor
    oss << ":irc.42.localhost ";

    // Código con 3 cifras
    oss << std::setw(3) << std::setfill('0') << code << " ";

    // Nick del destinatario
    oss << (nick.empty() ? "*" : nick);

    // Parámetros intermedios (como canal o nick destino)
    if (!params.empty())
        oss << " " << params;

    // Mensaje final con prefijo ":"
    if (!message.empty())
        oss << " :" << message;

    // Fin de línea IRC
    oss << "\r\n";

    return oss.str();
}
