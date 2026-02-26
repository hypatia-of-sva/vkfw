
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



symbols:


win32


CALLBACK
DBT_DEVICEARRIVAL
DBT_DEVICEREMOVECOMPLETE
DBT_DEVTYP_DEVICEINTERFACE
DefWindowProcW
DEV_BROADCAST_HDR
HWND
LPARAM
LRESULT
UINT
WM_DEVICECHANGE
WM_DISPLAYCHANGE
WPARAM






linux









void* open_lib_options(char** names, int names_count) {
    void* handle = NULL;
    for(int i = 0; i < names_count; i++) {
        handle = OSModule.open(names[i], VKFW_DEFAULT_FLAGS);
        if(handle != NULL) break;
    }
    return handle;
}
void safe_unload_handle(void** handle_location) {
    if(handle != NULL && handle[0] != NULL) {
        OSModule.close(handle[0]);
        handle[0] = NULL;
    }
}

// PFN_.... types
struct {
    .... // from win32_platform.h
} win32
bool win32_load_libs(void) {
    win32.user32.instance = OSModule.open("user32.dll", VKFW_DEFAULT_FLAGS);
    if (_glfw.win32.user32.instance == NULL) {
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
    safe_unload_handle(&win32.xinput.instance);
    safe_unload_handle(&win32.dinput8.instance);
    safe_unload_handle(&win32.user32.instance);
    safe_unload_handle(&win32.dwmapi.instance);
    safe_unload_handle(&win32.shcore.instance);
    safe_unload_handle(&win32.ntdll.instance);
    safe_unload_handle(&wgl.instance);
}

struct {
    .... // from x11_platform.h + XInitThreads, XrmInitialize, XOpenDisplay
} x11
bool linux_load_libs(bool load_x11_xcb) {
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
    

    const char** vidmode_names = {"libXxf86vm.so.1", "libXxf86vm.so"};
    x11.vidmode.handle = open_lib_options(vidmode_names, sizeof(vidmode_names)/sizeof(char*));
    if(x11.vidmode.handle != NULL) {
        x11.vidmode.QueryExtension            = (PFN_XF86VidModeQueryExtension)           OSModule.load(x11.vidmode.handle, "XF86VidModeQueryExtension");
        x11.vidmode.GetGammaRamp              = (PFN_XF86VidModeGetGammaRamp)             OSModule.load(x11.vidmode.handle, "XF86VidModeGetGammaRamp");
        x11.vidmode.SetGammaRamp              = (PFN_XF86VidModeSetGammaRamp)             OSModule.load(x11.vidmode.handle, "XF86VidModeSetGammaRamp");
        x11.vidmode.GetGammaRampSize          = (PFN_XF86VidModeGetGammaRampSize)         OSModule.load(x11.vidmode.handle, "XF86VidModeGetGammaRampSize");
    }

    const char** vidmode_names = {"libXi.so.6", "libXi.so", "libXi-6.so"};
    x11.xi.handle = open_lib_options(vidmode_names, sizeof(vidmode_names)/sizeof(char*));
    if (x11.xi.handle != NULL) {
        x11.xi.QueryVersion                   = (PFN_XIQueryVersion)                      OSModule.load(x11.xi.handle, "XIQueryVersion");
        x11.xi.SelectEvents                   = (PFN_XISelectEvents)                      OSModule.load(x11.xi.handle, "XISelectEvents");
    }
    
    const char** randr_names = {"libXrandr.so.2", "libXrandr.so", "libXrandr-2.so"};
    x11.randr.handle = open_lib_options(randr_names, sizeof(randr_names)/sizeof(char*));
    if (x11.randr.handle != NULL) {
        x11.randr.AllocGamma                  = (PFN_XRRAllocGamma)                       OSModule.load(x11.randr.handle, "XRRAllocGamma");
        x11.randr.FreeGamma                   = (PFN_XRRFreeGamma)                        OSModule.load(x11.randr.handle, "XRRFreeGamma");
        x11.randr.FreeCrtcInfo                = (PFN_XRRFreeCrtcInfo)                     OSModule.load(x11.randr.handle, "XRRFreeCrtcInfo");
        x11.randr.FreeGamma                   = (PFN_XRRFreeGamma)                        OSModule.load(x11.randr.handle, "XRRFreeGamma");
        x11.randr.FreeOutputInfo              = (PFN_XRRFreeOutputInfo)                   OSModule.load(x11.randr.handle, "XRRFreeOutputInfo");
        x11.randr.FreeScreenResources         = (PFN_XRRFreeScreenResources)              OSModule.load(x11.randr.handle, "XRRFreeScreenResources");
        x11.randr.GetCrtcGamma                = (PFN_XRRGetCrtcGamma)                     OSModule.load(x11.randr.handle, "XRRGetCrtcGamma");
        x11.randr.GetCrtcGammaSize            = (PFN_XRRGetCrtcGammaSize)                 OSModule.load(x11.randr.handle, "XRRGetCrtcGammaSize");
        x11.randr.GetCrtcInfo                 = (PFN_XRRGetCrtcInfo)                      OSModule.load(x11.randr.handle, "XRRGetCrtcInfo");
        x11.randr.GetOutputInfo               = (PFN_XRRGetOutputInfo)                    OSModule.load(x11.randr.handle, "XRRGetOutputInfo");
        x11.randr.GetOutputPrimary            = (PFN_XRRGetOutputPrimary)                 OSModule.load(x11.randr.handle, "XRRGetOutputPrimary");
        x11.randr.GetScreenResourcesCurrent   = (PFN_XRRGetScreenResourcesCurrent)        OSModule.load(x11.randr.handle, "XRRGetScreenResourcesCurrent");
        x11.randr.QueryExtension              = (PFN_XRRQueryExtension)                   OSModule.load(x11.randr.handle, "XRRQueryExtension");
        x11.randr.QueryVersion                = (PFN_XRRQueryVersion)                     OSModule.load(x11.randr.handle, "XRRQueryVersion");
        x11.randr.SelectInput                 = (PFN_XRRSelectInput)                      OSModule.load(x11.randr.handle, "XRRSelectInput");
        x11.randr.SetCrtcConfig               = (PFN_XRRSetCrtcConfig)                    OSModule.load(x11.randr.handle, "XRRSetCrtcConfig");
        x11.randr.SetCrtcGamma                = (PFN_XRRSetCrtcGamma)                     OSModule.load(x11.randr.handle, "XRRSetCrtcGamma");
        x11.randr.UpdateConfiguration         = (PFN_XRRUpdateConfiguration)              OSModule.load(x11.randr.handle, "XRRUpdateConfiguration");
    }
    
    const char** xcursor_names = {"libXcursor.so.1", "libXcursor.so", "libXcursor-1.so"};
    x11.xcursor.handle = open_lib_options(xcursor_names, sizeof(xcursor_names)/sizeof(char*));
    if (x11.xcursor.handle != NULL)
    {
        x11.xcursor.ImageCreate               = (PFN_XcursorImageCreate)                  OSModule.load(x11.xcursor.handle, "XcursorImageCreate");
        x11.xcursor.ImageDestroy              = (PFN_XcursorImageDestroy)                 OSModule.load(x11.xcursor.handle, "XcursorImageDestroy");
        x11.xcursor.ImageLoadCursor           = (PFN_XcursorImageLoadCursor)              OSModule.load(x11.xcursor.handle, "XcursorImageLoadCursor");
        x11.xcursor.GetTheme                  = (PFN_XcursorGetTheme)                     OSModule.load(x11.xcursor.handle, "XcursorGetTheme");
        x11.xcursor.GetDefaultSize            = (PFN_XcursorGetDefaultSize)               OSModule.load(x11.xcursor.handle, "XcursorGetDefaultSize");
        x11.xcursor.LibraryLoadImage          = (PFN_XcursorLibraryLoadImage)             OSModule.load(x11.xcursor.handle, "XcursorLibraryLoadImage");
    }
    
    const char** xinerama_names = {"libXinerama.so.1", "libXinerama.so", "libXinerama-1.so"};
    x11.xinerama.handle = open_lib_options(xinerama_names, sizeof(xinerama_names)/sizeof(char*));
    if (x11.xinerama.handle != NULL) {
        x11.xinerama.IsActive                 = (PFN_XineramaIsActive)                    OSModule.load(x11.xinerama.handle, "XineramaIsActive");
        x11.xinerama.QueryExtension           = (PFN_XineramaQueryExtension)              OSModule.load(x11.xinerama.handle, "XineramaQueryExtension");
        x11.xinerama.QueryScreens             = (PFN_XineramaQueryScreens)                OSModule.load(x11.xinerama.handle, "XineramaQueryScreens");
    }
    
    if(load_x11_xcb) {
        const char** x11xcb_names = {"libX11-xcb.so.1", "libX11-xcb.so", "libX11-xcb-1.so"};
        x11.x11xcb.handle = open_lib_options(x11xcb_names, sizeof(x11xcb_names)/sizeof(char*));
        if (x11.x11xcb.handle != NULL) {
            x11.x11xcb.GetXCBConnection       = (PFN_XGetXCBConnection)                   OSModule.load(x11.x11xcb.handle, "XGetXCBConnection");
        }
    }
    
    const char** xrender_names = {"libXrender.so.1", "libXrender.so", "libXrender-1.so"};
    x11.xrender.handle = open_lib_options(xrender_names, sizeof(xrender_names)/sizeof(char*));
    if (x11.xrender.handle != NULL) {
        x11.xrender.QueryExtension            = (PFN_XRenderQueryExtension)               OSModule.load(x11.xrender.handle, "XRenderQueryExtension");
        x11.xrender.QueryVersion              = (PFN_XRenderQueryVersion)                 OSModule.load(x11.xrender.handle, "XRenderQueryVersion");
        x11.xrender.FindVisualFormat          = (PFN_XRenderFindVisualFormat)             OSModule.load(x11.xrender.handle, "XRenderFindVisualFormat");
    }
    
    const char** xshape_names = {"libXext.so.6", "libXext.so", "libXext-6.so"};
    x11.xshape.handle = open_lib_options(xshape_names, sizeof(xshape_names)/sizeof(char*));
    if (x11.xshape.handle != NULL) {
        x11.xshape.QueryExtension       = (PFN_XShapeQueryExtension)    OSModule.load(x11.xshape.handle, "XShapeQueryExtension");
        x11.xshape.ShapeCombineRegion   = (PFN_XShapeCombineRegion)     OSModule.load(x11.xshape.handle, "XShapeCombineRegion");
        x11.xshape.QueryVersion         = (PFN_XShapeQueryVersion)      OSModule.load(x11.xshape.handle, "XShapeQueryVersion");
        x11.xshape.ShapeCombineMask     = (PFN_XShapeCombineMask)       OSModule.load(x11.xshape.handle, "XShapeCombineMask");
    }
    
    return true;
}
void linux_unload_libs(void) {
    safe_unload_handle(&x11.x11xcb.handle);
    safe_unload_handle(&x11.xcursor.handle);
    safe_unload_handle(&x11.randr.handle);
    safe_unload_handle(&x11.xinerama.handle);
    safe_unload_handle(&x11.xrender.handle);
    safe_unload_handle(&x11.vidmode.handle);
    safe_unload_handle(&x11.xi.handle);
    safe_unload_handle(&x11.xlib.handle);
    safe_unload_handle(&glx.handle);
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



    switch(platform) {
        case WIN32:
            int jid;

            for (jid = GLFW_JOYSTICK_1;  jid <= GLFW_JOYSTICK_LAST;  jid++)
                closeJoystick(_glfw.joysticks + jid);

            if (_glfw.win32.dinput8.api)
                IDirectInput8_Release(_glfw.win32.dinput8.api);
        break;
        case LINUX:
            for (int jid = 0;  jid <= GLFW_JOYSTICK_LAST;  jid++)
            {
                _GLFWjoystick* js = _glfw.joysticks + jid;
                if (js->connected)
                    closeJoystick(js);
            }

            if (_glfw.linjs.inotify > 0)
            {
                if (_glfw.linjs.watch > 0)
                    inotify_rm_watch(_glfw.linjs.inotify, _glfw.linjs.watch);

                close(_glfw.linjs.inotify);
            }

            if (_glfw.linjs.regexCompiled)
                regfree(&_glfw.linjs.regex);
        break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    switch(platform) {
        case WIN32:
            if (_glfw.win32.blankCursor)
                DestroyIcon((HICON) _glfw.win32.blankCursor);

            if (_glfw.win32.deviceNotificationHandle)
                UnregisterDeviceNotification(_glfw.win32.deviceNotificationHandle);

            if (_glfw.win32.helperWindowHandle)
                DestroyWindow(_glfw.win32.helperWindowHandle);
            if (_glfw.win32.helperWindowClass)
                UnregisterClassW(MAKEINTATOM(_glfw.win32.helperWindowClass), _glfw.win32.instance);
            if (_glfw.win32.mainWindowClass)
                UnregisterClassW(MAKEINTATOM(_glfw.win32.mainWindowClass), _glfw.win32.instance);

            _glfw_free(_glfw.win32.clipboardString);
            _glfw_free(_glfw.win32.rawInput);

            win32_unload_libs();
        break;
        case LINUX:
            if (_glfw.x11.helperWindowHandle)
            {
                if (XGetSelectionOwner(_glfw.x11.display, _glfw.x11.CLIPBOARD) ==
                    _glfw.x11.helperWindowHandle)
                {
                    _glfwPushSelectionToManagerX11();
                }

                XDestroyWindow(_glfw.x11.display, _glfw.x11.helperWindowHandle);
                _glfw.x11.helperWindowHandle = None;
            }

            if (_glfw.x11.hiddenCursorHandle)
            {
                XFreeCursor(_glfw.x11.display, _glfw.x11.hiddenCursorHandle);
                _glfw.x11.hiddenCursorHandle = (Cursor) 0;
            }

            _glfw_free(_glfw.x11.primarySelectionString);
            _glfw_free(_glfw.x11.clipboardString);

            XUnregisterIMInstantiateCallback(_glfw.x11.display,
                                             NULL, NULL, NULL,
                                             inputMethodInstantiateCallback,
                                             NULL);


            if (_glfw.x11.im)
            {
                XCloseIM(_glfw.x11.im);
                _glfw.x11.im = NULL;
            }

            if (_glfw.x11.display)
            {
                XCloseDisplay(_glfw.x11.display);
                _glfw.x11.display = NULL;
            }
            // NOTE: Libs (esp. EGL, GLX, xlib) need to be unloaded after XCloseDisplay, as they register
            //       cleanup callbacks that get called by that function


            if (_glfw.x11.emptyEventPipe[0] || _glfw.x11.emptyEventPipe[1])
            {
                close(_glfw.x11.emptyEventPipe[0]);
                close(_glfw.x11.emptyEventPipe[1]);
            }
    
            linux_unload_libs();

        break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    
    if (_glfw.egl.display)
    {
        eglTerminate(_glfw.egl.display);
        _glfw.egl.display = EGL_NO_DISPLAY;
    }

    if (_glfw.egl.handle)
    {
        _glfwPlatformFreeModule(_glfw.egl.handle);
        _glfw.egl.handle = NULL;
    }
    
    if (_glfw.osmesa.handle)
    {
        _glfwPlatformFreeModule(_glfw.osmesa.handle);
        _glfw.osmesa.handle = NULL;
    }

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
    if(pCreateInfo[0].flags & VKFW_INSTANCE_CREATE_X11_DISABLE_XCB_VULKAN_SURFACE_BIT_X11) {
        _glfwInitHints.x11.xcbVulkanSurface = GLFW_FALSE;
    } else {
        _glfwInitHints.x11.xcbVulkanSurface = GLFW_TRUE;
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

            if (!linux_load_libs(_glfw.hints.init.x11.xcbVulkanSurface)) {
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



            // Retrieve system content scale via folklore heuristics
            {
                // Start by assuming the default X11 DPI
                // NOTE: Some desktop environments (KDE) may remove the Xft.dpi field when it
                //       would be set to 96, so assume that is the case if we cannot find it
                float xdpi = 96.f, ydpi = 96.f;

                // NOTE: Basing the scale on Xft.dpi where available should provide the most
                //       consistent user experience (matches Qt, Gtk, etc), although not
                //       always the most accurate one
                char* rms = XResourceManagerString(_glfw.x11.display);
                if (rms)
                {
                    XrmDatabase db = XrmGetStringDatabase(rms);
                    if (db)
                    {
                        XrmValue value;
                        char* type = NULL;

                        if (XrmGetResource(db, "Xft.dpi", "Xft.Dpi", &type, &value))
                        {
                            if (type && strcmp(type, "String") == 0)
                                xdpi = ydpi = atof(value.addr);
                        }

                        XrmDestroyDatabase(db);
                    }
                }

                _glfw.x11.contentScaleX = xdpi / 96.f;
                _glfw.x11.contentScaleY = ydpi / 96.f;
            }

            //      Is this old GLFW trick really necessary? we do have pipe2 on linux and don't need to run on other OS necessarily right? or do want to keep BSD compat?...
            // Create the pipe for empty events without assumuing the OS has pipe2(2)
            {
                if (pipe(_glfw.x11.emptyEventPipe) != 0)
                {
                    _glfwInputError(GLFW_PLATFORM_ERROR,
                                    "X11: Failed to create empty event pipe: %s",
                                    strerror(errno));
                    terminate();
                    return VKFW_ERROR_PLATFORM_ERROR;
                }

                for (int i = 0; i < 2; i++)
                {
                    const int sf = fcntl(_glfw.x11.emptyEventPipe[i], F_GETFL, 0);
                    const int df = fcntl(_glfw.x11.emptyEventPipe[i], F_GETFD, 0);

                    if (sf == -1 || df == -1 ||
                        fcntl(_glfw.x11.emptyEventPipe[i], F_SETFL, sf | O_NONBLOCK) == -1 ||
                        fcntl(_glfw.x11.emptyEventPipe[i], F_SETFD, df | FD_CLOEXEC) == -1)
                    {
                        _glfwInputError(GLFW_PLATFORM_ERROR,
                                        "X11: Failed to set flags for empty event pipe: %s",
                                        strerror(errno));
                        terminate();
                        return VKFW_ERROR_PLATFORM_ERROR;
                    }
                }
            }



            // Look for and initialize supported X11 extensions
            {
                if (_glfw.x11.vidmode.handle)
                {
                    _glfw.x11.vidmode.available =
                        XF86VidModeQueryExtension(_glfw.x11.display,
                                                  &_glfw.x11.vidmode.eventBase,
                                                  &_glfw.x11.vidmode.errorBase);
                }

                if (_glfw.x11.xi.handle)
                {
                    if (XQueryExtension(_glfw.x11.display,
                                        "XInputExtension",
                                        &_glfw.x11.xi.majorOpcode,
                                        &_glfw.x11.xi.eventBase,
                                        &_glfw.x11.xi.errorBase))
                    {
                        _glfw.x11.xi.major = 2;
                        _glfw.x11.xi.minor = 0;

                        if (XIQueryVersion(_glfw.x11.display,
                                           &_glfw.x11.xi.major,
                                           &_glfw.x11.xi.minor) == Success)
                        {
                            _glfw.x11.xi.available = GLFW_TRUE;
                        }
                    }
                }

                if (_glfw.x11.randr.handle)
                {

                    if (XRRQueryExtension(_glfw.x11.display,
                                          &_glfw.x11.randr.eventBase,
                                          &_glfw.x11.randr.errorBase))
                    {
                        if (XRRQueryVersion(_glfw.x11.display,
                                            &_glfw.x11.randr.major,
                                            &_glfw.x11.randr.minor))
                        {
                            // The GLFW RandR path requires at least version 1.3
                            if (_glfw.x11.randr.major > 1 || _glfw.x11.randr.minor >= 3)
                                _glfw.x11.randr.available = GLFW_TRUE;
                        }
                        else
                        {
                            _glfwInputError(GLFW_PLATFORM_ERROR,
                                            "X11: Failed to query RandR version");
                        }
                    }
                }

                if (_glfw.x11.randr.available)
                {
                    XRRScreenResources* sr = XRRGetScreenResourcesCurrent(_glfw.x11.display,
                                                                          _glfw.x11.root);

                    if (!sr->ncrtc || !XRRGetCrtcGammaSize(_glfw.x11.display, sr->crtcs[0]))
                    {
                        // This is likely an older Nvidia driver with broken gamma support
                        // Flag it as useless and fall back to xf86vm gamma, if available
                        _glfw.x11.randr.gammaBroken = GLFW_TRUE;
                    }

                    if (!sr->ncrtc)
                    {
                        // A system without CRTCs is likely a system with broken RandR
                        // Disable the RandR monitor path and fall back to core functions
                        _glfw.x11.randr.monitorBroken = GLFW_TRUE;
                    }

                    XRRFreeScreenResources(sr);
                }

                if (_glfw.x11.randr.available && !_glfw.x11.randr.monitorBroken)
                {
                    XRRSelectInput(_glfw.x11.display, _glfw.x11.root,
                                   RROutputChangeNotifyMask);
                }

                if (_glfw.x11.xinerama.handle && XineramaQueryExtension(_glfw.x11.display, &_glfw.x11.xinerama.major, &_glfw.x11.xinerama.minor) && XineramaIsActive(_glfw.x11.display)) {
                    _glfw.x11.xinerama.available = GLFW_TRUE;
                }

                _glfw.x11.xkb.major = 1;
                _glfw.x11.xkb.minor = 0;
                _glfw.x11.xkb.available =
                    XkbQueryExtension(_glfw.x11.display,
                                      &_glfw.x11.xkb.majorOpcode,
                                      &_glfw.x11.xkb.eventBase,
                                      &_glfw.x11.xkb.errorBase,
                                      &_glfw.x11.xkb.major,
                                      &_glfw.x11.xkb.minor);

                if (_glfw.x11.xkb.available)
                {
                    Bool supported;

                    if (XkbSetDetectableAutoRepeat(_glfw.x11.display, True, &supported))
                    {
                        if (supported)
                            _glfw.x11.xkb.detectable = GLFW_TRUE;
                    }

                    XkbStateRec state;
                    if (XkbGetState(_glfw.x11.display, XkbUseCoreKbd, &state) == Success)
                        _glfw.x11.xkb.group = (unsigned int)state.group;

                    XkbSelectEventDetails(_glfw.x11.display, XkbUseCoreKbd, XkbStateNotify,
                                          XkbGroupStateMask, XkbGroupStateMask);
                }

                if (_glfw.x11.xrender.handle && XRenderQueryExtension(_glfw.x11.display, &_glfw.x11.xrender.errorBase, &_glfw.x11.xrender.eventBase)
                                             && XRenderQueryVersion(_glfw.x11.display, &_glfw.x11.xrender.major, &_glfw.x11.xrender.minor)) {
                    _glfw.x11.xrender.available = GLFW_TRUE;
                }

                if (_glfw.x11.xshape.handle && XShapeQueryExtension(_glfw.x11.display, &_glfw.x11.xshape.errorBase, &_glfw.x11.xshape.eventBase)
                                            && XShapeQueryVersion(_glfw.x11.display, &_glfw.x11.xshape.major, &_glfw.x11.xshape.minor)) {
                    _glfw.x11.xshape.available = GLFW_TRUE;
                }

                // Update the key code LUT
                // FIXME: We should listen to XkbMapNotify events to track changes to
                // the keyboard mapping.
                
                
                // Create key code translation tables
                {
                    int scancodeMin, scancodeMax;

                    memset(_glfw.x11.keycodes, -1, sizeof(_glfw.x11.keycodes));
                    memset(_glfw.x11.scancodes, -1, sizeof(_glfw.x11.scancodes));

                    if (_glfw.x11.xkb.available)
                    {
                        // Use XKB to determine physical key locations independently of the
                        // current keyboard layout

                        XkbDescPtr desc = XkbGetMap(_glfw.x11.display, 0, XkbUseCoreKbd);
                        XkbGetNames(_glfw.x11.display, XkbKeyNamesMask | XkbKeyAliasesMask, desc);

                        scancodeMin = desc->min_key_code;
                        scancodeMax = desc->max_key_code;

                        const struct
                        {
                            int key;
                            char* name;
                        } keymap[] =
                        {
                            { GLFW_KEY_GRAVE_ACCENT, "TLDE" },
                            { GLFW_KEY_1, "AE01" },
                            { GLFW_KEY_2, "AE02" },
                            { GLFW_KEY_3, "AE03" },
                            { GLFW_KEY_4, "AE04" },
                            { GLFW_KEY_5, "AE05" },
                            { GLFW_KEY_6, "AE06" },
                            { GLFW_KEY_7, "AE07" },
                            { GLFW_KEY_8, "AE08" },
                            { GLFW_KEY_9, "AE09" },
                            { GLFW_KEY_0, "AE10" },
                            { GLFW_KEY_MINUS, "AE11" },
                            { GLFW_KEY_EQUAL, "AE12" },
                            { GLFW_KEY_Q, "AD01" },
                            { GLFW_KEY_W, "AD02" },
                            { GLFW_KEY_E, "AD03" },
                            { GLFW_KEY_R, "AD04" },
                            { GLFW_KEY_T, "AD05" },
                            { GLFW_KEY_Y, "AD06" },
                            { GLFW_KEY_U, "AD07" },
                            { GLFW_KEY_I, "AD08" },
                            { GLFW_KEY_O, "AD09" },
                            { GLFW_KEY_P, "AD10" },
                            { GLFW_KEY_LEFT_BRACKET, "AD11" },
                            { GLFW_KEY_RIGHT_BRACKET, "AD12" },
                            { GLFW_KEY_A, "AC01" },
                            { GLFW_KEY_S, "AC02" },
                            { GLFW_KEY_D, "AC03" },
                            { GLFW_KEY_F, "AC04" },
                            { GLFW_KEY_G, "AC05" },
                            { GLFW_KEY_H, "AC06" },
                            { GLFW_KEY_J, "AC07" },
                            { GLFW_KEY_K, "AC08" },
                            { GLFW_KEY_L, "AC09" },
                            { GLFW_KEY_SEMICOLON, "AC10" },
                            { GLFW_KEY_APOSTROPHE, "AC11" },
                            { GLFW_KEY_Z, "AB01" },
                            { GLFW_KEY_X, "AB02" },
                            { GLFW_KEY_C, "AB03" },
                            { GLFW_KEY_V, "AB04" },
                            { GLFW_KEY_B, "AB05" },
                            { GLFW_KEY_N, "AB06" },
                            { GLFW_KEY_M, "AB07" },
                            { GLFW_KEY_COMMA, "AB08" },
                            { GLFW_KEY_PERIOD, "AB09" },
                            { GLFW_KEY_SLASH, "AB10" },
                            { GLFW_KEY_BACKSLASH, "BKSL" },
                            { GLFW_KEY_WORLD_1, "LSGT" },
                            { GLFW_KEY_SPACE, "SPCE" },
                            { GLFW_KEY_ESCAPE, "ESC" },
                            { GLFW_KEY_ENTER, "RTRN" },
                            { GLFW_KEY_TAB, "TAB" },
                            { GLFW_KEY_BACKSPACE, "BKSP" },
                            { GLFW_KEY_INSERT, "INS" },
                            { GLFW_KEY_DELETE, "DELE" },
                            { GLFW_KEY_RIGHT, "RGHT" },
                            { GLFW_KEY_LEFT, "LEFT" },
                            { GLFW_KEY_DOWN, "DOWN" },
                            { GLFW_KEY_UP, "UP" },
                            { GLFW_KEY_PAGE_UP, "PGUP" },
                            { GLFW_KEY_PAGE_DOWN, "PGDN" },
                            { GLFW_KEY_HOME, "HOME" },
                            { GLFW_KEY_END, "END" },
                            { GLFW_KEY_CAPS_LOCK, "CAPS" },
                            { GLFW_KEY_SCROLL_LOCK, "SCLK" },
                            { GLFW_KEY_NUM_LOCK, "NMLK" },
                            { GLFW_KEY_PRINT_SCREEN, "PRSC" },
                            { GLFW_KEY_PAUSE, "PAUS" },
                            { GLFW_KEY_F1, "FK01" },
                            { GLFW_KEY_F2, "FK02" },
                            { GLFW_KEY_F3, "FK03" },
                            { GLFW_KEY_F4, "FK04" },
                            { GLFW_KEY_F5, "FK05" },
                            { GLFW_KEY_F6, "FK06" },
                            { GLFW_KEY_F7, "FK07" },
                            { GLFW_KEY_F8, "FK08" },
                            { GLFW_KEY_F9, "FK09" },
                            { GLFW_KEY_F10, "FK10" },
                            { GLFW_KEY_F11, "FK11" },
                            { GLFW_KEY_F12, "FK12" },
                            { GLFW_KEY_F13, "FK13" },
                            { GLFW_KEY_F14, "FK14" },
                            { GLFW_KEY_F15, "FK15" },
                            { GLFW_KEY_F16, "FK16" },
                            { GLFW_KEY_F17, "FK17" },
                            { GLFW_KEY_F18, "FK18" },
                            { GLFW_KEY_F19, "FK19" },
                            { GLFW_KEY_F20, "FK20" },
                            { GLFW_KEY_F21, "FK21" },
                            { GLFW_KEY_F22, "FK22" },
                            { GLFW_KEY_F23, "FK23" },
                            { GLFW_KEY_F24, "FK24" },
                            { GLFW_KEY_F25, "FK25" },
                            { GLFW_KEY_KP_0, "KP0" },
                            { GLFW_KEY_KP_1, "KP1" },
                            { GLFW_KEY_KP_2, "KP2" },
                            { GLFW_KEY_KP_3, "KP3" },
                            { GLFW_KEY_KP_4, "KP4" },
                            { GLFW_KEY_KP_5, "KP5" },
                            { GLFW_KEY_KP_6, "KP6" },
                            { GLFW_KEY_KP_7, "KP7" },
                            { GLFW_KEY_KP_8, "KP8" },
                            { GLFW_KEY_KP_9, "KP9" },
                            { GLFW_KEY_KP_DECIMAL, "KPDL" },
                            { GLFW_KEY_KP_DIVIDE, "KPDV" },
                            { GLFW_KEY_KP_MULTIPLY, "KPMU" },
                            { GLFW_KEY_KP_SUBTRACT, "KPSU" },
                            { GLFW_KEY_KP_ADD, "KPAD" },
                            { GLFW_KEY_KP_ENTER, "KPEN" },
                            { GLFW_KEY_KP_EQUAL, "KPEQ" },
                            { GLFW_KEY_LEFT_SHIFT, "LFSH" },
                            { GLFW_KEY_LEFT_CONTROL, "LCTL" },
                            { GLFW_KEY_LEFT_ALT, "LALT" },
                            { GLFW_KEY_LEFT_SUPER, "LWIN" },
                            { GLFW_KEY_RIGHT_SHIFT, "RTSH" },
                            { GLFW_KEY_RIGHT_CONTROL, "RCTL" },
                            { GLFW_KEY_RIGHT_ALT, "RALT" },
                            { GLFW_KEY_RIGHT_ALT, "LVL3" },
                            { GLFW_KEY_RIGHT_ALT, "MDSW" },
                            { GLFW_KEY_RIGHT_SUPER, "RWIN" },
                            { GLFW_KEY_MENU, "MENU" }
                        };

                        // Find the X11 key code -> GLFW key code mapping
                        for (int scancode = scancodeMin;  scancode <= scancodeMax;  scancode++)
                        {
                            int key = GLFW_KEY_UNKNOWN;

                            // Map the key name to a GLFW key code. Note: We use the US
                            // keyboard layout. Because function keys aren't mapped correctly
                            // when using traditional KeySym translations, they are mapped
                            // here instead.
                            for (int i = 0;  i < sizeof(keymap) / sizeof(keymap[0]);  i++)
                            {
                                if (strncmp(desc->names->keys[scancode].name,
                                            keymap[i].name,
                                            XkbKeyNameLength) == 0)
                                {
                                    key = keymap[i].key;
                                    break;
                                }
                            }

                            // Fall back to key aliases in case the key name did not match
                            for (int i = 0;  i < desc->names->num_key_aliases;  i++)
                            {
                                if (key != GLFW_KEY_UNKNOWN)
                                    break;

                                if (strncmp(desc->names->key_aliases[i].real,
                                            desc->names->keys[scancode].name,
                                            XkbKeyNameLength) != 0)
                                {
                                    continue;
                                }

                                for (int j = 0;  j < sizeof(keymap) / sizeof(keymap[0]);  j++)
                                {
                                    if (strncmp(desc->names->key_aliases[i].alias,
                                                keymap[j].name,
                                                XkbKeyNameLength) == 0)
                                    {
                                        key = keymap[j].key;
                                        break;
                                    }
                                }
                            }

                            _glfw.x11.keycodes[scancode] = key;
                        }

                        XkbFreeNames(desc, XkbKeyNamesMask, True);
                        XkbFreeKeyboard(desc, 0, True);
                    }
                    else
                        XDisplayKeycodes(_glfw.x11.display, &scancodeMin, &scancodeMax);

                    int width;
                    KeySym* keysyms = XGetKeyboardMapping(_glfw.x11.display,
                                                          scancodeMin,
                                                          scancodeMax - scancodeMin + 1,
                                                          &width);

                    for (int scancode = scancodeMin;  scancode <= scancodeMax;  scancode++)
                    {
                        // Translate the un-translated key codes using traditional X11 KeySym
                        // lookups
                        if (_glfw.x11.keycodes[scancode] < 0)
                        {
                            const size_t base = (scancode - scancodeMin) * width;
                            _glfw.x11.keycodes[scancode] = translateKeySyms(&keysyms[base], width);
                        }

                        // Store the reverse translation for faster key name lookup
                        if (_glfw.x11.keycodes[scancode] > 0)
                            _glfw.x11.scancodes[_glfw.x11.keycodes[scancode]] = scancode;
                    }

                    XFree(keysyms);
                }


                // String format atoms
                _glfw.x11.NULL_             = XInternAtom(_glfw.x11.display, "NULL", False);
                _glfw.x11.UTF8_STRING       = XInternAtom(_glfw.x11.display, "UTF8_STRING", False);
                _glfw.x11.ATOM_PAIR         = XInternAtom(_glfw.x11.display, "ATOM_PAIR", False);
                // Custom selection property atom
                _glfw.x11.GLFW_SELECTION    = XInternAtom(_glfw.x11.display, "GLFW_SELECTION", False);
                // ICCCM standard clipboard atoms
                _glfw.x11.TARGETS           = XInternAtom(_glfw.x11.display, "TARGETS", False);
                _glfw.x11.MULTIPLE          = XInternAtom(_glfw.x11.display, "MULTIPLE", False);
                _glfw.x11.PRIMARY           = XInternAtom(_glfw.x11.display, "PRIMARY", False);
                _glfw.x11.INCR              = XInternAtom(_glfw.x11.display, "INCR", False);
                _glfw.x11.CLIPBOARD         = XInternAtom(_glfw.x11.display, "CLIPBOARD", False);
                // Clipboard manager atoms
                _glfw.x11.CLIPBOARD_MANAGER = XInternAtom(_glfw.x11.display, "CLIPBOARD_MANAGER", False);
                _glfw.x11.SAVE_TARGETS      = XInternAtom(_glfw.x11.display, "SAVE_TARGETS", False);
                // Xdnd (drag and drop) atoms
                _glfw.x11.XdndAware         = XInternAtom(_glfw.x11.display, "XdndAware", False);
                _glfw.x11.XdndEnter         = XInternAtom(_glfw.x11.display, "XdndEnter", False);
                _glfw.x11.XdndPosition      = XInternAtom(_glfw.x11.display, "XdndPosition", False);
                _glfw.x11.XdndStatus        = XInternAtom(_glfw.x11.display, "XdndStatus", False);
                _glfw.x11.XdndActionCopy    = XInternAtom(_glfw.x11.display, "XdndActionCopy", False);
                _glfw.x11.XdndDrop          = XInternAtom(_glfw.x11.display, "XdndDrop", False);
                _glfw.x11.XdndFinished      = XInternAtom(_glfw.x11.display, "XdndFinished", False);
                _glfw.x11.XdndSelection     = XInternAtom(_glfw.x11.display, "XdndSelection", False);
                _glfw.x11.XdndTypeList      = XInternAtom(_glfw.x11.display, "XdndTypeList", False);
                _glfw.x11.text_uri_list     = XInternAtom(_glfw.x11.display, "text/uri-list", False);
                // ICCCM, EWMH and Motif window property atoms
                // These can be set safely even without WM support
                // The EWMH atoms that require WM support are handled in detectEWMH
                _glfw.x11.WM_PROTOCOLS              = XInternAtom(_glfw.x11.display, "WM_PROTOCOLS", False);
                _glfw.x11.WM_STATE                  = XInternAtom(_glfw.x11.display, "WM_STATE", False);
                _glfw.x11.WM_DELETE_WINDOW          = XInternAtom(_glfw.x11.display, "WM_DELETE_WINDOW", False);
                _glfw.x11.NET_SUPPORTED             = XInternAtom(_glfw.x11.display, "_NET_SUPPORTED", False);
                _glfw.x11.NET_SUPPORTING_WM_CHECK   = XInternAtom(_glfw.x11.display, "_NET_SUPPORTING_WM_CHECK", False);
                _glfw.x11.NET_WM_ICON               = XInternAtom(_glfw.x11.display, "_NET_WM_ICON", False);
                _glfw.x11.NET_WM_PING               = XInternAtom(_glfw.x11.display, "_NET_WM_PING", False);
                _glfw.x11.NET_WM_PID                = XInternAtom(_glfw.x11.display, "_NET_WM_PID", False);
                _glfw.x11.NET_WM_NAME               = XInternAtom(_glfw.x11.display, "_NET_WM_NAME", False);
                _glfw.x11.NET_WM_ICON_NAME          = XInternAtom(_glfw.x11.display, "_NET_WM_ICON_NAME", False);
                _glfw.x11.NET_WM_BYPASS_COMPOSITOR  = XInternAtom(_glfw.x11.display, "_NET_WM_BYPASS_COMPOSITOR", False);
                _glfw.x11.NET_WM_WINDOW_OPACITY     = XInternAtom(_glfw.x11.display, "_NET_WM_WINDOW_OPACITY", False);
                _glfw.x11.MOTIF_WM_HINTS            = XInternAtom(_glfw.x11.display, "_MOTIF_WM_HINTS", False);

                // The compositing manager selection name contains the screen number
                {
                    char name[32];
                    snprintf(name, sizeof(name), "_NET_WM_CM_S%u", _glfw.x11.screen);
                    _glfw.x11.NET_WM_CM_Sx = XInternAtom(_glfw.x11.display, name, False);
                }

                // Detect whether an EWMH-conformant window manager is running
                // Check whether the running window manager is EWMH-compliant
                {
                    // First we read the _NET_SUPPORTING_WM_CHECK property on the root window

                    Window* windowFromRoot = NULL;
                    if (_glfwGetWindowPropertyX11(_glfw.x11.root, _glfw.x11.NET_SUPPORTING_WM_CHECK, XA_WINDOW, (unsigned char**) &windowFromRoot))
                    {
                        _glfwGrabErrorHandlerX11();

                        // If it exists, it should be the XID of a top-level window
                        // Then we look for the same property on that window

                        Window* windowFromChild = NULL;
                        if (_glfwGetWindowPropertyX11(*windowFromRoot, _glfw.x11.NET_SUPPORTING_WM_CHECK, XA_WINDOW, (unsigned char**) &windowFromChild)) {
                            _glfwReleaseErrorHandlerX11();

                            // If the property exists, it should contain the XID of the window
                            bool cond = *windowFromRoot == *windowFromChild;
                            XFree(windowFromRoot);
                            XFree(windowFromChild);
                            if (cond) {
                                // We are now fairly sure that an EWMH-compliant WM is currently running
                                // We can now start querying the WM about what features it supports by
                                // looking in the _NET_SUPPORTED property on the root window
                                // It should contain a list of supported EWMH protocol and state atoms

                                Atom* supportedAtoms = NULL;
                                const unsigned long atomCount =
                                    _glfwGetWindowPropertyX11(_glfw.x11.root,
                                                              _glfw.x11.NET_SUPPORTED,
                                                              XA_ATOM,
                                                              (unsigned char**) &supportedAtoms);

                                // See which of the atoms we support that are supported by the WM


// Return the atom ID only if it is listed in the specified array
//
static Atom getAtomIfSupported(Atom* supportedAtoms, unsigned long atomCount, const char* atomName)
{
    const Atom atom = XInternAtom(_glfw.x11.display, atomName, False);

    for (unsigned long i = 0;  i < atomCount;  i++)
    {
        if (supportedAtoms[i] == atom)
            return atom;
    }

    return None;
}

                                _glfw.x11.NET_WM_STATE =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE");
                                _glfw.x11.NET_WM_STATE_ABOVE =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_ABOVE");
                                _glfw.x11.NET_WM_STATE_FULLSCREEN =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_FULLSCREEN");
                                _glfw.x11.NET_WM_STATE_MAXIMIZED_VERT =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_MAXIMIZED_VERT");
                                _glfw.x11.NET_WM_STATE_MAXIMIZED_HORZ =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_MAXIMIZED_HORZ");
                                _glfw.x11.NET_WM_STATE_DEMANDS_ATTENTION =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_STATE_DEMANDS_ATTENTION");
                                _glfw.x11.NET_WM_FULLSCREEN_MONITORS =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_FULLSCREEN_MONITORS");
                                _glfw.x11.NET_WM_WINDOW_TYPE =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_WINDOW_TYPE");
                                _glfw.x11.NET_WM_WINDOW_TYPE_NORMAL =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_WM_WINDOW_TYPE_NORMAL");
                                _glfw.x11.NET_WORKAREA =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_WORKAREA");
                                _glfw.x11.NET_CURRENT_DESKTOP =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_CURRENT_DESKTOP");
                                _glfw.x11.NET_ACTIVE_WINDOW =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_ACTIVE_WINDOW");
                                _glfw.x11.NET_FRAME_EXTENTS =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_FRAME_EXTENTS");
                                _glfw.x11.NET_REQUEST_FRAME_EXTENTS =
                                    getAtomIfSupported(supportedAtoms, atomCount, "_NET_REQUEST_FRAME_EXTENTS");

                                if (supportedAtoms)
                                    XFree(supportedAtoms);
                            }
                        }
                        else {
                            XFree(windowFromRoot);
                        }
                    }
                }
            }




            // Create a helper window for IPC
            {
                XSetWindowAttributes wa;
                wa.event_mask = PropertyChangeMask;

                _glfw.x11.helperWindowHandle = XCreateWindow(_glfw.x11.display, _glfw.x11.root,
                                     0, 0, 1, 1, 0, 0,
                                     InputOnly,
                                     DefaultVisual(_glfw.x11.display, _glfw.x11.screen),
                                     CWEventMask, &wa);
            }


            // Create a blank cursor for hidden and disabled cursor modes
            {
                unsigned char pixels[16 * 16 * 4] = { 0 };
                GLFWimage image = { 16, 16, pixels };
                _glfw.x11.hiddenCursorHandle = _glfwCreateNativeCursorX11(&image, 0, 0);
            }


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
    
    
    
    switch(platform) {
        case WIN32:
            if (_glfw.win32.dinput8.instance)
            {
                if (FAILED(DirectInput8Create(_glfw.win32.instance,
                                              DIRECTINPUT_VERSION,
                                              &IID_IDirectInput8W,
                                              (void**) &_glfw.win32.dinput8.api,
                                              NULL)))
                {
                    _glfwInputError(GLFW_PLATFORM_ERROR,
                                    "Win32: Failed to create interface");
                    terminate();
                    return VKFW_ERROR_PLATFORM_ERROR;
                }
            }

            _glfwDetectJoystickConnectionWin32();
        break;
        case LINUX:
            const char* dirname = "/dev/input";

            _glfw.linjs.inotify = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
            if (_glfw.linjs.inotify > 0)
            {
                // HACK: Register for IN_ATTRIB to get notified when udev is done
                //       This works well in practice but the true way is libudev

                _glfw.linjs.watch = inotify_add_watch(_glfw.linjs.inotify,
                                                      dirname,
                                                      IN_CREATE | IN_ATTRIB | IN_DELETE);
            }

            // Continue without device connection notifications if inotify fails

            _glfw.linjs.regexCompiled = (regcomp(&_glfw.linjs.regex, "^event[0-9]\\+$", 0) == 0);
            if (!_glfw.linjs.regexCompiled)
            {
                _glfwInputError(GLFW_PLATFORM_ERROR, "Linux: Failed to compile regex");
                terminate();
                return VKFW_ERROR_PLATFORM_ERROR;
            }

            int count = 0;

            DIR* dir = opendir(dirname);
            if (dir)
            {
                struct dirent* entry;

                while ((entry = readdir(dir)))
                {
                    regmatch_t match;

                    if (regexec(&_glfw.linjs.regex, entry->d_name, 1, &match, 0) != 0)
                        continue;

                    char path[PATH_MAX];

                    snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

                    if (openJoystickDevice(path))
                        count++;
                }

                closedir(dir);
            }

            // Continue with no joysticks if enumeration fails

            qsort(_glfw.joysticks, count, sizeof(_GLFWjoystick), compareJoysticks);
        break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    _glfw.joysticksInitialized = GLFW_TRUE;

    
    _glfw.callbacks.monitor = (GLFWmonitorfun) pCreateInfo[0].callbacks.monitorConnection;
    _glfw.callbacks.joystick = (GLFWjoystickfun) pCreateInfo[0].callbacks.joystickConnection;
    
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
    
    switch(platform) {
        case WIN32:
            pProperties[0].usedPlatform = VKFW_INSTANCE_PLATFORM_WIN32;
            pProperties[0].rawMouseMotionSupported = VKFW_TRUE;
        break;
        case LINUX:
            pProperties[0].usedPlatform = VKFW_INSTANCE_PLATFORM_X11;
            pProperties[0].rawMouseMotionSupported = _glfw.x11.xi.available;
        break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
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

