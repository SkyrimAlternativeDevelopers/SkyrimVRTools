ScriptName PapyrusVR Hidden

;DeviceTypes
int property EDeviceTypeHMD                 = 0 autoReadOnly
int property EDeviceTypeRightController     = 1 autoReadOnly
int property EDeviceTypeLeftController      = 2 autoReadOnly

;EventTypes
int property Touched                        = 0 autoReadOnly
int property Untouched                      = 1 autoReadOnly
int property Pressed                        = 2 autoReadOnly
int property Released                       = 3 autoReadOnly

;SteamVR Buttons
int property k_EButton_System			    = 0 autoReadOnly
int property k_EButton_ApplicationMenu	    = 1 autoReadOnly
int property k_EButton_Grip				    = 2 autoReadOnly
int property k_EButton_DPad_Left			= 3 autoReadOnly
int property k_EButton_DPad_Up			    = 4 autoReadOnly
int property k_EButton_DPad_Right		    = 5 autoReadOnly
int property k_EButton_DPad_Down			= 6 autoReadOnly
int property k_EButton_A					= 7 autoReadOnly
int property k_EButton_ProximitySensor      = 31 autoReadOnly
int property k_EButton_Axis0				= 32 autoReadOnly
int property k_EButton_Axis1				= 33 autoReadOnly
int property k_EButton_Axis2				= 34 autoReadOnly
int property k_EButton_Axis3				= 35 autoReadOnly
int property k_EButton_Axis4				= 36 autoReadOnly
int property k_EButton_SteamVR_Touchpad	    = 32 autoReadOnly
int property k_EButton_SteamVR_Trigger	    = 33 autoReadOnly
int property k_EButton_Dashboard_Back	    = 2 autoReadOnly
int property k_EButton_Max				    = 64 autoReadOnly

;Debug Functions
Function TimeSinceLastCall() global native

;SteamVR Coordinates
Function GetSteamVRDeviceRotation_Native(int DeviceType, float[] returnValues) global native
Function GetSteamVRDeviceQRotation_Native(int DeviceType, float[] returnValues) global native
Function GetSteamVRDevicePosition_Native(int DeviceType, float[] returnValues) global native

;Skyrim Coordinates
Function GetSkyrimDeviceRotation_Native(int DeviceType, float[] returnValues) global native
Function GetSkyrimDeviceQRotation_Native(int DeviceType, float[] returnValues) global native
Function GetSkyrimDevicePosition_Native(int DeviceType, float[] returnValues) global native

;Events Registration
Function RegisterForPoseUpdates(Form form) global native
Function UnregisterForPoseUpdates(Form form) global native
Function RegisterForVRButtonEvents(Form form) global native
Function UnregisterForVRButtonEvents(Form form) global native

;Papyrus Native Functions Wrappers
float[] Function GetSteamVRDeviceRotation(int DeviceType) global
    float[] returnValues = new float[3]
    GetSteamVRDeviceRotation_Native(DeviceType, returnValues)
    return returnValues
EndFunction

float[] Function GetSteamVRDeviceQRotation(int DeviceType) global
    float[] returnValues = new float[4]
    GetSteamVRDeviceQRotation_Native(DeviceType, returnValues)
    return returnValues
EndFunction

float[] Function GetSteamVRDevicePosition(int DeviceType) global
    float[] returnValues = new float[3]
    GetSteamVRDevicePosition_Native(DeviceType, returnValues)
    return returnValues
EndFunction

float[] Function GetSkyrimDeviceRotation(int DeviceType) global
    float[] returnValues = new float[3]
    GetSkyrimDeviceRotation_Native(DeviceType, returnValues)
    return returnValues
EndFunction

float[] Function GetSkyrimDeviceQRotation(int DeviceType) global
    float[] returnValues = new float[4]
    GetSkyrimDeviceQRotation_Native(DeviceType, returnValues)
    return returnValues
EndFunction

float[] Function GetSkyrimDevicePosition(int DeviceType) global
    float[] returnValues = new float[3]
    GetSkyrimDevicePosition_Native(DeviceType, returnValues)
    return returnValues
EndFunction

;Events
Event OnVRButtonEvent(int eventType, int buttonId)
EndEvent