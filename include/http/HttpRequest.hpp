#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <string>

class HttpRequest
{
	private:
		std::string	method_;
		std::string	path_;
		std::string	version_;
		std::string	host_;
		size_t		content_length;

	public:
		HttpRequest(const std::string& rawData);
		~HttpRequest();

		std::string	getMethod() const;
		std::string getPath() const;
		std::string getHost() const;
};

#endif