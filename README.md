# PapyrusVR
Initial SKSE Plugin to expose OpenVR to Papyrus

#TODO
- Coordinate transform to Skyrim world space
- Papyrus Methods
- Overlap Events
- Gestures Events

## Solution Setup
### File Structure
In order to build this plugin you must have the following folder structure:

```
- Root 
|-- *PapyrusVR* (this repo)
|  |--PapyrusVR_Module
|    |--libs <------ Place .lib files used for both build types here
|	|-- Release 	<---- .lib file used for Release builds
|	|-- Debug	<---- .lib files used for Debug build
|  ...
|-- skse
|  |-- common
|  |-- sksevr
|   ....
|-- openvr
   |-- openvr.h (headers for OpenVR 1.0.10, version currently used by SkyrimVR v1.3.64)
```

You'll also need static .lib files for openvr and SKSEVR.

### Configuring Project Variables

In order to get the project to build you have to place the following files in *Root\PapyrusVR_Module\libs*:
- openvr_api.lib (OpenVR Version 1.0.10)
- common_vc14.lib    (SKSE common project built as static library)
- skse64_common.lib  (skse64_common project built as static library)
- sksevr_1_3_64.lib  (skse64 project built as static library)


### Configuring SkyrimVR Path

The last step is configuring SkyrimVR path in the User.props file. 
The solution will automatically copy the resulting .dll to the SKSE Plugins folder.

---

## Developing Plugins
You can use PapyrusVR functionalities in your plugin by using the PapyrusVR API.

Download the API headers files from the [releases page](https://github.com/artumino/PapyrusVR/releases) or get them from the current branch under the api folder.

Make sure to add the headers to your include path.

You'll also need the OpenVR v1.0.10 libs and headers file. (We plan to remove this dependecy further down the road)

The PapyrusVR API currently gives you access to the VRManager and to the pose event dispatcher.

During your plugin load register to the Messaging interface and listen for the kMessage_PostLoad coming from SKSE.
Once you recive this message you can use the messaging interface to listen for messages coming from PapyrusVR.

PapyrusVR will send a message on Init with every address needed to use the API correctly as payload.

Here is an example on how to use the API:
```C++
#include <shlobj.h>				// CSIDL_MYCODUMENTS

#include "skse64/PluginAPI.h"		// super
#include "skse64_common/skse_version.h"	// What version of SKSE is running?
#include "common/IDebugLog.h"

//Headers under api/ folder
#include "PapyrusVR_Module\PapyrusVR_Module\api\PapyrusVRAPI.h"
#include "PapyrusVR_Module\PapyrusVR_Module\api\VRManagerAPI.h"

static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
static SKSEMessagingInterface		* g_messaging = NULL;

using namespace std;

PapyrusVRAPI* g_papyrusvr;
extern "C" {

	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info) {	// Called by SKSE to learn about this plugin and check that it's safe to load it
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim VR\\SKSE\\MyFancyPlugin.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("MyFancyPlugin");

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = "MyFancyPlugin";
		info->version = 1;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();

		if (skse->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");

			return false;
		}
		else if (skse->runtimeVersion != RUNTIME_VR_VERSION_1_3_64)
		{
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);

			return false;
		}

		// ### do not do anything else in this callback
		// ### only fill out PluginInfo and return true/false

		// supported runtime version
		return true;
	}

	int frameCount = 0;
	//Called on each update (about 90-100 calls per second)
	void OnPoseUpdate(float DeltaTime)
	{
		//You can access VRManagerAPI like this, to get poses etc...
		if (g_papyrusvr->GetVRManager())
		{
			if ((frameCount %= 90) == 0)
				_MESSAGE("Recived 90 callbacks");
		}
		frameCount++;
	}

	//Listener for PapyrusVR Messages
	void OnPapyrusVRMessage(SKSEMessagingInterface::Message* msg)
	{
		if (msg)
		{
			if (msg->type == kPapyrusVR_Message_Init && msg->data)
			{
				_MESSAGE("PapyrusVR Init Message recived with valid data, registering for pose update callback");
				g_papyrusvr = (PapyrusVRAPI*)msg->data;

				//Registers for PoseUpdates
				g_papyrusvr->RegisterPoseUpdateListener(OnPoseUpdate);
			}
		}
	}

	//Listener for SKSE Messages
	void OnSKSEMessage(SKSEMessagingInterface::Message* msg)
	{
		if (msg)
		{
			if (msg->type == SKSEMessagingInterface::kMessage_PostLoad)
			{
				_MESSAGE("SKSE PostLoad recived, registering for PapyrusVR messages");
				g_messaging->RegisterListener(g_pluginHandle, "PapyrusVR", OnPapyrusVRMessage);
			}
		}
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse) {	// Called by SKSE to load this plugin
		_MESSAGE("MyFancyPlugin loaded");

		//Registers for SKSE Messages (PapyrusVR probably still need to load, wait for SKSE message PostLoad)
		_MESSAGE("Registering for SKSE messages");
		g_messaging = (SKSEMessagingInterface*)skse->QueryInterface(kInterface_Messaging);
		g_messaging->RegisterListener(g_pluginHandle, "SKSE", OnSKSEMessage);

		//DO STUFF...
		//wait for PapyrusVR init (during PostPostLoad SKSE Message)

		return true;
	}

};
```

WARNING: There's currently an export issue with VRManager and using it in a solution will cause link errors
