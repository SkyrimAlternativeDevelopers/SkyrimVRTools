#include "ScaleformLoader.h"
#include "Translation.h"

#include "skse64_common/Relocation.h"

bool g_logScaleform = false;

GFxLoader *GFxLoader::ctor_Hook(void)
{
	GFxLoader* result = CALL_MEMBER_FN(this, ctor)();

//	_MESSAGE("result == singleton %d", (UInt32)(result == GetSingleton()));

	// Read plugin list, load translation files
	Translation::ImportTranslationFiles(stateBag->GetTranslator());

	if(g_logScaleform) {
		SKSEGFxLogger * logger = new SKSEGFxLogger();
		stateBag->SetState(GFxState::kInterface_Log, (void*)logger);
	}

	return result;
}

GFxLoader * GFxLoader::GetSingleton()
{
	// 288124718F8102AA5D2B09022062AF83F3795442+AE
	RelocPtr<GFxLoader*> g_GFxLoader(0x02FEA518);
	return *g_GFxLoader;	
}

UInt64* GFxLoader::getCtorHookAddress()
{
	RelocPtr<UInt64> ctorHookAddress(0x005B5880 + 0xACE);
	return ctorHookAddress;
}
