/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xroca-pe <xroca-pe@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 19:15:53 by xroca-pe          #+#    #+#             */
/*   Updated: 2025/03/13 20:30:00 by xroca-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return EXIT_FAILURE;
    }

    int port = std::atoi(argv[1]);
    std::string password(argv[2]);

    Server server(port, password);
    if (!server.init()) {
        std::cerr << "Failed to initialize server" << std::endl;
        return EXIT_FAILURE;
    }
    
    try {
        server.run();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
    }

    return EXIT_SUCCESS;
}