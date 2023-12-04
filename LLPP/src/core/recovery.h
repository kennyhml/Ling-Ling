#pragma once
#include <string>

namespace llpp::core
{
	void Recover();

	void RecoverFromShooterGameError();
	void RecoverFromBotError();

	void ReconnectToServer();
	void RestartGame();
	void ExitGame();
}