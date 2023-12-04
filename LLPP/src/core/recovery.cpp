#include "recovery.h"
#include <Windows.h>
#include <asapp/game/window.h>

inline const std::string restartLink = "steam://rungameid/2399830";

void llpp::core::RestartGame()
{
	ShellExecuteA(NULL, "open", restartLink.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void llpp::core::ExitGame()
{
	DWORD pid;

	GetWindowThreadProcessId(asa::window::hWnd, &pid);
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if (!hProcess) {
		throw std::runtime_error("Process handle not acquired");
	}
	TerminateProcess(hProcess, 0);
	CloseHandle(hProcess);
}