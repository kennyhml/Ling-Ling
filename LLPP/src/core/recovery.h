#pragma once
#include <asapp/core/exceptions.h>
#include <chrono>
#include <string>

namespace llpp::core
{
	void Recover();

	void ReconnectToServer();
	void RestartGame();
	void ExitGame();

	void InformCrashDetected(asa::exceptions::ShooterGameError&);
	void InformRecoveryInitiated(bool restart, bool reconnect);
	void InformRecoverySuccessful(std::chrono::seconds timeTaken);

}