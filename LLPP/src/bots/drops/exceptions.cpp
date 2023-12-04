#include "exceptions.h"

const char* llpp::bots::paste::exceptions::CrateNotAccessed::what()
{
	return "Failed to access crate";
}