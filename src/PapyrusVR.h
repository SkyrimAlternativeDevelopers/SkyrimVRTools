#pragma once
#include <ctime>
#include <algorithm>
#include <functional>
#include <mutex> 
#include <list>

#include "skse64/PapyrusNativeFunctions.h"
#include "skse64/PapyrusEvents.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameReferences.h"
#include "skse64/BSModelDB.h"
#include "skse64/PluginManager.h"
#include "skse64/PluginAPI.h"
#include "skse64/NiNodes.h"
#include "skse64/NiObjects.h"

#include "api/PapyrusVRAPI.h"
#include "api/PapyrusVRTypes.h"
#include "api/OpenVRTypes.h"
#include "api/VRManagerAPI.h"

#include "api/utils/OpenVRUtils.h"
#include "VRManager.h"

enum PoseParam
{
	Position,
	Rotation,
	QRotation
};

namespace PapyrusVR
{
	#pragma region Papyrus Native Functions
	
	/*
		Common Data Structures:
		|		Name		|	ReturnType	|	Indexes Order
		+-------------------+---------------+--------------------
		|	Vector3			|	float[3]	|	(x,y,z)
		+-------------------+---------------+--------------------
		|	Quaternion		|	float[4]	|	(x,y,z,w)
		+-------------------+---------------+--------------------
		|					|				|	( 0, 1, 2,	3 
		|	Matrix34		|	float[12]	|	  4, 5, 6,	7 
		|					|				|	  8, 9, 10, 11 )
		------------------------------------+--------------------
	*/

	// Returns a Vector3 with the device rotation(in degrees) in SteamVR world space
	void GetSteamVRDeviceRotation(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues);

	// Returns a Quaternion with the device rotation in SteamVR world space
	void GetSteamVRDeviceQRotation(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues);

	// Returns a Vector3 with the device position(in meters) in SteamVR world space
	void GetSteamVRDevicePosition(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues);

	// Returns a Vector3 with the device rotation(in degrees) in Skyrim world space
	void GetSkyrimDeviceRotation(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues);

	// Returns a Quaternion with the device rotation in Skyrim world space
	void GetSkyrimDeviceQRotation(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues);

	// Returns a Vector3 with the device position(in meters) in Skyrim world space
	void GetSkyrimDevicePosition(StaticFunctionTag *base, SInt32 deviceEnum, VMArray<float> returnValues);

	// Creates new local collision sphere with parameters (radius, Vector3 position, Quaternion rotation, Attached Device (can be none))
	UInt32 RegisterLocalOverlapSphere(StaticFunctionTag *base, float radius, VMArray<float> position, VMArray<float> rotation, SInt32 deviceEnum);
	void DestroyLocalOverlapObject(StaticFunctionTag *base, UInt32 objectHandle);

	#pragma region Event Registration
		void FormRegisterForEvent(TESForm* object, RegistrationSetHolder<TESForm*>* regHolder);
		void FormRegisterForEvent(TESForm* object, RegistrationSetHolder<TESForm*>* regHolder);

		//Used by papyrus scripts to register for Events
		void RegisterForPoseUpdates(StaticFunctionTag *base, TESForm * thisForm);	
		void UnregisterForPoseUpdates(StaticFunctionTag *base, TESForm * thisForm);
		void RegisterForVRButtonEvents(StaticFunctionTag *base, TESForm * thisForm);
		void UnregisterForVRButtonEvents(StaticFunctionTag *base, TESForm * thisForm);
		void RegisterForVROverlapEvents(StaticFunctionTag *base, TESForm * thisForm);
		void UnregisterForVROverlapEvents(StaticFunctionTag *base, TESForm * thisForm);
		void RegisterForVRHapticEvents(StaticFunctionTag *base, TESForm * thisForm);
		void UnregisterForVRHapticEvents(StaticFunctionTag *base, TESForm * thisForm);
	#pragma endregion

	void TimeSinceLastCall(StaticFunctionTag* base);

	#pragma endregion

	bool RegisterFuncs(VMClassRegistry* registry);

	void OnVRButtonEventReceived(VREventType eventType, EVRButtonId buttonId, VRDevice deviceId);
	void OnVROverlapEventReceived(VROverlapEvent eventType, UInt32 objectHandle, VRDevice deviceId);
	void OnVRHapticEventReceived(UInt32 axisID, UInt32 pulseDuration, VRDevice device);


	#pragma region Utility Methods
	//Generic method to handle all pose requests from Papyrus
	void CopyPoseToVMArray(UInt32 deviceType, VMArray<float>* resultArray, PoseParam parameter, bool skyrimWorldSpace = false);
	#pragma endregion
}