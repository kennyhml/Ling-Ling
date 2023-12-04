#pragma once
#include <stdexcept>

namespace llpp::bots::paste::exceptions
{
	class CrateNotAccessed : public std::exception
	{

	public:
		const char* what();
	};




}