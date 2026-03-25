#include "StitchPeek.h"
#include "Registry.h"
#include <shlwapi.h>
#include <new>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")

HINSTANCE g_hInst = NULL;
long g_cDllRef = 0;

class StitchPeekClassFactory : public IClassFactory {
public:
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv) {
        static const QITAB qit[] = {
            QITABENT(StitchPeekClassFactory, IClassFactory),
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }
    IFACEMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&m_cRef); }
    IFACEMETHODIMP_(ULONG) Release() {
        ULONG cRef = InterlockedDecrement(&m_cRef);
        if (0 == cRef) delete this;
        return cRef;
    }
    IFACEMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv) {
        if (pUnkOuter) return CLASS_E_NOAGGREGATION;
        StitchPeekProvider *pExt = new (std::nothrow) StitchPeekProvider();
        if (!pExt) return E_OUTOFMEMORY;
        HRESULT hr = pExt->QueryInterface(riid, ppv);
        pExt->Release();
        return hr;
    }
    IFACEMETHODIMP LockServer(BOOL fLock) {
        if (fLock) InterlockedIncrement(&g_cDllRef);
        else InterlockedDecrement(&g_cDllRef);
        return S_OK;
    }
    StitchPeekClassFactory() : m_cRef(1) {}
    ~StitchPeekClassFactory() {}
private:
    long m_cRef;
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
    case DLL_PROCESS_ATTACH: {
        g_hInst = hModule;
        DisableThreadLibraryCalls(hModule);
        break;
    }
    case DLL_PROCESS_DETACH: {
        break;
    }
    }
    return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv) {
    if (rclsid == CLSID_StitchPeekProvider) {
        StitchPeekClassFactory *pcf = new (std::nothrow) StitchPeekClassFactory();
        if (pcf) {
            HRESULT hr = pcf->QueryInterface(riid, ppv);
            pcf->Release();
            return hr;
        }
        return E_OUTOFMEMORY;
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}

STDAPI DllCanUnloadNow() {
    return g_cDllRef > 0 ? S_FALSE : S_OK;
}

STDAPI DllRegisterServer() {
    return RegisterCOMServer(g_hInst);
}

STDAPI DllUnregisterServer() {
    return UnregisterCOMServer();
}
