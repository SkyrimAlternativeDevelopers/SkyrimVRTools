#include "CppUnitTest.h"
#include "../PapyrusVR_Module/api/utils/OpenVRUtils.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define MAX_DELTA_METERS 0.0001f
#define MAX_DELTA_SKYRIM_UNITS 0.01f

namespace papyrusvr_test
{		
	enum ECoordinateSystem
	{
		SkyrimUnits,
		Meters
	};

	bool EqualsWithinDelta(float a, float b, ECoordinateSystem coordSystem)
	{
		return fabs(a - b) <  ((coordSystem == Meters) ? MAX_DELTA_METERS : MAX_DELTA_SKYRIM_UNITS);
	}

	TEST_CLASS(WorldConversionUnitTest)
	{
	public:
		
		TEST_METHOD(WorldCoordinateConversionTest)
		{
			PapyrusVR::OpenVRUtils::SetVRGameScale(75);

			PapyrusVR::Matrix34 targetSkyrimMatrix(-0.597697, 0.655990, 0.460907, -7.794079,
										   -0.799636, -0.446332, -0.401709, -49.165329,
										   -0.057799, -0.608658, 0.791325, 59.451958);

			PapyrusVR::Matrix34 targetSteamVRMatrix(-0.597697, 0.460907, -0.655990, -0.103921,
											  -0.057799, 0.791325, 0.608658, 0.792693,
											  0.799636, 0.401709, -0.446332, 0.655538);
			
			PapyrusVR::Matrix34 skyrimMatrix = targetSteamVRMatrix;
			PapyrusVR::OpenVRUtils::SteamVRTransformToSkyrimTransform(&skyrimMatrix);
			
			PapyrusVR::Matrix34 returnMatrix = skyrimMatrix;
			PapyrusVR::OpenVRUtils::SkyrimTransformToSteamVRTransform(&returnMatrix);

			PapyrusVR::Matrix34 steamVRMatrix = targetSkyrimMatrix;
			PapyrusVR::OpenVRUtils::SkyrimTransformToSteamVRTransform(&steamVRMatrix);

			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 4; j++)
					Assert::IsTrue(EqualsWithinDelta(targetSteamVRMatrix.m[i][j], returnMatrix.m[i][j], Meters));


			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 4; j++)
					Assert::IsTrue(EqualsWithinDelta(targetSkyrimMatrix.m[i][j], skyrimMatrix.m[i][j], SkyrimUnits));


			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 4; j++)
					Assert::IsTrue(EqualsWithinDelta(targetSteamVRMatrix.m[i][j], steamVRMatrix.m[i][j], Meters));
		}

	};
}