#pragma once
/*
	SKSE-VRInputPlugin - VR Input plugin for mods of Skyrim VR
	Copyright (C) 2018 L Frazer
	https://github.com/lfrazer

	This file is part of SKSE-VRInputPlugin.

	SKSE-VRInputPlugin is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	SKSE-VRInputPlugin is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with SKSE-VRInputPlugin.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <windows.h>
#include <inttypes.h>
#include <unordered_set>
#include "../api/VRHookAPI.h"
#include "IHookInterfaceFactory.h"
#include "HookVRSystem.h"
#include "HookVRCompositor.h"
#include "openvr.h"

typedef void * (*VR_GetGenericInterfaceFunc)(const char *pchInterfaceVersion, vr::EVRInitError *peError);
static std::map<std::string, IHookInterfaceFactory*> interfaceTranslationMap;

bool DoOpenVRHook();

//TODO: Rename all namespaces to something else other than PapyrusVR
//TODO: Create VRInputMgr
//TODO: Move to VRInputMgr
bool getControllerStateUpdateShutoff();
void setControllerStateUpdateShutoff(bool enable);


/* OpenVR hook manager
*  OpenVRHookMgr offers direct hooks into RAW OpenVR calls to other modules
*/
class OpenVRHookMgr : OpenVRHookManagerAPI
{
friend class HookVRSystem;
friend class HookVRCompositor;

public:
	static OpenVRHookMgr* GetInstance();

	void SetVRSystem(vr::IVRSystem* vrSystem)
	{
		mVRSystem = vrSystem;
	}

	void SetFakeVRSystem(HookVRSystem* vrSystem)
	{
		mFakeVRSystem = vrSystem;
	}

	void SetVRCompositor(vr::IVRCompositor* vrCompositor)
	{
		mVRCompositor = vrCompositor;
	}

	void SetFakeVRCompositor(HookVRCompositor* vrCompositor)
	{
		mFakeVRCompositor = vrCompositor;
	}
	
	vr::IVRSystem* GetVRSystem() const
	{
		return mVRSystem;
	}

	HookVRSystem* GetFakeVRSystem() const
	{
		return mFakeVRSystem;
	}
	
	vr::IVRCompositor* GetVRCompositor() const
	{
		return mVRCompositor;
	}

	HookVRCompositor* GetFakeVRCompositor() const
	{
		return mFakeVRCompositor;
	}

	bool IsInitialized();
	void RegisterControllerStateCB(GetControllerState_CB cbfunc);
	void RegisterGetPosesCB(WaitGetPoses_CB cbfunc);
	void UnregisterControllerStateCB(GetControllerState_CB cbfunc);
	void UnregisterGetPosesCB(WaitGetPoses_CB cbfunc);
	
private:
	vr::IVRSystem* mVRSystem = nullptr;
	vr::IVRCompositor* mVRCompositor = nullptr;
	HookVRSystem* mFakeVRSystem = nullptr;
	HookVRCompositor* mFakeVRCompositor = nullptr;

	std::unordered_set<GetControllerState_CB> mGetControllerStateCallbacks;
	std::unordered_set<WaitGetPoses_CB>	mWaitGetPosesCallbacks;

	static OpenVRHookMgr* sInstance;
};


