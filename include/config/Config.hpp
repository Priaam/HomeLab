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

		std::string dbHost_;
    	std::string dbUser_;
    	std::string dbPass_;
    	std::string dbName_;

		void	parseFile_(const std::string& filename);
		void	processLine_(const std::string& line, bool& inServerBlock, ServerConfig& currentServer);
		void	cleanValue_(std::string& value);
		void	parseDirective_(ServerConfig& currentServer, const std::string& keyword, std::istringstream& iss);

	public:
		Config(const std::string& filename);
		~Config();

		const std::vector<ServerConfig>&	getServers() const;

		std::string getDbHost() const { return dbHost_; }
    	std::string getDbUser() const { return dbUser_; }
		std::string getDbPass() const { return dbPass_; }
    	std::string getDbName() const { return dbName_; }
};

#endif