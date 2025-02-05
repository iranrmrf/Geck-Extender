#include <string>
#include <Richedit.h>
#include <CommCtrl.h>
#include <list>
#include <iostream>
#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <set>
#include <regex>
#include <libdeflate/libdeflate.h>
#include <unordered_set>
#include <filesystem>

#include "nvse/GameData.h"
#include "nvse/PluginAPI.h"
#include "nvse/GameForms.h"
#include "nvse/GameObjects.h"
#include "nvse/GameRTTI.h"
#include "nvse/SafeWrite.h"
#include "nvse/ParamInfos.h"
#include "xutil.h"

#include "ZeGaryHax.h"
#include "ZeLogWindow.h"
#include "ZeWarningPatches.h"
#include "ZeWarningHax.h"
#include "Editor.h"
#include "UISpeedHooks.h"
#include "DebugCellShaders.h"
#include "ZeEditBoxHax.h"

BOOL __stdcall ScriptEditCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ((WNDPROC)(0x5C3D40))(hWnd, msg, wParam, lParam);
}

extern "C"
{
	BOOL WINAPI DllMain(HANDLE  hDllHandle, DWORD dwReason, LPVOID lpreserved)
	{
		if (dwReason == DLL_PROCESS_ATTACH)
		{
			ZeGaryHaxHandle = (HMODULE)hDllHandle;
		}
		return TRUE;
	}
}

bool NVSEPlugin_Query(const NVSEInterface* nvse, PluginInfo* info)
{
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = GH_NAME;
	info->version = GH_VERSION;
	return (nvse->isEditor && nvse->nvseVersion >= NVSE_VERSION_INTEGER);
}

bool NVSEPlugin_Load(const NVSEInterface* nvse)
{
	_DMESSAGE("Geck Extender Base Address: %08X", GetModuleHandle("ZeGaryHax.dll"));

	//	ini thing - credit to carxt
	GetModuleFileNameA(NULL, iniName, MAX_PATH);
	strcpy((char*)(strrchr(iniName, '\\') + 1), "Data\\nvse\\plugins\\geckextender.ini");

	bEnableSpellChecker = GetOrCreateINIInt("General", "bEnableSpellChecker", 0, iniName);
	bFastExit = GetOrCreateINIInt("General", "bFastExit", 1, iniName);
	bListEditFix = GetOrCreateINIInt("General", "bListEditFix", 0, iniName);
	bIgnoreNAMFiles = GetOrCreateINIInt("General", "bIgnoreNAMFiles", 1, iniName);
	bVersionControlMode = GetOrCreateINIInt("General", "bVersionControlMode", 0, iniName);
	bHighResLandscapeLOD = GetOrCreateINIInt("General", "bHighResLandscapeLOD", 0, iniName);
	bRemoveStatusBarLoadingText = GetOrCreateINIInt("General", "bRemoveLoadingText", 1, iniName);
	bPlaySoundEndOfLoading = GetOrCreateINIInt("General", "bPlaySoundEndOfLoading", 1, iniName);
	bNoDXSoundCaptureErrorPopup = GetOrCreateINIInt("General", "bNoDXSoundCaptureErrorPopup", 0, iniName);
	bNoPreviewWindowAutoFocus = GetOrCreateINIInt("General", "bNoPreviewWindowAutoFocus", 1, iniName);
	bNoLODMeshMessage = GetOrCreateINIInt("General", "bNoLODMeshMessage", 0, iniName);
	bAutoLoadFiles = GetOrCreateINIInt("General", "bAutoLoadFiles", 0, iniName);
	bShowLoadFilesAtStartup = GetOrCreateINIInt("General", "bShowLoadFilesAtStartup", 0, iniName) | bAutoLoadFiles;
	bNoVersionControlWarning = GetOrCreateINIInt("General", "bNoVersionControlWarning", 0, iniName);
	bSkipVanillaLipGen = GetPrivateProfileIntA("General", "bSkipVanillaLipGen", 0, iniName);
	bShowAdditionalToolbarButtons = GetOrCreateINIInt("General", "bShowAdditionalToolbarButtons", 0, iniName);
	bAllowMultipleSearchAndReplace = GetOrCreateINIInt("General", "bAllowMultipleSearchAndReplace", 0, iniName);
	bNoFactionReactionMessage = GetOrCreateINIInt("General", "bNoFactionReactionMessage", 0, iniName);
	bUISpeedHooks = GetOrCreateINIInt("General", "bUISpeedHooks", 1, iniName);
	bLibdeflate = GetOrCreateINIInt("General", "bLibDeflate", 0, iniName);
	bExpandFormIDColumn = GetOrCreateINIInt("General", "bExpandFormIDColumn", 0, iniName);
	bAllowEditLandEdges = GetOrCreateINIInt("General", "bAllowEditLandEdges", 0, iniName);
	bAllowRecompileAll = GetOrCreateINIInt("General", "bAllowRecompileAll", 0, iniName);
	bNavmeshFindCoverConfirmPrompt = GetOrCreateINIInt("General", "bNavmeshFindCoverConfirmPrompt", 0, iniName);
	bFaceGenOnlyEdited = GetOrCreateINIInt("General", "bFaceGenOnlyEdited", 1, iniName);
	bDisableTextureMirroring = GetOrCreateINIInt("General", "bDisableTextureMirroring", 0, iniName);
	bObjectWindowOnlyShowEditedByDefault = GetOrCreateINIInt("General", "bObjectWindowOnlyShowEditedForms", 0, iniName);
	bPreventFaceAndBodyModExports = GetOrCreateINIInt("General", "bFaceBodyExportPreventTGAFiles", 0, iniName);
	bIgnoreD3D9 = GetOrCreateINIInt("General", "bIgnoreD3D9", 1, iniName);

	bPatchScriptEditorFont = GetOrCreateINIInt("Script", "bPatchEditorFont", 1, iniName);
	bScriptCompileWarningPopup = GetOrCreateINIInt("Script", "bScriptCompileWarningPopup", 0, iniName);
	bShowScriptChangeTypeWarning = GetOrCreateINIInt("Script", "bShowChangeScriptTypeWarning", 0, iniName);

	bAutoScroll = GetOrCreateINIInt("Log", "bAutoScroll", 1, iniName);

	bRenderWindowUncap = GetOrCreateINIInt("Render Window", "bRenderWindowUncap", 1, iniName);
	bPreviewWindowUncap = GetOrCreateINIInt("Render Window", "bPreviewWindowUncap", 1, iniName);
	bSwapRenderCYKeys = GetOrCreateINIInt("Render Window", "bSwapCandYKeys", 0, iniName);
	bUseAltShiftMultipliers = GetOrCreateINIInt("Render Window", "bUseAltShiftMultipliers", 1, iniName);
	fMovementAltMultiplier = GetOrCreateINIInt("Render Window", "iAltSpeedPct", 15, iniName) / 100.0F;
	fMovementShiftMultiplier = GetOrCreateINIInt("Render Window", "iShiftSpeedPct", 200, iniName) / 100.0F;
	bShowTimeOfDaySlider = GetOrCreateINIInt("Render Window", "bShowTimeOfDaySlider", 1, iniName);
	bNavmeshAllowPlaceAboveOthers = GetOrCreateINIInt("Render Window", "bNavmeshAllowPlaceAboveOthers", 1, iniName);
	bSnapToGridRotationUseDoublePrecision = GetOrCreateINIInt("Render Window", "bSnapToGridRotationUseDoublePrecision", 0, iniName);

	bSmoothFlycamRotation = GetOrCreateINIInt("Flycam", "bSmoothRotation", 1, iniName);
	bFlycamUpDownRelativeToWorld = GetOrCreateINIInt("Flycam", "bFlycamUpDownRelativeToWorld", 1, iniName);
	fFlycamRotationSpeed = GetOrCreateINIInt("Flycam", "iRotationSpeedPct", 100, iniName) * -0.001F;
	fFlycamNormalMovementSpeed = GetOrCreateINIInt("Flycam", "iMovementSpeed", 10, iniName) * 3.0F;
	fFlycamShiftMovementSpeed = GetOrCreateINIInt("Flycam", "iRunMovementSpeedPct", 300, iniName) / 100.0F;
	fFlycamAltMovementSpeed = GetOrCreateINIInt("Flycam", "iWalkMovementSpeedPct", 20, iniName) / 100.0F;

	bAppendAnimLengthToName = GetOrCreateINIInt("Preview Window", "bAppendAnimLengthToName", 0, iniName);

	bObjectPaletteAllowRandom = GetOrCreateINIInt("Object Palette", "bObjectPaletteAllowRandom", 1, iniName);
	bObjectPaletteRandomByDefault = GetOrCreateINIInt("Object Palette", "bObjectPaletteRandomByDefault", 0, iniName) != 0;

	bRemoveDialogSoundFilter = GetOrCreateINIInt("Dialog", "bRemoveDialogSoundFilter", 0, iniName) != 0;
	bCacheComboboxes = GetOrCreateINIInt("Dialog", "bCacheComboboxes", 0, iniName) != 0;

	//	stop geck crash with bUseMultibounds = 0 in exterior cells with multibounds - credit to roy
	WriteRelCall(0x004CA48F, (UInt32)FixMultiBounds);
	XUtil::PatchMemoryNop(0x004CA494, 0x05);


	// fix various file path vtable entries to exclude the "//" in the path - credit to roy
	SafeWrite32(0x00D39FB0, 0x004FE910);	// skills icon in actor values
	SafeWrite32(0x00D47E28, 0x004FE910);	// landscape (and texture sets?)
	SafeWrite32(0x00D527D0, 0x004FE910);	// class image
	SafeWrite32(0x00D53980, 0x004FE910);	// eyes
	SafeWrite32(0x00D54050, 0x004FE910);	// hair
	SafeWrite32(0x00D58330, 0x004FE910);	// skills (not sure where this is viewable)
	SafeWrite32(0x00D5DFD8, 0x004FE910);	// loading screen
	SafeWrite32(0x00D7BED8, 0x004FE910);	// worldspace map

	SafeWrite32(0x00D73EE0, UInt32(SpeedTreeGetTexturePath));	// destructable tree (not sure where this is viewable)
	SafeWrite32(0x00D73F68, UInt32(SpeedTreeGetTexturePath));	// tree

	//	enable wasteland level 2 lod generation - credit to roy
	SafeWrite8(0x00419557, 0xEB);

	//	enable 512px landscape LOD textures and normals - credit to roy
	if (bHighResLandscapeLOD)
	{
		SafeWrite8(0x00728A3C, 0x02);
		SafeWrite8(0x00728A41, 0x02);
		SafeWrite8(0x0072971B, 0x02);
		SafeWrite8(0x00729720, 0x02);
		SafeWrite8(0x0072F090, 0x02);
		SafeWrite8(0x0072F095, 0x02);
	}

	// increase landscape edit settings max icon size to 4096x4096 from 512x512
	SafeWrite8(0x448AA3, 0x10);

	//	change help urls to our wiki - credit to roy
	SafeWrite32(0x00441952, (UInt32)geckwikiurl);
	SafeWrite32(0x005C4B0F, (UInt32)geckwikiscriptingurl);
	SafeWrite32(0x005C4B29, (UInt32)geckwikicommandsurl);
	SafeWrite32(0x005C4B43, (UInt32)geckwikifunctionsurl);

	//	force patch script editor font - credit to nvse team
	if (bPatchScriptEditorFont)
	{
		FixEditorFont();
	}

	//	expand scriptlet box size - credit to roy and luthien
	SafeWrite32(0x0059B3FB, 0x7FFF);	// 2650
	SafeWrite32(0x0059B40F, 0x7FFF);	// 1445
	SafeWrite32(0x00582FA4, 0x7FFF);	// 1444

	//	double script size limit - credit to jazzisparis
	SafeWrite8(0x005C56B6, 0x7F);
	SafeWrite8(0x005C56D3, 0x7F);
	SafeWrite8(0x005C92F6, 0x7F);

	//	fast exit - credit to roy - tweaks by jazzisparis
	WriteRelCall(0x00440A4F, (UInt32)FastExitHook);

	//	enable big guns for weapon skill
	SafeWrite32(0x492D0C, 0x410);

	//	uncap framerate on render window - credit to shademe
	if (bRenderWindowUncap)
	{
		SafeWrite8(0x0045911B, 0x0A);
	}

	//	uncap framerate on preview window - credit to shademe
	if (bPreviewWindowUncap)
	{
		SafeWrite8(0x004100E5, 0x0A);
	}

	//	window handle leak fix - credit to nukem
	//	now uses thread local storage to fix parent issues
	SafeWrite32(0x00D234CC, (UInt32)hk_CreateDialogParamA);
	SafeWrite32(0x00D23510, (UInt32)hk_DialogBoxParamA);
	SafeWrite32(0x00D23530, (UInt32)hk_EndDialog);
	SafeWrite32(0x00D23550, (UInt32)hk_SendMessageA);

	//	fix WM_CLOSE in destruction data dialog - credit to roy
	SafeWrite32(0x004E640F, (UInt32)hk_DialogProc);

	//  removes sound filters that cause dailog window to take ages to load - credit to iranrmrf
	if (bRemoveDialogSoundFilter)
	{
		SafeWriteBuf(0x0058E4F6, "\x83\xC4\x10\xB0\x01", 5);
		SafeWriteBuf(0x0058E510, "\x83\xC4\x10\xB0\x01", 5);
	}

	if (bCacheComboboxes)
	{
		WriteRelCall(0x0057E5E1, (UInt32)hk_QuestWindowLoad);
	}

	if (bListEditFix)
	{
		//	fix windows 8/10 conditions collapsed column bug - credit to nukem
		WriteRelCall(0x004A382D, (UInt32)hk_sub_4A1C10);
		WriteRelCall(0x00765214, (UInt32)hk_sub_4A1C10);
		WriteRelCall(0x00766A4D, (UInt32)hk_sub_4A1C10);
		WriteRelCall(0x0076E195, (UInt32)hk_sub_4A1C10);
		WriteRelCall(0x004A2791, (UInt32)hk_sub_4A1C10); // necessary, but causes a crash when viewing the anims list
	}

	if (bExpandFormIDColumn)
	{
		// fix FormID column being collapsed
		WriteRelJump(0x42EFBB, UInt32(CellViewListViewCreateFormIDColumnHook));
		WriteRelJump(0x44965A, UInt32(ObjectWindowListViewColumnSizeHook));
		WriteRelCall(0x4682FE, UInt32(InsertListViewHeaderSetSizeHook));

		// set default object window formID column width
		SafeWrite16(0xE8E5C2, 0x42);
	}

	//	ignore .nam Files - initial credit to roy
	if (bIgnoreNAMFiles)
	{
		SafeWriteBuf(0xD412C1, "GAL", 3); // replace NAM extension with GAL ('Geck Auto-Load')
	}

	if (bVersionControlMode == 0)
	{
		//	enable esm as active file - credit to hlp
		XUtil::PatchMemoryNop(0x00430F07, 0x09);
		XUtil::PatchMemoryNop(0x004CF429, 0x09);

		//	fix crash
		WriteRelCall(0x0042CD31, (UInt32)hk_addr_42CD31);
		XUtil::PatchMemoryNop(0x0042CD36, 0x03);

		//	patch formid
		SafeWrite16(0x004DF7D8, 0x9090);

		//	patch esp as master
		XUtil::PatchMemoryNop(0x004D9CA5, 0x09);

		//	patch ONAM count
		WriteRelCall(0x004E26BA, (UInt32)hk_addr_4E26BA);
		XUtil::PatchMemoryNop(0x004E26BF, 0x0B);

		// enable editing file author and summary in an ESM
		SafeWrite8(0x00432776, 0xEB);
		SafeWrite8(0x004327BE, 0xEB);
	}

	//	fix Rock-It Launcher crash - credit to jazzisparis
	WriteRelJump(0x005B8FF4, (UInt32)CheckIsRILHook);

	WriteErrorMessageHooks();

	//	fix %s:%d: %s : %s warning message crashes - credit to roy and nukem
	SafeWrite16(0x006794F9, 0x9090);
	SafeWrite8(0x00679502, 0x14);

	SafeWrite16(0x006794C9, 0x9090);
	SafeWrite8(0x006794D2, 0x14);

	SafeWrite16(0x006794F9, 0x9090);
	SafeWrite8(0x00679502, 0x14);

	//	Suppress old bhkrigidbody cinfo warning that's useless - credit to roy
	SafeWrite16(0x006796CD, 0x9090);

	//	Spell Checker hack - credit to roy
	WriteRelJump(0x0041B60D, (UInt32)hk_SpellCheck);
	SafeWrite8(0x0041B612, 0x90);

	//	Splash Screen Hax - credit to nukem and roy
	WriteRelCall(0x4463CD, UInt32(SplashScreenHook));
	SafeWrite8(0x4463CD + 5, 0x90);

	//	About Dialog Hax - credit to nukem and roy
	WriteRelCall(0x00441976, UInt32(AboutDialogHook));
	SafeWrite8(0x00441976 + 5, 0x90);

	//	Patch some SendMessageA to speed up loading - credit to nukem
	XUtil::PatchMemoryNop(0x004DC191, 0x05);
	XUtil::PatchMemoryNop(0x004DC641, 0x05);
	XUtil::PatchMemoryNop(0x004DC91C, 0x05);

	//	nop useless calls to _vsprintf and SendMessage for 'Processing topic %s' - credit to roy/nukem
	XUtil::PatchMemoryNop(0x00591CCE, 0x24);
	XUtil::PatchMemoryNop(0x00592F08, 0x3F);
	SafeWriteBuf(0x592F4A, "\x10\x90\x90\x90", 4); // above removes pushed args, so need to change mov offset and nop the add esp

	//	Make search and replace window stay open unless explicitly close - credit to StewieA
	if (bAllowMultipleSearchAndReplace) {
		WriteRelJump(0x0047CE7F, (UINT32)hk_SearchDestroyHook);
		SafeWrite8(0x0047CE84, 0x90);
	}

	// Make the "Do" button of "Reference Batch Action" not grayed out if an action is already selected when initialising the dialog
	WriteRelJump(0x411CCF, UInt32(ReferenceBatchActionDoButtonHook));

	// Remove call to SetFocus(0) when closing Reference Batch Action dialog
	XUtil::PatchMemoryNop(0x411CFA, 8);

	//	Replace zlib with libinflate - credit to nukem/StewieA
	if (bLibdeflate)
	{
		WriteRelCall(0x004DFB34, (UInt32)hk_inflateInit);
		WriteRelCall(0x004DFB9E, (UInt32)hk_inflate);
	}

	//	Expand ini GetPrivateProfileString buffer to 1024 bytes - credit to jazzisparis
	SafeWrite8(0x009D3D01, 0x04);
	SafeWrite8(0x009D3D65, 0x04);
	SafeWrite8(0x009D3EE4, 0x04);
	SafeWrite8(0x009D3F4B, 0x04);
	SafeWrite8(0x009D4027, 0x04);
	SafeWrite8(0x009D3EEA, 0xF9);
	SafeWrite8(0x009D3F21, 0xF9);
	SafeWrite8(0x009D3F51, 0xF9);
	SafeWrite8(0x009D3F98, 0xF9);
	SafeWrite8(0x009D402D, 0xF9);
	SafeWrite8(0x009D407B, 0xF9);

	// stop the Preview Object window taking focus when hovered over (unless the parent window is focussed)
	if (bNoPreviewWindowAutoFocus) {
		WriteRelJump(0x48CAC9, UInt32(hk_PreviewWindowCheckForeground));
	}
	// no "DirectSoundCaptureCreate Error: No sound driver is available for use" popup
	if (bNoDXSoundCaptureErrorPopup) {
		WriteRelCall(0x42B6E0, UInt32(EditorUI_Log));
	}

	if (bRemoveStatusBarLoadingText) {
		// patch additional loading messages to the bottom status bar
		XUtil::PatchMemoryNop(0x4DCC24, 5);
		XUtil::PatchMemoryNop(0x4DC96F, 5);
		XUtil::PatchMemoryNop(0x4DDD4F, 5);

		// make the cell loading bar only update every 128 references instead of every ref
		SafeWriteBuf(0x4BFFA2, "\xF7\xC3\x7F\x00\x00\x00\x74\x3C\xEB\x68", 10); // replaces a test ebx, ebx with test ebx, 0x7F
	}

	if (bPlaySoundEndOfLoading) {
		WriteRelJump(0x464D32, UInt32(EndLoadingHook));
	}

	// add a more detailed warning when recompiling all
	WriteRelJump(0x5C497E, UInt32(RecompileAllWarningScriptHook));
	WriteRelJump(0x4442C7, UInt32(RecompileAllWarningMainHook));

	// fix arrow keys in dialogue menu making the window become unfocussed, allowing consecutive arrow presses without having to click again
	SafeWrite16(0x5A1817, 0x0FEB); // jump over SetFocus(0)
	SafeWrite16(0x5993EC, 0x0FEB);

	// speed up UI
	WriteUIHooks();

	// hook Load ESP/ESM window callback
	SafeWrite32(0x44192A, UInt32(hk_LoadESPESMCallback));

	// make flycam mouse rotation grid smaller (1*1 instead of 2*2)
	// the game was checking if the difference in mouse position was > 1 rather than >= 1
	SafeWrite8(0x45D3AD, 0x7C);
	SafeWrite8(0x45D3BD, 0x7F);
	SafeWrite8(0x45D3D9, 0x7C);
	SafeWrite8(0x45D3EA, 0x7F);

	// make holding shift slow down the flycam movement by 80%
	WriteRelJump(0x455D12, UInt32(FlycamMovementSpeedMultiplierHook));

	if (bSmoothFlycamRotation) {
		WriteRelJump(0x45D3A3, UInt32(FlycamRotationSpeedMultiplierHook));
		WriteRelJump(0x456A9F, 0x456AE6); // skip call to SetCursorPos when changing to flycam mode
	}

	if (bSwapRenderCYKeys) {
		// set C as hotkey for restricting movement to Y direction
		SafeWrite8(0x462D8B, 0x6); // (patch a switch table offset)
		SafeWrite8(0x462F5B, 0xF); // (patch a switch table offset)

		// remove Y as hotkey for Y movement
		SafeWrite8(0x462DA1, 0x8);
		SafeWrite8(0x462F71, 0x17);

		// allow Y as a hotkey in render window preferences
		SafeWrite8(0x4136C1, 'Z');
	}

	/* Hook the Script Editor Window */
	SafeWrite32(0x437892, UInt32(ScriptEditCallback));
	SafeWrite32(0x441CB0, UInt32(ScriptEditCallback));
	SafeWrite32(0x509F6B, UInt32(ScriptEditCallback));
	SafeWrite32(0x5C50C8, UInt32(ScriptEditCallback));

	/* allow ctrl S to save */
	WriteRelJump(0x5C3ECD, UInt32(ScriptEditKeypressHook));

	/* fix vanilla bug where modified flag was not reset upon successful saving */
	WriteRelCall(0x5C4974, UInt32(ScriptEdit__Save));

	// Fix for crash (invalid parameter termination) when the "Unable to find variable" warning would exceed the buffer size, credits to nukem
	XUtil::PatchMemory(0xD59DCC, (PBYTE)", Text \"%.240s\"", strlen(", Text \"%.240s\"") + 1);

	if (bUseAltShiftMultipliers) {
		// Scroll wheel and pan speed affected by shift/alt
		WriteRelCall(0x48B8C5, UInt32(hk_DoRenderMousewheelScroll)); // preview window
		WriteRelCall(0x48B74C, UInt32(hk_DoRenderMousewheelScroll)); // preview window
		WriteRelCall(0x48B7AC, UInt32(hk_DoRenderMousewheelScroll)); // preview window pan
		WriteRelCall(0x46040E, UInt32(hk_DoRenderMousewheelScroll)); // render window scroll

		// reference movement speed affected by shift/alt
		WriteRelJump(0x455392, UInt32(RenderWindowReferenceMovementSpeedHook));

		// render window panning, doesn't apply shift multiplier as it conflicts with keybinding
		WriteRelCall(0x45F7E5, UInt32(hk_DoRenderPan));
		WriteRelCall(0x45F846, UInt32(hk_DoRenderPan));

		// orthographic zoom 
		WriteRelJump(0x45F661, UInt32(hk_OrthographicZoom));
		WriteRelJump(0x4602D8, UInt32(hk_OrthographicZoom2));

		// camera rotation with a selected ref
		WriteRelJump(0x45F5FB, UInt32(hk_RefCameraRotation));

		// ref rotation
		WriteRelJump(0x4523C2, UInt32(RenderWindowHandlesRefRotationHook));
	}

	if (bNoVersionControlWarning) {
		// Don't show "Version control is currently disabled. Would you like to continue without version control?" prompt
		SafeWrite8(0x43F854, 0xEB);
	}

	if (bAutoLoadFiles) {
		// makes the load files prompt jump to OK if it's the first time it's opened
		WriteRelJump(0x432CC2, UInt32(hk_LoadFilesInit));
	}

	// add Q/E keys for up/down movement in flycam mode for render window
	// if the movment is applied Post transform, it will be relative to the world
	if (bFlycamUpDownRelativeToWorld) {
		WriteRelJump(0x455DC9, UInt32(RenderWindowFlycamPostTransformMovementHook));
	}
	else {
		WriteRelJump(0x455DAB, UInt32(RenderWindowFlycamPreTransformMovementHook));
	}

	if (bShowTimeOfDaySlider) {
		// hook for setting main menu scrollbar position when changed in the preferences pane
		WriteRelCall(0x44DD69, UInt32(UpdateTimeOfDayScrollbarHook));
		XUtil::PatchMemoryNop(0x44DD69 + 5, 1);
		WriteRelCall(0x44DDF5, UInt32(UpdateTimeOfDayScrollbarHook));
		XUtil::PatchMemoryNop(0x44DDF5 + 5, 1);
		WriteRelCall(0x44D04B, UInt32(UpdateTimeOfDayInputBoxHook));
		XUtil::PatchMemoryNop(0x44D04B + 5, 1);
	}

	if (bShowAdditionalToolbarButtons) {
		// hook preferences window apply button to update the "allow render window cell loads" toolbar button
		WriteRelCall(0x44EA74, UInt32(PreferencesWindowApplyButtonHook));
	}

	// remove broken "Textures" pane from Scene Info Window
	XUtil::PatchMemoryNop(0x4156A8, 5);

	// fix bug where disabling orthogonal mode would cause a shader to be applied to the camera
	XUtil::PatchMemoryNop(0x456BD6, 4);

	// fix missing null check in embedded render window when checking a sound form
	WriteRelJump(0x893576, UInt32(EmbeddedRenderWindowSoundNullCheck));

	if (bSkipVanillaLipGen) {
		// skip topics that aren't modified
		WriteRelJump(0x41EA7B, UInt32(LipGenLoopHook));

		// don't include topics that aren't modified for the progress bar counter
		WriteRelJump(0x41EA30, UInt32(LipGenCountTopicsHook));

		// replace the "Do you want to forcibly recreate all facial animation files?" to for the current plugin
		static char* facialAnimationsPopupText = "Do you want to forcibly recreate all facial animation files for the current plugin?";
		SafeWrite32(0x440C43, UInt32(facialAnimationsPopupText));
	}

	SafeWrite32(0x4411A1, UInt32(RenderWindowCallbackHook));

	// allow toggling of "scroll" in TESObjectBOOK dialog
	SafeWrite8(0x10CD577, 0x50);

	// fix crash when sorting by model type in categories with Base Effects
	SafeWrite16(0x43BA8B, 0x9090);

	// workaround infinite wait if saving fails bug
	WriteRelJump(0x4E1DB9, UInt32(SaveFailureHook));

	// fix null pointer with checking edges when using landscape editor
	WriteRelJump(0x61CA59, UInt32(EditLandCheckLandIsNull));

	if (bAllowEditLandEdges)
	{
		// changes: for(int i = 1; i < uGridsToLoad - 1; ++i);
		// to :		for(int i = 0; i < uGridsToLoad; ++i);

		// north
		SafeWrite8(0x459B93, 0x90);
		SafeWrite8(0x45DADC, 0x90); // flattening 
		SafeWrite8(0x45DEFB, 0); // texture painter

		// east 
		SafeWrite8(0x459B79, 0x90);
		SafeWrite8(0x45DAB9, 0x90); // flattening 
		SafeWrite8(0x45DF84, 0x90);	// texture painter

		// south
		SafeWrite8(0x459AF5, 0);
		SafeWrite8(0x45DA30, 0); // flattening
		SafeWrite8(0x45DF21, 0); // texture painter

		// west
		SafeWrite8(0x459B16, 0);
		SafeWrite8(0x45DA56, 0); // flattening
		SafeWrite8(0x45DFA5, 0x90); // texture painter
	}

	// fix crash when doubling clicking an empty area in a form list
	WriteRelJump(0x501450, UInt32(FormListCheckNull));
	WriteRelJump(0x5AE0A5, UInt32(FormListCheckNull2));

	// give more informative error when "Bad forms are encountered by printing the bad form's formID"
	WriteRelJump(0x4D9577, UInt32(BadFormLoadHook));

	// add option to continue loading if multiple master files are selected
	WriteRelJump(0x4DD2E6, UInt32(MultipleMasterLoadHook));

	if (bNavmeshFindCoverConfirmPrompt)
	{
		WriteRelJump(0x456F22, UInt32(RenderWindowNavMeshConfirmFindCover));
		WriteRelJump(0x444511, UInt32(MainWindowNavMeshConfirmFindCover));
		WriteRelJump(0x40AC6F, UInt32(NavMeshToolbarConfirmFindCover)); // skip 0x40AC87, retn 0x40AC7B
	}

	if (bShowScriptChangeTypeWarning)
	{
		WriteRelCall(0x5C2FC2, UInt32(SaveScriptChangedType));
	}

	SetupHavokPreviewWindow();

	// attempt to save the active plugin to "CrashSave - PLUGINNAME.esp" when crashing (not compatible with NVAC)
	SetCrashSaveHandler();

	// hide markers where appropriate when refreshing a cell
	WriteRelJump(0x458593, UInt32(RefreshCellHook));
	WriteRelJump(0x4585B3, UInt32(RefreshCellHook));
	WriteRelJz(0x4585BF, UInt32(RefreshCellHook));

	// allow resizing the FormList dialog (3274)
	SafeWrite32(0x43768B, UInt32(FormListCallback));

	// allow resizing the objects palette window (375)
	SafeWrite32(0x4440FA, UInt32(ObjectPaletteCallback));

	// allow resizing the Use Report window (220)
	for (UInt32 patchAddr : {0x47F429, 0x4822E3, 0x48280C, 0x483607})
	{
		SafeWrite32(patchAddr, UInt32(UseReportCallback));
	}

	// add modifier CAPSLOCK for placing a random object from the objects palette 
	if (bObjectPaletteAllowRandom)
	{
		WriteRelCall(0x45A6B8, UInt32(PlaceOPALObjectHook));
	}

	// make the preferences window use 4 decimal places for settings
	for (UInt32 patchAddr : {0x44DC03, 0x44DC1D, 0x44DC51, 0x44D483, 0x44D4D5, 0x44D573, 0x44D58D, 0x44D5C1, 0x44D5DB})
	{
		SafeWrite8(patchAddr, 4);
	}

	// make the perk entries value use 4 decimal places
	SafeWrite8(0x548A62, 4);
	SafeWrite8(0x548A8C, 4);
	SafeWrite32(0x548AEA, 0x00D5DB0C);

	if (bSnapToGridRotationUseDoublePrecision)
	{
		// use double precision when calculating reference rotation to fix floating point errors
		WriteRelJump(0x4523E2, UInt32(RenderWindowHandleRefRotationHook));
	}

	if (bFaceGenOnlyEdited)
	{
		char* faceGenMessage = "Export mod face gen textures for all edited NPCs?";
		WriteRelJump(0x442050, UInt32(ExportFaceGenCheckIsFormEdited));
		SafeWrite32(0x442026, UInt32(faceGenMessage));
	}

	//	Create log window - credit to nukem
	InitCommonControls();
	LoadLibraryA("MSFTEDIT.dll");

	if (bDisableTextureMirroring)
	{
		patchTextureMemoryAllocator();
	}

	if (!EditorUI_CreateLogWindow())
	{
		MessageBoxA(nullptr, "Failed to create console log window", "Error", MB_ICONERROR);
	}

	SafeWrite32(0x0044612D, (UInt32)EditorUI_WndProc);
	OldEditorUI_WndProc = (WNDPROC)0x00440780;

	PatchRememberLandscapeEditSettingsWindowPosition();

	PatchClearLandscapeEditUndoStackIfNearlyOOM();

	PatchFasterLipGen();

	PatchCellExtraDataCrash();
	// prevent geck from crashing when using search
	// buffer too small, move to 16 megs - Kormakur
	SafeWrite32(0x485865 + 1, 0x100000);

	// Remove script data size limit in GECK - Kormakur
	WriteRelJump(0x5C92E6, UInt32(Hook_RemoveScriptDataLimit));

	// make weapon mods in object window show the same information as misc items
	SafeWrite8(0x438B94 + kFormType_ItemMod, 0x11);

	if (bPreventFaceAndBodyModExports)
	{
		// prevent .tga files when exporting
		SafeWriteBuf(0x574A0F, "\x83\xC4\x0C\x90\x90", 5); // face mods
		SafeWriteBuf(0x570D6B, "\x83\xC4\x0C\x90\x90", 5); // body mods
		SafeWriteBuf(0x570C50, "\x83\xC4\x0C\x90\x90", 5); // body mods
	}

	if (bIgnoreD3D9)
	{
		// ignore the d3d9.dll from the game's folder since dxvk makes the geck super slow
		SafeWrite32(0xD2318C, (UInt32)hook_LoadLibrary);
	}

	// allow Telekinesis in base effect archtype dropdown
	SafeWrite8(0xEA8EE8, 0x0);

	return true;
}


static int konamiStage = 0;
void doKonami(int key) {
	switch (konamiStage) {
	case 0:
		(key == VK_UP) ? konamiStage++ : konamiStage = 0;
		break;
	case 1:
		(key == VK_UP) ? konamiStage++ : konamiStage = 0;
		break;
	case 2:
		if (key == VK_DOWN) konamiStage++;
		else if (key != VK_UP) konamiStage = 0;
		break;
	case 3:
		(key == VK_DOWN) ? konamiStage++ : konamiStage = 0;
		break;
	case 4:
		(key == VK_LEFT) ? konamiStage++ : konamiStage = 0;
		break;
	case 5:
		(key == VK_RIGHT) ? konamiStage++ : konamiStage = 0;
		break;
	case 6:
		(key == VK_LEFT) ? konamiStage++ : konamiStage = 0;
		break;
	case 7:
		(key == VK_RIGHT) ? konamiStage++ : konamiStage = 0;
		break;
	case 8:
		(key == 'B') ? konamiStage++ : konamiStage = 0;
		break;
	case 9:
		if (key == 'A') {
			EditorUI_Log("Konami!");
		}
		konamiStage = 0;
		break;
	}
	/* handles the case where up is pressed in the middle of the sequence */
	if (konamiStage == 0 && key == VK_UP) konamiStage++;
}

void SaveWindowPositions() {
	SaveWindowPositionToINI(*(HWND*)(0xECFB40), "Render Window");

	char buffer[8];

	WINDOWPLACEMENT pos;
	GetWindowPlacement(g_ConsoleHwnd, &pos);
	WritePrivateProfileString("Log", "iWindowPosX", _itoa(pos.rcNormalPosition.left, buffer, 10), iniName);
	WritePrivateProfileString("Log", "iWindowPosY", _itoa(pos.rcNormalPosition.top, buffer, 10), iniName);
	WritePrivateProfileString("Log", "iWindowPosDX", _itoa(pos.rcNormalPosition.right - pos.rcNormalPosition.left, buffer, 10), iniName);
	WritePrivateProfileString("Log", "iWindowPosDY", _itoa(pos.rcNormalPosition.bottom - pos.rcNormalPosition.top, buffer, 10), iniName);
}

void __fastcall FastExitHook(volatile LONG** thiss)
{
	SaveWindowPositions();
	if (GetINIExists() && bFastExit) TerminateProcess(GetCurrentProcess(), 0);
	((void(__thiscall*)(volatile LONG** thiss))(0x4CC540))(thiss);
}

void __stdcall BadFormPrintID(TESForm* form)
{
	EditorUI_Log("FORMS: Bad form %08X could not be removed from the file.", form->refID);
}

_declspec(naked) void BadFormLoadHook()
{
	static const UInt32 retnAddr = 0x4D957C;
	_asm
	{
		push esi
		call BadFormPrintID
		
		//	originalCode
		mov byte ptr ss : [esp + 0x1A] , 0x1 // wasBadFormEncountered
		jmp retnAddr
	}
}

char** scriptTypes = (char**)0xE9BC6C;
void __cdecl PrintScriptTypeChangedMessage(UInt32 script, UInt8 newType)
{

	UInt8 scriptType = *(UInt8*)(script + 0x3C);
	char* previousTypeStr = scriptTypes[scriptType];
	char* newTypeStr = scriptTypes[newType];

	char errorMsg[100];
	sprintf(errorMsg, "Script Type Changed, Previous: %s - New: %s", previousTypeStr, newTypeStr);
	MessageBoxA(nullptr, errorMsg, "Warning", MB_ICONWARNING);
}

_declspec(naked) void SaveScriptChangedType()
{
	_asm
	{
		push ecx
		push esi
		call PrintScriptTypeChangedMessage
		pop esi
		pop ecx
		mov byte ptr ss : [esp + 0x15] , 1
		ret
	}
}

/* TODO
Creature + Leveled creature/character windows
NPC
GameEffects->BaseEffect
Items->Ammo
Items->LeveledItem
Items->Weapon
SpecialEffects->Explosion
WorldObjects->Container

Dirty editors:
ActorData->Race
Audio->Media Location
Audio->Media Set
Items->ArmorAddon
Items->Weapon

MultiCombo Boxes
0x6017A0
0x49A091
*/