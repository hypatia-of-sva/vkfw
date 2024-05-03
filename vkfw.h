/*************************************************************************
 * VKFW 1.0
 * A library for window and input to use for Vulkan, based on GLFW 3.4
 *------------------------------------------------------------------------
 * Copyright (c) 2002-2006 Marcus Geelnard
 * Copyright (c) 2006-2019 Camilla Löwy <elmindreda@glfw.org>
 * Copyright (c) 2024-     Hypatia of Sva <hypatia.sva@posteo.eu>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 *************************************************************************/

#ifndef _vkfw_h_
#define _vkfw_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define VKFWAPI
#define VKFWAPI_PTR VKFWAPI
#define VKFWAPI_ATTR VKFWAPI
#define VKFWAPI_CALL 

#define VKFW_VERSION_MAJOR          1
#define VKFW_VERSION_MINOR          0
#define VKFW_VERSION_REVISION       0

#define VKFW_FALSE                  0U
#define VKFW_TRUE                   1U
#define VKFW_DONT_CARE              -1


typedef uint32_t VkfwFlags;
typedef uint32_t VkfwBool32;

typedef struct VkfwInstance_t* VkfwInstance;
/* the other four are direct matches of corresponding GLFW types. If this gets changed at some point, VkfwJoystick can also become a true handle */
typedef struct VkfwMonitor_t* VkfwMonitor;
typedef struct int32_t VkfwJoystick;
typedef struct VkfwWindow_t* VkfwWindow;
typedef struct VkfwCursor_t* VkfwCursor;

/* Vulkan Mappings */
#if defined(VK_VERSION_1_0)
    #define VKFW_VKAPI_PTR VVKAPI_PTR
    typedef VkBool32 VkfwVkBool32;
    typedef VkInstance VkfwVkInstance;
    typedef VkPhysicalDevice VkfwVkPhysicalDevice;
    typedef VkSurfaceKHR VkfwVkSurfaceKHR;
    typedef VkAllocationCallbacks VkfwVkAllocationCallbacks;
    typedef VkResult VkfwVkResult;
    typedef PFN_vkVoidFunction PFN_vkfwVkVoidFunction;
    typedef PFN_vkGetInstanceProcAddr PFN_vkfwVkGetInstanceProcAddr;
#else
    #define VKFW_VKAPI_PTR VKFWAPI_PTR
    typedef VkfwBool32 VkfwVkBool32;
    typedef struct VkfwVkInstance_t* VkfwVkInstance;
    typedef struct VkfwVkPhysicalDevice_t* VkfwVkPhysicalDevice;
    typedef struct VkfwVkSurfaceKHR_t* VkfwVkSurfaceKHR;
    typedef struct VkfwVkAllocationCallbacks_t* VkfwVkAllocationCallbacks;  /* can be treated as oblique struct since only pointer to it is used */
    typedef enum VkfwVkResult {
        VK_SUCCESS = 0,
        /* ... */
        VK_RESULT_MAX_ENUM = 0x7FFFFFFF
    } VkfwVkResult;
    typedef void (VKFW_VKAPI_PTR *PFN_vkfwVkVoidFunction)(void);
    typedef PFN_vkfwVkVoidFunction (VKFW_VKAPI_PTR *PFN_vkfwVkGetInstanceProcAddr)(VkfwVkInstance instance, const char* pName);
#endif


typedef enum VkfwResult {
    VKFW_SUCCESS                                = 0,
    VKFW_ERROR_UNKNOWN                          = -1,
    VKFW_ERROR_INITIALIZATION_FAILED            = -2,
    VKFW_ERROR_INVALID_ENUM_VALUE               = -3,
    VKFW_ERROR_INVALID_NUMERIC_VALUE            = -4,
    VKFW_ERROR_INVALID_POINTER_VALUE            = -5,
    VKFW_ERROR_INVALID_HANDLE                   = -6,
    VKFW_ERROR_OUT_OF_MEMORY                    = -7,   
    VKFW_ERROR_API_UNAVAILABLE                  = -8,
    VKFW_ERROR_PLATFORM_ERROR                   = -9,
    VKFW_ERROR_PIXEL_FORMAT_NOT_SUPPORTED       = -10,
    VKFW_ERROR_CURSOR_SHAPE_NOT_SUPPORTED       = -11,
    VKFW_ERROR_FEATURE_NOT_SUPPORTED            = -12,
    VKFW_ERROR_PLATFORM_UNAVAILABLE             = -13,
    VKFW_ERROR_RESULT_NOT_AVAILABLE             = -14,
    VKFW_RESULT_MAX_ENUM                        = 0x7FFFFFFF
} VkfwResult;
typedef enum VkfwStructureType {
    VKFW_STRUCTURE_TYPE_INSTANCE_CREATE_INFO            = 0,
    VKFW_STRUCTURE_TYPE_WINDOW_CREATE_INFO              = 1,
    VKFW_STRUCTURE_TYPE_CURSOR_CREATE_INFO              = 2,

    VKFW_STRUCTURE_TYPE_MAX_ENUM                        = 0x7FFFFFFF
} VkfwStructureType;
typedef enum VkfwAction {
    VKFW_ACTION_RELEASE         = 0,
    VKFW_ACTION_PRESS           = 1,
    VKFW_ACTION_MAX_ENUM        = 0x7F
} VkfwAction;
typedef enum VkfwKeyAction {
    VKFW_KEY_ACTION_RELEASE         = 0,
    VKFW_KEY_ACTION_PRESS           = 1,
    VKFW_KEY_ACTION_REPEAT          = 2,
    VKFW_KEY_ACTION_MAX_ENUM        = 0x7FFFFFFF
} VkfwKeyAction;
typedef enum VkfwJoystickHatState {
    VKFW_JOYSTICK_HAT_STATE_CENTERED           = 0,
    VKFW_JOYSTICK_HAT_STATE_UP                 = 1,
    VKFW_JOYSTICK_HAT_STATE_RIGHT              = 2,
    VKFW_JOYSTICK_HAT_STATE_DOWN               = 4,
    VKFW_JOYSTICK_HAT_STATE_LEFT               = 8,
    VKFW_JOYSTICK_HAT_STATE_RIGHT_UP           = (VKFW_HAT_RIGHT | VKFW_HAT_UP),
    VKFW_JOYSTICK_HAT_STATE_RIGHT_DOWN         = (VKFW_HAT_RIGHT | VKFW_HAT_DOWN),
    VKFW_JOYSTICK_HAT_STATE_LEFT_UP            = (VKFW_HAT_LEFT  | VKFW_HAT_UP),
    VKFW_JOYSTICK_HAT_STATE_LEFT_DOWN          = (VKFW_HAT_LEFT  | VKFW_HAT_DOWN),
    VKFW_JOYSTICK_HAT_STATE_MAX_ENUM = 0x7FFFFFFF
} VkfwJoystickHatState;
typedef enum VkfwKey {
    VKFW_KEY_UNKNOWN            = -1,
        /* Printable keys */
    VKFW_KEY_SPACE              = 32,
    VKFW_KEY_APOSTROPHE         = 39,  /* ' */
    VKFW_KEY_COMMA              = 44,  /* , */
    VKFW_KEY_MINUS              = 45,  /* - */
    VKFW_KEY_PERIOD             = 46,  /* . */
    VKFW_KEY_SLASH              = 47,  /* / */
    VKFW_KEY_0                  = 48,
    VKFW_KEY_1                  = 49,
    VKFW_KEY_2                  = 50,
    VKFW_KEY_3                  = 51,
    VKFW_KEY_4                  = 52,
    VKFW_KEY_5                  = 53,
    VKFW_KEY_6                  = 54,
    VKFW_KEY_7                  = 55,
    VKFW_KEY_8                  = 56,
    VKFW_KEY_9                  = 57,
    VKFW_KEY_SEMICOLON          = 59,  /* ; */
    VKFW_KEY_EQUAL              = 61,  /* = */
    VKFW_KEY_A                  = 65,
    VKFW_KEY_B                  = 66,
    VKFW_KEY_C                  = 67,
    VKFW_KEY_D                  = 68,
    VKFW_KEY_E                  = 69,
    VKFW_KEY_F                  = 70,
    VKFW_KEY_G                  = 71,
    VKFW_KEY_H                  = 72,
    VKFW_KEY_I                  = 73,
    VKFW_KEY_J                  = 74,
    VKFW_KEY_K                  = 75,
    VKFW_KEY_L                  = 76,
    VKFW_KEY_M                  = 77,
    VKFW_KEY_N                  = 78,
    VKFW_KEY_O                  = 79,
    VKFW_KEY_P                  = 80,
    VKFW_KEY_Q                  = 81,
    VKFW_KEY_R                  = 82,
    VKFW_KEY_S                  = 83,
    VKFW_KEY_T                  = 84,
    VKFW_KEY_U                  = 85,
    VKFW_KEY_V                  = 86,
    VKFW_KEY_W                  = 87,
    VKFW_KEY_X                  = 88,
    VKFW_KEY_Y                  = 89,
    VKFW_KEY_Z                  = 90,
    VKFW_KEY_LEFT_BRACKET       = 91,  /* [ */
    VKFW_KEY_BACKSLASH          = 92,  /* \ */
    VKFW_KEY_RIGHT_BRACKET      = 93,  /* ] */
    VKFW_KEY_GRAVE_ACCENT       = 96,  /* ` */
    VKFW_KEY_WORLD_1            = 161, /* non-US #1 */
    VKFW_KEY_WORLD_2            = 162, /* non-US #2 */
        /* Function keys */
    VKFW_KEY_ESCAPE             = 256,
    VKFW_KEY_ENTER              = 257,
    VKFW_KEY_TAB                = 258,
    VKFW_KEY_BACKSPACE          = 259,
    VKFW_KEY_INSERT             = 260,
    VKFW_KEY_DELETE             = 261,
    VKFW_KEY_RIGHT              = 262,
    VKFW_KEY_LEFT               = 263,
    VKFW_KEY_DOWN               = 264,
    VKFW_KEY_UP                 = 265,
    VKFW_KEY_PAGE_UP            = 266,
    VKFW_KEY_PAGE_DOWN          = 267,
    VKFW_KEY_HOME               = 268,
    VKFW_KEY_END                = 269,
    VKFW_KEY_CAPS_LOCK          = 280,
    VKFW_KEY_SCROLL_LOCK        = 281,
    VKFW_KEY_NUM_LOCK           = 282,
    VKFW_KEY_PRINT_SCREEN       = 283,
    VKFW_KEY_PAUSE              = 284,
    VKFW_KEY_F1                 = 290,
    VKFW_KEY_F2                 = 291,
    VKFW_KEY_F3                 = 292,
    VKFW_KEY_F4                 = 293,
    VKFW_KEY_F5                 = 294,
    VKFW_KEY_F6                 = 295,
    VKFW_KEY_F7                 = 296,
    VKFW_KEY_F8                 = 297,
    VKFW_KEY_F9                 = 298,
    VKFW_KEY_F10                = 299,
    VKFW_KEY_F11                = 300,
    VKFW_KEY_F12                = 301,
    VKFW_KEY_F13                = 302,
    VKFW_KEY_F14                = 303,
    VKFW_KEY_F15                = 304,
    VKFW_KEY_F16                = 305,
    VKFW_KEY_F17                = 306,
    VKFW_KEY_F18                = 307,
    VKFW_KEY_F19                = 308,
    VKFW_KEY_F20                = 309,
    VKFW_KEY_F21                = 310,
    VKFW_KEY_F22                = 311,
    VKFW_KEY_F23                = 312,
    VKFW_KEY_F24                = 313,
    VKFW_KEY_F25                = 314,
    VKFW_KEY_KP_0               = 320,
    VKFW_KEY_KP_1               = 321,
    VKFW_KEY_KP_2               = 322,
    VKFW_KEY_KP_3               = 323,
    VKFW_KEY_KP_4               = 324,
    VKFW_KEY_KP_5               = 325,
    VKFW_KEY_KP_6               = 326,
    VKFW_KEY_KP_7               = 327,
    VKFW_KEY_KP_8               = 328,
    VKFW_KEY_KP_9               = 329,
    VKFW_KEY_KP_DECIMAL         = 330,
    VKFW_KEY_KP_DIVIDE          = 331,
    VKFW_KEY_KP_MULTIPLY        = 332,
    VKFW_KEY_KP_SUBTRACT        = 333,
    VKFW_KEY_KP_ADD             = 334,
    VKFW_KEY_KP_ENTER           = 335,
    VKFW_KEY_KP_EQUAL           = 336,
    VKFW_KEY_LEFT_SHIFT         = 340,
    VKFW_KEY_LEFT_CONTROL       = 341,
    VKFW_KEY_LEFT_ALT           = 342,
    VKFW_KEY_LEFT_SUPER         = 343,
    VKFW_KEY_RIGHT_SHIFT        = 344,
    VKFW_KEY_RIGHT_CONTROL      = 345,
    VKFW_KEY_RIGHT_ALT          = 346,
    VKFW_KEY_RIGHT_SUPER        = 347,
    VKFW_KEY_MENU               = 348,

    VKFW_KEY_LAST               = VKFW_KEY_MENU,
    VKFW_KEY_MAX_ENUM           = 0x7FFFFFFF
} VkfwKey;
typedef enum VkfwModifierKeyFlagBits {
    VKFW_MODIFIER_KEY_NONE                = 0,
    VKFW_MODIFIER_KEY_SHIFT_BIT           = 0x0001,
    VKFW_MODIFIER_KEY_CONTROL_BIT         = 0x0002,
    VKFW_MODIFIER_KEY_ALT_BIT             = 0x0004,
    VKFW_MODIFIER_KEY_SUPER_BIT           = 0x0008,
    VKFW_MODIFIER_KEY_CAPS_LOCK_BIT       = 0x0010,
    VKFW_MODIFIER_KEY_NUM_LOCK_BIT        = 0x0020,
    VKFW_MODIFIER_KEY_FLAG_BITS_MAX_ENUM  = 0x7FFFFFFF
} VkfwModifierKeyFlagBits;
typedef VkfwFlags VkfwModifierKeyMask;
typedef enum VkfwMouseButton {
    VKFW_MOUSE_BUTTON_1         = 0,
    VKFW_MOUSE_BUTTON_2         = 1,
    VKFW_MOUSE_BUTTON_3         = 2,
    VKFW_MOUSE_BUTTON_4         = 3,
    VKFW_MOUSE_BUTTON_5         = 4,
    VKFW_MOUSE_BUTTON_6         = 5,
    VKFW_MOUSE_BUTTON_7         = 6,
    VKFW_MOUSE_BUTTON_8         = 7,
    VKFW_MOUSE_BUTTON_LAST      = VKFW_MOUSE_BUTTON_8,
    VKFW_MOUSE_BUTTON_LEFT      = VKFW_MOUSE_BUTTON_1,
    VKFW_MOUSE_BUTTON_RIGHT     = VKFW_MOUSE_BUTTON_2,
    VKFW_MOUSE_BUTTON_MIDDLE    = VKFW_MOUSE_BUTTON_3,
    VKFW_MOUSE_BUTTON_MAX_ENUM  = 0x7FFFFFFF
} VkfwMouseButton;
typedef enum VkfwGamepadButton {
    VKFW_GAMEPAD_BUTTON_A               = 0,
    VKFW_GAMEPAD_BUTTON_B               = 1,
    VKFW_GAMEPAD_BUTTON_X               = 2,
    VKFW_GAMEPAD_BUTTON_Y               = 3,
    VKFW_GAMEPAD_BUTTON_LEFT_BUMPER     = 4,
    VKFW_GAMEPAD_BUTTON_RIGHT_BUMPER    = 5,
    VKFW_GAMEPAD_BUTTON_BACK            = 6,
    VKFW_GAMEPAD_BUTTON_START           = 7,
    VKFW_GAMEPAD_BUTTON_GUIDE           = 8,
    VKFW_GAMEPAD_BUTTON_LEFT_THUMB      = 9,
    VKFW_GAMEPAD_BUTTON_RIGHT_THUMB     = 10,
    VKFW_GAMEPAD_BUTTON_DPAD_UP         = 11,
    VKFW_GAMEPAD_BUTTON_DPAD_RIGHT      = 12,
    VKFW_GAMEPAD_BUTTON_DPAD_DOWN       = 13,
    VKFW_GAMEPAD_BUTTON_DPAD_LEFT       = 14,
    VKFW_GAMEPAD_BUTTON_LAST            = VKFW_GAMEPAD_BUTTON_DPAD_LEFT,

    VKFW_GAMEPAD_BUTTON_CROSS           = VKFW_GAMEPAD_BUTTON_A,
    VKFW_GAMEPAD_BUTTON_CIRCLE          = VKFW_GAMEPAD_BUTTON_B,
    VKFW_GAMEPAD_BUTTON_SQUARE          = VKFW_GAMEPAD_BUTTON_X,
    VKFW_GAMEPAD_BUTTON_TRIANGLE        = VKFW_GAMEPAD_BUTTON_Y,

    VKFW_GAMEPAD_BUTTON_MAX_ENUM        = 0x7FFFFFFF
} VkfwGamepadButton;
typedef enum VkfwGamepadAxis {
    VKFW_GAMEPAD_AXIS_LEFT_X         = 0,
    VKFW_GAMEPAD_AXIS_LEFT_Y         = 1,
    VKFW_GAMEPAD_AXIS_RIGHT_X        = 2,
    VKFW_GAMEPAD_AXIS_RIGHT_Y        = 3,
    VKFW_GAMEPAD_AXIS_LEFT_TRIGGER   = 4,
    VKFW_GAMEPAD_AXIS_RIGHT_TRIGGER  = 5,
    VKFW_GAMEPAD_AXIS_LAST           = VKFW_GAMEPAD_AXIS_RIGHT_TRIGGER,

    VKFW_GAMEPAD_AXIS_MAX_ENUM       = 0x7FFFFFFF
} VkfwGamepadAxis;
typedef enum VkfwCursorMode {
    VKFW_CURSOR_MODE_NORMAL          = 0x00034001,
    VKFW_CURSOR_MODE_HIDDEN          = 0x00034002,
    VKFW_CURSOR_MODE_DISABLED        = 0x00034003,
    VKFW_CURSOR_MODE_CAPTURED        = 0x00034004,
    VKFW_CURSOR_MODE_MAX_ENUM        = 0x7FFFFFFF
} VkfwCursorMode;
typedef enum VkfwCursorShape {
    VKFW_CURSOR_SHAPE_STANDARD_ARROW_CURSOR           = 0x00036001,
    VKFW_CURSOR_SHAPE_STANDARD_IBEAM_CURSOR           = 0x00036002,
    VKFW_CURSOR_SHAPE_STANDARD_CROSSHAIR_CURSOR       = 0x00036003,
    VKFW_CURSOR_SHAPE_STANDARD_POINTING_HAND_CURSOR   = 0x00036004,
    VKFW_CURSOR_SHAPE_STANDARD_RESIZE_EW_CURSOR       = 0x00036005,
    VKFW_CURSOR_SHAPE_STANDARD_RESIZE_NS_CURSOR       = 0x00036006,
    VKFW_CURSOR_SHAPE_STANDARD_RESIZE_NWSE_CURSOR     = 0x00036007,
    VKFW_CURSOR_SHAPE_STANDARD_RESIZE_NESW_CURSOR     = 0x00036008,
    VKFW_CURSOR_SHAPE_STANDARD_RESIZE_ALL_CURSOR      = 0x00036009,
    VKFW_CURSOR_SHAPE_STANDARD_NOT_ALLOWED_CURSOR     = 0x0003600A,
    VKFW_CURSOR_SHAPE_CUSTOM                          = 0x00037001,
    VKFW_CURSOR_SHAPE_MAX_ENUM                        = 0x7FFFFFFF
} VkfwCursorShape;
typedef enum VkfwConnectionEvent {
    VKFW_CONNECTION_EVENT_CONNECTED               = 0x00040001,
    VKFW_CONNECTION_EVENT_DISCONNECTED            = 0x00040002,
    VKFW_CONNECTION_EVENT_MAX_ENUM                = 0x7FFFFFFF
} VkfwConnectionEvent;
typedef enum VkfwInstancePlatform {
    VKFW_INSTANCE_PLATFORM_DEFAULT       = 0x00060000,
    VKFW_INSTANCE_PLATFORM_WIN32         = 0x00060001,
    VKFW_INSTANCE_PLATFORM_COCOA         = 0x00060002,
    VKFW_INSTANCE_PLATFORM_WAYLAND       = 0x00060004,
    VKFW_INSTANCE_PLATFORM_X11           = 0x00060008,
    VKFW_INSTANCE_PLATFORM_NULL          = 0x00060010,
    VKFW_INSTANCE_PLATFORM_MAX_ENUM      = 0x7FFFFFFF
} VkfwInstancePlatform:
typedef VkfwFlags VkfwInstancePlatformMask;

typedef enum VkfwInstanceCreateFlagBits {
    VKFW_INSTANCE_CREATE_DEFAULTS                                   = 0,
    VKFW_INSTANCE_CREATE_DISABLE_JOYSTICK_HAT_BUTTONS_BIT           = 0x00000001,
    VKFW_INSTANCE_CREATE_WAYLAND_DISABLE_LIBDECOR_BIT_WL            = 0x00001000,
    VKFW_INSTANCE_CREATE_X11_DISABLE_XCB_VULKAN_SURFACE_BIT_X11     = 0x00002000,
    VKFW_INSTANCE_CREATE_COCOA_DISABLE_MENUBAR_BIT_COCOA            = 0x00004000,
    VKFW_INSTANCE_CREATE_COCOA_DISABLE_CHDIR_RESOURCES_BIT_COCOA    = 0x00008000,
    VKFW_INSTANCE_CREATE_FLAG_BITS_MAX_ENUM                         = 0x7FFFFFFF
} VkfwInstanceCreateFlagBits;
typedef VkfwFlags VkfwInstanceCreateFlags;
typedef enum VkfwWindowCreateFlagBits {
    VKFW_WINDOW_CREATE_DEFAULTS                             = 0,
    VKFW_WINDOW_CREATE_NO_INITIAL_FOCUS_BIT                 = 0x0001,
    VKFW_WINDOW_CREATE_DONT_CENTER_CURSOR_BIT               = 0x0002,
    VKFW_WINDOW_CREATE_TRANSPARENT_FRAMEBUFFER_BIT          = 0x0004,
    VKFW_WINDOW_CREATE_SCALE_TO_MONITOR_BIT                 = 0x0008,
    VKFW_WINDOW_CREATE_NO_SCALE_FRAMEBUFFER_BIT             = 0x0010, /* platform specific? ignored except on MacOS */
    VKFW_WINDOW_CREATE_SET_INITIAL_CURSOR_POSITION_BIT      = 0x0020, /* since before creation one cannot chack the previous position, to see if it would need to be changed, there needs to be a flag for it */
    VKFW_WINDOW_CREATE_COCOA_GRAPHICS_SWITCHING_BIT_COCOA   = 0x00100000,
    VKFW_WINDOW_CREATE_WIN32_KEYBOARD_MENU_BIT_WIN32        = 0x00200000,
    VKFW_WINDOW_CREATE_WIN32_SHOWDEFAULT_BIT_WIN32          = 0x00400000,
    VKFW_WINDOW_CREATE_FLAG_BITS_MAX_ENUM                   = 0x7FFFFFFF
} VkfwWindowCreateFlagBits;
typedef VkfwFlags VkfwWindowCreateFlags;
typedef VkfwFlags VkfwCursorCreateFlags;

typedef struct VkfwVideoMode {
    /* width/height in screen coordinates */
    int32_t width;
    int32_t height;
    int32_t redBits;
    int32_t greenBits;
    int32_t blueBits;
    /* refresh rate in Hz */
    int32_t refreshRate;
} VkfwVideoMode;
typedef struct VkfwGammaRamp {
    uint16_t* red;
    uint16_t* green;
    uint16_t* blue;
    uint32_t size;
} VkfwGammaRamp;
typedef struct VkfwImageData {
    /* width/height in pixels */
    int32_t width;
    int32_t height;
    /* pixel data of this image, arranged left-to-right, top-to-bottom. */
    uint8_t* pixels;
} VkfwImageData;
typedef struct VkfwGamepadInputState {
    VkfwAction buttons[15];
    /* in the range -1.0 to 1.0 inclusive. */
    float axes[6];
} VkfwGamepadInputState;
typedef struct VkfwExtent2D {
    /* different from VkExtent2D! Uses signed integers because of GLFW interface */
    int32_t    width;
    int32_t    height;
} VkfwExtent2D;
typedef struct VkfwOffset2D {
    int32_t    x;
    int32_t    y;
} VkfwOffset2D;
typedef struct VkfwRect2D {
    VkfwOffset2D    offset;
    VkfwExtent2D    extent;
} VkfwRect2D;
typedef struct VkfwPosition {
    double    x;
    double    y;
} VkfwPosition;
typedef struct VkfwContentScale {
    float xScale;
    float yScale;
} VkfwContentScale;
typedef struct VkfwQuad {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} VkfwContentScale;
typedef struct VkfwGamepadMappingElement {
    uint8_t         type;
    uint8_t         index;
    int8_t          axisScale;
    int8_t          axisOffset;
} VkfwGamepadMappingElement;
typedef struct VkfwGamepadMapping {
    char                        name[128];
    char                        guid[33];
    VkfwGamepadMappingElement   buttons[15];
    VkfwGamepadMappingElement   axes[6];
} VkfwGamepadMapping;

typedef struct VkfwGlobalProperties {
    uint32_t                    majorVersion;
    uint32_t                    minorVersion;
    uint32_t                    revisionVersion;
    const char*                 underlyingVersionString;
    VkfwInstancePlatformMask    supportedPlatforms;
} VkfwGlobalProperties;
typedef struct VkfwInstanceProperties {
    VkfwInstancePlatform                    usedPlatform;
    VkfwBool32                              rawMouseMotionSupported;
    VkfwBool32                              vulkanFound;
    PFN_vkfwVkGetInstanceProcAddr           vulkanLoader
    uint32_t                                requiredInstanceExtensionCount;
    const char**                            requiredInstanceExtensions;
    uint64_t                                timerFrequency;
} VkfwInstanceProperties;
typedef struct VkfwMonitorProperties {
    VkfwOffset2D                viewportPosition; /* from glfwGetMonitorPos; GL specific? */
    VkfwRect2D                  workarea;
    VkfwExtent2D                physicalSizeMM;
    VkfwContentScale            contentScale;
    const char*                 pName;
    void*                       pUserPointer;
    const VkfwVideoMode*        pVideoModes;
    int32_t                     videoModeCount;  /* signed because of GLFW */
    const VkfwVideoMode*        pCurrentVideoMode; /* may be replaced with an index into pVideoModes */
    const VkfwGammaRamp*        pCurrentGammeRamp;
} VkfwMonitorProperties;
typedef struct VkfwJoystickProperties {
    int32_t                 joystickID;
    uint32_t                axesCount;
    const float*            axesStates;´
    uint32_t                buttonCount;
    VkfwAction*             buttonStates;
    uint32_t                hatCount;
    VkfwJoystickHatState*   hatStates;
    const char*             name;
    const char*             GUID;
    VkfwBool32              isGamepad;
    const char*             gamepadName;
    VkfwGamepadInputState   gamepadState;
    VkfwBool32              gamepadStateRetrievalSuccessfull;
    void*                   userPointer;
} VkfwJoystickProperties;
typedef struct VkfwWindowProperties {
    VkfwWindowState     state;
    VkfwBool32          focused;
    VkfwBool32          transparentFramebuffer;
    VkfwBool32          hovered;
    VkfwExtent2D        framebufferSize;
    VkfwQuad            frameSize;
    VkfwContentScale    contentScale;
    VkfwMonitor         monitor;
} VkfwWindowProperties;
typedef struct VkfwWindowState {
    /* 15 flags; can be combined into 1 uint16_t, reducing the struct by 58 bytes to ~64 bytes from ca 120 bytes, almost halving it. */
    VkfwBool32      fullscreen; /* with this off, the monitor is curently ignored, but one could move the window to the monitor in windowed mode on certain platforms */
    VkfwBool32      iconified;
    VkfwBool32      resizable;
    VkfwBool32      visible;
    VkfwBool32      decorated;
    VkfwBool32      autoIconify;
    VkfwBool32      floating;
    VkfwBool32      maximized;
    VkfwBool32      focusOnShow;
    VkfwBool32      mousePassthrough;
    VkfwBool32      shouldClose;
    VkfwBool32      stickyKeys;
    VkfwBool32      stickyMouseButtons;
    VkfwBool32      lockKeyMods;
    VkfwBool32      rawMouseMotion;
    VkfwCursorMode  cursorMode;
    const char*     title;
    VkfwOffset2D    position;
    VkfwExtent2D    size;
    VkfwPosition    cursorPosition;
    float           opacity;
    void*           pUserPointer;
} VkfwWindowState;

typedef void* (VKAPI_PTR *PFN_vkfwAllocationFunction)(
    size_t                                      size,
    void*                                       pUserData);
typedef void (VKAPI_PTR *PFN_vkfwFreeFunction)(
    void*                                       pMemory,
    void*                                       pUserData);
typedef void* (VKAPI_PTR *PFN_vkfwReallocationFunction)(
    void*                                       pOriginal,
    size_t                                      size,
    void*                                       pUserData);
typedef struct VkfwAllocationCallbacks {
    void*                                     pUserData;
    PFN_vkfwAllocationFunction                pfnAllocation;
    PFN_vkfwReallocationFunction              pfnReallocation;
    PFN_vkfwFreeFunction                      pfnFree;
} VkfwAllocationCallbacks;

typedef void (* VkfwInstanceMonitorConnectionCallback)(VkfwMonitor monitor, VkfwConnectionEvent event);
typedef void (* VkfwInstanceJoystickConnectionCallback)(VkfwJoystick jid, VkfwConnectionEvent event);

typedef struct VkfwInstanceCallbacks {
    VkfwInstanceMonitorConnectionCallback   monitorConnection;
    VkfwInstanceJoystickConnectionCallback  joystickConnection;
} VkfwInstanceCallbacks;

typedef void (* VkfwWindowPositionChangeCallback)                       (VkfwWindow window, int32_t xPosition, int32_t yPosition);
typedef void (* VkfwWindowSizeChangeCallback)                           (VkfwWindow window, int32_t width, int32_t height);
typedef void (* VkfwWindowFramebufferSizeChangeCallback)                (VkfwWindow window, int32_t width, int32_t height);
typedef void (* VkfwWindowContentScaleChangeCallback)                   (VkfwWindow window, float xScale, float yScale);
typedef void (* VkfwWindowCursorPositionChangeCallback)                 (VkfwWindow window, double xPosition, double yPosition);
typedef void (* VkfwWindowFocusChangeCallback)                          (VkfwWindow window, VkfwBool32 focused);
typedef void (* VkfwWindowIconficationChangeCallback)                   (VkfwWindow window, VkfwBool32 iconified);
typedef void (* VkfwWindowMaximizationChangeCallback)                   (VkfwWindow window, VkfwBool32 maximized);

typedef void (* VkfwWindowCloseButtonClickedCallback)                   (VkfwWindow window);
typedef void (* VkfwWindowContentAreaNeedsToBeRedrawnCallback)          (VkfwWindow window);
typedef void (* VkfwWindowPathDropCallback)                             (VkfwWindow window, int32_t pathCount, const char** paths);

typedef void (* VkfwWindowMouseButtonInputCallback)                     (VkfwWindow window, VkfwMouseButton button, VkfwAction action, VkfwModifierKeyMask mods);
typedef void (* VkfwWindowScrollInputCallback)                          (VkfwWindow window, double xAxisOffset, double yAxisOffset);
typedef void (* VkfwCursorEnterOrLeaveContentAreaCallback)              (VkfwWindow window, VkfwBool32 entered);
typedef void (* VkfwWindowKeyInputCallback)                             (VkfwWindow window, VkfwKey key, int32_t scancode, VkfwKeyAction action, VkfwModifierKeyMask mods);
typedef void (* VkfwWindowUnicodeCharacterInputCallback)                (VkfwWindow window, uint32_t codepoint);
typedef void (* VkfwWindowUnicodeCharacterInputWithModifiersCallback)   (VkfwWindow window, uint32_t codepoint, VkfwModifierKeyMask mods); /* deprecated in GLFW 3.4, to be removed in GLFW 4.0; should VKFW keep it? */

typedef struct VkfwWindowCallbacks {
    VkfwWindowPositionChangeCallback                        positionChange;
    VkfwWindowSizeChangeCallback                            sizeChange;
    VkfwWindowFramebufferSizeChangeCallback                 framebufferSizeChange;
    VkfwWindowContentScaleChangeCallback                    contentScaleChange;
    VkfwWindowCursorPositionChangeCallback                  cursorPositionChange;
    VkfwWindowFocusChangeCallback                           focusChange;
    VkfwWindowIconficationChangeCallback                    iconficationChange;
    VkfwWindowMaximizationChangeCallback                    maximizationChange;

    VkfwWindowCloseButtonClickedCallback                    closeButtonClicked;
    VkfwWindowContentAreaNeedsToBeRedrawnCallback           contentAreaNeedsToBeRedrawn;
    VkfwWindowPathDropCallback                              pathDrop;

    VkfwWindowMouseButtonInputCallback                      mouseButtonInput;
    VkfwWindowScrollInputCallback                           scrollInput;
    VkfwCursorEnterOrLeaveContentAreaCallback               cursorEnterOrLeaveContentArea;
    VkfwWindowKeyInputCallback                              keyInput;
    VkfwWindowUnicodeCharacterInputCallback                 unicodeCharacterInput;
    VkfwWindowUnicodeCharacterInputWithModifiersCallback    unicodeCharacterInputWithModifiers;
} VkfwWindowCallbacks;


typedef struct VkfwInstanceCreateInfo {
    VkfwStructureType                       sType;
    const void*                             pNext;
    VkfwInstanceCreateFlags                 flags;

    VkfwInstancePlatform                    desiredPlatform;
    PFN_vkfwVkGetInstanceProcAddr           desiredVulkanLoader;
    VkfwInstanceCallbacks                   callbacks;
} VkfwInstanceCreateInfo;

typedef struct VkfwWindowCreateInfo {
    VkfwStructureType           sType;
    const void*                 pNext;
    VkfwWindowCreateFlags       flags;

    VkfwWindowState             initialState;
    VkfwVideoMode               requestedVideoMode; //size has to be the same as in initalState
    VkfwWindowCallbacks         callbacks;

    const char*                 cocoaFrameName_COCOA;
    const char*                 x11ClassName_X11;
    const char*                 x11InstanceName_X11;
    const char*                 waylandAppID_WL;
} VkfwWindowCreateInfo;

typedef struct VkfwCursorCreateInfo {
    VkfwStructureType           sType;
    const void*                 pNext;
    VkfwCursorCreateFlags       flags;

    VkfwCursorShape             shape;
    const VkfwImageData*        customCursorImageData;
    VkfwOffset2D                customCursorHotspotCoordinate;
} VkfwCursorCreateInfo;



typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwEnumerateGlobalProperties)(VkfwGlobalProperties* pProperties);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwCreateInstance)(const VkfwInstanceCreateInfo* pCreateInfo, const VkfwAllocationCallbacks* pAllocator, VkfwInstance* pInstance);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwDestroyInstance)(VkfwInstance instance, const VkfwAllocationCallbacks* pAllocator);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwEnumerateInstanceProperties)(VkfwInstance instance, VkfwInstanceProperties* pProperties);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwGetKeyScancode)(VkfwInstance instance, VkfwKey key, int32_t* pScancode);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwGetKeyName)(VkfwInstance instance, VkfwKey key, int32_t scancode, const char** pKeyName);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwPostEmptyEvent)(VkfwInstance instance);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwProcessEvents)(VkfwInstance instance, double timeout, VkfwBool32 waitIndefinitely);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwGetClipboardString)(VkfwInstance instance, const char** pClipboardString);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwSetClipboardString)(VkfwInstance instance, const char* clipboardString);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwGetTimerValue)(VkfwInstance instance, uint64_t* pTimerValue);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwEnumerateMonitors)(VkfwInstance instance, uint32_t* pMonitorCount, VkfwMonitor* pMonitors);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwEnumerateMonitorProperties)(VkfwMonitor monitor, VkfwMonitorProperties* pProperties);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwSetMonitorUserPointer)(VkfwMonitor monitor, void* pUserPointer);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwSetMonitorGammaRamp)(VkfwMonitor monitor, const VkfwGammaRamp* pGammaRamp);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwGetGammeRampFromGammaValue)(VkfwMonitor monitor, float gamma, VkfwGammaRamp* pGammaRamp);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwCreateWindow)(VkfwMonitor monitor, const VkfwWindowCreateInfo* pCreateInfo, const VkfwAllocationCallbacks* pAllocator, VkfwWindow* pWindow);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwDestroyWindow)(VkfwWindow window, const VkfwAllocationCallbacks* pAllocator);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwEnumerateWindowProperties)(VkfwWindow window, VkfwWindowProperties* pProperties);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwSetWindowState)(VkfwWindow window, VkfwWindowState newState);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwSetWindowIcon)(VkfwWindow window, uint32_t imageCount, const VkfwImageData* images);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwSwitchWindowMonitor)(VkfwWindow window, VkfwMonitor monitor, VkfwVideoMode* requestedVideoMode);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwFocusWindow)(VkfwWindow window);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwRequestWindowAttention)(VkfwWindow window);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwSetWindowAspectRatio)(VkfwWindow window, int32_t numerator, int32_t denominator);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwSetWindowSizeLimits)(VkfwWindow window, VkfwExtent2D minimum, VkfwExtent2D maximum);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwCreateCursor)(VkfwInstance instance, const VkfwCursorCreateInfo* pCreateInfo, const VkfwAllocationCallbacks* pAllocator, VkfwCursor* pCursor);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwDestroyCursor)(VkfwCursor cursor, const VkfwAllocationCallbacks* pAllocator);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwSetWindowCursor)(VkfwWindow window, VkfwCursor cursor);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwUpdateGamepadMappings)(VkfwInstance instance, uint32_t mappingCount, const VkfwGamepadMapping* mappings);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwEnumerateJoysticks)(VkfwInstance instance, uint32_t* pJoystickCount, VkfwJoystick* pJoysticks);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwEnumerateJoystickProperties)(VkfwJoystick joystick, VkfwJoystickProperties* pProperties);
typedef VkfwResult   (VKFWAPI_PTR *PFN_vkfwSetJoystickUserPointer)(VkfwJoystick joystick, void* pUserPointer);
typedef VkfwVkBool32 (VKFWAPI_PTR *PFN_vkfwVkGetPhysicalDevicePresentationSupport)(VkfwVkInstance instance, VkfwVkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
typedef VkfwVkResult (VKFWAPI_PTR *PFN_vkfwVkCreateWindowSurface)(VkfwVkInstance instance, VkfwWindow window, const VkfwVkAllocationCallbacks* pAllocator, VkfwVkSurfaceKHR* pSurface);


#ifndef VKFW_NO_PROTOTYPES
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateGlobalProperties(VkfwGlobalProperties* pProperties);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwCreateInstance(const VkfwInstanceCreateInfo* pCreateInfo, const VkfwAllocationCallbacks* pAllocator, VkfwInstance* pInstance);
VKFWAPI_ATTR void         VKFWAPI_CALL vkfwDestroyInstance(VkfwInstance instance, const VkfwAllocationCallbacks* pAllocator);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateInstanceProperties(VkfwInstance instance, VkfwInstanceProperties* pProperties);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwGetKeyScancode(VkfwInstance instance, VkfwKey key, int32_t* pScancode);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwGetKeyName(VkfwInstance instance, VkfwKey key, int32_t scancode, const char** pKeyName);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwPostEmptyEvent(VkfwInstance instance);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwProcessEvents(VkfwInstance instance, double timeout, VkfwBool32 waitIndefinitely);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwGetClipboardString(VkfwInstance instance, const char** pClipboardString);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetClipboardString(VkfwInstance instance, const char* clipboardString);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwGetTimerValue(VkfwInstance instance, uint64_t* pTimerValue);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateMonitors(VkfwInstance instance, uint32_t* pMonitorCount, VkfwMonitor* pMonitors);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateMonitorProperties(VkfwMonitor monitor, VkfwMonitorProperties* pProperties);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetMonitorUserPointer(VkfwMonitor monitor, void* pUserPointer);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetMonitorGammaRamp(VkfwMonitor monitor, const VkfwGammaRamp* pGammaRamp);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwGetGammeRampFromGammaValue(VkfwMonitor monitor, float gamma, VkfwGammaRamp* pGammaRamp);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwCreateWindow(VkfwMonitor monitor, const VkfwWindowCreateInfo* pCreateInfo, const VkfwAllocationCallbacks* pAllocator, VkfwWindow* pWindow);
VKFWAPI_ATTR void         VKFWAPI_CALL vkfwDestroyWindow(VkfwWindow window, const VkfwAllocationCallbacks* pAllocator);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateWindowProperties(VkfwWindow window, VkfwWindowProperties* pProperties);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetWindowState(VkfwWindow window, VkfwWindowState newState);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetWindowIcon(VkfwWindow window, uint32_t imageCount, const VkfwImageData* images);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSwitchWindowMonitor(VkfwWindow window, VkfwMonitor monitor, VkfwVideoMode* requestedVideoMode);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwFocusWindow(VkfwWindow window);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwRequestWindowAttention(VkfwWindow window);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetWindowAspectRatio(VkfwWindow window, int32_t numerator, int32_t denominator);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetWindowSizeLimits(VkfwWindow window, VkfwExtent2D minimum, VkfwExtent2D maximum);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwCreateCursor(VkfwInstance instance, const VkfwCursorCreateInfo* pCreateInfo, const VkfwAllocationCallbacks* pAllocator, VkfwCursor* pCursor);
VKFWAPI_ATTR void         VKFWAPI_CALL vkfwDestroyCursor(VkfwCursor cursor, const VkfwAllocationCallbacks* pAllocator);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetWindowCursor(VkfwWindow window, VkfwCursor cursor);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwUpdateGamepadMappings(VkfwInstance instance, uint32_t mappingCount, const VkfwGamepadMapping* mappings);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateJoysticks(VkfwInstance instance, uint32_t* pJoystickCount, VkfwJoystick* pJoysticks);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwEnumerateJoystickProperties(VkfwJoystick joystick, VkfwJoystickProperties* pProperties);
VKFWAPI_ATTR VkfwResult   VKFWAPI_CALL vkfwSetJoystickUserPointer(VkfwJoystick joystick, void* pUserPointer);
VKFWAPI_ATTR VkfwVkBool32 VKFWAPI_CALL vkfwVkGetPhysicalDevicePresentationSupport(VkfwVkInstance instance, VkfwVkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex);
VKFWAPI_ATTR VkfwVkResult VKFWAPI_CALL vkfwVkCreateWindowSurface(VkfwVkInstance instance, VkfwWindow window, const VkfwVkAllocationCallbacks* pAllocator, VkfwVkSurfaceKHR* pSurface);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _vkfw_h_ */

