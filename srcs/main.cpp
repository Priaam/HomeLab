#include "core/Server.hpp"
#include "config/Config.hpp"
#include "database/DatabaseManager.hpp"

#include <iostream>
#include <exception>

int	main()
{
	try {
		Config config("homelab.conf");

		DatabaseManager db(config.getDbHost(), config.getDbUser(), config.getDbPass(), config.getDbName());
		db.initDatabase();

		Server myServer(config);
		myServer.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}