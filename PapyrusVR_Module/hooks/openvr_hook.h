#pragma once
#include "openvr.h"
#include "IHookInterfaceFactory.h"

/*
	Thanks to lfrazer (https://github.com/lfrazer) for the hook idea,
	This is a rewrite in order to keep the compatibility between the two without affecting the license
*/

static void* (*OpenVR_VR_GetGenericInterface)(const char *pchInterfaceVersion, vr::EVRInitError *peError) = nullptr;
static std::map<std::string, IHookInterfaceFactory*> interfaceTranslationMap;
bool DoHook();