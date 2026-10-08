#include "core/Server.hpp"
#include "cloud/CloudManager.hpp"
#include "http/HttpUtils.hpp"

#include <iostream>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <cstdlib>

void	Server::handleGetRequest_(const HttpRequest& request, HttpResponse& response, const ServerConfig& config)
{
	if (request.getPath() == "/")
	{
		std::string	fullPath = config.rootPath + config.indexName;
		response.setBodyFromFile(fullPath);

		if (config.indexName == "cloud.html")
		{
			std::string fileList = CloudManager::getFileList(config.uploadDir);
			response.replaceInBody("{{FILE_LIST}}", fileList);
		}
		response.setStatusCode(200);
		std::cout << "[SERVER] Sending " << config.indexName << " page to the client " << std::endl;
	}
	else
	{
		std::string	target = request.getPath();
		if (!target.empty() && target[0] == '/')
			target.erase(0, 1);

		std::string	fullPath = "";
		if (!config.uploadDir.empty())
			fullPath = config.uploadDir + target;
		else
			fullPath = config.rootPath + target;

		response.setBodyFromFile(fullPath);

		if (response.getStatusCode() == 404)
			servError_(404, response, config);
		else
		{
			response.setStatusCode(200);
			std::string	extension = HttpUtils::getExtensionFile(fullPath);
			std::string	mimeType = HttpUtils::getMimeType(extension);
			response.setContentType(mimeType);
		}
	}
}

void	Server::handlePostRequest_(int clientFd, const HttpRequest& request, HttpResponse& response, const ServerConfig& config)
{
	(void)request;

	if (config.uploadDir.empty())
    {
        servError_(405, response, config);
        return;
    }

	std::cout << "[SERVER] Processing a POST request..." << std::endl;

	std::string	rawData = clientStates_.at(clientFd).buffer;
	size_t		headerEndPos = rawData.find("\r\n\r\n");

	if (headerEndPos != std::string::npos)
	{
		std::string	fileBody = rawData.substr(headerEndPos + 4);

		if (fileBody.find("filename=\"\"") != std::string::npos)
        {
            std::cerr << "[SERVER] Requete bloquee : Aucun fichier selectionne." << std::endl;
            servError_(400, response, config);
            return;
        }

		if (CloudManager::processUpload(fileBody, config.uploadDir))
		{
			response.setStatusCode(200);
			response.setBody("<h1>Upload successful!</h1>");
		}
		else
		{
			servError_(500, response, config);
			std::cerr << "[SERVER] Failed to write to disk." << std::endl;
		}
	}
}

void	Server::handleDeleteRequest_(const HttpRequest& request, HttpResponse& response, const ServerConfig& config)
{
	if (config.uploadDir.empty())
    {
        servError_(405, response, config);
        return;
    }

	std::cout << "[SERVER] Processing a DELETE request..." << std::endl;

	if (CloudManager::processDelete(request.getPath(), config.uploadDir))
	{
		response.setStatusCode(200);
		response.setBody("<h1>200 - Fichier supprime avec succes</h1>");
    }
	else
		servError_(404, response, config);
}

void	Server::servError_(int code, HttpResponse& response, const ServerConfig& config)
{
	response.setStatusCode(code);

	if (config.errorPages.count(code) > 0)
	{
		std::string errorFilePath = config.rootPath + config.errorPages.at(code);
		response.setBodyFromFile(errorFilePath);
	}
	else
	{
		std::ostringstream oss;
        oss << "<h1>" << code << " - Error</h1>";
        response.setBody(oss.str());
	}
}