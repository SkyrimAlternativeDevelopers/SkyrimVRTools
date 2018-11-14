#include "Hooks_Gameplay.h"
#include "skse64_common/SafeWrite.h"
#include "skse64_common/Utilities.h"
#include "PapyrusEvents.h"
#include "skse64/GameReferences.h"
#include "skse64_common/skse_version.h"
#include "skse64_common/BranchTrampoline.h"
#include "xbyak/xbyak.h"


static UInt32 g_forceContainerCategorization = 0;

// 59347F2939D95BA10F7C25036702DA321C7D56F1+FA
RelocPtr<UInt32 *> g_containerMode(0x03011720);
RelocAddr<uintptr_t> kHook_ContainerMode_Base(0x0088A2C0);
uintptr_t kHook_ContainerMode_Categories = kHook_ContainerMode_Base + 0x6E;
uintptr_t kHook_ContainerMode_NoCategories = kHook_ContainerMode_Base + 0x82;

void Hooks_Gameplay_EnableForceContainerCategorization(bool enable)
{
	g_forceContainerCategorization = enable ? 1 : 0;
}

UInt32 g_invalidateKeywordCache = 0;
RelocAddr<uintptr_t> kHook_BGSKeyword_Base(0x003433F0);
uintptr_t kHook_BGSKeyword_Create_Return = kHook_BGSKeyword_Base + 6;

// 4C1457C3040DCD34A7E7B2326F1EA2023930C56B+71
RelocAddr <char *> g_gameVersion(0x015BC158);
RelocAddr <uintptr_t> kHook_ShowVersion_Offset(0x00923D50 + 0x78);
static char		kHook_ShowVersion_FormatString[] =
"%s.%d (SKSEVR " __PREPRO_TOKEN_STR__(SKSE_VERSION_INTEGER) "."
__PREPRO_TOKEN_STR__(SKSE_VERSION_INTEGER_MINOR) "."
__PREPRO_TOKEN_STR__(SKSE_VERSION_INTEGER_BETA) " rel "
__PREPRO_TOKEN_STR__(SKSE_VERSION_RELEASEIDX) ")";

RelocAddr<uintptr_t> kHook_Crosshair_LookupREFRByHandle_Enter(0x006D3770 + 0x112);

TESObjectREFR*	g_curCrosshairRef = NULL;

bool __cdecl Hook_Crosshair_LookupREFRByHandle(UInt32 * refHandle, TESObjectREFR ** refrOut)
{
	bool result = LookupREFRByHandle(refHandle, refrOut);

	if (refrOut)
	{
		g_curCrosshairRef = *refrOut;

		SKSECrosshairRefEvent evn(*refrOut);
		g_crosshairRefEventDispatcher.SendEvent(&evn);
	}
	else
	{
		g_curCrosshairRef = NULL;
	}

	return result;
}

TESObjectREFR* Hooks_Gameplay_GetCrosshairRef()
{
	return g_curCrosshairRef;
}

static UInt8 s_disableMapMenuMouseWheel = 1;

RelocAddr<uintptr_t> kHook_MapMenuMouseWheel_Enter(0x00910440 + 0x340);

void Hooks_Gameplay_EnableMapMenuMouseWheel(bool enable)
{
	s_disableMapMenuMouseWheel = enable ? 0 : 1;
}

void Hooks_Gameplay_Commit(void)
{
	// optionally force containers in to "npc" mode, showing categories
	{
		struct HookContainerMode_Code : Xbyak::CodeGenerator {
			HookContainerMode_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label useCategories;

				push(rax);
				mov(rax, (uintptr_t)&g_forceContainerCategorization);
				mov(eax, ptr[rax]);
				cmp(eax, 0);
				pop(rax);
				jnz(useCategories);

				push(rax);
				mov(rax, g_containerMode.GetUIntPtr());
				mov(eax, ptr[rax]);
				cmp(eax, 3);
				pop(rax);
				jz(useCategories);
				jmp(ptr[rip]);
				dq(kHook_ContainerMode_NoCategories);
				
				L(useCategories);
				jmp(ptr[rip]);
				dq(kHook_ContainerMode_Categories);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		HookContainerMode_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());		
		
		g_branchTrampoline.Write5Branch(kHook_ContainerMode_Base.GetUIntPtr() + 0x65, uintptr_t(code.getCode()));		
		SafeWrite16(kHook_ContainerMode_Base.GetUIntPtr() + 0x65 + 5, 0x9090); 		
	}

	// read config
	UInt32	enableContainerCategorization = 0;
	if(GetConfigOption_UInt32("Interface", "EnableContainerCategorization", &enableContainerCategorization))
	{
		if(enableContainerCategorization)
		{
			_MESSAGE(".ini enabled container categorization");
			g_forceContainerCategorization = 1;
		}
	}

	// show SKSE version in menu
	{
		struct ShowVersion_Code : Xbyak::CodeGenerator {
			ShowVersion_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;
				Xbyak::Label dataLabel;

				mov(r8, ptr[rip + dataLabel]);
				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(kHook_ShowVersion_Offset.GetUIntPtr() + 0x7);

				L(dataLabel);
				dq(uintptr_t(&kHook_ShowVersion_FormatString));
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		ShowVersion_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(kHook_ShowVersion_Offset.GetUIntPtr(), uintptr_t(code.getCode()));
		SafeWrite8(kHook_ShowVersion_Offset.GetUIntPtr() + 6, 0x90);
	}

	//// hook BGSKeyword ctor so we can rebuild the lookup table
	{
		struct Hook_BGSKeyword_Create_Code : Xbyak::CodeGenerator {
			Hook_BGSKeyword_Create_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				// rax is safe to use here
				mov(rax, (uintptr_t)&g_invalidateKeywordCache);
				mov(dword[rax], 1);

				// Original code
				push(rbx);
				sub(rsp, 0x30);
				jmp(ptr[rip]);
				dq(kHook_BGSKeyword_Create_Return);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		Hook_BGSKeyword_Create_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(kHook_BGSKeyword_Base.GetUIntPtr(), uintptr_t(code.getCode()));
		SafeWrite8(kHook_BGSKeyword_Base.GetUIntPtr() + 5, 0x90);
	}

	//// hook crosshair ref update
	g_branchTrampoline.Write5Call(kHook_Crosshair_LookupREFRByHandle_Enter, (uintptr_t)Hook_Crosshair_LookupREFRByHandle);

	// change return value of LocalMapMenu::InputHandler::HandleMouseMoveEvent
	// for zoomIn/out case so it passes on the event to flash
	{
		struct Hook_MapMenuMouseWheel_Code : Xbyak::CodeGenerator {
			Hook_MapMenuMouseWheel_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				mov(rax, (uintptr_t)&s_disableMapMenuMouseWheel);
				mov(al, ptr[rax]);

				// Original code
				pop(r15);
				pop(r14);
				pop(rbp);
				ret();
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		Hook_MapMenuMouseWheel_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(kHook_MapMenuMouseWheel_Enter.GetUIntPtr(), uintptr_t(code.getCode()));		
	}
}
