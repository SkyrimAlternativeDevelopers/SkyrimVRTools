#include "OpenVRUtils.h"

namespace PapyrusVR
{
	/*
	Thanks to Omnifinity for this conversion
	https://github.com/Omnifinity/OpenVR-Tracking-Example/blob/master/HTC%20Lighthouse%20Tracking%20Example/LighthouseTracking.cpp
	*/
	// Get the quaternion representing the rotation
	Quaternion OpenVRUtils::GetRotation(Matrix34* steam_vr_matrix) {
		Quaternion q;

		q.w = sqrt(fmax(0, 1 + steam_vr_matrix->m[0][0] + steam_vr_matrix->m[1][1] + steam_vr_matrix->m[2][2])) / 2;
		q.x = sqrt(fmax(0, 1 + steam_vr_matrix->m[0][0] - steam_vr_matrix->m[1][1] - steam_vr_matrix->m[2][2])) / 2;
		q.y = sqrt(fmax(0, 1 - steam_vr_matrix->m[0][0] + steam_vr_matrix->m[1][1] - steam_vr_matrix->m[2][2])) / 2;
		q.z = sqrt(fmax(0, 1 - steam_vr_matrix->m[0][0] - steam_vr_matrix->m[1][1] + steam_vr_matrix->m[2][2])) / 2;
		q.x = copysign(q.x, steam_vr_matrix->m[2][1] - steam_vr_matrix->m[1][2]);
		q.y = copysign(q.y, steam_vr_matrix->m[0][2] - steam_vr_matrix->m[2][0]);
		q.z = copysign(q.z, steam_vr_matrix->m[1][0] - steam_vr_matrix->m[0][1]);
		return q;
	}

	// Get the vector representing the position
	Vector3 OpenVRUtils::GetPosition(Matrix34* steam_vr_matrix) {
		Vector3 vector;

		vector.x = steam_vr_matrix->m[0][3];
		vector.y = steam_vr_matrix->m[1][3];
		vector.z = steam_vr_matrix->m[2][3];

		return vector;
	}

	Vector3 OpenVRUtils::QuatToEuler(const Quaternion* quat)
	{
		Vector3 result;
		double sqw;
		double sqx;
		double sqy;
		double sqz;

		double rotxrad;
		double rotyrad;
		double rotzrad;

		sqw = quat->w * quat->w;
		sqx = quat->x * quat->x;
		sqy = quat->y * quat->y;
		sqz = quat->z * quat->z;

		rotxrad = (double)atan2l(2.0 * (quat->y * quat->z + quat->x * quat->w), (-sqx - sqy + sqz + sqw));
		rotyrad = (double)asinl(-2.0 * (quat->x * quat->z - quat->y * quat->w));
		rotzrad = (double)atan2l(2.0 * (quat->x * quat->y + quat->z * quat->w), (sqx - sqy - sqz + sqw));

		result.x = OpenVRUtils::Rad2Deg(rotxrad);
		result.y = OpenVRUtils::Rad2Deg(rotyrad);
		result.z = OpenVRUtils::Rad2Deg(rotzrad);

		return result;
	}

	//Matrix Operations
	Matrix34 OpenVRUtils::CreateTranslationMatrix(Vector3* translation)
	{
		Matrix34 result;

		return result;
	}

	Matrix34 OpenVRUtils::CreateScaleMatrix(Vector3* scale)
	{
		Matrix34 result;

		return result;
	}

	Matrix34 OpenVRUtils::CreateRotationMatrix(Quaternion* quaternion)
	{
		Matrix34 result;

		return result;
	}

	Matrix34 OpenVRUtils::CreateRotationMatrix(Vector3* euler)
	{
		Matrix34 result;

		return result;
	}

	//Conversions
	void OpenVRUtils::CopyQuaternionToVMArray(Quaternion* quaternion, VMArray<float>* arr)
	{
		float value;
		//Check Size
		if (quaternion && arr && arr->arr && arr->Length() == 4)
		{
			value = (float)quaternion->x;
			arr->Set(&value, 0);

			value = (float)quaternion->y;
			arr->Set(&value, 1);

			value = (float)quaternion->z;
			arr->Set(&value, 2);

			value = (float)quaternion->w;
			arr->Set(&value, 3);
		}
	}

	void OpenVRUtils::CopyVector3ToVMArray(Vector3* vector, VMArray<float>* arr)
	{
		//Check Size
		if (vector && arr && arr->arr && arr->Length() == 3)
		{
			arr->Set(&(vector->x), 0);
			arr->Set(&(vector->y), 1);
			arr->Set(&(vector->z), 2);
		}
	}
}
