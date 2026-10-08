#include "http/HttpResponse.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

HttpResponse::HttpResponse()
{
	body_ = "";
	statusCode_ = 200;
	contentType_ = "text/html";
}

HttpResponse::~HttpResponse()
{}

void	HttpResponse::setStatusCode(int code) {statusCode_ = code;}
void	HttpResponse::setBody(const std::string& body) {body_ = body;} 

void	HttpResponse::setBodyFromFile(const std::string& filepath)
{
	std::ifstream	file(filepath.c_str());

	if (!file.is_open())
	{
		std::cerr << "Erreur : Impossible d'ouvrir le fichier." << std::endl;
		
		statusCode_ = 404;
		body_ = "<html><body style='background-color: #282a36; color: #ff5555; font-family: monospace; padding: 50px;'>"
                "<h1>[Erreur Interne] Fichier HTML introuvable</h1>"
                "<p>Le serveur n'a pas trouve le fichier : <b>" + filepath + "</b></p>"
                "</body></html>";
		return ;
	}

	std::ostringstream	buffer;
	buffer << file.rdbuf();
	body_ = buffer.str();

	file.close();
}

void	HttpResponse::replaceInBody(const std::string& target, const std::string& replacement)
{
	size_t	pos = body_.find(target);
	if (pos != std::string::npos)
		body_.replace(pos, target.length(), replacement);
}

void	HttpResponse::setContentType(const std::string& contentType) {contentType_ = contentType;}

std::string	HttpResponse::generateResponse() const
{
	std::ostringstream	stream;

	if (statusCode_ == 200)
		stream << "HTTP/1.1 200 OK\r\n";
	else if (statusCode_ == 400)
        stream << "HTTP/1.1 400 Bad Request\r\n";
	else if (statusCode_ == 404)
		stream << "HTTP/1.1 404 Not Found\r\n";
	else if (statusCode_ == 405)
        stream << "HTTP/1.1 405 Method Not Allowed\r\n";
    else if (statusCode_ == 500)
        stream << "HTTP/1.1 500 Internal Server Error\r\n";
	else
        stream << "HTTP/1.1 " << statusCode_ << " Unknown\r\n";
	stream << "Content-Type: " << contentType_ << "\r\n";
	stream << "Content-Length: " << body_.length() << "\r\n";

	stream << "\r\n";

	stream << body_;

	return stream.str();
}

int HttpResponse::getStatusCode() const {return (statusCode_);}