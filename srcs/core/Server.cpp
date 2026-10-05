#include "core/Server.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>

Server::Server(const Config& config) : config_(config)
{
	setupSockets_();
	bindAndListen_();
}

Server::~Server()
{
	for (size_t i = 0; i < serverFds_.size(); i++) {
        if (serverFds_[i] != -1)
            close(serverFds_[i]);
    }
	std::cout << "[SERVER] All listening sockets are closed." << std::endl;
}

void	Server::setupSockets_()
{
	const std::vector<ServerConfig>& configs = config_.getServers();

	for (size_t i = 0; i < configs.size(); i++)
	{
		int	fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0)
			throw
				std::runtime_error("[SERVER] Error: Unable to create a socket.");
		
		int opt = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		{
			close(fd);
			throw
				std::runtime_error("[SERVER] Error: fcntl failed on a socket.");
		}

		serverFds_.push_back(fd);
	}
}

void Server::bindAndListen_()
{
    const std::vector<ServerConfig>& configs = config_.getServers();

    // On parcourt les sockets qu'on a créés dans setupSockets_
    for (size_t i = 0; i < serverFds_.size(); i++)
    {
        int currentFd = serverFds_[i];
        int currentPort = configs[i].port;

        struct sockaddr_in address;
        memset(&address, 0, sizeof(address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(currentPort);

        if (bind(currentFd, (struct sockaddr *)&address, sizeof(address)) < 0)
            throw
				std::runtime_error("[SERVER] Erreur: Impossible de bind le port (déjà utilisé ?)");

        if (listen(currentFd, 10) < 0)
            throw
				std::runtime_error("[SERVER] Erreur: Impossible d'écouter le port");

        // Ajout du socket au tableau de poll() pour écouter les connexions entrantes
        struct pollfd serverPoll;
        serverPoll.fd = currentFd;
        serverPoll.events = POLLIN;
        serverPoll.revents = 0;
        pollFds_.push_back(serverPoll);

        std::cout << "[SERVER] Démarré, écoute sur le port : " << currentPort << std::endl;
    }
}

void	Server::acceptNewClient_(int serverFd)
{
	struct sockaddr_in clientAddress;
	socklen_t addrLen = sizeof(clientAddress);

	int clientFd = accept(serverFd, (struct sockaddr *)&clientAddress, &addrLen);

	if (clientFd < 0)
	{
		std::cerr << "[SERVER] Erreur sur accept(), impossible de connecter le client." << std::endl;
		return;
	}
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "[SERVER] Erreur fcntl sur le client" << std::endl;
        close(clientFd);

        return;
	}

	clientToServer_[clientFd] = serverFd;

	ClientState	newState;
	newState.buffer = "";
	newState.headerParsed = false;
	newState.contentLength = 0;
	newState.isComplete = false;

	clientStates_[clientFd] = newState;
	
	std::cout << "[SERVER] Client " << clientFd << " connected on the server : " << serverFd << std::endl;

	struct pollfd	clientPoll;
	clientPoll.fd = clientFd;
	clientPoll.events = POLLIN;
	clientPoll.revents = 0;

	pollFds_.push_back(clientPoll);
}

void	Server::run()
{
	while (true)
	{
		int	pollResult = poll(&pollFds_[0], pollFds_.size(), -1);

		if (pollResult < 0)
		{
			std::cerr << "[SERVER] Erreur critique sur poll()" << std::endl;
			break;
		}

		for (size_t i = 0; i < pollFds_.size(); i++)
		{
			if (pollFds_[i].revents & POLLIN)
			{
				bool isNewConnection = false;
				for (size_t j = 0; j < serverFds_.size(); j++)
                {
                    if (pollFds_[i].fd == serverFds_[j])
                    {
                        isNewConnection = true;
                        break;
                    }
                }

				if (isNewConnection)
                {
                    acceptNewClient_(pollFds_[i].fd);
                }
                else 
                {
                    handleClientData_(i);
                    i--;
                }
			}
		}
	}
}