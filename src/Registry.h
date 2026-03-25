#pragma once
#include <windows.h>

/**
 * Handles the registration of the COM Server and Shell Extensions in the Windows Registry.
 */
HRESULT RegisterCOMServer(HINSTANCE hInstance);
HRESULT UnregisterCOMServer();
