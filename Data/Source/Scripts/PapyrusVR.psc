ScriptName PapyrusVR Hidden

Function GetTrackedDevicePoseByIDNative(int DeviceID, float[] returnValues) global native
Function RegisterForPoseUpdates(Form form) global native
Function UnregisterForPoseUpdates(Form form) global native

float[] Function GetTrackedDevicePoseByID(int DeviceID) global
    float[] returnValues = new float[7]
    GetTrackedDevicePoseByIDNative(DeviceID, returnValues)
    return returnValues
EndFunction