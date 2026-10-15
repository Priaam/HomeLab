#include "database/DatabaseManager.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>

DatabaseManager::DatabaseManager(const std::string& host, const std::string& user, const std::string& pass, const std::string& db)
{
	conn_ = mysql_init(NULL);
	if (conn_ == NULL)
		throw
			std::runtime_error("[DATABASE] Initialization error on MariaDB");

	if (mysql_real_connect(conn_, host.c_str(), user.c_str(), pass.c_str(), db.c_str(), 3306, NULL, 0) == NULL)
	{
		std::string err = mysql_error(conn_);
		throw
			std::runtime_error("[DATABASE] Database connection error : " + err);
	}
	std::cout << "[DATABASE] Successful connection to MariaDB!" << std::endl;
}

DatabaseManager::~DatabaseManager()
{
	if (conn_ != NULL) 
    {
        mysql_close(conn_);
        std::cout << "[DATABASE] Connection closed cleanly." << std::endl;
    }
}

bool	DatabaseManager::initDatabase()
{
	const char*	query = "CREATE TABLE IF NOT EXISTS Users ("
                        "id INT AUTO_INCREMENT PRIMARY KEY, "
                        "username VARCHAR(50) NOT NULL UNIQUE, "
                        "password VARCHAR(255) NOT NULL, "
                        "role VARCHAR(20) DEFAULT 'user'"
                        ");";

	if (mysql_query(conn_, query)) 
    {
        std::cerr << "[DATABASE] Error creating Users table: " << mysql_error(conn_) << std::endl;
        return false;
    }
	std::cout << "[DATABASE] 'Users' table ready!" << std::endl;
	return (true);
}

bool	DatabaseManager::createUser(const std::string& username, const std::string& password)
{
	size_t	userLen	= username.length();
	size_t	passLen = password.length();

	std::vector<char>	escapedUsername((userLen * 2) + 1, '\0');
	std::vector<char>	escapedPassword((passLen * 2) + 1, '\0');

	mysql_real_escape_string(conn_, &escapedUsername[0], username.c_str(), userLen);
	mysql_real_escape_string(conn_, &escapedPassword[0], password.c_str(), userLen);

	std::string	query = "INSERT INTO Users (username, password) VALUES ('";
	query += &escapedUsername[0];
	query += "', '";
	query += &escapedPassword[0];
	query += "');";

	if (mysql_query(conn_, query.c_str()) != 0)
	{
		std::cerr << "[DATABASE] Insertion error: " << mysql_error(conn_) << std::endl;
		return (false);
	}
	std::cout << "[DATABASE] User '" << username << "' successfully created !" << std::endl;
	return (true);
}