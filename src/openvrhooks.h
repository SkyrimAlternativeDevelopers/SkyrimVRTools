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
#include "openvr.h"

typedef void * (*VR_GetGenericInterfaceFunc)(const char *pchInterfaceVersion, vr::EVRInitError *peError);


bool DoOpenVRHook();

bool getControllerStateUpdateShutoff();
void setControllerStateUpdateShutoff(bool enable);

class FakeVRSystem;
class FakeVRCompositor;

// OpenVR hook manager
class OpenVRHookMgr
{
public:
	static OpenVRHookMgr* GetInstance();

	void SetVRSystem(vr::IVRSystem* vrSystem)
	{
		mVRSystem = vrSystem;
	}

	void SetFakeVRSystem(FakeVRSystem* vrSystem)
	{
		mFakeVRSystem = vrSystem;
	}

	void SetVRCompositor(vr::IVRCompositor* vrCompositor)
	{
		mVRCompositor = vrCompositor;
	}

	void SetFakeVRCompositor(FakeVRCompositor* vrCompositor)
	{
		mFakeVRCompositor = vrCompositor;
	}
	
	vr::IVRSystem* GetVRSystem() const
	{
		return mVRSystem;
	}

	FakeVRSystem* GetFakeVRSystem() const
	{
		return mFakeVRSystem;
	}
	
	vr::IVRCompositor* GetVRCompositor() const
	{
		return mVRCompositor;
	}

	FakeVRCompositor* GetFakeVRCompositor() const
	{
		return mFakeVRCompositor;
	}

	bool IsInitialized() const
	{
		return mVRCompositor != nullptr && mVRSystem != nullptr;
	}

	bool IsInputProcessingReady() const
	{
		return mInputProcessingReady;
	}

	void SetInputProcessingReady(bool readyflag)
	{
		mInputProcessingReady = readyflag;
	}
	
private:
	vr::IVRSystem* mVRSystem = nullptr;
	vr::IVRCompositor* mVRCompositor = nullptr;
	FakeVRSystem* mFakeVRSystem = nullptr;
	FakeVRCompositor* mFakeVRCompositor = nullptr;
	bool mInputProcessingReady = false;

	static OpenVRHookMgr* sInstance;
};


