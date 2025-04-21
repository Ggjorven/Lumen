#include <iostream>

#include "Lumen/Internal/IO/Print.hpp"

int main(int argc, char* argv[])
{
	LU_LOG_TRACE("Trace message");
	LU_LOG_INFO("Info message");
	LU_LOG_WARN("Warn message");
	LU_LOG_ERROR("Error message");
	LU_LOG_FATAL("Fatal message");

    return 0;
}