#pragma once
#include "openvr.h"
#include "IHookInterfaceFactory.h"

static void* (*OpenVR_VR_GetGenericInterface)(const char *pchInterfaceVersion, vr::EVRInitError *peError) = nullptr;
static std::map<std::string, IHookInterfaceFactory*> interfaceTranslationMap;
bool DoHook();