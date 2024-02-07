#include "util.h"

#include <shobjidl_core.h>
#include <Windows.h>

namespace llpp::gui::util
{
    bool get_folder_path(std::string& path_out)
    {
        HRESULT f_SysHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(f_SysHr)) { return FALSE; }

        // CREATE FileOpenDialog OBJECT
        IFileOpenDialog* f_FileSystem;
        f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                                   IID_IFileOpenDialog,
                                   reinterpret_cast<void**>(&f_FileSystem));
        if (FAILED(f_SysHr)) {
            CoUninitialize();
            return FALSE;
        }

        // SET OPTIONS FOR FOLDER SELECTION
        DWORD dwOptions;
        f_SysHr = f_FileSystem->GetOptions(&dwOptions);
        if (SUCCEEDED(f_SysHr)) {
            f_SysHr = f_FileSystem->SetOptions(dwOptions | FOS_PICKFOLDERS);
        }
        if (FAILED(f_SysHr)) {
            f_FileSystem->Release();
            CoUninitialize();
            return FALSE;
        }

        // SHOW OPEN FILE DIALOG WINDOW
        f_SysHr = f_FileSystem->Show(nullptr);
        if (FAILED(f_SysHr)) {
            f_FileSystem->Release();
            CoUninitialize();
            return FALSE;
        }

        // RETRIEVE FOLDER PATH FROM THE SELECTED ITEM
        IShellItem* f_Folder;
        f_SysHr = f_FileSystem->GetResult(&f_Folder);
        if (FAILED(f_SysHr)) {
            f_FileSystem->Release();
            CoUninitialize();
            return FALSE;
        }

        // STORE AND CONVERT THE FOLDER PATH
        PWSTR f_Path;
        f_SysHr = f_Folder->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
        if (FAILED(f_SysHr)) {
            f_Folder->Release();
            f_FileSystem->Release();
            CoUninitialize();
            return FALSE;
        }

        // FORMAT AND STORE THE FOLDER PATH
        std::wstring path(f_Path);
        path_out = std::string(path.begin(), path.end());

        // SUCCESS, CLEAN UP
        CoTaskMemFree(f_Path);
        f_Folder->Release();
        f_FileSystem->Release();
        CoUninitialize();
        return TRUE;
    }
}
