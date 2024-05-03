#include "vkfw.h"

#define GLFW_INCLUDE_VULKAN /* necessary to get Vulkan functions of GLFW */
#include <GLFW/glfw3.h>
#include <stdlib.h> /* for malloc and free for the window handle structs */

VkfwBool32 vfkwInstanceInitialized = VKFW_FALSE;
uint32_t instanceHandleAddress = 0;
const VkfwAllocationCallbacks* initAllocator = NULL;

GLFWallocator ourGLFWAllocator;

/* we need this struct, instead of casting to GLFWwindow*, in order to keep the monitor handle we want to go to fullscreen mode in.
   the video mode is stored because the refreshRate is needed for glfwSetWindowMonitor and not retrievable by state retrieval functions. */
struct VkfwWindow_t {
    GLFWwindow*     windowHandle;
    GLFWmonitor*    monitorHandle;
    VkfwVideoMode   usedVideoMode;
}

VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateGlobalProperties(VkfwGlobalProperties* pProperties) {
    if(pProperties == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    pProperties[0].majorVersion                = VKFW_VERSION_MAJOR
    pProperties[0].minorVersion                = VKFW_VERSION_MINOR
    pProperties[0].revisionVersion             = VKFW_VERSION_REVISION
    pProperties[0].underlyingVersionString     = glfwGetVersionString();
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0].supportedPlatforms         |= VKFW_INSTANCE_PLATFORM_WIN32   * glfwPlatformSupported(GLFW_PLATFORM_WIN32);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0].supportedPlatforms         |= VKFW_INSTANCE_PLATFORM_COCOA   * glfwPlatformSupported(GLFW_PLATFORM_COCOA);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0].supportedPlatforms         |= VKFW_INSTANCE_PLATFORM_WAYLAND * glfwPlatformSupported(GLFW_PLATFORM_WAYLAND);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0].supportedPlatforms         |= VKFW_INSTANCE_PLATFORM_X11     * glfwPlatformSupported(GLFW_PLATFORM_X11);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0].supportedPlatforms         |= VKFW_INSTANCE_PLATFORM_NULL    * glfwPlatformSupported(GLFW_PLATFORM_NULL);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwCreateInstance(const VkfwInstanceCreateInfo* pCreateInfo, const VkfwAllocationCallbacks* pAllocator, VkfwInstance* pInstance) {    
    if(vfkwInstanceInitialized) return VKFW_ERROR_FEATURE_NOT_SUPPORTED; /* multiple Instances currently not supported because GLFW has only one initialization state */
    
    if(pCreateInfo == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    if(pInstance == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    initAllocator = pAllocator;
    
    if(pAllocator == NULL) {
        glfwInitAllocator(NULL);
    } else {
        ourGLFWAllocator.allocate   = pAllocator[0].pfnAllocation;
        ourGLFWAllocator.reallocate = pAllocator[0].pfnReallocation;
        ourGLFWAllocator.deallocate = pAllocator[0].pfnFree;
        ourGLFWAllocator.user       = pAllocator[0].pUserData;
        if((ourGLFWAllocator.allocate == NULL) || (ourGLFWAllocator.reallocate == NULL) || (ourGLFWAllocator.deallocate == NULL)) return VKFW_ERROR_INVALID_POINTER_VALUE;
        glfwInitAllocator(&ourGLFWAllocator);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    if(pCreateInfo[0].sType != VKFW_STRUCTURE_TYPE_INSTANCE_CREATE_INFO)    return VKFW_ERROR_INVALID_ENUM_VALUE;
    if(pCreateInfo[0].pNext != NULL)                                        return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
    
    if(pCreateInfo[0].flags & VKFW_INSTANCE_CREATE_DISABLE_JOYSTICK_HAT_BUTTONS_BIT) {
        glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);
    } else {
        glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_INSTANCE_CREATE_WAYLAND_DISABLE_LIBDECOR_BIT_WL) {
        glfwInitHint(GLFW_WAYLAND_LIBDECOR, GLFW_WAYLAND_DISABLE_LIBDECOR);
    } else {
        glfwInitHint(GLFW_WAYLAND_LIBDECOR, GLFW_WAYLAND_PREFER_LIBDECOR);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_INSTANCE_CREATE_X11_DISABLE_XCB_VULKAN_SURFACE_BIT_X11) {
        glfwInitHint(GLFW_X11_XCB_VULKAN_SURFACE, GLFW_FALSE);
    } else {
        glfwInitHint(GLFW_X11_XCB_VULKAN_SURFACE, GLFW_TRUE);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_INSTANCE_CREATE_COCOA_DISABLE_MENUBAR_BIT_COCOA) {
        glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE);
    } else {
        glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_TRUE);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_INSTANCE_CREATE_COCOA_DISABLE_CHDIR_RESOURCES_BIT_COCOA) {
        glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
    } else {
        glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_TRUE);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    switch(pCreateInfo[0].desiredPlatform) {
        case VKFW_INSTANCE_PLATFORM_DEFAULT : glfwInitHint(GLFW_PLATFORM, GLFW_ANY_PLATFORM);      break;
        case VKFW_INSTANCE_PLATFORM_WIN32   : glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WIN32);    break;
        case VKFW_INSTANCE_PLATFORM_COCOA   : glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_COCOA);    break;
        case VKFW_INSTANCE_PLATFORM_WAYLAND : glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);  break;
        case VKFW_INSTANCE_PLATFORM_X11     : glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);      break;
        case VKFW_INSTANCE_PLATFORM_NULL    : glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_NULL);     break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    glfwInitVulkanLoader(pCreateInfo[0].desiredVulkanLoader);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    int initCode = glfwInit();
    
    if(!initCode) {
        switch(glfwGetError()) {
            case GLFW_PLATFORM_UNAVAILABLE: return VKFW_ERROR_PLATFORM_UNAVAILABLE;
            case GLFW_PLATFORM_ERROR:       return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    
    glfwSetMonitorCallback((GLFWmonitorfun) pCreateInfo[0].callbacks.monitorConnection);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetJoystickCallback((GLFWjoystickfun) pCreateInfo[0].callbacks.joystickConnection);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    vfkwInstanceInitialized = VKFW_TRUE;
    pInstance[0] = (VkfwInstance) &instanceHandleAddress;
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwDestroyInstance(VkfwInstance instance, const VkfwAllocationCallbacks* pAllocator) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pAllocator != initAllocator) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    glfwTerminate();
    switch(glfwGetError()) {
        case GLFW_NO_ERROR              break;
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
        case LFW_PLATFORM_WAYLAND : pProperties[0].usedPlatform = VKFW_INSTANCE_PLATFORM_WAYLAND; break;
        case GLFW_PLATFORM_X11    : pProperties[0].usedPlatform = VKFW_INSTANCE_PLATFORM_X11    ; break;
        case GLFW_PLATFORM_NULL   : pProperties[0].usedPlatform = VKFW_INSTANCE_PLATFORM_NULL   ; break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].rawMouseMotionSupported = glfwRawMouseMotionSupported();
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].vulkanFound = glfwVulkanSupported();
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].vulkanLoader = glfwGetInstanceProcAddress(NULL, "vkGetInstanceProcAddr");
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].requiredInstanceExtensions = glfwGetRequiredInstanceExtensions(&pProperties[0].requiredInstanceExtensionCount);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_API_UNAVAILABLE: return VKFW_ERROR_API_UNAVAILABLE;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    pProperties[0].timerFrequency = glfwGetTimerFrequency();
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwGetKeyScancode(VkfwInstance instance, VkfwKey key, int32_t* pScancode) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pScancode == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    pScancode[0] = glfwGetKeyScancode(key);
    switch(glfwGetError()) {
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
    
    pScancode[0] = glfwGetKeyName(key, scancode);
    switch(glfwGetError()) {
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
    
    switch(glfwGetError()) {
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
    
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
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
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateMonitors(VkfwInstance instance, uint32_t* pMonitorCount, VkfwMonitor* pMonitors) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(instance != (VkfwInstance) &instanceHandleAddress) return VKFW_ERROR_INVALID_HANDLE;
    if(pMonitorCount == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    GLFWmonitor** localMonitorArray = glfwGetMonitors(pMonitorCount);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    if(pMonitors != NULL) {
        for(uint32_t i = 0; i < pMonitorCount[0]; i++) {
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
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    ´
    glfwGetMonitorWorkarea(localMonitor, &pProperties[0].workarea.offset.x, &pProperties[0].workarea.offset.y, &pProperties[0].workarea.extent.width, &pProperties[0].workarea.extent.height);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    glfwGetMonitorPhysicalSize(localMonitor, &pProperties[0].physicalSizeMM.width, &pProperties[0].physicalSizeMM.height);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    glfwGetMonitorContentScale(localMonitor, &pProperties[0].contentScale.xScale, &pProperties[0].contentScale.yScale);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    pProperties[0].pName = glfwGetMonitorName(localMonitor);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].pUserPointer = glfwGetMonitorUserPointer(localMonitor);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].pVideoModes = (const VkfwVideoMode*) glfwGetVideoModes(localMonitor, &pProperties[0].videoModeCount);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    pProperties[0].pCurrentVideoMode = (const VkfwVideoMode*) glfwGetVideoMode(localMonitor);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    pProperties[0].pCurrentGammeRamp = (const VkfwGammaRamp*) glfwGetGammaRamp(localMonitor);
    switch(glfwGetError()) {
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
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetMonitorGammaRamp(VkfwMonitor monitor, const VkfwGammaRamp* pGammaRamp) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(monitor == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(pGammaRamp == NULL) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    GLFWmonitor* localMonitor = (GLFWmonitor*) monitor;
    
    glfwSetGammaRamp(localMonitor, (const GLFWgammaramp*) pGammaRamp);
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    if(currentGammaRampPtr == NULL) return VKFW_ERROR_UNKNOWN;
    VkfwGammaRamp oldGammaRamp = currentGammaRampPtr[0];
    
    /* 2. set the new ramp from the gamme value. */
    glfwSetGamma(localMonitor, gamma);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_INVALID_VALUE: return VKFW_ERROR_INVALID_NUMERIC_VALUE;
        case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    /* 3. copy new gamma ramp. */
    currentGammaRampPtr = (const VkfwGammaRamp*) glfwGetGammaRamp(localMonitor);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    if(currentGammaRampPtr == NULL) return VKFW_ERROR_UNKNOWN;
    VkfwGammaRamp newGammaRamp = currentGammaRampPtr[0];
    
    /* 4. reset monitor to old gamma ramp. */
    glfwSetGammaRamp(localMonitor, (const GLFWgammaramp*) &oldGammaRamp);
    switch(glfwGetError()) {
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
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_DONT_CENTER_CURSOR_BIT) {
        glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_FALSE);
    } else {
        glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_TRANSPARENT_FRAMEBUFFER_BIT) {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_SCALE_TO_MONITOR_BIT) {
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_NO_SCALE_FRAMEBUFFER_BIT) {
        glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE);
    } else {
        glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_COCOA_GRAPHICS_SWITCHING_BIT_COCOA) {
        glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, GLFW_FALSE);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_WIN32_KEYBOARD_MENU_BIT_WIN32) {
        glfwWindowHint(GLFW_WIN32_KEYBOARD_MENU, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_WIN32_KEYBOARD_MENU, GLFW_FALSE);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_WIN32_SHOWDEFAULT_BIT_WIN32) {
        glfwWindowHint(GLFW_WIN32_SHOWDEFAULT, GLFW_TRUE);
    } else {
        glfwWindowHint(GLFW_WIN32_SHOWDEFAULT, GLFW_FALSE);
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    switch(pCreateInfo[0].initialState.resizable) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.visible) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.decorated) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.autoIconify) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.floating) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.maximized) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.focusOnShow) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.mousePassthrough) {
        case VKFW_TRUE:
            glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    glfwWindowHint(GLFW_POSITION_X, pCreateInfo[0].initialState.position.x);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwWindowHint(GLFW_POSITION_Y, pCreateInfo[0].initialState.position.y);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    if(pCreateInfo[0].requestedVideoMode.width != pCreateInfo[0].initialState.size.width)   return VKFW_ERROR_INVALID_NUMERIC_VALUE;
    if(pCreateInfo[0].requestedVideoMode.height != pCreateInfo[0].initialState.size.height) return VKFW_ERROR_INVALID_NUMERIC_VALUE;
    
    glfwWindowHint(GLFW_RED_BITS, pCreateInfo[0].requestedVideoMode.redBits);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwWindowHint(GLFW_GREEN_BITS, pCreateInfo[0].requestedVideoMode.greenBits);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwWindowHint(GLFW_BLUE_BITS, pCreateInfo[0].requestedVideoMode.blueBits);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwWindowHint(GLFW_REFRESH_RATE, pCreateInfo[0].requestedVideoMode.refreshRate);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    glfwWindowHintString(GLFW_COCOA_FRAME_NAME, pCreateInfo[0].cocoaFrameName_COCOA);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwWindowHintString(GLFW_X11_CLASS_NAME, pCreateInfo[0].x11ClassName_X11);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwWindowHintString(GLFW_X11_INSTANCE_NAME, pCreateInfo[0].x11InstanceName_X11);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, pCreateInfo[0].waylandAppID_WL);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    
    /* 2. actually creating the handle */
    
    GLFWmonitor* createParameterMonitorHandle;
    switch(pCreateInfo[0].initialState.fullscreen) {
        case VKFW_TRUE:
            createParameterMonitorHandle = returnedStruct[0].monitorHandle;
        break;
        case VKFW_FALSE:
            createParameterMonitorHandle = NULL;
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    
    GLFWmonitor* underlyingWindowHandle = glfwCreateWindow(pCreateInfo[0].initialState.size.width, pCreateInfo[0].initialState.size.height, pCreateInfo[0].initialState.title, createParameterMonitorHandle, NULL);
    if(underlyingWindowHandle == NULL) {
        switch(glfwGetError()) {
            case GLFW_INVALID_VALUE:        return VKFW_ERROR_INVALID_NUMERIC_VALUE;
            case GLFW_FORMAT_UNAVAILABLE:   return VKFW_ERROR_PIXEL_FORMAT_NOT_SUPPORTED;
            case GLFW_PLATFORM_ERROR:       return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    
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
    switch(glfwGetError()) {
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
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    switch(pCreateInfo[0].initialState.stickyKeys) {
        case VKFW_TRUE:
            glfwSetInputMode(underlyingWindowHandle, GLFW_STICKY_KEYS, GLFW_TRUE);
        break;
        case VKFW_FALSE:
            /* off by default */
        break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    if(pCreateInfo[0].flags & VKFW_WINDOW_CREATE_SET_INITIAL_CURSOR_POSITION_BIT) {
        glfwSetCursorPos(underlyingWindowHandle, pCreateInfo[0].initialState.cursorPosition.x, pCreateInfo[0].initialState.cursorPosition.y);
        switch(glfwGetError()) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(pCreateInfo[0].initialState.opacity != 1.0f) {
        glfwSetWindowOpacity(underlyingWindowHandle, pCreateInfo[0].initialState.opacity);
        switch(glfwGetError()) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    glfwSetWindowUserPointer(underlyingWindowHandle, pCreateInfo[0].initialState.pUserPointer);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    
    /* 4. all the callbacks! */
    
    glfwSetWindowPosCallback(underlyingWindowHandle, (GLFWwindowposfun) pCreateInfo[0].callbacks.positionChange);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowSizeCallback(underlyingWindowHandle, (GLFWwindowsizefun) pCreateInfo[0].callbacks.sizeChange);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetFramebufferSizeCallback(underlyingWindowHandle, (GLFWframebuffersizefun) pCreateInfo[0].callbacks.framebufferSizeChange);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowContentScaleCallback(underlyingWindowHandle, (GLFWwindowcontentscalefun) pCreateInfo[0].callbacks.contentScaleChange);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetCursorPosCallback(underlyingWindowHandle, (GLFWcursorposfun) pCreateInfo[0].callbacks.cursorPositionChange);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowFocusCallback(underlyingWindowHandle, (GLFWwindowfocusfun) pCreateInfo[0].callbacks.focusChange);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowIconifyCallback(underlyingWindowHandle, (GLFWwindowiconifyfun) pCreateInfo[0].callbacks.iconficationChange);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowMaximizeCallback(underlyingWindowHandle, (GLFWwindowmaximizefun) pCreateInfo[0].callbacks.maximizationChange);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    glfwSetWindowCloseCallback(underlyingWindowHandle, (GLFWwindowclosefun) pCreateInfo[0].callbacks.closeButtonClicked);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetWindowRefreshCallback(underlyingWindowHandle, (GLFWwindowrefreshfun) pCreateInfo[0].callbacks.contentAreaNeedsToBeRedrawn);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetDropCallback(underlyingWindowHandle, (GLFWdropfun) pCreateInfo[0].callbacks.pathDrop);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    glfwSetMouseButtonCallback(underlyingWindowHandle, (GLFWmousebuttonfun) pCreateInfo[0].callbacks.mouseButtonInput);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetScrollCallback(underlyingWindowHandle, (GLFWscrollfun) pCreateInfo[0].callbacks.scrollInput);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetCursorEnterCallback(underlyingWindowHandle, (GLFWcursorenterfun) pCreateInfo[0].callbacks.cursorEnterOrLeaveContentArea);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetKeyCallback(underlyingWindowHandle, (GLFWkeyfun) pCreateInfo[0].callbacks.keyInput);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetCharCallback(underlyingWindowHandle, (GLFWcharfun) pCreateInfo[0].callbacks.unicodeCharacterInput);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwSetCharModsCallback(underlyingWindowHandle, (GLFWcharmodsfun) pCreateInfo[0].callbacks.unicodeCharacterInputWithModifiers);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    
    /* 5. finally, return the handle */
    
    pWindow[0} = malloc(sizeof(VkfwWindow_t));
    if(pWindow[0} == NULL) return VKFW_ERROR_OUT_OF_MEMORY;
    pWindow[0][0].monitorHandle = underlyingMonitorHandle;
    pWindow[0][0].windowHandle  = underlyingWindowHandle;
    pWindow[0][0].usedVideoMode = pCreateInfo[0].requestedVideoMode; /* at this point, creation was successful so the request for the mode was honored, so it is the actually used mode */
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR void         VKFWAPI_CALL vkfwDestroyWindow(VkfwWindow window, const VkfwAllocationCallbacks* pAllocator) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(pAllocator != initAllocator) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    glfwDestroyWindow(window[0].windowHandle);
    switch(glfwGetError()) {
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
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(localMonitor != NULL && localMonitor != window[0].monitorHandle) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.fullscreen = (localMonitor != NULL);
    pProperties[0].state.monitor = (VkfwMonitor) localMonitor;
    
    pProperties[0].state.iconified = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_ICONIFIED);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.resizable = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_RESIZABLE);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.visible = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_VISIBLE);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.decorated = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_DECORATED);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.autoIconify = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_AUTO_ICONIFY);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.floating = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_FLOATING);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.maximized = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_MAXIMIZED);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.focusOnShow = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_FOCUS_ON_SHOW);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.mousePassthrough = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_MOUSE_PASSTHROUGH);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.shouldClose = glfwWindowShouldClose(underlyingWindowHandle);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.stickyKeys = glfwGetInputMode(underlyingWindowHandle, GLFW_STICKY_KEYS);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.stickyMouseButtons = glfwGetInputMode(underlyingWindowHandle, GLFW_STICKY_MOUSE_BUTTONS);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.lockKeyMods = glfwGetInputMode(underlyingWindowHandle, GLFW_LOCK_KEY_MODS);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.rawMouseMotion = glfwGetInputMode(underlyingWindowHandle, GLFW_RAW_MOUSE_MOTION);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    switch(glfwGetInputMode(underlyingWindowHandle, GLFW_CURSOR)) {
        case GLFW_CURSOR_NORMAL: pProperties[0].state.cursorMode = VKFW_CURSOR_MODE_NORMAL; break;
        case GLFW_CURSOR_HIDDEN: pProperties[0].state.cursorMode = VKFW_CURSOR_MODE_HIDDEN; break;
        case GLFW_CURSOR_DISABLED: pProperties[0].state.cursorMode = VKFW_CURSOR_MODE_DISABLED; break;
        case GLFW_CURSOR_CAPTURED: pProperties[0].state.cursorMode = VKFW_CURSOR_MODE_CAPTURED; break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0].state.title = glfwGetWindowTitle(underlyingWindowHandle);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    glfwGetWindowPos(underlyingWindowHandle, &pProperties[0].state.position.x, &pProperties[0].state.position.y);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        case GLFW_FEATURE_UNAVAILABLE: /* this is not a mistake, but intended behavior on Wayland. Therefore, the VKFW_DONT_CARE value is enough, and there needs to be no error code returned from VKFW. */
            pProperties[0].state.position.x = VKFW_DONT_CARE;
            pProperties[0].state.position.y = VKFW_DONT_CARE;
            break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    glfwGetWindowSize(underlyingWindowHandle, &pProperties[0].state.size.width, &pProperties[0].state.size.height);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    glfwGetCursorPos(underlyingWindowHandle, &pProperties[0].state.cursorPosition.x, &pProperties[0].state.cursorPosition.y);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.opacity = glfwGetWindowOpacity(underlyingWindowHandle);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].state.pUserPointer = glfwGetWindowUserPointer(underlyingWindowHandle);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    pProperties[0].focused = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_FOCUSED);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].transparentFramebuffer = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_TRANSPARENT_FRAMEBUFFER);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0].hovered = glfwGetWindowAttrib(underlyingWindowHandle, GLFW_HOVERED);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    glfwGetFramebufferSize(underlyingWindowHandle, &pProperties[0].framebufferSize.width, &pProperties[0].framebufferSize.height);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    glfwGetWindowFrameSize(underlyingWindowHandle, &pProperties[0].frameSize.left, &pProperties[0].frameSize.top, &pProperties[0].frameSize.right, &pProperties[0].frameSize.bottom);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    glfwGetWindowContentScale(underlyingWindowHandle, &pProperties[0].contentScale.xScale, &pProperties[0].contentScale.yScale);
    switch(glfwGetError()) {
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
        switch(glfwGetError()) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.iconified != newState.iconified) {
        switch(newState.iconified) {
            case VKFW_TRUE:
                glfwIconifyWindow(underlyingWindowHandle);
                switch(glfwGetError()) {
                    case GLFW_NO_ERROR: break;
                    case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                    default: return VKFW_ERROR_UNKNOWN;
                }
            break;
            case VKFW_FALSE:
                glfwRestoreWindow(underlyingWindowHandle);
                switch(glfwGetError()) {
                    case GLFW_NO_ERROR: break;
                    case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                    default: return VKFW_ERROR_UNKNOWN;
                }
                switch(newState.maximized) {
                    case VKFW_TRUE:
                        glfwMaximizeWindow(underlyingWindowHandle);
                        switch(glfwGetError()) {
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
        switch(glfwGetError()) {
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
        switch(glfwGetError()) {
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
        switch(glfwGetError()) {
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
        switch(glfwGetError()) {
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
        switch(glfwGetError()) {
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
                switch(glfwGetError()) {
                    case GLFW_NO_ERROR: break;
                    case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                    default: return VKFW_ERROR_UNKNOWN;
                }
            break;
            case VKFW_FALSE:
                glfwRestoreWindow(underlyingWindowHandle);
                switch(glfwGetError()) {
                    case GLFW_NO_ERROR: break;
                    case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                    default: return VKFW_ERROR_UNKNOWN;
                }
                switch(newState.iconified) {
                    case VKFW_TRUE:
                        glfwIconifyWindow(underlyingWindowHandle);
                        switch(glfwGetError()) {
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
        switch(glfwGetError()) {
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
        switch(glfwGetError()) {
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
        if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
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
        switch(glfwGetError()) {
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
        switch(glfwGetError()) {
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
        switch(glfwGetError()) {
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
        switch(glfwGetError()) {
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
        switch(glfwGetError()) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.title != newState.title) {
        glfwSetWindowTitle(underlyingWindowHandle, newState.title);
        switch(glfwGetError()) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.position != newState.position) {
        glfwSetWindowPos(underlyingWindowHandle, newState.position.x, newState.position.y);
        switch(glfwGetError()) {
            case GLFW_NO_ERROR: break;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.size != newState.size) {
        glfwSetWindowSize(underlyingWindowHandle, newState.size.width, newState.size.height);
        switch(glfwGetError()) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.cursorPosition != newState.cursorPosition) {
        glfwSetCursorPos(underlyingWindowHandle, newState.cursorPosition.x, newState.cursorPosition.y);
        switch(glfwGetError()) {
            case GLFW_NO_ERROR: break;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.opacity != newState.opacity) {
        glfwSetWindowOpacity(underlyingWindowHandle, newState.opacity);
        switch(glfwGetError()) {
            case GLFW_NO_ERROR: break;
            case GLFW_FEATURE_UNAVAILABLE: return VKFW_ERROR_FEATURE_NOT_SUPPORTED;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    if(oldProperties.state.pUserPointer != newState.pUserPointer) {
        glfwSetWindowUserPointer(underlyingWindowHandle, newState.pUserPointer);
        if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    }
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetWindowIcon(VkfwWindow window, uint32_t imageCount, const VkfwImageData* images) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(window == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].monitorHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(window[0].windowHandle == NULL) return VKFW_ERROR_INVALID_HANDLE;
    
    GLFWwindow* underlyingWindowHandle  = window[0].windowHandle;
    
    glfwSetWindowIcon(underlyingWindowHandle, imageCount, images);
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        case GLFW_FEATURE_UNAVAILABLE: /* this is not a mistake, but intended behavior on Wayland. Since the value in glfwSetWindowMonitor is then ignored, we don't need to set any specific value.. */
            break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    
    /* get information if we are in fullscreen mode or not */
    VkfwBool32 weAreInFullscreenMode;
    GLFWmonitor* localMonitor = glfwGetWindowMonitor(underlyingWindowHandle);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    if(localMonitor != NULL && localMonitor != underlyingOldMonitorHandle) return VKFW_ERROR_UNKNOWN;
    weAreInFullscreenMode = (localMonitor != NULL);
    
    /* _only_ if we currently are in fullscreen mode, we actually set the GLFW monitor, otherwise we just store it for the future */
    if(weAreInFullscreenMode) {
        glfwSetWindowMonitor(underlyingWindowHandle, underlyingNewMonitorHandle, currentX, currentY, newVideoMode.width, newVideoMode.height, newVideoMode.refreshRate);
        switch(glfwGetError()) {
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
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
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
        case VKFW_CURSOR_SHAPE_CUSTOM                       :  underlyingCursorHandle = glfwCreateCursor(pCreateInfo[0].customCursorImageData, 
                                                                    pCreateInfo[0].customCursorHotspotCoordinate.x, pCreateInfo[0].customCursorHotspotCoordinate.y); break;
        default: return VKFW_ERROR_INVALID_ENUM_VALUE;
    }
    if(underlyingCursorHandle == NULL) {
        if(pCreateInfo[0].shape == VKFW_CURSOR_SHAPE_CUSTOM) {
            switch(glfwGetError()) {
                case GLFW_INVALID_VALUE: return VKFW_ERROR_INVALID_NUMERIC_VALUE;
                case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                default: return VKFW_ERROR_UNKNOWN;
            }
        } else {
            switch(glfwGetError()) {
                case GLFW_CURSOR_UNAVAILABLE: return VKFW_ERROR_CURSOR_SHAPE_NOT_SUPPORTED;
                case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
                default: return VKFW_ERROR_UNKNOWN;
            }
        }
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    pCursor[0] = (VkfwCursor) underlyingCursorHandle;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR void         VKFWAPI_CALL vkfwDestroyCursor(VkfwCursor cursor, const VkfwAllocationCallbacks* pAllocator) {
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if(cursor == NULL) return VKFW_ERROR_INVALID_HANDLE;
    if(pAllocator != initAllocator) return VKFW_ERROR_INVALID_POINTER_VALUE;
    
    GLFWcursor* underlyingCursorHandle = (GLFWcursor*) cursor;
    
    glfwDestroyCursor(underlyingCursorHandle);
    switch(glfwGetError()) {
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
    switch(glfwGetError()) {
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
    for(int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++) {
        if(glfwJoystickPresent(jid)) count++;
        switch(glfwGetError()) {
            case GLFW_NO_ERROR: break;
            case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
            default: return VKFW_ERROR_UNKNOWN;
        }
    }
    
    pJoystickCount[0] = count;
    
    if(pJoysticks != NULL) {
        count = 0;
        for(int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++) {
            if(glfwJoystickPresent(jid)) {
                pJoysticks[count] = (VkfwJoystick) jid;
                count++;
            }
            switch(glfwGetError()) {
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
    
    pProperties[0}.joystickID = underlyingJoystickID;
    pProperties[0}.axesStates = glfwGetJoystickAxes(underlyingJoystickID, &pProperties[0}.axesCount);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0}.buttonStates = glfwGetJoystickButtons(underlyingJoystickID, &pProperties[0}.buttonCount);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0}.hatStates = glfwGetJoystickHats(underlyingJoystickID, &pProperties[0}.hatCount);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0}.name = glfwGetJoystickName(underlyingJoystickID);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    pProperties[0}.GUID = glfwGetJoystickGUID(underlyingJoystickID);
    switch(glfwGetError()) {
        case GLFW_NO_ERROR: break;
        case GLFW_PLATFORM_ERROR: return VKFW_ERROR_PLATFORM_ERROR;
        default: return VKFW_ERROR_UNKNOWN;
    }
    switch(glfwJoystickIsGamepad(underlyingJoystickID)) {
        case GLFW_TRUE: pProperties[0}.isGamepad = VKFW_TRUE; break;
        case GLFW_FALSE: pProperties[0}.isGamepad = VKFW_FALSE; break;
        default: return VKFW_ERROR_UNKNOWN;
    }
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0}.gamepadName = glfwGetGamepadName(underlyingJoystickID);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0}.gamepadStateRetrievalSuccessfull = glfwGetGamepadState(underlyingJoystickID, &pProperties[0}.gamepadState);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    pProperties[0}.userPointer = glfwGetJoystickUserPointer(underlyingJoystickID);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
    return VKFW_SUCCESS;
}
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetJoystickUserPointer(VkfwJoystick joystick, void* pUserPointer) {    
    if(!vfkwInstanceInitialized) return VKFW_ERROR_INITIALIZATION_FAILED;
    if((int32_t) joystick < GLFW_JOYSTICK_1 || (int32_t) joystick > GLFW_JOYSTICK_LAST) return VKFW_ERROR_INVALID_HANDLE;
    
    int32_t underlyingJoystickID = (int32_t) joystick;
    
    glfwSetJoystickUserPointer(underlyingJoystickID, pUserPointer);
    if(glfwGetError()) return VKFW_ERROR_UNKNOWN;
    
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

