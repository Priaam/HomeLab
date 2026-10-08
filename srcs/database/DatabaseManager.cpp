#include "database/DatabaseManager.hpp"
#include <iostream>
#include <stdexcept>

DatabaseManager::DatabaseManager()
{
	conn_ = mysql_init(NULL);
	if (conn_ == NULL)
		throw
			std::runtime_error("[DATABASE] Initialization error on MariaDB");

	if (mysql_real_connect(conn_, "127.0.0.1", "priam", "CBB934sx21!", "homelab", 3306, NULL, 0) == NULL)
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