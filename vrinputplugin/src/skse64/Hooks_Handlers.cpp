#include "Hooks_Handlers.h"
#include "skse64_common/SafeWrite.h"

#include "skse64/GameRTTI.h"
#include "skse64/GameData.h"
#include "skse64/GameForms.h"
#include "skse64/GameObjects.h"
#include "skse64/GameHandlers.h"

#include "skse64/PapyrusEvents.h"

template<class T, UInt32 type, UInt32 slot>
UInt8 IHandlerFunctor<Actor, UInt32>::ProcessAction(Actor * actor, UInt32 unk04)
{
	TESForm * object = slot == SKSEActionEvent::kSlot_Voice ? actor->equippedShout : actor->GetEquippedObject(slot == SKSEActionEvent::kSlot_Left);
	SKSEActionEvent evn(type, actor, object, slot);
	g_actionEventDispatcher.SendEvent(&evn);
	return CALL_MEMBER_FN(static_cast<T*>(this), Process_Origin)(actor, unk04);
}

void Hooks_Handlers_Init(void)
{

}

// ??_7WeaponRightSwingHandler@@6B@
RelocAddr <uintptr_t> WeaponRightSwingHandler_Vtable(0x016F4F90);
// ??_7WeaponLeftSwingHandler@@6B@
RelocAddr <uintptr_t> WeaponLeftSwingHandler_Vtable(0x016F4FA8);
// ??_7RightHandSpellFireHandler@@6B@
RelocAddr <uintptr_t> RightHandSpellFireHandler_Vtable(0x016F5038);
// ??_7LeftHandSpellFireHandler@@6B@
RelocAddr <uintptr_t> LeftHandSpellFireHandler_Vtable(0x016F5020);
// ??_7RightHandSpellCastHandler@@6B@
RelocAddr <uintptr_t> RightHandSpellCastHandler_Vtable(0x016F5080);
// ??_7LeftHandSpellCastHandler@@6B@
RelocAddr <uintptr_t> LeftHandSpellCastHandler_Vtable(0x016F5068);
// ??_7VoiceSpellFireHandler@@6B@
RelocAddr <uintptr_t> VoiceSpellFireHandler_Vtable(0x016F5050);
// ??_7VoiceSpellCastHandler@@6B@
RelocAddr <uintptr_t> VoiceSpellCastHandler_Vtable(0x016F5098);
// ??_7BowDrawnHandler@@6B@
RelocAddr <uintptr_t> BowDrawnHandler_Vtable(0x016F53C8);
// ??_7BowReleaseHandler@@6B@
RelocAddr <uintptr_t> BowReleaseHandler_Vtable(0x016F53E0);
// ??_7WeaponBeginDrawRightHandler@@6B@
RelocAddr <uintptr_t> WeaponBeginDrawRightHandler_Vtable(0x016F50B0);
// ??_7WeaponBeginSheatheRightHandler@@6B@
RelocAddr <uintptr_t> WeaponBeginSheatheRightHandler_Vtable(0x016F50C8);
// ??_7RightHandWeaponDrawHandler@@6B@
RelocAddr <uintptr_t> RightHandWeaponDrawHandler_Vtable(0x016F50E0);
// ??_7RightHandWeaponSheatheHandler@@6B@
RelocAddr <uintptr_t> RightHandWeaponSheatheHandler_Vtable(0x016F50F8);

void Hooks_Handlers_Commit(void)
{
	// vtbl overwrites
	SafeWrite64(WeaponRightSwingHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<WeaponRightSwingHandler, SKSEActionEvent::kType_WeaponSwing, SKSEActionEvent::kSlot_Right>));
	SafeWrite64(WeaponLeftSwingHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<WeaponLeftSwingHandler, SKSEActionEvent::kType_WeaponSwing, SKSEActionEvent::kSlot_Left>));

	SafeWrite64(RightHandSpellFireHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<RightHandSpellFireHandler, SKSEActionEvent::kType_SpellFire, SKSEActionEvent::kSlot_Right>));
	SafeWrite64(LeftHandSpellFireHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<LeftHandSpellFireHandler, SKSEActionEvent::kType_SpellFire, SKSEActionEvent::kSlot_Left>));

	SafeWrite64(RightHandSpellCastHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<RightHandSpellCastHandler, SKSEActionEvent::kType_SpellCast, SKSEActionEvent::kSlot_Right>));
	SafeWrite64(LeftHandSpellCastHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<LeftHandSpellCastHandler, SKSEActionEvent::kType_SpellCast, SKSEActionEvent::kSlot_Left>));

	SafeWrite64(VoiceSpellFireHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<VoiceSpellFireHandler, SKSEActionEvent::kType_VoiceFire, SKSEActionEvent::kSlot_Voice>));
	SafeWrite64(VoiceSpellCastHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<VoiceSpellCastHandler, SKSEActionEvent::kType_VoiceCast, SKSEActionEvent::kSlot_Voice>));

	SafeWrite64(BowDrawnHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<BowDrawnHandler, SKSEActionEvent::kType_BowDraw, SKSEActionEvent::kSlot_Right>));
	SafeWrite64(BowReleaseHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<BowReleaseHandler, SKSEActionEvent::kType_BowRelease, SKSEActionEvent::kSlot_Right>));

	SafeWrite64(WeaponBeginDrawRightHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<WeaponBeginDrawRightHandler, SKSEActionEvent::kType_BeginDraw, SKSEActionEvent::kSlot_Right>));
	SafeWrite64(WeaponBeginSheatheRightHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<WeaponBeginSheatheRightHandler, SKSEActionEvent::kType_BeginSheathe, SKSEActionEvent::kSlot_Right>));

	SafeWrite64(RightHandWeaponDrawHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<RightHandWeaponDrawHandler, SKSEActionEvent::kType_EndDraw, SKSEActionEvent::kSlot_Right>));
	SafeWrite64(RightHandWeaponSheatheHandler_Vtable.GetUIntPtr() + 0x08, GetFnAddr(&IHandlerFunctor<Actor, UInt32>::ProcessAction<RightHandWeaponSheatheHandler, SKSEActionEvent::kType_EndSheathe, SKSEActionEvent::kSlot_Right>));
}
