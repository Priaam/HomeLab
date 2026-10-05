#include "http/HttpRequest.hpp"

#include <sstream>
#include <iostream>

HttpRequest::HttpRequest(const std::string& rawData)
{
	std::istringstream	stream(rawData);
	stream >> method_ >> path_ >> version_;

	size_t hostPos = rawData.find("Host: ");
    if (hostPos != std::string::npos)
	{
        size_t endPos = rawData.find("\r\n", hostPos);
        host_ = rawData.substr(hostPos + 6, endPos - (hostPos + 6));
    }
	else
        host_ = "";

	std::cout << "[PARSING] Requete traduite -> Methode: " << method_ << " | Chemin: " << path_ << std::endl;
}

HttpRequest::~HttpRequest()
{}

std::string HttpRequest::getMethod() const {return (method_);}
std::string HttpRequest::getPath() const {return (path_);}
std::string HttpRequest::getHost() const {return (host_);}
