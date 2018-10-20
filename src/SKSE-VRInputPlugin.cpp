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

#include "skse64/PluginAPI.h"		// SKSE plugin API
#include "skse64_common/skse_version.h"
#include "openvrhooks.h"

#include "common/IDebugLog.h"
#include <shlobj.h>				// for use of CSIDL_MYCODUMENTS
#include "papyrusvr_api/PapyrusVR.h"
#include "ScaleformVR.h"

static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
static SKSEPapyrusInterface         * g_papyrus = NULL;
static SKSEScaleformInterface       * g_scaleform = NULL;

extern "C" {

	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info) {	// Called by SKSE to learn about this plugin and check that it's safe to load it
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim VR\\SKSE\\SKSE-VRInputPlugin.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "SKSE-VRInputPlugin";
		info->version = 1;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();
		PapyrusVR::RegisterHandle(&g_pluginHandle);

		//Registers for messages
		PapyrusVR::RegisterMessagingInterface((SKSEMessagingInterface *)skse->QueryInterface(kInterface_Messaging));

		if (skse->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");

			return false;
		}
		else if (skse->runtimeVersion != RUNTIME_VR_VERSION_1_4_15)
		{
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);

			return false;
		}

		// ### do not do anything else in this callback
		// ### only fill out PluginInfo and return true/false

		// supported runtime version
		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse) {	// Called by SKSE to load this plugin
		_MESSAGE("SKSE-VRInputPlugin loaded");

		g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);
		g_scaleform = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);

		//Updates pointer
		_MESSAGE("Current register plugin function at memory address: %p", PapyrusVR::RegisterForPoseUpdates);

		//Check if the function registration was a success...
		bool btest = g_papyrus->Register(PapyrusVR::RegisterFuncs);
		btest &= g_scaleform->Register("vrinput", ScaleformVR::RegisterFuncs);

		if (btest) {
			_MESSAGE("Register Succeeded");
		}


		DoOpenVRHook();


		return true;
	}

	void VRInput_RegisterControllerStateCB(GetControllerState_CB cbfunc)
	{
		OpenVRHookMgr::GetInstance()->RegisterControllerStateCB(cbfunc);
	}
	void VRInput_RegisterGetPosesCB(WaitGetPoses_CB cbfunc)
	{
		OpenVRHookMgr::GetInstance()->RegisterGetPosesCB(cbfunc);
	}
	void VRInput_UnregisterControllerStateCB(GetControllerState_CB cbfunc)
	{
		OpenVRHookMgr::GetInstance()->UnregisterControllerStateCB(cbfunc);
	}
	void VRInput_UnregisterGetPosesCB(WaitGetPoses_CB cbfunc)
	{
		OpenVRHookMgr::GetInstance()->UnregisterGetPosesCB(cbfunc);
	}

};