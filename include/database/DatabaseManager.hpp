#ifndef DATABASEMANAGER_HPP
# define DATABASEMANAGER_HPP

# include <mariadb/mysql.h>
# include <string>

class DatabaseManager {

private:
	MYSQL*	conn_;

public:
	DatabaseManager(const std::string& host, const std::string& user, const std::string& pass, const std::string& db);
	~DatabaseManager();

	bool	initDatabase();

	bool	createUser(const std::string& username, const std::string& password);
};

#endif