#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <string>

class HttpResponse
{
	private:
		int				statusCode_;
		std::string		body_;
		std::string		contentType_;

	public:
		HttpResponse();
		~HttpResponse();

		void	setStatusCode(int code);
		void	setBody(const std::string& body);
		void	setBodyFromFile(const std::string& filepath);
		void	setContentType(const std::string& contentType);
		void	replaceInBody(const std::string& target, const std::string& replacement);

		std::string	generateResponse() const;

		int		getStatusCode() const;

};

#endif