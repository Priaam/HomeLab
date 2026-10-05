#include "config/Config.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

Config::Config(const std::string& filename)
{
	parseFile_(filename);
}

Config::~Config()
{}

const std::vector<ServerConfig>& Config::getServers() const
{
    return servers_;
}

void	Config::processLine_(const std::string& line, bool& inServerBlock, ServerConfig& currentServer)
{
	std::istringstream 	iss(line);
	std::string			keyword;

	if (!(iss >> keyword))
        return;
	
	if (keyword[0] == '#')
		return;
	
	if (keyword == "server")
	{
		currentServer.port = 8080;
		currentServer.serverName = "localhost";
        currentServer.rootPath = "www/";
        currentServer.indexName = "index.html";
		currentServer.uploadDir = "";
		currentServer.errorPages.clear();
        return;
	}
	if (keyword == "{")
	{
		inServerBlock = true;
		return;
	}
	if (keyword == "}")
	{
		if (!inServerBlock)
			throw
				std::runtime_error("Error Config : '}' without '{' .");
		servers_.push_back(currentServer);
		inServerBlock = false;
		return;
	}
	
	if (inServerBlock)
		parseDirective_(currentServer, keyword, iss);
	else
		throw std::runtime_error("Erreur : Instruction hors d'un bloc 'server {}' -> " + keyword);
}

void	Config::cleanValue_(std::string& value)
{
	if (!value.empty() && value[value.length() - 1] == ';')
		value.erase(value.length()- 1);
}

void Config::parseDirective_(ServerConfig& currentServer, const std::string& keyword, std::istringstream& iss)
{
	std::string		value;
	if (!(iss >> value))
		throw
			std::runtime_error("Error Config : Missing value for the directive -> " + keyword);

	cleanValue_(value);

	if (keyword == "port")
	{
		currentServer.port = std::atoi(value.c_str());
		if (currentServer.port <= 0 || currentServer.port > 65535)
            throw
				std::runtime_error("Error Config : Invalid port.");
	}
	else if (keyword == "serverName") 
    {
        currentServer.serverName = value;
    }
	else if (keyword == "root")
	{
		if (value.empty())
			throw
				std::runtime_error("Error Config : The 'root' path can't be set.");
		currentServer.rootPath = value;
	}
	else if (keyword == "indexName")
	{
		if (value.empty())
			throw
				std::runtime_error("Error Config : The 'indexName' path can't be set.");
		currentServer.indexName = value;
	}
	else if (keyword == "uploadDir")
    {
        if (value.empty())
            throw std::runtime_error("Error Config : The 'uploadDir' path can't be set.");
        currentServer.uploadDir = value;
    }
	else if (keyword == "error_page") 
    {
        int errorCode = std::atoi(value.c_str());
        std::string errorFile;
        if (!(iss >> errorFile))
            throw std::runtime_error("Error Config : Missing file for error_page.");
        cleanValue_(errorFile);
        currentServer.errorPages[errorCode] = errorFile;
    }
	else
		throw
			std::runtime_error("Error Config : Unknown directive -> " + keyword);
}

void	Config::parseFile_(const std::string& filename)
{
	std::ifstream	file(filename.c_str());
	if (!file.is_open())
		throw
			std::runtime_error("Erreur Config : Impossible d'ouvrir le fichier de configuration.");

	std::string 	line;
	bool			inServerBlock = false;
	ServerConfig	currentServer;

	while (std::getline(file, line))
		processLine_(line, inServerBlock, currentServer);

	file.close();

	if (inServerBlock)
        throw std::runtime_error("Error Config: A 'server' block was not closed with '}'.");
	if (servers_.empty())
        throw std::runtime_error("Error Config : No 'server' block found in the file.");

	std::cout << "[CONFIG] Parsing successful. Number of servers configured : " << servers_.size() << std::endl;
}