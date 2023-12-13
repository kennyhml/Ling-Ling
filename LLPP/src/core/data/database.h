#pragma once
#include "../../../external/sqlite3.h"
#include <filesystem>

namespace llpp::core::data
{
	inline sqlite3* db;

	bool init(const std::filesystem::path& db_path);

}
