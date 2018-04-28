#pragma once
#include "openvr.h"
#include "skse64/PapyrusNativeFunctions.h"

namespace PapyrusVR
{
	class OpenVRUtils
	{
	public:
		static vr::HmdQuaternion_t GetRotation(vr::HmdMatrix34_t* steam_vr_matrix);
		static vr::HmdVector3_t GetPosition(vr::HmdMatrix34_t* steam_vr_matrix);
		static vr::HmdVector3_t OpenVRUtils::QuatToEuler(const vr::HmdQuaternion_t* quat);

		//Matrix Operations
		static vr::HmdMatrix34_t CreateTranslationMatrix(vr::HmdVector3_t* translation);
		static vr::HmdMatrix34_t CreateScaleMatrix(vr::HmdVector3_t* scale);
		static vr::HmdMatrix34_t CreateRotationMatrix(vr::HmdQuaternion_t* quaternion);
		static vr::HmdMatrix34_t CreateRotationMatrix(vr::HmdVector3_t* euler);

		//Conversions
		static void CopyQuaternionToVMArray(vr::HmdQuaternion_t* quaternion, VMArray<float>* arr);
		static void CopyVector3ToVMArray(vr::HmdVector3_t* vector, VMArray<float>* arr);

		//In-Line Constants
		static double OpenVRUtils::Rad2Deg(double radiants) { return radiants * 57.2957795131; }
		static double OpenVRUtils::Deg2Rad(double degrees) { return degrees / 57.2957795131; }
	};
}