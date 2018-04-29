#pragma once
#include "../PapyrusVRTypes.h"
#include "skse64/PapyrusNativeFunctions.h"

namespace PapyrusVR
{
	class OpenVRUtils
	{
	public:
		static Quaternion GetRotation(Matrix34* steam_vr_matrix);
		static Vector3 GetPosition(Matrix34* steam_vr_matrix);
		static Vector3 QuatToEuler(const Quaternion* quat);

		//Matrix Operations
		static Matrix34 CreateTransformMatrix(Vector3* translation, Vector3* euler);
		static Matrix34 CreateTransformMatrix(Vector3* translation, Quaternion* quaternion);
		static Matrix34 CreateTranslationMatrix(Vector3* translation);
		static Matrix34 CreateScaleMatrix(Vector3* scale);
		static Matrix34 CreateRotationMatrix(Quaternion* quaternion);
		static Matrix34 CreateRotationMatrix(Vector3* euler);
		static void SetRotationMatrix(Matrix34* matrix, Vector3* euler);
		static void SetRotationMatrix(Matrix34* matrix, Quaternion* quaternion);
		static void SetTranslationMatrix(Matrix34* matrix, Vector3* translation);
		static void SetScaleMatrix(Matrix34* matrix, Vector3* scale);

		//Conversions
		static void CopyQuaternionToVMArray(Quaternion* quaternion, VMArray<float>* arr);
		static void CopyVector3ToVMArray(Vector3* vector, VMArray<float>* arr);
		static void CopyVMArrayToQuaternion(VMArray<float>* arr, Quaternion* quaternion);
		static void CopyVMArrayToVector3(VMArray<float>* arr, Vector3* vector);

		//In-Line Constants
		static double Rad2Deg(double radiants) { return radiants * 57.2957795131; }
		static double Deg2Rad(double degrees) { return degrees / 57.2957795131; }
	};
}