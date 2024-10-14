#ifndef MAKE_SHIM
#define MAKE_SHIM(x,y)
#define __MAKE_SHIM_UNDEF
#endif

MAKE_SHIM(XInputGetState, 1)
MAKE_SHIM(XInputSetState, 2)
MAKE_SHIM(XInputGetCapabilities, 3)
MAKE_SHIM(XInputEnable, 4)
MAKE_SHIM(XInputGetDSoundAudioDeviceGuids, 5)
MAKE_SHIM(XInputGetBatteryInformation, 7)
MAKE_SHIM(XInputGetKeystroke, 8)

#ifdef __MAKE_SHIM_UNDEF
#undef __MAKE_SHIM_UNDEF
#undef MAKE_SHIM
#endif