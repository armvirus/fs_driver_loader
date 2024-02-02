#include <iostream>
#include <windows.h>
#include <string>
#include <filesystem>

int main(int argc, char* argv[])
{
    if (argc < 3) {
        std::cerr << "Please provide the driver path and operation (-load or -unload) as arguments.\n";
        return -1;
    }

    std::filesystem::path driverPath(argv[1]);
    std::string driverName = driverPath.filename().string();
    std::filesystem::path systemDriverPath = "C:\\Windows\\System32\\drivers\\" + driverName;
    std::string regPath = "SYSTEM\\CurrentControlSet\\Services\\" + driverName.substr(0, driverName.find_last_of("."));

    std::string operation(argv[2]);
    if (operation == "-load") {
        if (!std::filesystem::exists(driverPath)) {
            std::cerr << driverPath << " does not exist.\n";
            return -1;
        }

        if (std::filesystem::exists(systemDriverPath)) {
            std::cerr << "Driver already exists in System32\\drivers. (try using -unload to remove this service)\n";
            return -1;
        }

        std::filesystem::copy(driverPath, systemDriverPath);

        HKEY hKey;
        if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
            std::cerr << "Failed to create registry key.\n";
            return -1;
        }

        std::string group = "FSFilter Infrastructure";
        std::string imagePath = "system32\\DRIVERS\\" + driverName;
        DWORD start = 0x0;
        DWORD type = 0x2;
        DWORD errorControl = 0;

        RegSetValueExA(hKey, "ErrorControl", 0, REG_DWORD, (BYTE*)&errorControl, sizeof(DWORD));
        RegSetValueExA(hKey, "ImagePath", 0, REG_SZ, (BYTE*)imagePath.c_str(), imagePath.size() + 1);
        RegSetValueExA(hKey, "Start", 0, REG_DWORD, (BYTE*)&start, sizeof(DWORD));
        RegSetValueExA(hKey, "Type", 0, REG_DWORD, (BYTE*)&type, sizeof(DWORD));
        RegSetValueExA(hKey, "Group", 0, REG_SZ, (BYTE*)group.c_str(), group.size() + 1);

        RegCloseKey(hKey);

        std::cout << "Created service for [" << driverName << "] successfully.\n";
    }
    else if (operation == "-unload") {
        if (std::filesystem::exists(systemDriverPath)) {
            std::filesystem::remove(systemDriverPath);
            std::cout << "Deleted [" << driverName << "] successfully.\n";
        }
        else
        {
            std::cerr << "Driver does not exist in System32\\drivers.\n";
        }

        if (RegDeleteKeyA(HKEY_LOCAL_MACHINE, regPath.c_str()) != ERROR_SUCCESS) {
            std::cerr << "Failed to delete registry key.\n";
            return -1;
        }
        else
        {
            std::cout << "Removed service for [" << driverName.substr(0, driverName.find_last_of(".")) << "] successfully.\n";
        }
    }
    else {
        std::cerr << "Invalid operation. Please specify -load or -unload.\n";
        return -1;
    }

    return 0;
}