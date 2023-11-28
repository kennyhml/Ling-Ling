#pragma once
#include <stdexcept>

namespace llpp::bots::paste::exceptions
{

	class PasteNotDeposited : public std::exception
	{
	public:
		const char* what()
		{
			return "Failed to deposit Achatina Paste into Dedicated Storage.";
		}
	};









}