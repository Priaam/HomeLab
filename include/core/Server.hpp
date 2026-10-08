#ifndef SERVER_HPP
# define SERVER_HPP

# include "config/Config.hpp"
# include "http/HttpRequest.hpp"
# include "http/HttpResponse.hpp"

# include <netinet/in.h>
# include <poll.h>
# include <vector>
# include <map>

struct ClientState
{
	std::string		buffer;
	bool			headerParsed;
	size_t			contentLength;
	bool			isComplete;
};

class Server
{
	private:
		const						Config& config_;
		std::vector<int>			serverFds_;
		std::vector<struct pollfd>	pollFds_;

		std::map<int, int>			clientToServer_;
		std::map<int, ClientState>	clientStates_;

		void				setupSockets_();
		void				bindAndListen_();

		void				acceptNewClient_(int serverFd);
		void				handleClientData_(size_t pollIndex);
		
		size_t				extractHeaderContentLength_(const std::string& header);

		void				handleGetRequest_(const HttpRequest& request, HttpResponse& response, const ServerConfig& config);
		void				handlePostRequest_(int clientFd, const HttpRequest& request, HttpResponse& response, const ServerConfig& config);
		void				handleDeleteRequest_(const HttpRequest& request, HttpResponse& response, const ServerConfig& config);

		const				ServerConfig& getClientConfig_(int clientFd, const HttpRequest& request) const;
        void				processRequest_(int clientFd);
        void				closeClientConnection_(size_t pollIndex);

		void				servError_(int code, HttpResponse& response, const ServerConfig& config);

	public:

		
		Server(const Config& config);
		~Server();

		void				run();
};

#endif