#include "cloud/CloudManager.hpp"

#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>

std::string	CloudManager::parseMultipartNameFile_(const std::string& postBody)
{
	std::istringstream	iss(postBody);
	std::string			line;

	while (std::getline(iss, line))
	{
		size_t	fileNamePos = line.find("filename=\"");

		if (fileNamePos !=  std::string::npos)
		{
			size_t	start = fileNamePos + 10;
			size_t	end = line.find("\"", start);

			if (end != std::string::npos)
				return (line.substr(start, end - start));
		}
	}

	return ("fichier_inconnu.bin");
}

std::string	CloudManager::parsePureFileContent(const std::string& postBody)
{
	size_t	firstCrLf = postBody.find("\r\n");
	if (firstCrLf == std::string::npos)
		return ("");
	std::string	boundary = postBody.substr(0, firstCrLf);

	size_t	dataStart = postBody.find("\r\n\r\n");
	if (dataStart == std::string::npos)
		return ("");
	dataStart += 4;

	size_t	dataEnd = postBody.find("\r\n" + boundary, dataStart);
	if (dataEnd == std::string::npos)
		return (""); 
	
	std::string	pureFileContent = postBody.substr(dataStart, dataEnd - dataStart);
	return (pureFileContent);
}

bool	CloudManager::saveFileOnDisk_(const std::string& filePath, const std::string& fileContent)
{
	std::ofstream	file(filePath.c_str(), std::ios::binary);
	if (!file.is_open())
		return (false);

	file.write(fileContent.c_str(), fileContent.length());
	file.close();

	return (true);
}

bool	CloudManager::deleteFileOnDisk_(const std::string& filePath)
{
	if (std::remove(filePath.c_str()) == 0)
		return (true);
	return (false);
}

bool	CloudManager::processUpload(const std::string& postBody, const std::string& saveDirectory)
{
	std::string fileName = parseMultipartNameFile_(postBody);
	std::string	pureFileContent = parsePureFileContent(postBody);

	if (pureFileContent.empty() || fileName.empty())
		return (false); 

	std::string dirPath = saveDirectory;
	if (!dirPath.empty() && dirPath[dirPath.length() - 1] == '/')
		dirPath.erase(dirPath.length() - 1);

	mkdir(dirPath.c_str(), 0777);

	std::string	fullPath = saveDirectory + fileName;
	return (saveFileOnDisk_(fullPath, pureFileContent));
}

bool	CloudManager::processDelete(std::string targetPath, const std::string& saveDirectory)
{
	if (!targetPath.empty() && targetPath[0] == '/')
		targetPath.erase(0, 1);

	std::string	fullPath = saveDirectory + targetPath;
	return (deleteFileOnDisk_(fullPath));
}

std::string	CloudManager::getFileList(const std::string& directory)
{
	std::string	htmlList = "";
	DIR*	dir = opendir(directory.c_str());

	if (dir == NULL)
		return ("<li style='color: red;'>Erreur: Impossible d'ouvrir le dossier.</li>");

	struct dirent*	entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string	fileName = entry->d_name;
		if (fileName != "." && fileName != "..")
		{
			
			htmlList.append(
				"<li style=\"margin-bottom: 10px;\">"
				"<a href=\"/" + fileName + "\" style=\"color: #8be9fd; text-decoration: none;\">" + fileName + "</a>"
				"<button onclick=\"fetch('/" + fileName + "', {method: 'DELETE'}).then(()=>location.reload())\" style=\"color: #ff5555; background: none; border: none; cursor: pointer; font-weight: bold; margin-left: 15px;\">[X]</button>"
			);
		}
	}
	closedir(dir);

	if (htmlList.empty())
		return ("<li>Le dossier est vide.</li>");


	return (htmlList);
}