#include "OpenVRUtils.h"
#include "skse64/GameSettings.h"

namespace PapyrusVR
{
	float OpenVRUtils::MetersToSkyrimUnitsFactor = 0.0f;
	float OpenVRUtils::SkyrimUnitsToMetersFactor = 0.0f;

	Matrix33 OpenVRUtils::ConversionMatrix = Matrix33(1, 0, 0,
													  0, 0, -1,
													  0, 1, 0);
	Matrix33 OpenVRUtils::TConversionMatrix = Matrix33(1, 0, 0,
														0, 0, 1,
														0, -1, 0);

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

	#pragma region Matrix Operations
		Matrix34 OpenVRUtils::CreateTransformMatrix(Vector3* translation, Vector3* euler)
		{
			Matrix34 result;
			OpenVRUtils::SetRotationMatrix(&result, euler);
			OpenVRUtils::SetTranslationMatrix(&result, translation);
			return result;
		}

		Matrix34 OpenVRUtils::CreateTransformMatrix(Vector3* translation, Quaternion* quaternion)
		{
			Matrix34 result;
			OpenVRUtils::SetRotationMatrix(&result, quaternion);
			OpenVRUtils::SetTranslationMatrix(&result, translation);
			return result;
		}

		Matrix34 OpenVRUtils::CreateTranslationMatrix(Vector3* translation)
		{
			Matrix34 result;
			OpenVRUtils::SetTranslationMatrix(&result, translation);
			return result;
		}

		Matrix34 OpenVRUtils::CreateScaleMatrix(Vector3* scale)
		{
			Matrix34 result;
			OpenVRUtils::SetScaleMatrix(&result, scale);
			return result;
		}

		Matrix34 OpenVRUtils::CreateRotationMatrix(Quaternion* quaternion)
		{
			Matrix34 result;
			OpenVRUtils::SetRotationMatrix(&result, quaternion);
			return result;
		}

		Matrix34 OpenVRUtils::CreateRotationMatrix(Vector3* euler)
		{
			Matrix34 result;
			OpenVRUtils::SetRotationMatrix(&result, euler);
			return result;
		}

		// Thanks to https://www.flipcode.com/documents/matrfaq.html#Q54
		// I really didn't want to write these :)
		void OpenVRUtils::SetRotationMatrix(Matrix34* matrix, Vector3* euler)
		{
			float A = cos(euler->x);
			float B = sin(euler->x);
			float C = cos(euler->y);
			float D = sin(euler->y);
			float E = cos(euler->z);
			float F = sin(euler->z);

			float AD = A * D;
			float BD = B * D;

			matrix->m[0][0] = C * E;
			matrix->m[0][1] = -C * F;
			matrix->m[0][2] = -D;
			matrix->m[1][0] = -BD * E + A * F;
			matrix->m[1][1] = BD * F + A * E;
			matrix->m[1][2] = -B * C;
			matrix->m[2][0] = AD * E + B * F;
			matrix->m[2][1] = -AD * F + B * E;
			matrix->m[2][2] = A * C;
		}

		void OpenVRUtils::SetRotationMatrix(Matrix34* matrix, Quaternion* quaternion)
		{
			float xx = quaternion->x * quaternion->x;
			float xy = quaternion->x * quaternion->y;
			float xz = quaternion->x * quaternion->z;
			float xw = quaternion->x * quaternion->w;

			float yy = quaternion->y * quaternion->y;
			float yz = quaternion->y * quaternion->z;
			float yw = quaternion->y * quaternion->w;

			float zz = quaternion->z * quaternion->z;
			float zw = quaternion->z * quaternion->w;

			matrix->m[0][0] = 1 - 2 * (yy + zz);
			matrix->m[0][1] = 2 * (xy - zw);
			matrix->m[0][2] = 2 * (xz + yw);

			matrix->m[1][0] = 2 * (xy + zw);
			matrix->m[1][1] = 1 - 2 * (xx + zz);
			matrix->m[1][2] = 2 * (yz - xw);

			matrix->m[2][0] = 2 * (xz - yw);
			matrix->m[2][1] = 2 * (yz + xw);
			matrix->m[2][2] = 1 - 2 * (xx + yy);
		}

		void OpenVRUtils::SetTranslationMatrix(Matrix34* matrix, Vector3* position)
		{
			matrix->m[0][3] = position->x;
			matrix->m[1][3] = position->y;
			matrix->m[2][3] = position->z;
		}

		void OpenVRUtils::SetScaleMatrix(Matrix34* matrix, Vector3* scale)
		{
			matrix->m[0][0] = scale->x;
			matrix->m[1][1] = scale->y;
			matrix->m[2][2] = scale->z;
		}
	#pragma endregion

	#pragma region Conversions

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

	void OpenVRUtils::CopyVMArrayToQuaternion(VMArray<float>* arr, Quaternion* quaternion)
	{
		float value;
		//Check Size
		if (quaternion && arr && arr->arr && arr->Length() == 4)
		{
			arr->Get(&value, 0);
			quaternion->x = value;

			arr->Get(&value, 1);
			quaternion->y = value;

			arr->Get(&value, 2);
			quaternion->z = value;

			arr->Get(&value, 3);
			quaternion->w = value;
		}
	}

	void OpenVRUtils::CopyVMArrayToVector3(VMArray<float>* arr, Vector3* vector)
	{
		//Check Size
		if (vector && arr && arr->arr && arr->Length() == 3)
		{
			arr->Get(&(vector->x), 0);
			arr->Get(&(vector->y), 1);
			arr->Get(&(vector->z), 2);
		}
	}

	void OpenVRUtils::CopyMatrix34ToNiTrasform(Matrix34* matrix, NiTransform* transform)
	{
		//Position
		transform->pos.x = matrix->m[0][3];
		transform->pos.y = matrix->m[1][3];
		transform->pos.z = matrix->m[2][3];

		//Rotation
		transform->rot.data[0][0] = matrix->m[0][0];
		transform->rot.data[0][1] = matrix->m[0][1];
		transform->rot.data[0][2] = matrix->m[0][2];

		transform->rot.data[1][0] = matrix->m[1][0];
		transform->rot.data[1][1] = matrix->m[1][1];
		transform->rot.data[1][2] = matrix->m[1][2];

		transform->rot.data[2][0] = matrix->m[2][0];
		transform->rot.data[2][1] = matrix->m[2][1];
		transform->rot.data[2][2] = matrix->m[2][2];
	}

	void OpenVRUtils::CopyNiTrasformToMatrix34(NiTransform* transform, Matrix34* matrix)
	{
		//Position
		matrix->m[0][3] = transform->pos.x;
		matrix->m[1][3] = transform->pos.y;
		matrix->m[2][3] = transform->pos.z;

		//Rotation
		matrix->m[0][0] = transform->rot.data[0][0];
		matrix->m[0][1] = transform->rot.data[0][1];
		matrix->m[0][2] = transform->rot.data[0][2];

		matrix->m[1][0] = transform->rot.data[1][0];
		matrix->m[1][1] = transform->rot.data[1][1];
		matrix->m[1][2] = transform->rot.data[1][2];

		matrix->m[2][0] = transform->rot.data[2][0];
		matrix->m[2][1] = transform->rot.data[2][1];
		matrix->m[2][2] = transform->rot.data[2][2];
	}

	#pragma endregion

	#pragma region Coordinate Transforms

	void OpenVRUtils::SkyrimTransformToSteamVRTransform(Matrix34* matrix)
	{
		Vector3 position = OpenVRUtils::GetPosition(matrix);
		Matrix33 rotation = Matrix33FromTransform(matrix);

		Matrix33 SkyrimRotation = TConversionMatrix * (rotation * ConversionMatrix);
		*matrix = Matrix34FromRotation(&SkyrimRotation);

		float temp = position.y;
		matrix->m[0][3] = position.x * OpenVRUtils::SkyrimUnitsToMetersFactor;
		matrix->m[1][3] = position.z * OpenVRUtils::SkyrimUnitsToMetersFactor;
		matrix->m[2][3] = -temp * OpenVRUtils::SkyrimUnitsToMetersFactor;
	}

	void OpenVRUtils::SteamVRTransformToSkyrimTransform(Matrix34* matrix)
	{
		Vector3 position = OpenVRUtils::GetPosition(matrix);
		Matrix33 rotation = Matrix33FromTransform(matrix);

		Matrix33 SkyrimRotation = ConversionMatrix * (rotation * TConversionMatrix);
		*matrix = Matrix34FromRotation(&SkyrimRotation);

		float temp = position.y;
		matrix->m[0][3] = position.x * OpenVRUtils::MetersToSkyrimUnitsFactor;
		matrix->m[1][3] = -position.z * OpenVRUtils::MetersToSkyrimUnitsFactor;
		matrix->m[2][3] = temp * OpenVRUtils::MetersToSkyrimUnitsFactor;
	}

	#pragma endregion

	void OpenVRUtils::SetVRGameScale(float VRWorldScale)
	{
		OpenVRUtils::MetersToSkyrimUnitsFactor = VRWorldScale;
		OpenVRUtils::SkyrimUnitsToMetersFactor = 1 / VRWorldScale;
	}

	void OpenVRUtils::SetupConversion()
	{
		_MESSAGE("[OpenVRUtils] Game Loaded, getting VR Scale");
		//Set VR Scale
		double vrScale = 75.0;
		Setting * setting = GetINISetting("fVrScale:VR");
		if (!setting || setting->GetType() != Setting::kType_Float)
			_MESSAGE("[OpenVRUtils] Failed to get vr scale from INI, defaulting back to 75");
		else
			vrScale = setting->data.f32;
		_MESSAGE("[OpenVRUtils] Loaded vrScale with value %f", vrScale);
		PapyrusVR::OpenVRUtils::SetVRGameScale(vrScale);
	}
}
