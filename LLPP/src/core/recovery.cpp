#include "recovery.h"
#include <Windows.h>
#include <asapp/game/window.h>

void llpp::core::RestartGame()
{
	int result = system("start steam://rungameid/2399830");
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