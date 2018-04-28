#pragma once

namespace PapyrusVR
{
	typedef struct Vector3
	{
		float x;
		float y;
		float z;
	} Vector3;

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
	} Matrix34;

	typedef struct Matrix44
	{
		float m[4][4];
	} Matrix44;


	enum VRDevice
	{
		HMD = 0,
		RightController = 1,
		LeftController = 2
	};

	enum VREvent
	{
		Negative = -1,
		None = 0,
		Positive = 1
	};

	enum VREventType
	{
		Touched = 0,
		Untouched = 1,
		Pressed = 2,
		Released = 3
	};
}