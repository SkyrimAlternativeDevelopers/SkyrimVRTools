#pragma once
#include "openvr.h"

using namespace vr;
namespace PapyrusVR
{
	class OpenVRUtils
	{
	public:
		static HmdQuaternion_t GetRotation(HmdMatrix34_t steam_vr_matrix);
		static HmdVector3_t GetPosition(HmdMatrix34_t steam_vr_matrix);
	};
}