#include "exceptions.h"

const char* llpp::bots::paste::exceptions::PasteNotDeposited::what()
{
	return "Failed to deposit Achatina Paste into Dedicated Storage.";
}