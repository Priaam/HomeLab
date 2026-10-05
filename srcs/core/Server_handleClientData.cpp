#include "core/Server.hpp"
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"
#include "cloud/cloudManager.hpp"

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cerrno>

const ServerConfig&	Server::getClientConfig_(int clientFd, const HttpRequest& request) const
{
	int					targetServerFd = clientToServer_.at(clientFd);
	const ServerConfig*	defaultConf = NULL;

	std::string	requestHost = request.getHost();

	for (size_t i = 0; i < serverFds_.size(); i++)
	{
		if (serverFds_[i] == targetServerFd)
		{
			const ServerConfig&	conf = config_.getServers()[i];

			if (defaultConf == NULL)
				defaultConf = &conf;
			
			if (requestHost.find(conf.serverName) != std::string::npos)
				return (conf);
		}
	}

	return (*defaultConf);
}

void	Server::processRequest_(int clientFd)
{
	std::string	rawData = clientStates_.at(clientFd).buffer;
	HttpRequest	request(rawData);
	HttpResponse response;

	const ServerConfig& currentConfig = getClientConfig_(clientFd, request);

	if (request.getMethod() == "GET")
		handleGetRequest_(request, response, currentConfig);
	else if (request.getMethod() == "POST")
		handlePostRequest_(clientFd, request, response, currentConfig);
	else if (request.getMethod() == "DELETE")
		handleDeleteRequest_(request, response, currentConfig);
	else
		servError_(405, response, currentConfig);

	std::string	finalText = response.generateResponse();
	send(clientFd, finalText.c_str(), finalText.length(), 0);
}

void	Server::closeClientConnection_(size_t pollIndex)
{
	int	clientFd = pollFds_[pollIndex].fd;

	close(clientFd);
	clientToServer_.erase(clientFd);
	clientStates_.erase(clientFd);
	pollFds_.erase(pollFds_.begin() + pollIndex);
}

size_t	Server::extractHeaderContentLength_(const std::string& header)
{
	std::istringstream	iss(header);
	std::string			line;

	std::string			contentLengthValue;
	
	while (std::getline(iss, line))
	{
		if (line.find("Content-Length:") != std::string::npos)
		{
			size_t	colonPos = line.find(":");
			
			std::string	valueStr = line.substr(colonPos + 1);
			return (std::atoi(valueStr.c_str()));
		}
	}
	return (0);
}

void	Server::handleClientData_(size_t pollIndex)
{
	int				currentClientFd = pollFds_[pollIndex].fd;
	ClientState&	state = clientStates_[currentClientFd];

	char			buffer[16384] = {0};

	int		bytesRead = recv(currentClientFd, buffer, sizeof(buffer) - 1, 0);

	if (bytesRead > 0)
	{
		state.buffer.append(buffer, bytesRead);

		if (!state.headerParsed)
		{
			if (state.buffer.find("\r\n\r\n") != std::string::npos)
			{
				state.headerParsed = true;
				HttpRequest whatMethod(state.buffer);

				if (whatMethod.getMethod() == "POST")
				{
					state.contentLength = extractHeaderContentLength_(state.buffer);

					if (state.contentLength > 0)
						state.buffer.reserve(state.buffer.length() + state.contentLength + 1024);
				}
				else
					state.isComplete = true;
			}
		}
		if (state.headerParsed && !state.isComplete)
		{
			size_t	headerEndPos = state.buffer.find("\r\n\r\n");
			size_t	totalExpectedSize = (headerEndPos + 4) + state.contentLength;
			
			if (state.buffer.length() >= totalExpectedSize)
				state.isComplete = true;
		}
		if (state.isComplete)
		{
			processRequest_(currentClientFd);
			closeClientConnection_(pollIndex);
		}
	}
	else if (bytesRead < 0)
    {
        // Si le système dit juste "reviens plus tard, je suis occupé", on ignore et on attend le prochain poll()
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
            
        std::cerr << "[SERVER] Error recv() on the client " << currentClientFd << " (errno: " << errno << ")" << std::endl;
        closeClientConnection_(pollIndex);
    }
	else
	{
		if (bytesRead == 0)
			std::cout << "[SERVER] Client " << currentClientFd << " disconnected by himself." << std::endl;
		else
			std::cerr << "[SERVER] Error recv() on the client " << currentClientFd << std::endl;
		closeClientConnection_(pollIndex);
	}
}