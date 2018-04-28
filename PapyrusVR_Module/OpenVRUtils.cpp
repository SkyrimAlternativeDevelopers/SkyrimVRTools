#include "OpenVRUtils.h"

namespace PapyrusVR
{
	/*
	Thanks to Omnifinity for this conversion
	https://github.com/Omnifinity/OpenVR-Tracking-Example/blob/master/HTC%20Lighthouse%20Tracking%20Example/LighthouseTracking.cpp
	*/
	// Get the quaternion representing the rotation
	vr::HmdQuaternion_t OpenVRUtils::GetRotation(vr::HmdMatrix34_t* matrix) {
		vr::HmdQuaternion_t q;

		q.w = sqrt(fmax(0, 1 + matrix->m[0][0] + matrix->m[1][1] + matrix->m[2][2])) / 2;
		q.x = sqrt(fmax(0, 1 + matrix->m[0][0] - matrix->m[1][1] - matrix->m[2][2])) / 2;
		q.y = sqrt(fmax(0, 1 - matrix->m[0][0] + matrix->m[1][1] - matrix->m[2][2])) / 2;
		q.z = sqrt(fmax(0, 1 - matrix->m[0][0] - matrix->m[1][1] + matrix->m[2][2])) / 2;
		q.x = copysign(q.x, matrix->m[2][1] - matrix->m[1][2]);
		q.y = copysign(q.y, matrix->m[0][2] - matrix->m[2][0]);
		q.z = copysign(q.z, matrix->m[1][0] - matrix->m[0][1]);
		return q;
	}

	// Get the vector representing the position
	vr::HmdVector3_t OpenVRUtils::GetPosition(vr::HmdMatrix34_t* matrix) {
		vr::HmdVector3_t vector;

		vector.v[0] = matrix->m[0][3];
		vector.v[1] = matrix->m[1][3];
		vector.v[2] = matrix->m[2][3];

		return vector;
	}

	vr::HmdVector3_t OpenVRUtils::QuatToEuler(const vr::HmdQuaternion_t* quat)
	{
		vr::HmdVector3_t result;
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

		result.v[0] = OpenVRUtils::Rad2Deg(rotxrad);
		result.v[1] = OpenVRUtils::Rad2Deg(rotyrad);
		result.v[2] = OpenVRUtils::Rad2Deg(rotzrad);

		return result;
	}

	//Matrix Operations
	vr::HmdMatrix34_t OpenVRUtils::CreateTranslationMatrix(vr::HmdVector3_t* translation)
	{
		vr::HmdMatrix34_t result;

		return result;
	}

	vr::HmdMatrix34_t OpenVRUtils::CreateScaleMatrix(vr::HmdVector3_t* scale)
	{
		vr::HmdMatrix34_t result;

		return result;
	}

	vr::HmdMatrix34_t OpenVRUtils::CreateRotationMatrix(vr::HmdQuaternion_t* quaternion)
	{
		vr::HmdMatrix34_t result;

		return result;
	}

	vr::HmdMatrix34_t OpenVRUtils::CreateRotationMatrix(vr::HmdVector3_t* euler)
	{
		vr::HmdMatrix34_t result;

		return result;
	}

	//Conversions
	void OpenVRUtils::CopyQuaternionToVMArray(vr::HmdQuaternion_t* quaternion, VMArray<float>* arr)
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

	void OpenVRUtils::CopyVector3ToVMArray(vr::HmdVector3_t* vector, VMArray<float>* arr)
	{
		//Check Size
		if (vector && arr && arr->arr && arr->Length() == 3)
		{
			arr->Set(vector->v, 0);
			arr->Set(vector->v+1, 1);
			arr->Set(vector->v+2, 2);
		}
	}
}
