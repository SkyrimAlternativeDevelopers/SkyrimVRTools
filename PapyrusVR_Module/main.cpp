#include "skse64/PluginAPI.h"		// super
#include "skse64_common/skse_version.h"	// What version of SKSE is running?
#include "common/IDebugLog.h"
#include <shlobj.h>				// CSIDL_MYCODUMENTS

#include "PapyrusVR.h"
#include "VRManager.h"
#include "api/VRManagerAPI.h"
#include "hooks/openvr_hook.h"
#include "api/utils/OpenVRUtils.h"
#include "kinematrix.h"

static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
static SKSEPapyrusInterface         * g_papyrus = NULL;
static SKSEMessagingInterface*		g_messagingInterface;

//API
static PapyrusVRAPI apiMessage;




#pragma region API

//Returns the VRManager singleton instance
PapyrusVR::VRManagerAPI* GetVRManager()
{
	return &PapyrusVR::VRManager::GetInstance();
}
#pragma endregion

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
			Kinematrix::GameLoaded();
		}
	}
}


#pragma endregion

extern "C" 
{
	
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

	bool SKSEPlugin_Load(const SKSEInterface * skse) {	// Called by SKSE to load this plugin
		_MESSAGE("PapyrusVR loaded");

		g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);

		//Updates pointer
		_MESSAGE("Current register plugin function at memory address: %p", PapyrusVR::RegisterForPoseUpdates);

		//Debug
		//__debugbreak();

		//Check if the function registration was a success...
		_MESSAGE("Registering Papyrus native functions...");
		bool btest = g_papyrus->Register(PapyrusVR::RegisterFuncs);
		if (btest) {
			_MESSAGE("Papyrus Functions Register Succeeded");
		}

		_MESSAGE("Hooking into OpenVR calls");
		if (!DoHook())
		{
			_MESSAGE("Failed to hook to OpenVR...");
			btest = false;
		}

		return btest;
	}

};