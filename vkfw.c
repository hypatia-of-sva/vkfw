
#define GLFW_INCLUDE_VULKAN /* necessary to get Vulkan functions of GLFW */
#include <GLFW/glfw3.h>
#include <stdlib.h> /* for malloc and free for the window handle structs */

#include "vkfw.h"

enum { WIN32, LINUX, UNKNOWN } platform = UNKNOWN;
VkfwModuleOperations OSModule;
VkfwBool32 vfkwInstanceInitialized = VKFW_FALSE;
uint32_t instanceHandleAddress = 0;
const VkfwAllocationCallbacks* initAllocator = NULL;

GLFWallocator ourGLFWAllocator;

/* we need this struct, instead of casting to GLFWwindow*, in order to keep the monitor handle we want to go to fullscreen mode in.
   the video mode is stored because the refreshRate is needed for glfwSetWindowMonitor and not retrievable by state retrieval functions. */
typedef struct VkfwWindow_t {
    GLFWwindow*     windowHandle;
    GLFWmonitor*    monitorHandle;
    VkfwVideoMode   usedVideoMode;
} VkfwWindow_t;



/*



    switch(platform) {
        case WIN32:
        break;
        case LINUX:
        break;
        default: return VKFW_ERROR_UNKNOWN;
    }


*/



void* open_lib_options(char** names, int names_count) {
    void* handle = NULL;
    for(int i = 0; i < names_count; i++) {
        handle = OSModule.open(names[i], VKFW_DEFAULT_FLAGS);
        if(handle != NULL) break;
    }
    return handle;
}


struct {
    .... // from win32_platform.h
} win32
bool win32_load_libs(void) {
    win32.user32.instance = OSModule.open("user32.dll", VKFW_DEFAULT_FLAGS);
    if (!_glfw.win32.user32.instance) {
        return false;
    }
    win32.user32.SetProcessDPIAware_            = (PFN_SetProcessDPIAware)              OSModule.load(win32.user32.instance, "SetProcessDPIAware");
    win32.user32.ChangeWindowMessageFilterEx_   = (PFN_ChangeWindowMessageFilterEx)     OSModule.load(win32.user32.instance, "ChangeWindowMessageFilterEx");
    win32.user32.EnableNonClientDpiScaling_     = (PFN_EnableNonClientDpiScaling)       OSModule.load(win32.user32.instance, "EnableNonClientDpiScaling");
    win32.user32.SetProcessDpiAwarenessContext_ = (PFN_SetProcessDpiAwarenessContext)   OSModule.load(win32.user32.instance, "SetProcessDpiAwarenessContext");
    win32.user32.GetDpiForWindow_               = (PFN_GetDpiForWindow)                 OSModule.load(win32.user32.instance, "GetDpiForWindow");
    win32.user32.AdjustWindowRectExForDpi_      = (PFN_AdjustWindowRectExForDpi)        OSModule.load(win32.user32.instance, "AdjustWindowRectExForDpi");
    win32.user32.GetSystemMetricsForDpi_        = (PFN_GetSystemMetricsForDpi)          OSModule.load(win32.user32.instance, "GetSystemMetricsForDpi");
    
    win32.dinput8.instance = OSModule.open("dinput8.dll", VKFW_DEFAULT_FLAGS);
    if (win32.dinput8.instance != NULL) {
        win32.dinput8.Create                    = (PFN_DirectInput8Create)              OSModule.load(win32.dinput8.instance, "DirectInput8Create");
    }
    
    const char** xinput_names = {"xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll", "xinput1_2.dll", "xinput1_1.dll"};
    win32.xinput.instance = open_lib_options(xinput_names, sizeof(xinput_names)/sizeof(char*));
    if (win32.xinput.instance != NULL) {
        win32.xinput.GetCapabilities    = (PFN_XInputGetCapabilities)   OSModule.load(win32.xinput.instance, "XInputGetCapabilities");
        win32.xinput.GetState           = (PFN_XInputGetState)          OSModule.load(win32.xinput.instance, "XInputGetState");
    }

    win32.dwmapi.instance = OSModule.load("dwmapi.dll", VKFW_DEFAULT_FLAGS);
    if (win32.dwmapi.instance != NULL)
    {
        win32.dwmapi.IsCompositionEnabled   = (PFN_DwmIsCompositionEnabled)     OSModule.load(win32.dwmapi.instance, "DwmIsCompositionEnabled");
        win32.dwmapi.Flush                  = (PFN_DwmFlush)                    OSModule.load(win32.dwmapi.instance, "DwmFlush");
        win32.dwmapi.EnableBlurBehindWindow = (PFN_DwmEnableBlurBehindWindow)   OSModule.load(win32.dwmapi.instance, "DwmEnableBlurBehindWindow");
        win32.dwmapi.GetColorizationColor   = (PFN_DwmGetColorizationColor)     OSModule.load(win32.dwmapi.instance, "DwmGetColorizationColor");
    }

    win32.shcore.instance = OSModule.load("shcore.dll", VKFW_DEFAULT_FLAGS);
    if (win32.shcore.instance != NULL)
    {
        win32.shcore.SetProcessDpiAwareness_    = (PFN_SetProcessDpiAwareness)  OSModule.load(win32.shcore.instance, "SetProcessDpiAwareness");
        win32.shcore.GetDpiForMonitor_          = (PFN_GetDpiForMonitor)        OSModule.load(win32.shcore.instance, "GetDpiForMonitor");
    }

    win32.ntdll.instance = OSModule.load("ntdll.dll", VKFW_DEFAULT_FLAGS);
    if (win32.ntdll.instance != NULL)
    {
        win32.ntdll.RtlVerifyVersionInfo_       = (PFN_RtlVerifyVersionInfo)    OSModule.load(win32.ntdll.instance, "RtlVerifyVersionInfo");
    }
    
    return true;
}
void win32_unload_libs(void) {
}

struct {
    .... // from x11_platform.h + XInitThreads, XrmInitialize, XOpenDisplay
} x11
bool linux_load_libs(void) {
    const char** xlib_names = {"libX11.so.6", "libX11.so", "libX11-6.so"};
    x11.xlib.handle = open_lib_options(xlib_names, sizeof(xlib_names)/sizeof(char*));
    if(x11.xlib.handle == NULL) {
        return false;
    }
    
    x11.xlib.XInitThreads                     = (PFN_XInitThreads)                        OSModule.load(x11.xlib.handle, "XInitThreads");
    x11.xlib.XrmInitialize                    = (PFN_XrmInitialize)                       OSModule.load(x11.xlib.handle, "XrmInitialize");
    x11.xlib.XOpenDisplay                     = (PFN_XOpenDisplay)                        OSModule.load(x11.xlib.handle, "XOpenDisplay");
    if(x11.xlib.XInitThreads == NULL || x11.xlib.XrmInitialize == NULL || x11.xlib.XOpenDisplay == NULL) {
        return false;
    }
    
    x11.xlib.AllocClassHint                   = (PFN_XAllocClassHint)                     OSModule.load(x11.xlib.handle, "XAllocClassHint");
    x11.xlib.AllocSizeHints                   = (PFN_XAllocSizeHints)                     OSModule.load(x11.xlib.handle, "XAllocSizeHints");
    x11.xlib.AllocWMHints                     = (PFN_XAllocWMHints)                       OSModule.load(x11.xlib.handle, "XAllocWMHints");
    x11.xlib.ChangeProperty                   = (PFN_XChangeProperty)                     OSModule.load(x11.xlib.handle, "XChangeProperty");
    x11.xlib.ChangeWindowAttributes           = (PFN_XChangeWindowAttributes)             OSModule.load(x11.xlib.handle, "XChangeWindowAttributes");
    x11.xlib.CheckIfEvent                     = (PFN_XCheckIfEvent)                       OSModule.load(x11.xlib.handle, "XCheckIfEvent");
    x11.xlib.CheckTypedWindowEvent            = (PFN_XCheckTypedWindowEvent)              OSModule.load(x11.xlib.handle, "XCheckTypedWindowEvent");
    x11.xlib.CloseDisplay                     = (PFN_XCloseDisplay)                       OSModule.load(x11.xlib.handle, "XCloseDisplay");
    x11.xlib.CloseIM                          = (PFN_XCloseIM)                            OSModule.load(x11.xlib.handle, "XCloseIM");
    x11.xlib.ConvertSelection                 = (PFN_XConvertSelection)                   OSModule.load(x11.xlib.handle, "XConvertSelection");
    x11.xlib.CreateColormap                   = (PFN_XCreateColormap)                     OSModule.load(x11.xlib.handle, "XCreateColormap");
    x11.xlib.CreateFontCursor                 = (PFN_XCreateFontCursor)                   OSModule.load(x11.xlib.handle, "XCreateFontCursor");
    x11.xlib.CreateIC                         = (PFN_XCreateIC)                           OSModule.load(x11.xlib.handle, "XCreateIC");
    x11.xlib.CreateRegion                     = (PFN_XCreateRegion)                       OSModule.load(x11.xlib.handle, "XCreateRegion");
    x11.xlib.CreateWindow                     = (PFN_XCreateWindow)                       OSModule.load(x11.xlib.handle, "XCreateWindow");
    x11.xlib.DefineCursor                     = (PFN_XDefineCursor)                       OSModule.load(x11.xlib.handle, "XDefineCursor");
    x11.xlib.DeleteContext                    = (PFN_XDeleteContext)                      OSModule.load(x11.xlib.handle, "XDeleteContext");
    x11.xlib.DeleteProperty                   = (PFN_XDeleteProperty)                     OSModule.load(x11.xlib.handle, "XDeleteProperty");
    x11.xlib.DestroyIC                        = (PFN_XDestroyIC)                          OSModule.load(x11.xlib.handle, "XDestroyIC");
    x11.xlib.DestroyRegion                    = (PFN_XDestroyRegion)                      OSModule.load(x11.xlib.handle, "XDestroyRegion");
    x11.xlib.DestroyWindow                    = (PFN_XDestroyWindow)                      OSModule.load(x11.xlib.handle, "XDestroyWindow");
    x11.xlib.DisplayKeycodes                  = (PFN_XDisplayKeycodes)                    OSModule.load(x11.xlib.handle, "XDisplayKeycodes");
    x11.xlib.EventsQueued                     = (PFN_XEventsQueued)                       OSModule.load(x11.xlib.handle, "XEventsQueued");
    x11.xlib.FilterEvent                      = (PFN_XFilterEvent)                        OSModule.load(x11.xlib.handle, "XFilterEvent");
    x11.xlib.FindContext                      = (PFN_XFindContext)                        OSModule.load(x11.xlib.handle, "XFindContext");
    x11.xlib.Flush                            = (PFN_XFlush)                              OSModule.load(x11.xlib.handle, "XFlush");
    x11.xlib.Free                             = (PFN_XFree)                               OSModule.load(x11.xlib.handle, "XFree");
    x11.xlib.FreeColormap                     = (PFN_XFreeColormap)                       OSModule.load(x11.xlib.handle, "XFreeColormap");
    x11.xlib.FreeCursor                       = (PFN_XFreeCursor)                         OSModule.load(x11.xlib.handle, "XFreeCursor");
    x11.xlib.FreeEventData                    = (PFN_XFreeEventData)                      OSModule.load(x11.xlib.handle, "XFreeEventData");
    x11.xlib.GetErrorText                     = (PFN_XGetErrorText)                       OSModule.load(x11.xlib.handle, "XGetErrorText");
    x11.xlib.GetEventData                     = (PFN_XGetEventData)                       OSModule.load(x11.xlib.handle, "XGetEventData");
    x11.xlib.GetICValues                      = (PFN_XGetICValues)                        OSModule.load(x11.xlib.handle, "XGetICValues");
    x11.xlib.GetIMValues                      = (PFN_XGetIMValues)                        OSModule.load(x11.xlib.handle, "XGetIMValues");
    x11.xlib.GetInputFocus                    = (PFN_XGetInputFocus)                      OSModule.load(x11.xlib.handle, "XGetInputFocus");
    x11.xlib.GetKeyboardMapping               = (PFN_XGetKeyboardMapping)                 OSModule.load(x11.xlib.handle, "XGetKeyboardMapping");
    x11.xlib.GetScreenSaver                   = (PFN_XGetScreenSaver)                     OSModule.load(x11.xlib.handle, "XGetScreenSaver");
    x11.xlib.GetSelectionOwner                = (PFN_XGetSelectionOwner)                  OSModule.load(x11.xlib.handle, "XGetSelectionOwner");
    x11.xlib.GetVisualInfo                    = (PFN_XGetVisualInfo)                      OSModule.load(x11.xlib.handle, "XGetVisualInfo");
    x11.xlib.GetWMNormalHints                 = (PFN_XGetWMNormalHints)                   OSModule.load(x11.xlib.handle, "XGetWMNormalHints");
    x11.xlib.GetWindowAttributes              = (PFN_XGetWindowAttributes)                OSModule.load(x11.xlib.handle, "XGetWindowAttributes");
    x11.xlib.GetWindowProperty                = (PFN_XGetWindowProperty)                  OSModule.load(x11.xlib.handle, "XGetWindowProperty");
    x11.xlib.GrabPointer                      = (PFN_XGrabPointer)                        OSModule.load(x11.xlib.handle, "XGrabPointer");
    x11.xlib.IconifyWindow                    = (PFN_XIconifyWindow)                      OSModule.load(x11.xlib.handle, "XIconifyWindow");
    x11.xlib.InternAtom                       = (PFN_XInternAtom)                         OSModule.load(x11.xlib.handle, "XInternAtom");
    x11.xlib.LookupString                     = (PFN_XLookupString)                       OSModule.load(x11.xlib.handle, "XLookupString");
    x11.xlib.MapRaised                        = (PFN_XMapRaised)                          OSModule.load(x11.xlib.handle, "XMapRaised");
    x11.xlib.MapWindow                        = (PFN_XMapWindow)                          OSModule.load(x11.xlib.handle, "XMapWindow");
    x11.xlib.MoveResizeWindow                 = (PFN_XMoveResizeWindow)                   OSModule.load(x11.xlib.handle, "XMoveResizeWindow");
    x11.xlib.MoveWindow                       = (PFN_XMoveWindow)                         OSModule.load(x11.xlib.handle, "XMoveWindow");
    x11.xlib.NextEvent                        = (PFN_XNextEvent)                          OSModule.load(x11.xlib.handle, "XNextEvent");
    x11.xlib.OpenIM                           = (PFN_XOpenIM)                             OSModule.load(x11.xlib.handle, "XOpenIM");
    x11.xlib.PeekEvent                        = (PFN_XPeekEvent)                          OSModule.load(x11.xlib.handle, "XPeekEvent");
    x11.xlib.Pending                          = (PFN_XPending)                            OSModule.load(x11.xlib.handle, "XPending");
    x11.xlib.QueryExtension                   = (PFN_XQueryExtension)                     OSModule.load(x11.xlib.handle, "XQueryExtension");
    x11.xlib.QueryPointer                     = (PFN_XQueryPointer)                       OSModule.load(x11.xlib.handle, "XQueryPointer");
    x11.xlib.RaiseWindow                      = (PFN_XRaiseWindow)                        OSModule.load(x11.xlib.handle, "XRaiseWindow");
    x11.xlib.RegisterIMInstantiateCallback    = (PFN_XRegisterIMInstantiateCallback)      OSModule.load(x11.xlib.handle, "XRegisterIMInstantiateCallback");
    x11.xlib.ResizeWindow                     = (PFN_XResizeWindow)                       OSModule.load(x11.xlib.handle, "XResizeWindow");
    x11.xlib.ResourceManagerString            = (PFN_XResourceManagerString)              OSModule.load(x11.xlib.handle, "XResourceManagerString");
    x11.xlib.SaveContext                      = (PFN_XSaveContext)                        OSModule.load(x11.xlib.handle, "XSaveContext");
    x11.xlib.SelectInput                      = (PFN_XSelectInput)                        OSModule.load(x11.xlib.handle, "XSelectInput");
    x11.xlib.SendEvent                        = (PFN_XSendEvent)                          OSModule.load(x11.xlib.handle, "XSendEvent");
    x11.xlib.SetClassHint                     = (PFN_XSetClassHint)                       OSModule.load(x11.xlib.handle, "XSetClassHint");
    x11.xlib.SetErrorHandler                  = (PFN_XSetErrorHandler)                    OSModule.load(x11.xlib.handle, "XSetErrorHandler");
    x11.xlib.SetICFocus                       = (PFN_XSetICFocus)                         OSModule.load(x11.xlib.handle, "XSetICFocus");
    x11.xlib.SetIMValues                      = (PFN_XSetIMValues)                        OSModule.load(x11.xlib.handle, "XSetIMValues");
    x11.xlib.SetInputFocus                    = (PFN_XSetInputFocus)                      OSModule.load(x11.xlib.handle, "XSetInputFocus");
    x11.xlib.SetLocaleModifiers               = (PFN_XSetLocaleModifiers)                 OSModule.load(x11.xlib.handle, "XSetLocaleModifiers");
    x11.xlib.SetScreenSaver                   = (PFN_XSetScreenSaver)                     OSModule.load(x11.xlib.handle, "XSetScreenSaver");
    x11.xlib.SetSelectionOwner                = (PFN_XSetSelectionOwner)                  OSModule.load(x11.xlib.handle, "XSetSelectionOwner");
    x11.xlib.SetWMHints                       = (PFN_XSetWMHints)                         OSModule.load(x11.xlib.handle, "XSetWMHints");
    x11.xlib.SetWMNormalHints                 = (PFN_XSetWMNormalHints)                   OSModule.load(x11.xlib.handle, "XSetWMNormalHints");
    x11.xlib.SetWMProtocols                   = (PFN_XSetWMProtocols)                     OSModule.load(x11.xlib.handle, "XSetWMProtocols");
    x11.xlib.SupportsLocale                   = (PFN_XSupportsLocale)                     OSModule.load(x11.xlib.handle, "XSupportsLocale");
    x11.xlib.Sync                             = (PFN_XSync)                               OSModule.load(x11.xlib.handle, "XSync");
    x11.xlib.TranslateCoordinates             = (PFN_XTranslateCoordinates)               OSModule.load(x11.xlib.handle, "XTranslateCoordinates");
    x11.xlib.UndefineCursor                   = (PFN_XUndefineCursor)                     OSModule.load(x11.xlib.handle, "XUndefineCursor");
    x11.xlib.UngrabPointer                    = (PFN_XUngrabPointer)                      OSModule.load(x11.xlib.handle, "XUngrabPointer");
    x11.xlib.UnmapWindow                      = (PFN_XUnmapWindow)                        OSModule.load(x11.xlib.handle, "XUnmapWindow");
    x11.xlib.UnsetICFocus                     = (PFN_XUnsetICFocus)                       OSModule.load(x11.xlib.handle, "XUnsetICFocus");
    x11.xlib.VisualIDFromVisual               = (PFN_XVisualIDFromVisual)                 OSModule.load(x11.xlib.handle, "XVisualIDFromVisual");
    x11.xlib.WarpPointer                      = (PFN_XWarpPointer)                        OSModule.load(x11.xlib.handle, "XWarpPointer");
    x11.xkb.FreeKeyboard                      = (PFN_XkbFreeKeyboard)                     OSModule.load(x11.xlib.handle, "XkbFreeKeyboard");
    x11.xkb.FreeNames                         = (PFN_XkbFreeNames)                        OSModule.load(x11.xlib.handle, "XkbFreeNames");
    x11.xkb.GetMap                            = (PFN_XkbGetMap)                           OSModule.load(x11.xlib.handle, "XkbGetMap");
    x11.xkb.GetNames                          = (PFN_XkbGetNames)                         OSModule.load(x11.xlib.handle, "XkbGetNames");
    x11.xkb.GetState                          = (PFN_XkbGetState)                         OSModule.load(x11.xlib.handle, "XkbGetState");
    x11.xkb.KeycodeToKeysym                   = (PFN_XkbKeycodeToKeysym)                  OSModule.load(x11.xlib.handle, "XkbKeycodeToKeysym");
    x11.xkb.QueryExtension                    = (PFN_XkbQueryExtension)                   OSModule.load(x11.xlib.handle, "XkbQueryExtension");
    x11.xkb.SelectEventDetails                = (PFN_XkbSelectEventDetails)               OSModule.load(x11.xlib.handle, "XkbSelectEventDetails");
    x11.xkb.SetDetectableAutoRepeat           = (PFN_XkbSetDetectableAutoRepeat)          OSModule.load(x11.xlib.handle, "XkbSetDetectableAutoRepeat");
    x11.xrm.DestroyDatabase                   = (PFN_XrmDestroyDatabase)                  OSModule.load(x11.xlib.handle, "XrmDestroyDatabase");
    x11.xrm.GetResource                       = (PFN_XrmGetResource)                      OSModule.load(x11.xlib.handle, "XrmGetResource");
    x11.xrm.GetStringDatabase                 = (PFN_XrmGetStringDatabase)                OSModule.load(x11.xlib.handle, "XrmGetStringDatabase");
    x11.xrm.UniqueQuark                       = (PFN_XrmUniqueQuark)                      OSModule.load(x11.xlib.handle, "XrmUniqueQuark");
    x11.xlib.UnregisterIMInstantiateCallback  = (PFN_XUnregisterIMInstantiateCallback)    OSModule.load(x11.xlib.handle, "XUnregisterIMInstantiateCallback");
    x11.xlib.utf8LookupString                 = (PFN_Xutf8LookupString)                   OSModule.load(x11.xlib.handle, "Xutf8LookupString");
    x11.xlib.utf8SetWMProperties              = (PFN_Xutf8SetWMProperties)                OSModule.load(x11.xlib.handle, "Xutf8SetWMProperties");
    
    return true;
}
void linux_unload_libs(void) {
    if(x11.xlib.handle != NULL) {
        OSModule.close(x11.xlib.handle);
    }
}





// WIN32
// Window procedure for the hidden helper window
static LRESULT CALLBACK win32_helperWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
        case WM_DISPLAYCHANGE:
            _glfwPollMonitorsWin32();
            break;

        case WM_DEVICECHANGE:
        {
            if (!_glfw.joysticksInitialized)
                break;

            if (wParam == DBT_DEVICEARRIVAL)
            {
                DEV_BROADCAST_HDR* dbh = (DEV_BROADCAST_HDR*) lParam;
                if (dbh && dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                    _glfwDetectJoystickConnectionWin32();
            }
            else if (wParam == DBT_DEVICEREMOVECOMPLETE)
            {
                DEV_BROADCAST_HDR* dbh = (DEV_BROADCAST_HDR*) lParam;
                if (dbh && dbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
                    _glfwDetectJoystickDisconnectionWin32();
            }

            break;
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}






// Terminate the library
static void terminate(void) {
    int i;

    memset(&_glfw.callbacks, 0, sizeof(_glfw.callbacks));

    while (_glfw.windowListHead)
        glfwDestroyWindow((GLFWwindow*) _glfw.windowListHead);

    while (_glfw.cursorListHead)
        glfwDestroyCursor((GLFWcursor*) _glfw.cursorListHead);

    for (i = 0;  i < _glfw.monitorCount;  i++)
    {
        _GLFWmonitor* monitor = _glfw.monitors[i];
        if (monitor->originalRamp.size)
            _glfw.platform.setGammaRamp(monitor, &monitor->originalRamp);
        _glfwFreeMonitor(monitor);
    }

    _glfw_free(_glfw.monitors);
    _glfw.monitors = NULL;
    _glfw.monitorCount = 0;

    _glfw_free(_glfw.mappings);
    _glfw.mappings = NULL;
    _glfw.mappingCount = 0;



    if (_glfw.vk.handle)
        OSModule.close(_glfw.vk.handle);

    _glfw.platform.terminateJoysticks();
    _glfw.platform.terminate();

    _glfw.initialized = GLFW_FALSE;

    memset(&_glfw, 0, sizeof(_glfw));
}




VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateGlobalProperties(VkfwGlobalProperties* pProperties) {
    if(pProperties == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    pProperties[0].majorVersion                = VKFW_VERSION_MAJOR;
    pProperties[0].minorVersion                = VKFW_VERSION_MINOR;
    pProperties[0].revisionVersion             = VKFW_VERSION_REVISION;
    pProperties[0].underlyingVersionString     = "VKFW 1.1 Native Starter Win32 X11, based on GLFW 3.4.0 code";    
    pProperties[0].supportedPlatforms         = VKFW_INSTANCE_PLATFORM_WIN32 | VKFW_INSTANCE_PLATFORM_X11;
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwCreateInstance(const VkfwInstanceCreateInfo* pCreateInfo, const VkfwAllocationCallbacks* pAllocator, VkfwInstance* pInstance) {    
    if(vfkwInstanceInitialized) return VKFW_ERROR_FEATURE_NOT_SUPPORTED; /* multiple Instances currently not supported because GLFW has only one initialization state */
    
    if(pCreateInfo == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    if(pInstance == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    
    initAllocator = pAllocator;
    
    if(pAllocator != NULL) {
        ourGLFWAllocator.allocate   = pAllocator[0].pfnAllocation;
        ourGLFWAllocator.reallocate = pAllocator[0].pfnReallocation;
        ourGLFWAllocator.deallocate = pAllocator[0].pfnFree;
        ourGLFWAllocator.user       = pAllocator[0].pUserData;
        if((ourGLFWAllocator.allocate == NULL) || (ourGLFWAllocator.reallocate == NULL) || (ourGLFWAllocator.deallocate == NULL)) return VKFW_ERROR_INVALID_POINTER_VALUE;
        _glfwInitAllocator = ourGLFWAllocator
    }
    
    if(pCreateInfo[0].sType != VKFW_STRUCTURE_TYPE_INSTANCE_CREATE_INFO)    return VKFW_ERROR_INVALID_ENUM_VALUE;
    
    VkfwInstanceSystemReferenceInfo* pSystemReferenceInfo = (VkfwInstanceSystemReferenceInfo*) pCreateInfo[0].pNext;
    while(pSystemReferenceInfo != NULL) {
        if(pSystemReferenceInfo[0].sType == VKFW_STRUCTURE_TYPE_INSTANCE_SYSTEM_REFERENCE_INFO) break;
        pSystemReferenceInfo = pSystemReferenceInfo[0].pNext;
    }
    if(pSystemReferenceInfo == NULL) return VKFW_ERROR_SYSTEM_REFERENCE_INFO_NOT_PROVIDED;
    OSModuleOperations = pSystemReferenceInfo[0].desiredModuleOperations;
    
    if(OSModule.load(VKFW_GLOBAL_HANDLE, "STARTER_WE_ARE_ON_WIN32") != NULL) {
        platform = WIN32;
    } else {
        platform = LINUX;
    }
    
    
    if(pCreateInfo[0].flags & VKFW_INSTANCE_CREATE_DISABLE_JOYSTICK_HAT_BUTTONS_BIT) {
        _glfwInitHints.hatButtons = GLFW_FALSE;
    } else {
        _glfwInitHints.hatButtons = GLFW_TRUE;
    }
    if(pCreateInfo[0].flags & VKFW_INSTANCE_CREATE_WAYLAND_DISABLE_LIBDECOR_BIT_WL) {
        _glfwInitHints.wl.libdecorMode = GLFW_WAYLAND_DISABLE_LIBDECOR;
    } else {
        _glfwInitHints.wl.libdecorMode = GLFW_WAYLAND_PREFER_LIBDECOR;
    }
    if(pCreateInfo[0].flags & VKFW_INSTANCE_CREATE_X11_DISABLE_XCB_VULKAN_SURFACE_BIT_X11) {
        _glfwInitHints.x11.xcbVulkanSurface = GLFW_FALSE;
    } else {
        _glfwInitHints.x11.xcbVulkanSurface = GLFW_TRUE;
    }
    if(pCreateInfo[0].flags & VKFW_INSTANCE_CREATE_COCOA_DISABLE_MENUBAR_BIT_COCOA) {
        _glfwInitHints.ns.menubar = GLFW_FALSE;
    } else {
        _glfwInitHints.ns.menubar = GLFW_TRUE;
    }
    if(pCreateInfo[0].flags & VKFW_INSTANCE_CREATE_COCOA_DISABLE_CHDIR_RESOURCES_BIT_COCOA) {
        _glfwInitHints.ns.chdir = GLFW_FALSE;
    } else {
        _glfwInitHints.ns.chdir = GLFW_TRUE;
    }
    switch(platform) {
        case WIN32:
            switch(pCreateInfo[0].desiredPlatform) {
                case VKFW_INSTANCE_PLATFORM_DEFAULT :
                case VKFW_INSTANCE_PLATFORM_WIN32   :
                    break;
                default: return VKFW_ERROR_PLATFORM_UNAVAILABLE;
            }
        break;
        case LINUX:
            switch(pCreateInfo[0].desiredPlatform) {
                case VKFW_INSTANCE_PLATFORM_DEFAULT :
                case VKFW_INSTANCE_PLATFORM_X11   :
                    break;
                default: return VKFW_ERROR_PLATFORM_UNAVAILABLE;
            }
        break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    _glfwInitHints.vulkanLoader = pCreateInfo[0].desiredVulkanLoader;
    
    
    
    _glfw.hints.init = _glfwInitHints;
    _glfw.allocator = _glfwInitAllocator;
    if (!_glfw.allocator.allocate)
    {
        _glfw.allocator.allocate   = defaultAllocate;
        _glfw.allocator.reallocate = defaultReallocate;
        _glfw.allocator.deallocate = defaultDeallocate;
    }


    
    
    switch(platform) {
        case WIN32:
            const _GLFWplatform win32 = {
                .platformID = GLFW_PLATFORM_WIN32,
                .init = _glfwInitWin32,
                .terminate = _glfwTerminateWin32,
                .getCursorPos = _glfwGetCursorPosWin32,
                .setCursorPos = _glfwSetCursorPosWin32,
                .setCursorMode = _glfwSetCursorModeWin32,
                .setRawMouseMotion = _glfwSetRawMouseMotionWin32,
                .rawMouseMotionSupported = _glfwRawMouseMotionSupportedWin32,
                .createCursor = _glfwCreateCursorWin32,
                .createStandardCursor = _glfwCreateStandardCursorWin32,
                .destroyCursor = _glfwDestroyCursorWin32,
                .setCursor = _glfwSetCursorWin32,
                .getScancodeName = _glfwGetScancodeNameWin32,
                .getKeyScancode = _glfwGetKeyScancodeWin32,
                .setClipboardString = _glfwSetClipboardStringWin32,
                .getClipboardString = _glfwGetClipboardStringWin32,
                .initJoysticks = _glfwInitJoysticksWin32,
                .terminateJoysticks = _glfwTerminateJoysticksWin32,
                .pollJoystick = _glfwPollJoystickWin32,
                .getMappingName = _glfwGetMappingNameWin32,
                .updateGamepadGUID = _glfwUpdateGamepadGUIDWin32,
                .freeMonitor = _glfwFreeMonitorWin32,
                .getMonitorPos = _glfwGetMonitorPosWin32,
                .getMonitorContentScale = _glfwGetMonitorContentScaleWin32,
                .getMonitorWorkarea = _glfwGetMonitorWorkareaWin32,
                .getVideoModes = _glfwGetVideoModesWin32,
                .getVideoMode = _glfwGetVideoModeWin32,
                .getGammaRamp = _glfwGetGammaRampWin32,
                .setGammaRamp = _glfwSetGammaRampWin32,
                .createWindow = _glfwCreateWindowWin32,
                .destroyWindow = _glfwDestroyWindowWin32,
                .setWindowTitle = _glfwSetWindowTitleWin32,
                .setWindowIcon = _glfwSetWindowIconWin32,
                .getWindowPos = _glfwGetWindowPosWin32,
                .setWindowPos = _glfwSetWindowPosWin32,
                .getWindowSize = _glfwGetWindowSizeWin32,
                .setWindowSize = _glfwSetWindowSizeWin32,
                .setWindowSizeLimits = _glfwSetWindowSizeLimitsWin32,
                .setWindowAspectRatio = _glfwSetWindowAspectRatioWin32,
                .getFramebufferSize = _glfwGetFramebufferSizeWin32,
                .getWindowFrameSize = _glfwGetWindowFrameSizeWin32,
                .getWindowContentScale = _glfwGetWindowContentScaleWin32,
                .iconifyWindow = _glfwIconifyWindowWin32,
                .restoreWindow = _glfwRestoreWindowWin32,
                .maximizeWindow = _glfwMaximizeWindowWin32,
                .showWindow = _glfwShowWindowWin32,
                .hideWindow = _glfwHideWindowWin32,
                .requestWindowAttention = _glfwRequestWindowAttentionWin32,
                .focusWindow = _glfwFocusWindowWin32,
                .setWindowMonitor = _glfwSetWindowMonitorWin32,
                .windowFocused = _glfwWindowFocusedWin32,
                .windowIconified = _glfwWindowIconifiedWin32,
                .windowVisible = _glfwWindowVisibleWin32,
                .windowMaximized = _glfwWindowMaximizedWin32,
                .windowHovered = _glfwWindowHoveredWin32,
                .framebufferTransparent = _glfwFramebufferTransparentWin32,
                .getWindowOpacity = _glfwGetWindowOpacityWin32,
                .setWindowResizable = _glfwSetWindowResizableWin32,
                .setWindowDecorated = _glfwSetWindowDecoratedWin32,
                .setWindowFloating = _glfwSetWindowFloatingWin32,
                .setWindowOpacity = _glfwSetWindowOpacityWin32,
                .setWindowMousePassthrough = _glfwSetWindowMousePassthroughWin32,
                .pollEvents = _glfwPollEventsWin32,
                .waitEvents = _glfwWaitEventsWin32,
                .waitEventsTimeout = _glfwWaitEventsTimeoutWin32,
                .postEmptyEvent = _glfwPostEmptyEventWin32,
                .getEGLPlatform = _glfwGetEGLPlatformWin32,
                .getEGLNativeDisplay = _glfwGetEGLNativeDisplayWin32,
                .getEGLNativeWindow = _glfwGetEGLNativeWindowWin32,
                .getRequiredInstanceExtensions = _glfwGetRequiredInstanceExtensionsWin32,
                .getPhysicalDevicePresentationSupport = _glfwGetPhysicalDevicePresentationSupportWin32,
                .createWindowSurface = _glfwCreateWindowSurfaceWin32
            };
            _glfw.platform = win32;
        break;
        case LINUX:
            const _GLFWplatform x11 = {
                .platformID = GLFW_PLATFORM_X11,
                .init = _glfwInitX11,
                .terminate = _glfwTerminateX11,
                .getCursorPos = _glfwGetCursorPosX11,
                .setCursorPos = _glfwSetCursorPosX11,
                .setCursorMode = _glfwSetCursorModeX11,
                .setRawMouseMotion = _glfwSetRawMouseMotionX11,
                .rawMouseMotionSupported = _glfwRawMouseMotionSupportedX11,
                .createCursor = _glfwCreateCursorX11,
                .createStandardCursor = _glfwCreateStandardCursorX11,
                .destroyCursor = _glfwDestroyCursorX11,
                .setCursor = _glfwSetCursorX11,
                .getScancodeName = _glfwGetScancodeNameX11,
                .getKeyScancode = _glfwGetKeyScancodeX11,
                .setClipboardString = _glfwSetClipboardStringX11,
                .getClipboardString = _glfwGetClipboardStringX11,
                .initJoysticks = _glfwInitJoysticksLinux,
                .terminateJoysticks = _glfwTerminateJoysticksLinux,
                .pollJoystick = _glfwPollJoystickLinux,
                .getMappingName = _glfwGetMappingNameLinux,
                .updateGamepadGUID = _glfwUpdateGamepadGUIDLinux,
                .freeMonitor = _glfwFreeMonitorX11,
                .getMonitorPos = _glfwGetMonitorPosX11,
                .getMonitorContentScale = _glfwGetMonitorContentScaleX11,
                .getMonitorWorkarea = _glfwGetMonitorWorkareaX11,
                .getVideoModes = _glfwGetVideoModesX11,
                .getVideoMode = _glfwGetVideoModeX11,
                .getGammaRamp = _glfwGetGammaRampX11,
                .setGammaRamp = _glfwSetGammaRampX11,
                .createWindow = _glfwCreateWindowX11,
                .destroyWindow = _glfwDestroyWindowX11,
                .setWindowTitle = _glfwSetWindowTitleX11,
                .setWindowIcon = _glfwSetWindowIconX11,
                .getWindowPos = _glfwGetWindowPosX11,
                .setWindowPos = _glfwSetWindowPosX11,
                .getWindowSize = _glfwGetWindowSizeX11,
                .setWindowSize = _glfwSetWindowSizeX11,
                .setWindowSizeLimits = _glfwSetWindowSizeLimitsX11,
                .setWindowAspectRatio = _glfwSetWindowAspectRatioX11,
                .getFramebufferSize = _glfwGetFramebufferSizeX11,
                .getWindowFrameSize = _glfwGetWindowFrameSizeX11,
                .getWindowContentScale = _glfwGetWindowContentScaleX11,
                .iconifyWindow = _glfwIconifyWindowX11,
                .restoreWindow = _glfwRestoreWindowX11,
                .maximizeWindow = _glfwMaximizeWindowX11,
                .showWindow = _glfwShowWindowX11,
                .hideWindow = _glfwHideWindowX11,
                .requestWindowAttention = _glfwRequestWindowAttentionX11,
                .focusWindow = _glfwFocusWindowX11,
                .setWindowMonitor = _glfwSetWindowMonitorX11,
                .windowFocused = _glfwWindowFocusedX11,
                .windowIconified = _glfwWindowIconifiedX11,
                .windowVisible = _glfwWindowVisibleX11,
                .windowMaximized = _glfwWindowMaximizedX11,
                .windowHovered = _glfwWindowHoveredX11,
                .framebufferTransparent = _glfwFramebufferTransparentX11,
                .getWindowOpacity = _glfwGetWindowOpacityX11,
                .setWindowResizable = _glfwSetWindowResizableX11,
                .setWindowDecorated = _glfwSetWindowDecoratedX11,
                .setWindowFloating = _glfwSetWindowFloatingX11,
                .setWindowOpacity = _glfwSetWindowOpacityX11,
                .setWindowMousePassthrough = _glfwSetWindowMousePassthroughX11,
                .pollEvents = _glfwPollEventsX11,
                .waitEvents = _glfwWaitEventsX11,
                .waitEventsTimeout = _glfwWaitEventsTimeoutX11,
                .postEmptyEvent = _glfwPostEmptyEventX11,
                .getEGLPlatform = _glfwGetEGLPlatformX11,
                .getEGLNativeDisplay = _glfwGetEGLNativeDisplayX11,
                .getEGLNativeWindow = _glfwGetEGLNativeWindowX11,
                .getRequiredInstanceExtensions = _glfwGetRequiredInstanceExtensionsX11,
                .getPhysicalDevicePresentationSupport = _glfwGetPhysicalDevicePresentationSupportX11,
                .createWindowSurface = _glfwCreateWindowSurfaceX11
            };
            _glfw.platform = x11;
            // HACK: If the application has left the locale as "C" then both wide
            //       character text input and explicit UTF-8 input via XIM will break
            //       This sets the CTYPE part of the current locale from the environment
            //       in the hope that it is set to something more sane than "C"
            if (strcmp(setlocale(LC_CTYPE, NULL), "C") == 0)
                setlocale(LC_CTYPE, "");

            if (!linux_load_libs()) {
                linux_unload_libs();
                return VKFW_ERROR_PLATFORM_ERROR;
            }
            
            XInitThreads();
            XrmInitialize();

            Display* display = XOpenDisplay(NULL);
            if (!display) {
                OSModule.close(module);
                return VKFW_ERROR_PLATFORM_UNAVAILABLE;
            }
            _glfw.x11.display = display;
        break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    


    switch(platform) {
        case WIN32:
        
        
            // Load necessary libraries (DLLs)
            {
                if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (const WCHAR*) &_glfw, (HMODULE*) &_glfw.win32.instance)) {
                    terminate();
                    return VKFW_ERROR_PLATFORM_ERROR;
                }

                if (!win32_load_libs()) {
                    terminate();
                    return VKFW_ERROR_PLATFORM_ERROR;
                }
                
                
                
            }
        

            // Create key code translation tables
            {
                int scancode;

                memset(_glfw.win32.keycodes, -1, sizeof(_glfw.win32.keycodes));
                memset(_glfw.win32.scancodes, -1, sizeof(_glfw.win32.scancodes));

                _glfw.win32.keycodes[0x00B] = GLFW_KEY_0;
                _glfw.win32.keycodes[0x002] = GLFW_KEY_1;
                _glfw.win32.keycodes[0x003] = GLFW_KEY_2;
                _glfw.win32.keycodes[0x004] = GLFW_KEY_3;
                _glfw.win32.keycodes[0x005] = GLFW_KEY_4;
                _glfw.win32.keycodes[0x006] = GLFW_KEY_5;
                _glfw.win32.keycodes[0x007] = GLFW_KEY_6;
                _glfw.win32.keycodes[0x008] = GLFW_KEY_7;
                _glfw.win32.keycodes[0x009] = GLFW_KEY_8;
                _glfw.win32.keycodes[0x00A] = GLFW_KEY_9;
                _glfw.win32.keycodes[0x01E] = GLFW_KEY_A;
                _glfw.win32.keycodes[0x030] = GLFW_KEY_B;
                _glfw.win32.keycodes[0x02E] = GLFW_KEY_C;
                _glfw.win32.keycodes[0x020] = GLFW_KEY_D;
                _glfw.win32.keycodes[0x012] = GLFW_KEY_E;
                _glfw.win32.keycodes[0x021] = GLFW_KEY_F;
                _glfw.win32.keycodes[0x022] = GLFW_KEY_G;
                _glfw.win32.keycodes[0x023] = GLFW_KEY_H;
                _glfw.win32.keycodes[0x017] = GLFW_KEY_I;
                _glfw.win32.keycodes[0x024] = GLFW_KEY_J;
                _glfw.win32.keycodes[0x025] = GLFW_KEY_K;
                _glfw.win32.keycodes[0x026] = GLFW_KEY_L;
                _glfw.win32.keycodes[0x032] = GLFW_KEY_M;
                _glfw.win32.keycodes[0x031] = GLFW_KEY_N;
                _glfw.win32.keycodes[0x018] = GLFW_KEY_O;
                _glfw.win32.keycodes[0x019] = GLFW_KEY_P;
                _glfw.win32.keycodes[0x010] = GLFW_KEY_Q;
                _glfw.win32.keycodes[0x013] = GLFW_KEY_R;
                _glfw.win32.keycodes[0x01F] = GLFW_KEY_S;
                _glfw.win32.keycodes[0x014] = GLFW_KEY_T;
                _glfw.win32.keycodes[0x016] = GLFW_KEY_U;
                _glfw.win32.keycodes[0x02F] = GLFW_KEY_V;
                _glfw.win32.keycodes[0x011] = GLFW_KEY_W;
                _glfw.win32.keycodes[0x02D] = GLFW_KEY_X;
                _glfw.win32.keycodes[0x015] = GLFW_KEY_Y;
                _glfw.win32.keycodes[0x02C] = GLFW_KEY_Z;

                _glfw.win32.keycodes[0x028] = GLFW_KEY_APOSTROPHE;
                _glfw.win32.keycodes[0x02B] = GLFW_KEY_BACKSLASH;
                _glfw.win32.keycodes[0x033] = GLFW_KEY_COMMA;
                _glfw.win32.keycodes[0x00D] = GLFW_KEY_EQUAL;
                _glfw.win32.keycodes[0x029] = GLFW_KEY_GRAVE_ACCENT;
                _glfw.win32.keycodes[0x01A] = GLFW_KEY_LEFT_BRACKET;
                _glfw.win32.keycodes[0x00C] = GLFW_KEY_MINUS;
                _glfw.win32.keycodes[0x034] = GLFW_KEY_PERIOD;
                _glfw.win32.keycodes[0x01B] = GLFW_KEY_RIGHT_BRACKET;
                _glfw.win32.keycodes[0x027] = GLFW_KEY_SEMICOLON;
                _glfw.win32.keycodes[0x035] = GLFW_KEY_SLASH;
                _glfw.win32.keycodes[0x056] = GLFW_KEY_WORLD_2;

                _glfw.win32.keycodes[0x00E] = GLFW_KEY_BACKSPACE;
                _glfw.win32.keycodes[0x153] = GLFW_KEY_DELETE;
                _glfw.win32.keycodes[0x14F] = GLFW_KEY_END;
                _glfw.win32.keycodes[0x01C] = GLFW_KEY_ENTER;
                _glfw.win32.keycodes[0x001] = GLFW_KEY_ESCAPE;
                _glfw.win32.keycodes[0x147] = GLFW_KEY_HOME;
                _glfw.win32.keycodes[0x152] = GLFW_KEY_INSERT;
                _glfw.win32.keycodes[0x15D] = GLFW_KEY_MENU;
                _glfw.win32.keycodes[0x151] = GLFW_KEY_PAGE_DOWN;
                _glfw.win32.keycodes[0x149] = GLFW_KEY_PAGE_UP;
                _glfw.win32.keycodes[0x045] = GLFW_KEY_PAUSE;
                _glfw.win32.keycodes[0x039] = GLFW_KEY_SPACE;
                _glfw.win32.keycodes[0x00F] = GLFW_KEY_TAB;
                _glfw.win32.keycodes[0x03A] = GLFW_KEY_CAPS_LOCK;
                _glfw.win32.keycodes[0x145] = GLFW_KEY_NUM_LOCK;
                _glfw.win32.keycodes[0x046] = GLFW_KEY_SCROLL_LOCK;
                _glfw.win32.keycodes[0x03B] = GLFW_KEY_F1;
                _glfw.win32.keycodes[0x03C] = GLFW_KEY_F2;
                _glfw.win32.keycodes[0x03D] = GLFW_KEY_F3;
                _glfw.win32.keycodes[0x03E] = GLFW_KEY_F4;
                _glfw.win32.keycodes[0x03F] = GLFW_KEY_F5;
                _glfw.win32.keycodes[0x040] = GLFW_KEY_F6;
                _glfw.win32.keycodes[0x041] = GLFW_KEY_F7;
                _glfw.win32.keycodes[0x042] = GLFW_KEY_F8;
                _glfw.win32.keycodes[0x043] = GLFW_KEY_F9;
                _glfw.win32.keycodes[0x044] = GLFW_KEY_F10;
                _glfw.win32.keycodes[0x057] = GLFW_KEY_F11;
                _glfw.win32.keycodes[0x058] = GLFW_KEY_F12;
                _glfw.win32.keycodes[0x064] = GLFW_KEY_F13;
                _glfw.win32.keycodes[0x065] = GLFW_KEY_F14;
                _glfw.win32.keycodes[0x066] = GLFW_KEY_F15;
                _glfw.win32.keycodes[0x067] = GLFW_KEY_F16;
                _glfw.win32.keycodes[0x068] = GLFW_KEY_F17;
                _glfw.win32.keycodes[0x069] = GLFW_KEY_F18;
                _glfw.win32.keycodes[0x06A] = GLFW_KEY_F19;
                _glfw.win32.keycodes[0x06B] = GLFW_KEY_F20;
                _glfw.win32.keycodes[0x06C] = GLFW_KEY_F21;
                _glfw.win32.keycodes[0x06D] = GLFW_KEY_F22;
                _glfw.win32.keycodes[0x06E] = GLFW_KEY_F23;
                _glfw.win32.keycodes[0x076] = GLFW_KEY_F24;
                _glfw.win32.keycodes[0x038] = GLFW_KEY_LEFT_ALT;
                _glfw.win32.keycodes[0x01D] = GLFW_KEY_LEFT_CONTROL;
                _glfw.win32.keycodes[0x02A] = GLFW_KEY_LEFT_SHIFT;
                _glfw.win32.keycodes[0x15B] = GLFW_KEY_LEFT_SUPER;
                _glfw.win32.keycodes[0x137] = GLFW_KEY_PRINT_SCREEN;
                _glfw.win32.keycodes[0x138] = GLFW_KEY_RIGHT_ALT;
                _glfw.win32.keycodes[0x11D] = GLFW_KEY_RIGHT_CONTROL;
                _glfw.win32.keycodes[0x036] = GLFW_KEY_RIGHT_SHIFT;
                _glfw.win32.keycodes[0x15C] = GLFW_KEY_RIGHT_SUPER;
                _glfw.win32.keycodes[0x150] = GLFW_KEY_DOWN;
                _glfw.win32.keycodes[0x14B] = GLFW_KEY_LEFT;
                _glfw.win32.keycodes[0x14D] = GLFW_KEY_RIGHT;
                _glfw.win32.keycodes[0x148] = GLFW_KEY_UP;

                _glfw.win32.keycodes[0x052] = GLFW_KEY_KP_0;
                _glfw.win32.keycodes[0x04F] = GLFW_KEY_KP_1;
                _glfw.win32.keycodes[0x050] = GLFW_KEY_KP_2;
                _glfw.win32.keycodes[0x051] = GLFW_KEY_KP_3;
                _glfw.win32.keycodes[0x04B] = GLFW_KEY_KP_4;
                _glfw.win32.keycodes[0x04C] = GLFW_KEY_KP_5;
                _glfw.win32.keycodes[0x04D] = GLFW_KEY_KP_6;
                _glfw.win32.keycodes[0x047] = GLFW_KEY_KP_7;
                _glfw.win32.keycodes[0x048] = GLFW_KEY_KP_8;
                _glfw.win32.keycodes[0x049] = GLFW_KEY_KP_9;
                _glfw.win32.keycodes[0x04E] = GLFW_KEY_KP_ADD;
                _glfw.win32.keycodes[0x053] = GLFW_KEY_KP_DECIMAL;
                _glfw.win32.keycodes[0x135] = GLFW_KEY_KP_DIVIDE;
                _glfw.win32.keycodes[0x11C] = GLFW_KEY_KP_ENTER;
                _glfw.win32.keycodes[0x059] = GLFW_KEY_KP_EQUAL;
                _glfw.win32.keycodes[0x037] = GLFW_KEY_KP_MULTIPLY;
                _glfw.win32.keycodes[0x04A] = GLFW_KEY_KP_SUBTRACT;

                for (scancode = 0;  scancode < 512;  scancode++)
                {
                    if (_glfw.win32.keycodes[scancode] > 0)
                        _glfw.win32.scancodes[_glfw.win32.keycodes[scancode]] = scancode;
                }
            }
            
            _glfwUpdateKeyNamesWin32();

            if (_glfwIsWindows10Version1703OrGreaterWin32())
                SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            else if (IsWindows8Point1OrGreater())
                SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
            else if (IsWindowsVistaOrGreater())
                SetProcessDPIAware();

            // Creates a dummy window for behind-the-scenes work
            {
                MSG msg;
                WNDCLASSEXW wc = { sizeof(wc) };

                wc.style         = CS_OWNDC;
                wc.lpfnWndProc   = (WNDPROC) win32_helperWindowProc;
                wc.hInstance     = _glfw.win32.instance;
                wc.lpszClassName = L"GLFW3 Helper";

                _glfw.win32.helperWindowClass = RegisterClassExW(&wc);
                if (!_glfw.win32.helperWindowClass) {
                    terminate();
                    return VKFW_ERROR_PLATFORM_ERROR;
                }

                _glfw.win32.helperWindowHandle =
                    CreateWindowExW(WS_EX_OVERLAPPEDWINDOW,
                                    MAKEINTATOM(_glfw.win32.helperWindowClass),
                                    L"GLFW message window",
                                    WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                                    0, 0, 1, 1,
                                    NULL, NULL,
                                    _glfw.win32.instance,
                                    NULL);

                if (!_glfw.win32.helperWindowHandle)
                {
                    terminate();
                    return VKFW_ERROR_PLATFORM_ERROR;
                }

                // HACK: The command to the first ShowWindow call is ignored if the parent
                //       process passed along a STARTUPINFO, so clear that with a no-op call
                ShowWindow(_glfw.win32.helperWindowHandle, SW_HIDE);

                // Register for HID device notifications
                {
                    DEV_BROADCAST_DEVICEINTERFACE_W dbi;
                    ZeroMemory(&dbi, sizeof(dbi));
                    dbi.dbcc_size = sizeof(dbi);
                    dbi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
                    dbi.dbcc_classguid = GUID_DEVINTERFACE_HID;

                    _glfw.win32.deviceNotificationHandle =
                        RegisterDeviceNotificationW(_glfw.win32.helperWindowHandle,
                                                    (DEV_BROADCAST_HDR*) &dbi,
                                                    DEVICE_NOTIFY_WINDOW_HANDLE);
                }

                while (PeekMessageW(&msg, _glfw.win32.helperWindowHandle, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }

            _glfwPollMonitorsWin32();
        break;
        case LINUX:

            if (_glfw.x11.xlib.utf8LookupString && _glfw.x11.xlib.utf8SetWMProperties)
                _glfw.x11.xlib.utf8 = GLFW_TRUE;

            _glfw.x11.screen = DefaultScreen(_glfw.x11.display);
            _glfw.x11.root = RootWindow(_glfw.x11.display, _glfw.x11.screen);
            _glfw.x11.context = XUniqueContext();

            getSystemContentScale(&_glfw.x11.contentScaleX, &_glfw.x11.contentScaleY);

            if (!createEmptyEventPipe())
            {
                terminate();
                switch(glfwGetError(NULL)) {
                    case GLFW_PLATFORM_UNAVAILABLE: return VKFW_ERROR_PLATFORM_UNAVAILABLE;
                    case GLFW_PLATFORM_ERROR:       return VKFW_ERROR_PLATFORM_ERROR;
                    default: return VKFW_ERROR_UNKNOWN;
                }
            }

            if (!initExtensions())
            {
                terminate();
                switch(glfwGetError(NULL)) {
                    case GLFW_PLATFORM_UNAVAILABLE: return VKFW_ERROR_PLATFORM_UNAVAILABLE;
                    case GLFW_PLATFORM_ERROR:       return VKFW_ERROR_PLATFORM_ERROR;
                    default: return VKFW_ERROR_UNKNOWN;
                }
            }

            _glfw.x11.helperWindowHandle = createHelperWindow();
            _glfw.x11.hiddenCursorHandle = createHiddenCursor();

            if (XSupportsLocale() && _glfw.x11.xlib.utf8)
            {
                XSetLocaleModifiers("");

                // If an IM is already present our callback will be called right away
                XRegisterIMInstantiateCallback(_glfw.x11.display,
                                               NULL, NULL, NULL,
                                               inputMethodInstantiateCallback,
                                               NULL);
            }

            _glfwPollMonitorsX11();
        break;
        default: return VKFW_ERROR_UNKNOWN;
    }



    
    // Adds the built-in set of gamepad mappings
    {
        size_t i;
        const size_t count = sizeof(_glfwDefaultMappings) / sizeof(char*);
        _glfw.mappings = _glfw_calloc(count, sizeof(_GLFWmapping));

        for (i = 0;  i < count;  i++)
        {
            if (parseMapping(&_glfw.mappings[_glfw.mappingCount], _glfwDefaultMappings[i]))
                _glfw.mappingCount++;
        }
    }

    
    switch(platform) {
        case WIN32:
            QueryPerformanceFrequency((LARGE_INTEGER*) &_glfw.timer.win32.frequency);
        break;
        case LINUX:
            _glfw.timer.posix.clock = CLOCK_REALTIME;
            _glfw.timer.posix.frequency = 1000000000;
            
            struct timespec ts;
            if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
                _glfw.timer.posix.clock = CLOCK_MONOTONIC;
        break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    
    _glfw.timer.offset = _glfwPlatformGetTimerValue();

    _glfw.initialized = GLFW_TRUE;


    {
        // The default is OpenGL with minimum version 1.0
        memset(&_glfw.hints.context, 0, sizeof(_glfw.hints.context));
        _glfw.hints.context.client = GLFW_OPENGL_API;
        _glfw.hints.context.source = GLFW_NATIVE_CONTEXT_API;
        _glfw.hints.context.major  = 1;
        _glfw.hints.context.minor  = 0;

        // The default is a focused, visible, resizable window with decorations
        memset(&_glfw.hints.window, 0, sizeof(_glfw.hints.window));
        _glfw.hints.window.resizable    = GLFW_TRUE;
        _glfw.hints.window.visible      = GLFW_TRUE;
        _glfw.hints.window.decorated    = GLFW_TRUE;
        _glfw.hints.window.focused      = GLFW_TRUE;
        _glfw.hints.window.autoIconify  = GLFW_TRUE;
        _glfw.hints.window.centerCursor = GLFW_TRUE;
        _glfw.hints.window.focusOnShow  = GLFW_TRUE;
        _glfw.hints.window.xpos         = GLFW_ANY_POSITION;
        _glfw.hints.window.ypos         = GLFW_ANY_POSITION;
        _glfw.hints.window.scaleFramebuffer = GLFW_TRUE;

        // The default is 24 bits of color, 24 bits of depth and 8 bits of stencil,
        // double buffered
        memset(&_glfw.hints.framebuffer, 0, sizeof(_glfw.hints.framebuffer));
        _glfw.hints.framebuffer.redBits      = 8;
        _glfw.hints.framebuffer.greenBits    = 8;
        _glfw.hints.framebuffer.blueBits     = 8;
        _glfw.hints.framebuffer.alphaBits    = 8;
        _glfw.hints.framebuffer.depthBits    = 24;
        _glfw.hints.framebuffer.stencilBits  = 8;
        _glfw.hints.framebuffer.doublebuffer = GLFW_TRUE;

        // The default is to select the highest available refresh rate
        _glfw.hints.refreshRate = GLFW_DONT_CARE;
    }
    
    
    
    _glfw.callbacks.monitor = (GLFWmonitorfun) pCreateInfo[0].callbacks.monitorConnection;
    if (initJoysticks()) {
        _glfw.callbacks.joystick = (GLFWjoystickfun) pCreateInfo[0].callbacks.joystickConnection;
    }
    
    vfkwInstanceInitialized = VKFW_TRUE;
    pInstance[0] = (VkfwInstance) &instanceHandleAddress;
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwDestroyInstance(VkfwInstance instance, const VkfwAllocationCallbacks* pAllocator) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pAllocator != initAllocator) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    terminate();
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR:             break;
        case GLFW_PLATFORM_ERROR:       return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    initAllocator = NULL;
    vfkwInstanceInitialized = VKFW_FALSE;
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateInstanceProperties(VkfwInstance instance, VkfwInstanceProperties* pProperties) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pProperties == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    switch(glfwGetPlatform()) {
        case GLFW_PLATFORM_WIN32  : pProperties[0].usedPlatform = VKFW_INSTANCE_PLATFORM_WIN32  ; break;
        case GLFW_PLATFORM_COCOA  : pProperties[0].usedPlatform = VKFW_INSTANCE_PLATFORM_COCOA  ; break;
        case GLFW_PLATFORM_WAYLAND: pProperties[0].usedPlatform = VKFW_INSTANCE_PLATFORM_WAYLAND; break;
        case GLFW_PLATFORM_X11    : pProperties[0].usedPlatform = VKFW_INSTANCE_PLATFORM_X11    ; break;
        case GLFW_PLATFORM_NULL   : pProperties[0].usedPlatform = VKFW_INSTANCE_PLATFORM_NULL   ; break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].rawMouseMotionSupported = glfwRawMouseMotionSupported();
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].vulkanFound = glfwVulkanSupported();
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].vulkanLoader = (PFN_vkfwVkGetInstanceProcAddr) glfwGetInstanceProcAddress(NULL, "vkGetInstanceProcAddr");
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].requiredInstanceExtensions = glfwGetRequiredInstanceExtensions(&pProperties[0].requiredInstanceExtensionCount);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_API_UNAVAILABLE: return VKFW_ERROR_API_UNAVAILABLE;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    pProperties[0].timerFrequency = glfwGetTimerFrequency();
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwGetKeyScancode(VkfwInstance instance, VkfwKey key, int32_t* pScancode) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pScancode == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    pScancode[0] = glfwGetKeyScancode(key);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_INVALID_ENUM: return VKFW_ERROR_INVALID_ENUM_VALUE;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwGetKeyName(VkfwInstance instance, VkfwKey key, int32_t scancode, const char** pKeyName) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pKeyName == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    pKeyName[0] = glfwGetKeyName(key, scancode);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_INVALID_VALUE: return VKFW_ERROR_INVALID_NUMERIC_VALUE;
        case GLFW_INVALID_ENUM: return VKFW_ERROR_INVALID_ENUM_VALUE;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwPostEmptyEvent(VkfwInstance instance) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    
    glfwPostEmptyEvent();
    
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwProcessEvents(VkfwInstance instance, double timeout, VkfwBool32 waitIndefinitely) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    
    switch(waitIndefinitely) {
        case VKFW_TRUE: glfwWaitEvents(); break;
        case VKFW_FALSE:
            if(timeout == 0) glfwPollEvents();
            else glfwWaitEventsTimeout(timeout); 
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_INVALID_VALUE: return VKFW_ERROR_INVALID_NUMERIC_VALUE;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwGetClipboardString(VkfwInstance instance, const char** pClipboardString) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pClipboardString == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    pClipboardString[0] = glfwGetClipboardString(NULL);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_FORMAT_UNAVAILABLE: return VKFW_ERROR_RESULT_NOT_AVAILABLE;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetClipboardString(VkfwInstance instance, const char* clipboardString) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    
    if(clipboardString == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE; /* or should we allow setting clipboard to NULL ? */
    
    glfwSetClipboardString(NULL, clipboardString);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwGetTimerValue(VkfwInstance instance, uint64_t* pTimerValue) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pTimerValue == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    pTimerValue[0] = glfwGetTimerValue();
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateMonitors(VkfwInstance instance, uint32_t* pMonitorCount, VkfwMonitor* pMonitors) {
    uint32_t i;

    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pMonitorCount == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    GLFWmonitor** localMonitorArray = glfwGetMonitors((int*)pMonitorCount);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    if(pMonitors != NULL) {
        for(i = 0; i < pMonitorCount[0]; i++) {
            pMonitors[i] = (VkfwMonitor) localMonitorArray[i];
        }
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateMonitorProperties(VkfwMonitor monitor, VkfwMonitorProperties* pProperties) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(monitor == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(pProperties == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    GLFWmonitor* localMonitor = (GLFWmonitor*) monitor;
    
    glfwGetMonitorPos(localMonitor, &pProperties[0].viewportPosition.x, &pProperties[0].viewportPosition.y);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }

    glfwGetMonitorWorkarea(localMonitor, &pProperties[0].workarea.offset.x, &pProperties[0].workarea.offset.y, &pProperties[0].workarea.extent.width, &pProperties[0].workarea.extent.height);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    glfwGetMonitorPhysicalSize(localMonitor, &pProperties[0].physicalSizeMM.width, &pProperties[0].physicalSizeMM.height);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    glfwGetMonitorContentScale(localMonitor, &pProperties[0].contentScale.xScale, &pProperties[0].contentScale.yScale);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    pProperties[0].pName = glfwGetMonitorName(localMonitor);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].pUserPointer = glfwGetMonitorUserPointer(localMonitor);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].pVideoModes = (const VkfwVideoMode*) glfwGetVideoModes(localMonitor, &pProperties[0].videoModeCount);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    pProperties[0].pCurrentVideoMode = (const VkfwVideoMode*) glfwGetVideoMode(localMonitor);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    pProperties[0].pCurrentGammeRamp = (const VkfwGammaRamp*) glfwGetGammaRamp(localMonitor);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_FEATURE_UNAVAILABLE: break; /* this is not a mistake, but intended behavior on Wayland. Therefore, the returned NULL pointer is enough, and there needs to be no error code returned from VKFW. */
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetMonitorUserPointer(VkfwMonitor monitor, void* pUserPointer) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(monitor == NULL) return VKFW_ERROR_INVALID_HANDLE;
    
    GLFWmonitor* localMonitor = (GLFWmonitor*) monitor;
    
    glfwSetMonitorUserPointer(localMonitor, pUserPointer);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetMonitorGammaRamp(VkfwMonitor monitor, const VkfwGammaRamp* pGammaRamp) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(monitor == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(pGammaRamp == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    GLFWmonitor* localMonitor = (GLFWmonitor*) monitor;
    
    glfwSetGammaRamp(localMonitor, (const GLFWgammaramp*) pGammaRamp);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_FEATURE_UNAVAILABLE: break; /* this is not a mistake, but intended behavior on Wayland. Therefore, the returned NULL pointer is enough, and there needs to be no error code returned from VKFW. */
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwGetGammeRampFromGammaValue(VkfwMonitor monitor, float gamma, VkfwGammaRamp* pGammaRamp) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(monitor == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(pGammaRamp == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    GLFWmonitor* localMonitor = (GLFWmonitor*) monitor;
    
    /* HACK: since we don't have access to the internal utility function, we temporarily set the monitor to the new ramp, read out the value and then reset it. */
    
    /* 1. copy old gamma ramp. */
    const VkfwGammaRamp* currentGammaRampPtr = (const VkfwGammaRamp*) glfwGetGammaRamp(localMonitor);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    if(currentGammaRampPtr == NULL) return VKFW_ERROR_UNKNOWN;
    VkfwGammaRamp oldGammaRamp = currentGammaRampPtr[0];
    
    /* 2. set the new ramp from the gamme value. */
    glfwSetGamma(localMonitor, gamma);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_INVALID_VALUE: return VKFW_ERROR_INVALID_NUMERIC_VALUE;
        case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    /* 3. copy new gamma ramp. */
    currentGammaRampPtr = (const VkfwGammaRamp*) glfwGetGammaRamp(localMonitor);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    if(currentGammaRampPtr == NULL) return VKFW_ERROR_UNKNOWN;
    VkfwGammaRamp newGammaRamp = currentGammaRampPtr[0];
    
    /* 4. reset monitor to old gamma ramp. */
    glfwSetGammaRamp(localMonitor, (const GLFWgammaramp*) &oldGammaRamp);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    /* 5. return the generated new ramp. */
    pGammaRamp[0] = newGammaRamp;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwCreateWindow(VkfwMonitor monitor, const VkfwWindowCreateInfo* pCreateInfo, const VkfwAllocationCallbacks* pAllocator, VkfwWindow* pWindow) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(monitor == NULL) return VKFW_ERROR_INVALID_HANDLE; /* the GLFWmonitor* is set to NULL not by this parameter, but by the flag pCreateInfo[0].initialState.fullscreen */
    if(pCreateInfo == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    if(pWindow == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    if(pAllocator != initAllocator) return VKFW_ERROR_FEATURE_NOT_SUPPORTED; /* we don't allow different allocators between instance and window since GLFW doesn't support it */
    
    GLFWmonitor* underlyingMonitorHandle = (GLFWmonitor*) monitor;
    
    /* 1. setting the hints */
    
    if(pCreateInfo[0].sType != VKFW_STRUCTURE_TYPE_WINDOW_CREATE_INFO)      return VKFW_ERROR_INVALID_ENUM_VALUE;
    if(pCreateInfo[0].pNext != NULL)                                        return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
    
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_NO_INITIAL_FOCUS_BIT) {
        glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);
    } else {
        glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_DONT_CENTER_CURSOR_BIT) {
        glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_FALSE);
    } else {
        glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_TRANSPARENT_FRAMEBUFFER_BIT) {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_SCALE_TO_MONITOR_BIT) {
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_NO_SCALE_FRAMEBUFFER_BIT) {
        glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE);
    } else {
        glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_COCOA_GRAPHICS_SWITCHING_BIT_COCOA) {
        glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GLFW_FALSE);
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_WIN32_KEYBOARD_MENU_BIT_WIN32) {
        glfwWindowHint(GLFW_WIN32_KEYBOARD_MENU, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_WIN32_KEYBOARD_MENU, GLFW_FALSE);
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_WIN32_SHOWDEFAULT_BIT_WIN32) {
        glfwWindowHint(GLFW_WIN32_SHOWDEFAULT, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_WIN32_SHOWDEFAULT, GLFW_FALSE);
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    switch(pCreateInfo[0].initialState.resizable) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.visible) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.decorated) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.autoIconify) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.floating) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.maximized) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.focusOnShow) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.mousePassthrough) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    glfwWindowHint(GLFW_POSITION_X, pCreateInfo[0].initialState.position.x);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwWindowHint(GLFW_POSITION_Y, pCreateInfo[0].initialState.position.y);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    if(pCreateInfo[0].requestedVideoMode.width != pCreateInfo[0].initialState.size.width)   return VKFW_ERROR_INVALID_NUMERIC_VALUE;
    if(pCreateInfo[0].requestedVideoMode.height != pCreateInfo[0].initialState.size.height) return VKFW_ERROR_INVALID_NUMERIC_VALUE;
    
    glfwWindowHint(GLFW_RED_BITS, pCreateInfo[0].requestedVideoMode.redBits);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwWindowHint(GLFW_GREEN_BITS, pCreateInfo[0].requestedVideoMode.greenBits);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwWindowHint(GLFW_BLUE_BITS, pCreateInfo[0].requestedVideoMode.blueBits);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwWindowHint(GLFW_REFRESH_RATE, pCreateInfo[0].requestedVideoMode.refreshRate);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    glfwWindowHintString(GLFW_COCOA_FRAME_NAME, pCreateInfo[0].cocoaFrameName_COCOA);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwWindowHintString(GLFW_X11_CLASS_NAME, pCreateInfo[0].x11ClassName_X11);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, pCreateInfo[0].x11InstanceName_X11);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, pCreateInfo[0].waylandAppID_WL);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    
    /* 2. actually creating the handle */
    
    GLFWmonitor* createParameterMonitorHandle;
    switch(pCreateInfo[0].initialState.fullscreen) {
        case VKFW_TRUE:
            createParameterMonitorHandle = underlyingMonitorHandle;
        break;
        case VKFW_FALSE:
            createParameterMonitorHandle = NULL;
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    
    GLFWwindow* underlyingWindowHandle = glfwCreateWindow(pCreateInfo[0].initialState.size.width, pCreateInfo[0].initialState.size.height, pCreateInfo[0].initialState.title, createParameterMonitorHandle, NULL);
    if(underlyingWindowHandle == NULL) {
        switch(glfwGetError(NULL)) {
            case GLFW_INVALID_VALUE:        return VKFW_ERROR_INVALID_NUMERIC_VALUE;
            case GLFW_FORMAT_UNAVAILABLE:   return VKFW_ERROR_PIXEL_FORMAT_NOT_SUPPORTED;
            case GLFW_PLATFORM_ERROR:       return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    
    /* 3. post-creating settings not exposed in hints */
    
    switch(pCreateInfo[0].initialState.iconified) {
        case VKFW_TRUE:
            glfwIconifyWindow(underlyingWindowHandle);
        break;
        case VKFW_FALSE:
            /* off by default */
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    switch(pCreateInfo[0].initialState.shouldClose) {
        case VKFW_TRUE:
            glfwSetWindowShouldClose(underlyingWindowHandle, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            /* off by default */
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.stickyKeys) {
        case VKFW_TRUE:
            glfwSetInputMode(underlyingWindowHandle, GLFW_STICKY_KEYS, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            /* off by default */
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    switch(pCreateInfo[0].initialState.stickyMouseButtons) {
        case VKFW_TRUE:
            glfwSetInputMode(underlyingWindowHandle, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            /* off by default */
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    switch(pCreateInfo[0].initialState.lockKeyMods) {
        case VKFW_TRUE:
            glfwSetInputMode(underlyingWindowHandle, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            /* off by default */
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    switch(pCreateInfo[0].initialState.rawMouseMotion) {
        case VKFW_TRUE:
            glfwSetInputMode(underlyingWindowHandle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            /* off by default */
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    switch(pCreateInfo[0].initialState.cursorMode) {
        case VKFW_CURSOR_MODE_NORMAL:
            /* the default after creation */
        break;
        case VKFW_CURSOR_MODE_HIDDEN:
            glfwSetInputMode(underlyingWindowHandle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        break;
        case VKFW_CURSOR_MODE_DISABLED:
            glfwSetInputMode(underlyingWindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
        case VKFW_CURSOR_MODE_CAPTURED:
            glfwSetInputMode(underlyingWindowHandle, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_SET_INITIAL_CURSOR_POSITION_BIT) {
        glfwSetCursorPos(underlyingWindowHandle, pCreateInfo[0].initialState.cursorPosition.x, pCreateInfo[0].initialState.cursorPosition.y);
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(pCreateInfo[0].initialState.opacity != 1.0f) {
        glfwSetWindowOpacity(underlyingWindowHandle, pCreateInfo[0].initialState.opacity);
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    glfwSetWindowUserPointer(underlyingWindowHandle, pCreateInfo[0].initialState.pUserPointer);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    
    /* 4. all the callbacks! */
    
    glfwSetWindowPosCallback(underlyingWindowHandle, (GLFWwindowposfun) pCreateInfo[0].callbacks.positionChange);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowSizeCallback(underlyingWindowHandle, (GLFWwindowsizefun) pCreateInfo[0].callbacks.sizeChange);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetFramebufferSizeCallback(underlyingWindowHandle, (GLFWframebuffersizefun) pCreateInfo[0].callbacks.framebufferSizeChange);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowContentScaleCallback(underlyingWindowHandle, (GLFWwindowcontentscalefun) pCreateInfo[0].callbacks.contentScaleChange);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetCursorPosCallback(underlyingWindowHandle, (GLFWcursorposfun) pCreateInfo[0].callbacks.cursorPositionChange);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowFocusCallback(underlyingWindowHandle, (GLFWwindowfocusfun) pCreateInfo[0].callbacks.focusChange);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowIconifyCallback(underlyingWindowHandle, (GLFWwindowiconifyfun) pCreateInfo[0].callbacks.iconficationChange);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowMaximizeCallback(underlyingWindowHandle, (GLFWwindowmaximizefun) pCreateInfo[0].callbacks.maximizationChange);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    glfwSetWindowCloseCallback(underlyingWindowHandle, (GLFWwindowclosefun) pCreateInfo[0].callbacks.closeButtonClicked);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowRefreshCallback(underlyingWindowHandle, (GLFWwindowrefreshfun) pCreateInfo[0].callbacks.contentAreaNeedsToBeRedrawn);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetDropCallback(underlyingWindowHandle, (GLFWdropfun) pCreateInfo[0].callbacks.pathDrop);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    glfwSetMouseButtonCallback(underlyingWindowHandle, (GLFWmousebuttonfun) pCreateInfo[0].callbacks.mouseButtonInput);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetScrollCallback(underlyingWindowHandle, (GLFWscrollfun) pCreateInfo[0].callbacks.scrollInput);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetCursorEnterCallback(underlyingWindowHandle, (GLFWcursorenterfun) pCreateInfo[0].callbacks.cursorEnterOrLeaveContentArea);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetKeyCallback(underlyingWindowHandle, (GLFWkeyfun) pCreateInfo[0].callbacks.keyInput);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetCharCallback(underlyingWindowHandle, (GLFWcharfun) pCreateInfo[0].callbacks.unicodeCharacterInput);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwSetCharModsCallback(underlyingWindowHandle, (GLFWcharmodsfun) pCreateInfo[0].callbacks.unicodeCharacterInputWithModifiers);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    
    /* 5. finally, return the handle */
    
    pWindow[0] = malloc(sizeof(VkfwWindow_t));
    if(pWindow[0] == NULL) return VKFW_ERROR_OUT_OF_MEMORY;
    pWindow[0][0].monitorHandle = underlyingMonitorHandle;
    pWindow[0][0].windowHandle  = underlyingWindowHandle;
    pWindow[0][0].usedVideoMode = pCreateInfo[0].requestedVideoMode; /* at this point, creation was successful so the request for the mode was honored, so it is the actually used mode */
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwDestroyWindow(VkfwWindow window, const VkfwAllocationCallbacks* pAllocator) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(pAllocator != initAllocator) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    glfwDestroyWindow(window[0].windowHandle);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    free(window);
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateWindowProperties(VkfwWindow window, VkfwWindowProperties* pProperties) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(pProperties == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    GLFWwindow* underlyingWindowHandle = window[0].windowHandle;
    
    GLFWmonitor* localMonitor = glfwGetWindowMonitor(underlyingWindowHandle);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    if(localMonitor != NULL && localMonitor != window[0].monitorHandle) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.fullscreen = (localMonitor != NULL);
    pProperties[0].monitor = (VkfwMonitor) localMonitor;
    
    pProperties[0].state.iconified = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_ICONIFIED);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.resizable = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_RESIZABLE);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.visible = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_VISIBLE);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.decorated = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_DECORATED);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.autoIconify = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_AUTO_ICONIFY);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.floating = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_FLOATING);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.maximized = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_MAXIMIZED);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.focusOnShow = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_FOCUS_ON_SHOW);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.mousePassthrough = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_MOUSE_PASSTHROUGH);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.shouldClose = glfwWindowShouldClose(underlyingWindowHandle);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.stickyKeys = glfwGetInputMode(underlyingWindowHandle, GLFW_STICKY_KEYS);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.stickyMouseButtons = glfwGetInputMode(underlyingWindowHandle, GLFW_STICKY_MOUSE_BUTTONS);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.lockKeyMods = glfwGetInputMode(underlyingWindowHandle, GLFW_LOCK_KEY_MODS);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.rawMouseMotion = glfwGetInputMode(underlyingWindowHandle, GLFW_RAW_MOUSE_MOTION);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    switch(glfwGetInputMode(underlyingWindowHandle, GLFW_CURSOR)) {
        case GLFW_CURSOR_NORMAL: pProperties[0].state.cursorMode = VKFW_CURSOR_MODE_NORMAL; break;
        case GLFW_CURSOR_HIDDEN: pProperties[0].state.cursorMode = VKFW_CURSOR_MODE_HIDDEN; break;
        case GLFW_CURSOR_DISABLED: pProperties[0].state.cursorMode = VKFW_CURSOR_MODE_DISABLED; break;
        case GLFW_CURSOR_CAPTURED: pProperties[0].state.cursorMode = VKFW_CURSOR_MODE_CAPTURED; break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.title = glfwGetWindowTitle(underlyingWindowHandle);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    glfwGetWindowPos(underlyingWindowHandle, &pProperties[0].state.position.x, &pProperties[0].state.position.y);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        case GLFW_FEATURE_UNAVAILABLE: /* this is not a mistake, but intended behavior on Wayland. Therefore, the VKFW_DONT_CARE value is enough, and there needs to be no error code returned from VKFW. */
            pProperties[0].state.position.x = VKFW_DONT_CARE;
            pProperties[0].state.position.y = VKFW_DONT_CARE;
            break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    glfwGetWindowSize(underlyingWindowHandle, &pProperties[0].state.size.width, &pProperties[0].state.size.height);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    glfwGetCursorPos(underlyingWindowHandle, &pProperties[0].state.cursorPosition.x, &pProperties[0].state.cursorPosition.y);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.opacity = glfwGetWindowOpacity(underlyingWindowHandle);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.pUserPointer = glfwGetWindowUserPointer(underlyingWindowHandle);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].focused = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_FOCUSED);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].transparentFramebuffer = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_TRANSPARENT_FRAMEBUFFER);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].hovered = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_HOVERED);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    glfwGetFramebufferSize(underlyingWindowHandle, &pProperties[0].framebufferSize.width, &pProperties[0].framebufferSize.height);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    glfwGetWindowFrameSize(underlyingWindowHandle, &pProperties[0].frameSize.left, &pProperties[0].frameSize.top, &pProperties[0].frameSize.right, &pProperties[0].frameSize.bottom);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    glfwGetWindowContentScale(underlyingWindowHandle, &pProperties[0].contentScale.xScale, &pProperties[0].contentScale.yScale);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetWindowState(VkfwWindow window, VkfwWindowState newState) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    
    GLFWwindow* underlyingWindowHandle  = window[0].windowHandle;
    GLFWmonitor* underlyingMonitorHandle = window[0].monitorHandle;
    int32_t storedRefreshRate           = window[0].usedVideoMode.refreshRate;
    
    VkfwWindowProperties oldProperties;
    VkfwResult result = vkfwEnumerateWindowProperties(window, &oldProperties);
    if(result) return result;
    
    if(oldProperties.state.fullscreen != newState.fullscreen) {
        GLFWmonitor* newMonitorHandle;
        switch(newState.fullscreen) {
            case VKFW_TRUE:
                newMonitorHandle = underlyingMonitorHandle;
            break;
            case VKFW_FALSE:
                newMonitorHandle = NULL;
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        glfwSetWindowMonitor(underlyingWindowHandle, newMonitorHandle, newState.position.x, newState.position.y, newState.size.width, newState.size.height, storedRefreshRate);
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.iconified != newState.iconified) {
        switch(newState.iconified) {
            case VKFW_TRUE:
                glfwIconifyWindow(underlyingWindowHandle);
                switch(glfwGetError(NULL)) {
                    case GLFW_NO_ERROR: break;
                    case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                    default: return VKFW_ERROR_UNKNOWN;
                }
            break;
            case VKFW_FALSE:
                glfwRestoreWindow(underlyingWindowHandle);
                switch(glfwGetError(NULL)) {
                    case GLFW_NO_ERROR: break;
                    case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                    default: return VKFW_ERROR_UNKNOWN;
                }
                switch(newState.maximized) {
                    case VKFW_TRUE:
                        glfwMaximizeWindow(underlyingWindowHandle);
                        switch(glfwGetError(NULL)) {
                            case GLFW_NO_ERROR: break;
                            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                            default: return VKFW_ERROR_UNKNOWN;
                        }
                    break;
                    case VKFW_FALSE:
                    break;
                    default: return VKFW_ERROR_INVALID_ENUM_VALUE;
                }
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
    }
    if(oldProperties.state.resizable != newState.resizable) {
        switch(newState.resizable) {
            case VKFW_TRUE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_RESIZABLE, GLFW_TRUE);
            break;
            case VKFW_FALSE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_RESIZABLE, GLFW_FALSE);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.visible != newState.visible) {
        switch(newState.visible) {
            case VKFW_TRUE:
                glfwShowWindow(underlyingWindowHandle);
            break;
            case VKFW_FALSE:
                glfwHideWindow(underlyingWindowHandle);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.decorated != newState.decorated) {
        switch(newState.decorated) {
            case VKFW_TRUE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_DECORATED, GLFW_TRUE);
            break;
            case VKFW_FALSE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_DECORATED, GLFW_FALSE);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.autoIconify != newState.autoIconify) {
        switch(newState.autoIconify) {
            case VKFW_TRUE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_AUTO_ICONIFY, GLFW_TRUE);
            break;
            case VKFW_FALSE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_AUTO_ICONIFY, GLFW_FALSE);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.floating != newState.floating) {
        switch(newState.floating) {
            case VKFW_TRUE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_FLOATING, GLFW_TRUE);
            break;
            case VKFW_FALSE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_FLOATING, GLFW_FALSE);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED; /* expected on Wayland */
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.maximized != newState.maximized) {
        switch(newState.maximized) {
            case VKFW_TRUE:
                glfwMaximizeWindow(underlyingWindowHandle);
                switch(glfwGetError(NULL)) {
                    case GLFW_NO_ERROR: break;
                    case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                    default: return VKFW_ERROR_UNKNOWN;
                }
            break;
            case VKFW_FALSE:
                glfwRestoreWindow(underlyingWindowHandle);
                switch(glfwGetError(NULL)) {
                    case GLFW_NO_ERROR: break;
                    case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                    default: return VKFW_ERROR_UNKNOWN;
                }
                switch(newState.iconified) {
                    case VKFW_TRUE:
                        glfwIconifyWindow(underlyingWindowHandle);
                        switch(glfwGetError(NULL)) {
                            case GLFW_NO_ERROR: break;
                            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                            default: return VKFW_ERROR_UNKNOWN;
                        }
                    break;
                    case VKFW_FALSE:
                    break;
                    default: return VKFW_ERROR_INVALID_ENUM_VALUE;
                }
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
    }
    if(oldProperties.state.focusOnShow != newState.focusOnShow) {
        switch(newState.focusOnShow) {
            case VKFW_TRUE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
            break;
            case VKFW_FALSE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.mousePassthrough != newState.mousePassthrough) {
        switch(newState.mousePassthrough) {
            case VKFW_TRUE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
            break;
            case VKFW_FALSE:
                glfwSetWindowAttrib(underlyingWindowHandle, GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.shouldClose != newState.shouldClose) {
        switch(newState.shouldClose) {
            case VKFW_TRUE:
                glfwSetWindowShouldClose(underlyingWindowHandle, GLFW_TRUE);
            break;
            case VKFW_FALSE:
                glfwSetWindowShouldClose(underlyingWindowHandle, GLFW_FALSE);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    }
    if(oldProperties.state.stickyKeys != newState.stickyKeys) {
        switch(newState.stickyKeys) {
            case VKFW_TRUE:
                glfwSetInputMode(underlyingWindowHandle, GLFW_STICKY_KEYS, GLFW_TRUE);
            break;
            case VKFW_FALSE:
                glfwSetInputMode(underlyingWindowHandle, GLFW_STICKY_KEYS, GLFW_FALSE);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.stickyMouseButtons != newState.stickyMouseButtons) {
        switch(newState.stickyMouseButtons) {
            case VKFW_TRUE:
                glfwSetInputMode(underlyingWindowHandle, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
            break;
            case VKFW_FALSE:
                glfwSetInputMode(underlyingWindowHandle, GLFW_STICKY_MOUSE_BUTTONS, GLFW_FALSE);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.lockKeyMods != newState.lockKeyMods) {
        switch(newState.lockKeyMods) {
            case VKFW_TRUE:
                glfwSetInputMode(underlyingWindowHandle, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
            break;
            case VKFW_FALSE:
                glfwSetInputMode(underlyingWindowHandle, GLFW_LOCK_KEY_MODS, GLFW_FALSE);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.rawMouseMotion != newState.rawMouseMotion) {
        switch(newState.rawMouseMotion) {
            case VKFW_TRUE:
                glfwSetInputMode(underlyingWindowHandle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            break;
            case VKFW_FALSE:
                glfwSetInputMode(underlyingWindowHandle, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.cursorMode != newState.cursorMode) {
        switch(newState.cursorMode) {
            case VKFW_CURSOR_MODE_NORMAL:
                glfwSetInputMode(underlyingWindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
            case VKFW_CURSOR_MODE_HIDDEN:
                glfwSetInputMode(underlyingWindowHandle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
            case VKFW_CURSOR_MODE_DISABLED:
                glfwSetInputMode(underlyingWindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
            case VKFW_CURSOR_MODE_CAPTURED:
                glfwSetInputMode(underlyingWindowHandle, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
            break;
            default: return VKFW_ERROR_INVALID_ENUM_VALUE;
        }
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.title != newState.title) {
        glfwSetWindowTitle(underlyingWindowHandle, newState.title);
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.position.x != newState.position.x || oldProperties.state.position.y != newState.position.y) {
        glfwSetWindowPos(underlyingWindowHandle, newState.position.x, newState.position.y);
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.size.width != newState.size.width || oldProperties.state.size.height != newState.size.height) {
        glfwSetWindowSize(underlyingWindowHandle, newState.size.width, newState.size.height);
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.cursorPosition.x != newState.cursorPosition.x || oldProperties.state.cursorPosition.y != newState.cursorPosition.y) {
        glfwSetCursorPos(underlyingWindowHandle, newState.cursorPosition.x, newState.cursorPosition.y);
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.opacity != newState.opacity) {
        glfwSetWindowOpacity(underlyingWindowHandle, newState.opacity);
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.pUserPointer != newState.pUserPointer) {
        glfwSetWindowUserPointer(underlyingWindowHandle, newState.pUserPointer);
        if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetWindowIcon(VkfwWindow window, uint32_t imageCount, const VkfwImageData* images) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    
    GLFWwindow* underlyingWindowHandle  = window[0].windowHandle;
    
    glfwSetWindowIcon(underlyingWindowHandle, imageCount, (const GLFWimage*) images);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_INVALID_VALUE: return VKFW_ERROR_INVALID_NUMERIC_VALUE;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED; /* expected on macos and wayland */
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSwitchWindowMonitor(VkfwWindow window, VkfwMonitor monitor, VkfwVideoMode* requestedVideoMode) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(monitor == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(requestedVideoMode == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    GLFWwindow* underlyingWindowHandle  = window[0].windowHandle;
    GLFWmonitor* underlyingOldMonitorHandle = window[0].monitorHandle;
    GLFWmonitor* underlyingNewMonitorHandle = (GLFWmonitor*) monitor;
    VkfwVideoMode oldVideoMode          = window[0].usedVideoMode;
    VkfwVideoMode newVideoMode          = requestedVideoMode[0];
    
    /* because of the GLFW interface, we cannot change the bit request given to the monitor at window creation */
    if(newVideoMode.redBits != oldVideoMode.redBits)        return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
    if(newVideoMode.greenBits != oldVideoMode.greenBits)    return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
    if(newVideoMode.blueBits != oldVideoMode.blueBits)      return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
    
    /* get current position for glfwSetWindowMonitor, as it is not in the videoMode, and should just stay the same. */
    int32_t currentX, currentY;
    glfwGetWindowPos(underlyingWindowHandle, &currentX, &currentY);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        case GLFW_FEATURE_UNAVAILABLE: /* this is not a mistake, but intended behavior on Wayland. Since the value in glfwSetWindowMonitor is then ignored, we don't need to set any specific value.. */
            break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    /* get information if we are in fullscreen mode or not */
    VkfwBool32 weAreInFullscreenMode;
    GLFWmonitor* localMonitor = glfwGetWindowMonitor(underlyingWindowHandle);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    if(localMonitor != NULL && localMonitor != underlyingOldMonitorHandle) return VKFW_ERROR_UNKNOWN;
    weAreInFullscreenMode = (localMonitor != NULL);
    
    /* _only_ if we currently are in fullscreen mode, we actually set the GLFW monitor, otherwise we just store it for the future */
    if(weAreInFullscreenMode) {
        glfwSetWindowMonitor(underlyingWindowHandle, underlyingNewMonitorHandle, currentX, currentY, newVideoMode.width, newVideoMode.height, newVideoMode.refreshRate);
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    
    window[0].monitorHandle = underlyingNewMonitorHandle;
    window[0].usedVideoMode = newVideoMode;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwFocusWindow(VkfwWindow window) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    
    GLFWwindow* underlyingWindowHandle  = window[0].windowHandle;
    
    glfwFocusWindow(underlyingWindowHandle);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwRequestWindowAttention(VkfwWindow window) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    
    GLFWwindow* underlyingWindowHandle  = window[0].windowHandle;
    
    glfwRequestWindowAttention(underlyingWindowHandle);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetWindowAspectRatio(VkfwWindow window, int32_t numerator, int32_t denominator) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    
    GLFWwindow* underlyingWindowHandle  = window[0].windowHandle;
    
    glfwSetWindowAspectRatio(underlyingWindowHandle, numerator, denominator);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_INVALID_VALUE: return VKFW_ERROR_INVALID_NUMERIC_VALUE;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetWindowSizeLimits(VkfwWindow window, VkfwExtent2D minimum, VkfwExtent2D maximum) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    
    GLFWwindow* underlyingWindowHandle  = window[0].windowHandle;
    
    glfwSetWindowSizeLimits(underlyingWindowHandle, minimum.width, minimum.height, maximum.width, maximum.height);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_INVALID_VALUE: return VKFW_ERROR_INVALID_NUMERIC_VALUE;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwCreateCursor(VkfwInstance instance, const VkfwCursorCreateInfo* pCreateInfo, const VkfwAllocationCallbacks* pAllocator, VkfwCursor* pCursor) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pCreateInfo == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    if(pCursor == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    if(pAllocator != initAllocator) return VKFW_ERROR_FEATURE_NOT_SUPPORTED; /* we don't allow different allocators between instance and cursor since GLFW doesn't support it */
    
    if(pCreateInfo[0].sType != VKFW_STRUCTURE_TYPE_CURSOR_CREATE_INFO)      return VKFW_ERROR_INVALID_ENUM_VALUE;
    if(pCreateInfo[0].pNext != NULL)                                        return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
    
    /* currently no flags, so we ignore it */
    
    GLFWcursor* underlyingCursorHandle;
    
    switch(pCreateInfo[0].shape) {
        case VKFW_CURSOR_SHAPE_STANDARD_ARROW_CURSOR        :  underlyingCursorHandle = glfwCreateStandardCursor(GLFW_ARROW_CURSOR        ); break;
        case VKFW_CURSOR_SHAPE_STANDARD_IBEAM_CURSOR        :  underlyingCursorHandle = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR        ); break;
        case VKFW_CURSOR_SHAPE_STANDARD_CROSSHAIR_CURSOR    :  underlyingCursorHandle = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR    ); break;
        case VKFW_CURSOR_SHAPE_STANDARD_POINTING_HAND_CURSOR:  underlyingCursorHandle = glfwCreateStandardCursor(GLFW_POINTING_HAND_CURSOR); break;
        case VKFW_CURSOR_SHAPE_STANDARD_RESIZE_EW_CURSOR    :  underlyingCursorHandle = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR    ); break;
        case VKFW_CURSOR_SHAPE_STANDARD_RESIZE_NS_CURSOR    :  underlyingCursorHandle = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR    ); break;
        case VKFW_CURSOR_SHAPE_STANDARD_RESIZE_NWSE_CURSOR  :  underlyingCursorHandle = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR  ); break;
        case VKFW_CURSOR_SHAPE_STANDARD_RESIZE_NESW_CURSOR  :  underlyingCursorHandle = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR  ); break;
        case VKFW_CURSOR_SHAPE_STANDARD_RESIZE_ALL_CURSOR   :  underlyingCursorHandle = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR   ); break;
        case VKFW_CURSOR_SHAPE_STANDARD_NOT_ALLOWED_CURSOR  :  underlyingCursorHandle = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR  ); break;
        case VKFW_CURSOR_SHAPE_CUSTOM                       :  underlyingCursorHandle = glfwCreateCursor((const GLFWimage *) pCreateInfo[0].customCursorImageData,
                                                                    pCreateInfo[0].customCursorHotspotCoordinate.x, pCreateInfo[0].customCursorHotspotCoordinate.y); break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(underlyingCursorHandle == NULL) {
        if(pCreateInfo[0].shape == VKFW_CURSOR_SHAPE_CUSTOM) {
            switch(glfwGetError(NULL)) {
                case GLFW_INVALID_VALUE: return VKFW_ERROR_INVALID_NUMERIC_VALUE;
                case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                default: return VKFW_ERROR_UNKNOWN;
            }
        } else {
            switch(glfwGetError(NULL)) {
                case GLFW_CURSOR_UNAVAILABLE: return VKFW_ERROR_CURSOR_SHAPE_NOT_SUPPORTED;
                case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                default: return VKFW_ERROR_UNKNOWN;
            }
        }
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    pCursor[0] = (VkfwCursor) underlyingCursorHandle;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwDestroyCursor(VkfwCursor cursor, const VkfwAllocationCallbacks* pAllocator) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(cursor == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(pAllocator != initAllocator) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    GLFWcursor* underlyingCursorHandle = (GLFWcursor*) cursor;
    
    glfwDestroyCursor(underlyingCursorHandle);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetWindowCursor(VkfwWindow window, VkfwCursor cursor) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(cursor == NULL) return VKFW_ERROR_INVALID_HANDLE;
    
    GLFWwindow* underlyingWindowHandle  = window[0].windowHandle;
    GLFWcursor* underlyingCursorHandle = (GLFWcursor*) cursor;
    
    glfwSetCursor(underlyingWindowHandle, underlyingCursorHandle);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwUpdateGamepadMappings(VkfwInstance instance, uint32_t mappingCount, const VkfwGamepadMapping* mappings) {
    /* this is currently unimplemented, it would be better to connect it to GLFW internals directly, or possibly reverse that internal function, since passing the string seems unncessarily complicated */
    return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateJoysticks(VkfwInstance instance, uint32_t* pJoystickCount, VkfwJoystick* pJoysticks) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pJoystickCount == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;

    uint32_t count;
    int jid;
    
    for(jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++) {
        if(glfwJoystickPresent(jid)) count++;
        switch(glfwGetError(NULL)) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    
    pJoystickCount[0] = count;
    
    if(pJoysticks != NULL) {
        count = 0;
        for(jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++) {
            if(glfwJoystickPresent(jid)) {
                pJoysticks[count] = (VkfwJoystick) jid;
                count++;
            }
            switch(glfwGetError(NULL)) {
                case GLFW_NO_ERROR: break;
                case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                default: return VKFW_ERROR_UNKNOWN;
            }
        }
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateJoystickProperties(VkfwJoystick joystick, VkfwJoystickProperties* pProperties) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if((int32_t) joystick < GLFW_JOYSTICK_1 || (int32_t) joystick > GLFW_JOYSTICK_LAST) return VKFW_ERROR_INVALID_HANDLE;
    if(pProperties == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    int32_t underlyingJoystickID = (int32_t) joystick;
    
    pProperties[0].joystickID = underlyingJoystickID;
    pProperties[0].axesStates = glfwGetJoystickAxes(underlyingJoystickID, (int*) &pProperties[0].axesCount);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].buttonStates = glfwGetJoystickButtons(underlyingJoystickID, (int*) &pProperties[0].buttonCount);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].hatStates = glfwGetJoystickHats(underlyingJoystickID, (int*) &pProperties[0].hatCount);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].name = glfwGetJoystickName(underlyingJoystickID);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].GUID = glfwGetJoystickGUID(underlyingJoystickID);
    switch(glfwGetError(NULL)) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    switch(glfwJoystickIsGamepad(underlyingJoystickID)) {
        case GLFW_TRUE: pProperties[0].isGamepad = VKFW_TRUE; break;
        case GLFW_FALSE: pProperties[0].isGamepad = VKFW_FALSE; break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    pProperties[0].gamepadName = glfwGetGamepadName(underlyingJoystickID);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    pProperties[0].gamepadStateRetrievalSuccessfull = glfwGetGamepadState(underlyingJoystickID, (GLFWgamepadstate *) &pProperties[0].gamepadState);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    pProperties[0].userPointer = glfwGetJoystickUserPointer(underlyingJoystickID);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetJoystickUserPointer(VkfwJoystick joystick, void* pUserPointer) {    
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if((int32_t) joystick < GLFW_JOYSTICK_1 || (int32_t) joystick > GLFW_JOYSTICK_LAST) return VKFW_ERROR_INVALID_HANDLE;
    
    int32_t underlyingJoystickID = (int32_t) joystick;
    
    glfwSetJoystickUserPointer(underlyingJoystickID, pUserPointer);
    if(glfwGetError(NULL)) return VKFW_ERROR_UNKNOWN;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwVkBool32 VKFWAPI_CALL vkfwVkGetPhysicalDevicePresentationSupport(VkfwVkInstance instance, VkfwVkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
    /* since we need to return the same value, we cannot return an error code here on GLFW error, we hope it fails successfully by the original return value */
    return glfwGetPhysicalDevicePresentationSupport(instance, physicalDevice, queueFamilyIndex);
}
VKFWAPI_ATTR VkfwVkResult VKFWAPI_CALL vkfwVkCreateWindowSurface(VkfwVkInstance instance, VkfwWindow window, const VkfwVkAllocationCallbacks* pAllocator, VkfwVkSurfaceKHR* pSurface) {
    /* since we need to return the same value, we cannot return an error code here on GLFW error, we hope it fails successfully by the original return value */
    return glfwCreateWindowSurface(instance, window[0].windowHandle, pAllocator, pSurface);
}

