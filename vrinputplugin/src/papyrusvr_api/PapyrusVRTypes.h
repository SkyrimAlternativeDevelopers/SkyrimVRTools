#pragma once

namespace PapyrusVR
{
	typedef struct Vector3
	{
		float x;
		float y;
		float z;

        static const Vector3 zero;

        inline float lengthSquared(  )
        {
            return x * x + y * y + z * z;
        };
	} Vector3;

	Vector3& operator-(Vector3 const& lhs, Vector3 const& rhs);
	Vector3& operator+(Vector3 const& lhs, Vector3 const& rhs);

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

		Matrix34& operator+(Matrix34 const& rhs);
		Matrix34& operator-(Matrix34 const& rhs);

	} Matrix34;

	Matrix34& operator+(Matrix34 const& lhs, Matrix34 const& rhs);
	Matrix34& operator-(Matrix34 const& lhs, Matrix34 const& rhs);
	Vector3& operator*(Matrix34 const& lhs, Vector3 const& rhs);

	typedef struct Matrix44
	{
		float m[4][4];
	} Matrix44;

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