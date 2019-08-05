#include "skse64/PluginAPI.h"		// super
#include "skse64_common/skse_version.h"	// What version of SKSE is running?
#include "common/IDebugLog.h"
#include <shlobj.h>				// CSIDL_MYCODUMENTS

#include "PapyrusVR.h"
#include "ScaleformVR.h"
#include "VRManager.h"
#include "api/VRManagerAPI.h"
#include "api/VRHookAPI.h"
#include "hooks/openvr_hook.h"
#include "api/utils/OpenVRUtils.h"

static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
static SKSEPapyrusInterface         * g_papyrus = NULL;
static SKSEScaleformInterface       * g_scaleform = NULL;
static SKSEMessagingInterface		* g_messagingInterface = NULL;

//API
static PapyrusVRAPI apiMessage;


// Func prototype
void OnSKSEMessageReceived(SKSEMessagingInterface::Message* message);


#pragma region API

//Returns the VRManager singleton instance
PapyrusVR::VRManagerAPI* GetVRManager()
{
	return &PapyrusVR::VRManager::GetInstance();
}


OpenVRHookManagerAPI* GetVRHookManagerAPI()
{
	return (OpenVRHookManagerAPI*)OpenVRHookMgr::GetInstance();
}

#pragma endregion


extern "C" 
{
	
	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info) {	// Called by SKSE to learn about this plugin and check that it's safe to load it
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim VR\\SKSE\\SkyrimVRTools.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		//Test for enabling /GS security checks
		__security_init_cookie();

		_MESSAGE("SkyrimVRTools");

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "SkyrimVRTools";
		info->version = 3;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();

		//Registers for messages
		g_messagingInterface = (SKSEMessagingInterface *)skse->QueryInterface(kInterface_Messaging);
		if (g_messagingInterface && g_pluginHandle)
		{
			_MESSAGE("Registering for plugin loaded message!");
			g_messagingInterface->RegisterListener(g_pluginHandle, "SKSE", OnSKSEMessageReceived);
		}

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

	//TODO: Create proper namespaces for everything
	bool SKSEPlugin_Load(const SKSEInterface * skse) {	// Called by SKSE to load this plugin
		_MESSAGE("SkyrimVRTools loaded");

		g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);
		g_scaleform = (SKSEScaleformInterface *)skse->QueryInterface(kInterface_Scaleform);

		//Updates pointer
		_MESSAGE("Current register plugin function at memory address: %p", PapyrusVR::RegisterForPoseUpdates);

		//Debug
		//__debugbreak();

		//Papyrus
		_MESSAGE("Registering Papyrus native functions...");
		bool btest = g_papyrus->Register(PapyrusVR::RegisterFuncs);
		if (btest) {
			_MESSAGE("Papyrus Functions Register Succeeded");
		}

		//Scaleform
		btest &= g_scaleform->Register("vrinput", ScaleformVR::RegisterFuncs);
		if (btest) {
			_MESSAGE("Scaleform Functions Register Succeeded");
		}

		//OpenVR Hook
		_MESSAGE("Hooking into OpenVR calls");
		if (!DoOpenVRHook())
		{
			_MESSAGE("Failed to hook to OpenVR...");
			btest = false;
		}

		return btest;
	}

	//Returns the OpenVRHookMgr singleton instance (lets give the user full control here)
	OpenVRHookMgr* GetVRHookManager()
	{
		return OpenVRHookMgr::GetInstance();
	}



};



#pragma region Messaging Interface

// Listens for SKSE events
void OnSKSEMessageReceived(SKSEMessagingInterface::Message* message)
{
	if (message)
	{
		_MESSAGE("Received SKSE message %d", message->type);
		if (message->type == SKSEMessagingInterface::kMessage_PostPostLoad)
		{
			if (g_messagingInterface && g_pluginHandle)
			{
				_MESSAGE("Game Loaded, Dispatching Init messages to all listeners");
				apiMessage.GetVRManager = GetVRManager;
				apiMessage.GetOpenVRHook = GetVRHookManagerAPI;
				//apiMessage.RegisterPoseUpdateListener = GetVRManager()->RegisterVRUpdateListener;

				//Sends pointers to API functions/classes
				g_messagingInterface->Dispatch(g_pluginHandle, kPapyrusVR_Message_Init, &apiMessage, sizeof(apiMessage), NULL);
			}
		}

		if (message->type == SKSEMessagingInterface::kMessage_DataLoaded)
		{
			//Register manifest file
			PapyrusVR::VRManager::GetInstance().RegisterInputActions();
		}

		if (message->type == SKSEMessagingInterface::kMessage_PostLoadGame)
		{
			//Get player nodes, etc.
			PapyrusVR::OpenVRUtils::SetupConversion();
		}
	}
}


#pragma endregion

