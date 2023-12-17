#pragma once
#include <asapp/core/exceptions.h>
#include <chrono>
#include <string>

namespace llpp::core
{
	void recover();

	void reconnect_to_server();
	void restart_game();
	void exit_game();

	void inform_crash_detected(asa::core::ShooterGameError&);
	void inform_recovery_initiated(bool restart, bool reconnect);
	void inform_recovery_successful(std::chrono::seconds time_taken);
}