# PapyrusVR
## Current SKSE Target Version: 2.0.10
Initial SKSE Plugin to expose OpenVR to Papyrus

#TODO
- Coordinate transform to Skyrim world space
- Papyrus Methods
- Overlap Events
- Gestures Events
- SteamVR Input
- ScaleformAPI

## OpenVR Version
This branch was created to compile against OpenVR 1.0.16. In order to run the game you must download the correct [openvr_api.dll](https://github.com/ValveSoftware/openvr/raw/6aacebd1246592d9911439d5abd0c657b8948ab0/bin/win64/openvr_api.dll) from Valve's repository and replace the old SkyrimVR's version with it.

This change was made to try and get the SteamVR Input API working and enabling modders to make custom actions sets.

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
   |-- openvr.h (headers for OpenVR 1.0.16)
```

You'll also need static .lib files for openvr and SKSEVR.

### Configuring Project Variables

In order to get the project to build you have to place the following files in *Root\PapyrusVR_Module\libs*:
- openvr_api.lib (OpenVR Version 1.0.16)
- common_vc14.lib    (SKSE common project built as static library)
- skse64_common.lib  (skse64_common project built as static library)
- sksevr_1_4_15.lib  (skse64 project built as static library)


### Configuring SkyrimVR Path

The last step is configuring SkyrimVR path in the User.props file. 
The solution will automatically copy the resulting .dll to the SKSE Plugins folder.

---

## Developing Plugins
You can use PapyrusVR functionalities in your plugin by using the PapyrusVR API.

Download the API headers files from the [releases page](https://github.com/artumino/PapyrusVR/releases) or get them from the current branch under the api folder.

Make sure to add the headers to your include path.

You'll also need the **OpenVR v1.0.16** libs and headers file. (We plan to remove this dependecy further down the road)

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
#include "api/PapyrusVRAPI.h"
#include "api/VRManagerAPI.h"

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

## Papyrus Script Quest Example
```Papyrus
Scriptname MyFancyQuest extends Quest
int property testSphereHandle = 0 auto

Function Init()
    
    Debug.Notification("The mod has been installed")
    Actor player = Game.GetPlayer()
    float[] pose = PapyrusVR.GetSkyrimDevicePosition(0)
    Debug.Notification("Pose X: " + pose[0])
    Debug.Notification("Pose Y: " + pose[1])
    Debug.Notification("Pose Z: " + pose[2])
    Debug.Notification("Player X: " + player.X)
    Debug.Notification("Player Y: " + player.Y)
    Debug.Notification("Player Z: " + player.Z)
    Debug.Notification("Registering for VR Button events!")
    PapyrusVR.RegisterForVRButtonEvents(Game.GetForm(GetFormID()))

    Debug.Notification("Testing collision spheres!")
    PapyrusVR.RegisterForVROverlapEvents(Game.GetForm(GetFormID()))

    ;25cm sphere about 25cm on top of the headset
    float[] position = PapyrusVR.Vector3(0.0, 0.25, 0.0)
    float[] rotation = PapyrusVR.Quaternion_Zero()
    testSphereHandle = PapyrusVR.RegisterLocalOverlapSphere(0.125, position, rotation, 0)
    if testSphereHandle != 0
        Debug.Notification("Registered sphere " + testSphereHandle + " correctly!")
    endIf
    Debug.Notification("The mod has finished initializing")
EndFunction

Event OnVRButtonEvent(int buttonEvent, int buttonId, int deviceId)
    Debug.Notification("Got event from device" + deviceId + " with id " + buttonEvent + " for button " + buttonId)
EndEvent

Event OnVROverlapEvent(int overlapEventType, int objectHandle, int deviceId)
    if(overlapEventType == 1)
        Debug.Notification("Device " + deviceId + " entered local overlap object with handle " + objectHandle)
    else
        Debug.Notification("Device " + deviceId + " exited local overlap object with handle " + objectHandle)
    endIf
EndEvent
```

# SKSE-VRInputPlugin
Skyrim Script Extender Plugin to give mods access to VR controller input data.  Be warned this is currently an untested alpha version!

This plugin will use the API provided by artumino in PapyrusVR to give mod scripters access to VR input but it uses a different method of collecting OpenVR data which I think is more efficient and will be easier to update for SkyrimVR patches, it is also working with the current version of SkyrimVR/SKSE (1.4.15).
I may create a new script API that works more efficiently with this method of getting OpenVR data and also provides extra features as well.

Please see https://github.com/artumino/PapyrusVR for Papyrus script details.     

Thanks to the SKSE team at http://skse.silverlock.org/ and artumino for PapyrusVR at https://github.com/artumino/PapyrusVR for making this plugin possible and inspiring me to write it.  

Also a special thanks to Odie (https://github.com/Odie) for contributing Scaleform integration, and in general for his work on SkyUI-VR. 
