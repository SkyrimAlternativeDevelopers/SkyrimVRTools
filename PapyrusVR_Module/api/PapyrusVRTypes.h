#pragma once

namespace PapyrusVR
{
	typedef struct Vector3
	{
		float x;
		float y;
		float z;

        static const Vector3 zero;
	} Vector3;

    const Vector3 Vector3::zero = { 0,0,0 };

	typedef struct Quaternion
	{
		float x;
		float y;
		float z;
		float w;
	} Quaternion;

	typedef struct Matrix34
	{
		float m[3][4];

		inline
		Matrix34& operator+(Matrix34 const& rhs)
		{
			for(int i = 0; i < 3; i++)
				for(int j = 0; j < 4; j++)
					m[i][j] += rhs.m[i][j];
		}

		inline
		Matrix34& operator-(Matrix34 const& rhs)
		{
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 4; j++)
					m[i][j] -= rhs.m[i][j];
		}

	} Matrix34;

	typedef struct Matrix44
	{
		float m[4][4];
	} Matrix44;

	inline
	Vector3& operator-(Vector3 const& lhs, Vector3 const& rhs)
	{
		Vector3 result;
		result.x = lhs.x - rhs.x;
		result.y = lhs.y - rhs.y;
		result.z = lhs.z - rhs.z;
		return result;
	};

	inline
	Vector3& operator+(Vector3 const& lhs, Vector3 const& rhs)
	{
		Vector3 result;
		result.x = lhs.x + rhs.x;
		result.y = lhs.y + rhs.y;
		result.z = lhs.z + rhs.z;
		return result;
	};

	inline
	Vector3& operator*(Matrix34 const& lhs, Vector3 const& rhs)
	{
		Vector3 transformedVector;

		float ax = lhs.m[0][0] * rhs.x;
		float by = lhs.m[0][1] * rhs.y;
		float cz = lhs.m[0][2] * rhs.z;

		float ex = lhs.m[1][0] * rhs.x;
		float fy = lhs.m[1][1] * rhs.y;
		float gz = lhs.m[1][2] * rhs.z;


		float ix = lhs.m[2][0] * rhs.x;
		float jy = lhs.m[2][1] * rhs.y;
		float kz = lhs.m[2][2] * rhs.z;

		transformedVector.x = ax + by + cz + lhs.m[0][3];
		transformedVector.y = ex + fy + gz + lhs.m[1][3];
		transformedVector.z = ix + jy + kz + lhs.m[2][3];

		return transformedVector;
	}

	inline
	Matrix34& operator+(Matrix34 const& lhs, Matrix34 const& rhs)
	{
		Matrix34 result;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 4; j++)
				result.m[i][j] = lhs.m[i][j] + rhs.m[i][j];
		return result;
	}

	inline
	Matrix34& operator-(Matrix34 const& lhs, Matrix34 const& rhs)
	{
		Matrix34 result;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 4; j++)
				result.m[i][j] = lhs.m[i][j] - rhs.m[i][j];
		return result;
	}

	enum VRDevice
	{
		VRDevice_Unknown = -1,
		VRDevice_HMD = 0,
		VRDevice_RightController = 1,
		VRDevice_LeftController = 2
	};

	enum VREvent
	{
		VREvent_Negative = -1,
		VREvent_None = 0,
		VREvent_Positive = 1
	};

	enum VREventType
	{
		VREventType_Touched = 0,
		VREventType_Untouched = 1,
		VREventType_Pressed = 2,
		VREventType_Released = 3
	};

	enum VROverlapEvent
	{
		VROverlapEvent_None = 0,
		VROverlapEvent_OnEnter = 1,
		VROverlapEvent_OnExit = 2
	};
}