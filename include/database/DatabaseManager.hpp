#ifndef DATABASEMANAGER_HPP
# define DATABASEMANAGER_HPP

# include <mariadb/mysql.h>
# include <string>

class DatabaseManager {

private:
	MYSQL*	conn_;

public:
	DatabaseManager();
	~DatabaseManager();

	bool	initDatabase();
};

#endif