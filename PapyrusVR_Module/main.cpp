#include "skse64/PluginAPI.h"		// super
#include "skse64_common/skse_version.h"	// What version of SKSE is running?
#include "common/IDebugLog.h"
#include <shlobj.h>				// CSIDL_MYCODUMENTS

#include "PapyrusVR.h"
#include "VRManager.h"

static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
static SKSEPapyrusInterface         * g_papyrus = NULL;

extern "C" {
	
	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info) {	// Called by SKSE to learn about this plugin and check that it's safe to load it
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim VR\\SKSE\\PapyrusVR.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		//Test for enabling /GS security checks
		__security_init_cookie();

		_MESSAGE("PapyrusVR");

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "PapyrusVR";
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
		_MESSAGE("PapyrusVR loaded");

		g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);

		//Updates pointer
		_MESSAGE("Current register plugin function at memory address: %p", PapyrusVR::RegisterForPoseUpdates);

		//Debug
		//__debugbreak();

		//Check if the function registration was a success...
		bool btest = g_papyrus->Register(PapyrusVR::RegisterFuncs);

		if (btest) {
			_MESSAGE("Register Succeeded");
		}

		return true;
	}

};