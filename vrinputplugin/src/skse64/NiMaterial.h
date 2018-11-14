#pragma once

#include "skse64/NiTextures.h"
#include "skse64/GameTypes.h"

MAKE_NI_POINTER(BSTextureSet);
MAKE_NI_POINTER(NiTexture);

// 30
class BSShaderMaterial
{
public:
	virtual ~BSShaderMaterial();
	virtual BSShaderMaterial * Create(void);
	virtual void Copy(BSShaderMaterial * source); // Must be same type
	virtual bool Unk_03(void * unk1);
	virtual SInt32 Unk_04(void * unk1);
	virtual void * Unk_05(void);
	virtual UInt32 GetShaderType(void);
	virtual UInt32 Unk_07(void);	// Always seems to be 2

	enum {
		kShaderType_Default = 0,
		kShaderType_EnvironmentMap,
		kShaderType_GlowMap,
		kShaderType_Parallax,
		kShaderType_FaceGen,
		kShaderType_FaceGenRGBTint,
		kShaderType_HairTint,
		kShaderType_ParallaxOcc,
		kShaderType_MultiTexLand,
		kShaderType_LODLand,
		kShaderType_Unknown1,
		kShaderType_MultilayerParallax,
		kShaderType_TreeAnim,
		kShaderType_Unknown2,
		kShaderType_MultiIndexTriShapeSnow,
		kShaderType_LODObjectsHD,
		kShaderType_Eye,
		kShaderType_Cloud,
		kShaderType_LODLandNoise,
		kShaderType_MultiTexLandLODBlend
	};

	UInt32	unk04;	// 08 BSIntrusiveRefCounted?
	UInt32	unk08;	// 0C inited to 0
	UInt32	unk10;	// 10 inited to 0
	UInt32	unk14;	// 14 inited to 0
	UInt32	unk18;	// 18 inited to 0
	float	unk1C;	// 1C inited to 1.0
	float	unk20;	// 20 inited to 1.0
	float	unk24;	// 24 inited to 1.0
	float	unk28;	// 28 inited to 1.0
	UInt32	unk2C;	// 2C inited to -1 flags?
};

// 30
class BSShaderMaterialBase : public BSShaderMaterial
{
public:
	virtual ~BSShaderMaterialBase();

	virtual void SetTexture(UInt32 index, BSTextureSet * texture, SInt32 unk1);
	virtual void ReleaseTextures(void); // ReleaseRefs
	virtual void Unk_0A(UInt8 unk1, UInt8 unk2, UInt8 unk3, UInt8 unk4, UInt8 unk5, UInt32 unk6, UInt32 unk7); // AddRefs
	virtual void Unk_0B(void * unk1, UInt32 unk2);
	virtual void * Unk_0C(void * unk1);
	virtual void * Unk_0D(void * unk1);
};

class BSEffectShaderMaterial : public BSShaderMaterialBase
{
public:
	virtual ~BSEffectShaderMaterial();

	float	falloffStartAngle;			// 30
	float	falloffStopAngle;			// 34
	float	falloffStartOpacity;		// 38
	float	falloffStopOpacity;			// 3C
	NiColorA	emissiveColor;			// 40
	NiSourceTexture	* unk50;			// 50
	NiSourceTexture	* unk54;			// 54
	float	softFalloffDepth;			// 58
	float	emissiveMultiple;			// 5C
	BSFixedString	sourceTexture;		// 60
	BSFixedString	greyscaleTexture;	// 64
};

// A0
class BSLightingShaderMaterial : public BSShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterial();

	float	unk30;				// 30
	float	unk34;				// 34
	float	unk38;				// 38
	float	unk3C;				// 3C
	float	unk40;				// 40
	float	unk44;				// 44
	NiTexture * texture1;		// 48
	SInt32	unk50;				// 50
	UInt32	unk54;				// 54
	NiTexture * texture2;		// 58
	NiTexture * texture3;		// 60
	NiTexture * texture4;		// 68
	UInt32	unk70;				// 70
	UInt32	unk74;				// 74
	BSTextureSetPtr	textureSet;	// 78
	float	alpha;				// 80
	float	unk58;				// 84
	float	glossiness;			// 88
	float	specularStrength;	// 8C
	float	lightingEffect1;	// 90
	float	lightingEffect2;	// 94
	UInt64	unk98;				// 98

	void SetTextureSet(BSTextureSet * textureSet);

	MEMBER_FN_PREFIX(BSLightingShaderMaterial);
	DEFINE_MEMBER_FN(CopyFrom, void, 0x0130DE50, BSLightingShaderMaterial * other);
};
STATIC_ASSERT(sizeof(BSLightingShaderMaterial) == 0xA0);

class BSLightingShaderMaterialFacegen : public BSLightingShaderMaterial
{
public:
	virtual ~BSLightingShaderMaterialFacegen();

	NiTexturePtr renderedTexture;	// A0 inited to 0
	NiTexturePtr unkA8;				// A8 inited to 0
	NiTexturePtr unkB0;				// B0 inited to 0
};

class BSLightingShaderMaterialFacegenTint : public BSLightingShaderMaterial
{
public:
	virtual ~BSLightingShaderMaterialFacegenTint();

	NiColor	tintColor;	// A0
};

class BSLightingShaderMaterialHairTint : public BSLightingShaderMaterial
{
public:
	virtual ~BSLightingShaderMaterialHairTint();

	NiColor	tintColor;	// A0
};

typedef BSShaderMaterialBase * (* _CreateShaderMaterial)(UInt32 shaderType);
extern RelocAddr<_CreateShaderMaterial> CreateShaderMaterial;
