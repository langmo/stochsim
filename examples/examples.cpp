// MoiSim.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include "example1.h"

int main()
{
	try
	{
		example1();
	}
	catch (const std::runtime_error& re)
	{
		std::cerr << "Runtime error: " << re.what() << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error occurred: " << ex.what() << std::endl;
	}
	catch (std::string message)
	{
		std::cerr << "Error occured: " << message <<std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown error occured." << std::endl;
	}
	return 0;
}

