#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>

class HttpResponse
{
	private:
		int				statusCode_;
		std::string		body_;

	public:
		HttpResponse();
		~HttpResponse();

		void	setStatusCode(int code);
		void	setBody(const std::string& body);
		void	setBodyFromFile(const std::string& filepath);

		std::string	generateResponse() const;

};

#endif