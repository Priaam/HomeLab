#ifndef HTTPUTILS_HPP
# define HTTPUTILS_HPP

# include "string"

class HttpUtils {

public:
	static std::string	urlDecode(const std::string& str);
	static std::string	getExtensionFile(const std::string& filepath);
	static std::string	getMimeType(const std::string& extension);
};

#endif