#include "core/Server.hpp"
#include "config/Config.hpp"

#include <iostream>
#include <exception>

int	main()
{
	try {
		Config config("homelab.conf");

		Server myServer(config);
		myServer.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}