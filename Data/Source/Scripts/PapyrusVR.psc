ScriptName PapyrusVR Hidden

int property EDeviceTypeHMD = 0 autoReadOnly
int property EDeviceTypeRightController = 1 autoReadOnly
int property EDeviceTypeLeftController = 2 autoReadOnly

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