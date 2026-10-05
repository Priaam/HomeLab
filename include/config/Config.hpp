#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <vector>
# include <map>

struct ServerConfig
{
	int							port;
	std::string					rootPath;
	std::string					indexName;
	std::string					uploadDir;
	std::string					serverName;
	std::map<int, std::string>	errorPages;
};

class Config
{
	private:
		std::vector<ServerConfig> servers_;

		void	parseFile_(const std::string& filename);
		void	processLine_(const std::string& line, bool& inServerBlock, ServerConfig& currentServer);
		void	cleanValue_(std::string& value);
		void	parseDirective_(ServerConfig& currentServer, const std::string& keyword, std::istringstream& iss);

	public:
		Config(const std::string& filename);
		~Config();

		const std::vector<ServerConfig>&	getServers() const;
};

#endif