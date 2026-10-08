#include "http/HttpUtils.hpp"
#include <map>

std::string	HttpUtils::urlDecode(const std::string& str)
{
	size_t pos;
	std::string urlDecoded = str;

	while ((pos = urlDecoded.find("%20")) != std::string::npos)
	{
		urlDecoded.replace(pos, 3, " ");
	}
	return (urlDecoded);
}

std::string	HttpUtils::getExtensionFile(const std::string& filepath)
{
	size_t		pos = filepath.find_last_of(".");
	if (pos == std::string::npos)
		return ("");

	return (filepath.substr(pos));
}

std::string	HttpUtils::getMimeType(const std::string& extension)
{
	static std::map<std::string, std::string>	mimeTypes;
	if (mimeTypes.empty())
	{
		mimeTypes[".html"] = "text/html";
        mimeTypes[".css"]  = "text/css";
        mimeTypes[".jpg"]  = "image/jpeg";
        mimeTypes[".jpeg"] = "image/jpeg";
        mimeTypes[".png"]  = "image/png";
        mimeTypes[".pdf"]  = "application/pdf";
        mimeTypes[".txt"]  = "text/plain";
	}

	std::map<std::string, std::string>::const_iterator it = mimeTypes.find(extension);
	if (it != mimeTypes.end())
		return (it->second);
	return ("application/octet-stream");
}