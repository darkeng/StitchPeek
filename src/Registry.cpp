#include "Registry.h"
#include <shlwapi.h>

/**
 * Helper function to create a subkey and set a string value.
 */
static HRESULT SetRegistryKeyAndValue(PCWSTR pszSubKey, PCWSTR pszValueName, PCWSTR pszData) {
    HKEY hKey;
    HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CURRENT_USER, pszSubKey, 0, NULL, 
        REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));
    
    if (SUCCEEDED(hr)) {
        if (pszData != NULL) {
            hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, pszValueName, 0, REG_SZ, 
                reinterpret_cast<const BYTE *>(pszData), 
                static_cast<DWORD>((wcslen(pszData) + 1) * sizeof(WCHAR))));
        }
        RegCloseKey(hKey);
    }
    return hr;
}

/**
 * Registers the COM Server and Thumbnail Provider shell extensions.
 * Registers in HKEY_CURRENT_USER to avoid requiring Administrator privileges.
 */
HRESULT RegisterCOMServer(HINSTANCE hInstance) {
    WCHAR szModule[MAX_PATH];
    if (GetModuleFileNameW(hInstance, szModule, ARRAYSIZE(szModule)) == 0) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Register the component
    HRESULT hr = SetRegistryKeyAndValue(L"Software\\Classes\\CLSID\\{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}", NULL, L"StitchPeek Thumbnail Provider");
    if (FAILED(hr)) return hr;

    // Disables process isolation to allow the dllhost to access local files via libembroidery
    DWORD dwDisableProcessIsolation = 1;
    HKEY hKey = NULL;
    if (SUCCEEDED(RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL))) {
        RegSetValueExW(hKey, L"DisableProcessIsolation", 0, REG_DWORD, (const BYTE*)&dwDisableProcessIsolation, sizeof(dwDisableProcessIsolation));
        RegCloseKey(hKey);
    }

    // Server path
    hr = SetRegistryKeyAndValue(L"Software\\Classes\\CLSID\\{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}\\InProcServer32", NULL, szModule);
    if (FAILED(hr)) return hr + 10000;

    // Threading model
    hr = SetRegistryKeyAndValue(L"Software\\Classes\\CLSID\\{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}\\InProcServer32", L"ThreadingModel", L"Apartment");
    if (FAILED(hr)) return hr + 20000;

    // Register supported embroidery extensions
    LPCWSTR exts[] = { L".pes", L".dst", L".exp", L".jef", L".vp3", L".xxx", L".pec" };
    for (int i = 0; i < ARRAYSIZE(exts); i++) {
        WCHAR szExt[256];
        wsprintfW(szExt, L"Software\\Classes\\%s", exts[i]);
        SetRegistryKeyAndValue(szExt, L"PerceivedType", L"image");
        
        WCHAR szKey[256];
        wsprintfW(szKey, L"Software\\Classes\\%s\\ShellEx\\{e357fccd-a995-4576-b01f-234630154e96}", exts[i]);
        HRESULT hr2 = SetRegistryKeyAndValue(szKey, NULL, L"{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}");
        if (FAILED(hr2)) return hr2 + 30000 + i;
    }

    return S_OK;
}

/**
 * Unregisters the COM Server and clears associated file extensions.
 */
HRESULT UnregisterCOMServer() {
    RegDeleteTreeW(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\{C6F4CCFA-8C37-4E64-A9D6-7EB76DC284FD}");
    const wchar_t* exts[] = { L".pes", L".dst", L".exp", L".jef", L".vp3", L".xxx", L".pec" };
    for (int i = 0; i < ARRAYSIZE(exts); i++) {
        WCHAR szKey[256];
        wsprintfW(szKey, L"Software\\Classes\\%s\\ShellEx\\{e357fccd-a995-4576-b01f-234630154e96}", exts[i]);
        RegDeleteKeyW(HKEY_CURRENT_USER, szKey);
    }

    return S_OK;
}
