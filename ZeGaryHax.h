#pragma once
#pragma comment(lib, "libdeflate/libdeflatestatic.lib")

#include "Editor.h"
#include "resource.h"

#define GH_NAME				"ZeGaryHax"		// this is the string for IsPluginInstalled and GetPluginVersion (also shows in nvse.log)
#define GH_VERSION			0				// set this to 0 to enable log output from _DMESSAGE (useful for debug traces)

HMODULE ZeGaryHaxHandle;
extern HWND g_trackBarHwnd;
extern HWND g_timeOfDayTextHwnd;
extern HWND g_allowCellWindowLoadsButtonHwnd;
IDebugLog	gLog("EditorWarnings.log");

void PrintCmdTable();
void EditorUI_Log(const char* Format, ...);

struct z_stream_s
{
	const void *next_in;
	uint32_t avail_in;
	uint32_t total_in;
	void *next_out;
	uint32_t avail_out;
	uint32_t total_out;
	const char *msg;
	struct internal_state *state;
};

struct DialogOverrideData
{
    DLGPROC DialogFunc; // Original function pointer
    LPARAM Param;       // Original parameter
    bool IsDialog;      // True if it requires EndDialog()
};

std::recursive_mutex g_DialogMutex;
std::unordered_map<HWND, DialogOverrideData> g_DialogOverrides;
__declspec(thread) DialogOverrideData *DlgData;

char* nvseMSG[20] =
{
	"PostLoad",
	"ExitGame",
	"ExitToMainMenu",
	"LoadGame",
	"SaveGame",
	"Precompile",
	"PreLoadGame",
	"QQQ",
	"PostLoadGame",
	"PostPostLoad",
	"ScriptError",
	"DeleteGame",
	"RenameGame",
	"RenameNewGame",
	"NewGame",
	"DeleteGameName",
	"RenameGameName",
	"RenameNewGameName",
};

int bPatchScriptEditorFont = 0;
int bHighResLandscapeLOD = 0;
int bListEditFix = 0;
int bVersionControlMode = 0;
int bFastExit = 0;
int bIgnoreNAMFiles = 0;
int bEnableSpellChecker = 1;
int bAutoScroll = 1;
int bRenderWindowUncap = 1;
int bPreviewWindowUncap = 1;
int bRemoveStatusBarLoadingText = 1;
int bPlaySoundEndOfLoading = 1;
int bNoDXSoundCaptureErrorPopup = 0;
int bNoPreviewWindowAutoFocus = 1;
int bNoLODMeshMessage = 0;
int bSwapRenderCYKeys = 0;
int bAutoLoadFiles = 0;
int bShowLoadFilesAtStartup = 0;
int bScriptCompileWarningPopup = 0;
int bNoVersionControlWarning = 0;
int bShowTimeOfDaySlider = 1;
int bSkipVanillaLipGen = 0;
int bShowAdditionalToolbarButtons = 0;
int bAllowMultipleSearchAndReplace = 0;
int bNoFactionReactionMessage = 0;
int bUISpeedHooks = 1;
int bLibdeflate = 0;
int bExpandFormIDColumn = 0;
int bAllowEditLandEdges = 0;
int bNavmeshAllowPlaceAboveOthers = 0;
int bAllowRecompileAll = 0;
int bNavmeshFindCoverConfirmPrompt = 0;
int bShowScriptChangeTypeWarning = 0;

int bUseAltShiftMultipliers = 1;
float fMovementAltMultiplier = 0.15F;
float fMovementShiftMultiplier = 2.0F;

int bSmoothFlycamRotation = 1;
int bFlycamUpDownRelativeToWorld = 0;
float fFlycamRotationSpeed;
float fFlycamNormalMovementSpeed;
float fFlycamShiftMovementSpeed;
float fFlycamAltMovementSpeed;

UInt8 g_iPreviewWindowRed;
UInt8 g_iPreviewWindowGreen;
UInt8 g_iPreviewWindowBlue;

char filename[MAX_PATH];
static const char *geckwikiurl = "https://geckwiki.com/index.php/";
static const char *geckwikiscriptingurl = "https://geckwiki.com/index.php/Category:Scripting";
static const char *geckwikicommandsurl = "https://geckwiki.com/index.php/Category:Commands";
static const char *geckwikifunctionsurl = "https://geckwiki.com/index.php/Category:Functions";

// From NVSE Hooks_Editor.cpp
// Patch script window font

static HANDLE	fontHandle;
static LOGFONT	fontInfo;

static LOGFONT editorFont =
{
	13,			// lfHeight
	0,			// lfWidth
	0,			// lfEscapement
	0,			// lfOrientation
	FW_MEDIUM,		// lfWeight
	0,			// lfItalic
	0,			// lfUnderline
	0,			// lfStrikeOut
	0,			// lfCharSet
	3,			// lfOutPrecision
	2,			// lfClipPrecision
	1,			// lfQuality
	49,			// lfPitchAndFamily
	"Consolas"	// lfFaceName[LF_FACESIZE]
};

#define INI_SETTING_NOT_FOUND -1
int GetOrCreateINIInt(char* sectionName, char* keyName, int defaultValue, char* filename) {
	int settingValue = GetPrivateProfileIntA(sectionName, keyName, INI_SETTING_NOT_FOUND, filename);
	if (settingValue == INI_SETTING_NOT_FOUND) {
		char arr[11];
		WritePrivateProfileString(sectionName, keyName, _itoa(defaultValue, arr, 10), filename);
		settingValue = defaultValue;
	}
	return settingValue;
}
#undef INI_SETTING_NOT_FOUND


static void DoModScriptWindow(HWND wnd)
{
	SendMessage(wnd, EM_EXLIMITTEXT, 0, 0x00FFFFFF);

	GetPrivateProfileStringA("Script", "Font", "Consolas", editorFont.lfFaceName, 31, filename);
	editorFont.lfHeight = GetOrCreateINIInt("Script", "FontSize", 13, filename);
	editorFont.lfWeight = GetOrCreateINIInt("Script", "FontWeight", FW_MEDIUM, filename);

	// try something nice, otherwise fall back on SYSTEM_FIXED_FONT
	fontHandle = CreateFontIndirect(&editorFont);
	if (fontHandle)
	{
		fontInfo = editorFont;
	}
	else
	{
		fontHandle = GetStockObject(SYSTEM_FIXED_FONT);
		GetObject(fontHandle, sizeof(fontInfo), &fontInfo);
	}
	SendMessage(wnd, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
	SendMessage(wnd, WM_SETFONT, (WPARAM)fontHandle, 1);
	SendMessage(wnd, EM_SETMODIFY, 1, 0);

	// one tab stop every 16 dialog units
	UInt32	tabStopSize = 16;
	SendMessage(wnd, EM_SETTABSTOPS, 1, (LPARAM)&tabStopSize);
	DeleteObject(fontHandle);
}

static __declspec(naked) void ModScriptWindow()
{
	__asm
	{
		call	CreateWindowExA
		pushad
		push	eax
		call	DoModScriptWindow
		add		esp, 4
		popad
		push	0x0B0001
		push	0x005C4336
		ret
	}
}

static UInt32 pModScriptWindow = (UInt32)&ModScriptWindow;

static void FixEditorFont(void)
{
	// Right after call d:CreateWindowA after aRichedit20a
	UInt32	basePatchAddr = 0x005C432B;

	WriteRelJump(basePatchAddr, pModScriptWindow);
	SafeWrite8(basePatchAddr + 5, 0x90);
}

static __declspec(naked) void FixMultiBounds()
{
	__asm
	{
		cmp		eax, 0x7F7FFFFF
		je		AhhGary
		mov		ecx, dword ptr ds : [eax + 0x0C]
		mov		dword ptr ds : [ecx + 0x08], 0x00000000
	AhhGary:
		ret
	}
}

//	make sure geckcustom.ini is written if bFastExit is enabled - credit to jazzisparis
bool GetINIExists()
{
	char iniPath[MAX_PATH];
	strcpy(iniPath, (const char*)0x00F2CC80);
	strcat(iniPath, "GECKCustom.ini");
	DWORD attr = GetFileAttributes(iniPath);
	return (attr != INVALID_FILE_ATTRIBUTES) && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

HWND __stdcall SplashScreenHook(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	HWND hWnd = CreateDialogParamA(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
	SendMessage(GetDlgItem((HWND)hWnd, 1962), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoadImage(ZeGaryHaxHandle, MAKEINTRESOURCE(IDB_SPLASHBITMAP), IMAGE_BITMAP, 0, 0, 0));
	return hWnd;
}

//	patch about dialog - credit to roy and nukem
HWND __stdcall AboutDialogHook(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	HWND hWnd = CreateDialogParamA(hInstance, lpTemplateName, hWndParent, lpDialogFunc, dwInitParam);
	SendMessage(GetDlgItem((HWND)hWnd, 1963), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoadImage(ZeGaryHaxHandle, MAKEINTRESOURCE(IDB_SPLASHBITMAP), IMAGE_BITMAP, 0, 0, 0));
	return hWnd;
}

//	fix destruction dialog close button - credit to roy
BOOL WINAPI hk_DialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (hWnd && uMsg == WM_CLOSE)
	{
		EndDialog(hWnd, 0);
		DestroyWindow(hWnd);
		return TRUE;
	}
		__asm
		{
			pop esi
			pop ebp
			push 0x004E5E30
			ret
		}
}

//	fix handle memory leak - credit to nukem
INT_PTR WINAPI DialogFuncOverride(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DLGPROC proc = nullptr;

	g_DialogMutex.lock();
	{
		auto itr = g_DialogOverrides.find(hwndDlg);
		if (itr != g_DialogOverrides.end())
			proc = itr->second.DialogFunc;

		//	if (is new entry)
		if (!proc)
		{
			g_DialogOverrides[hwndDlg] = *DlgData;
			proc = DlgData->DialogFunc;

			delete DlgData;
			DlgData = nullptr;
		}

		//	Purge old entries every now and then
		if (g_DialogOverrides.size() >= 50)
		{
			for (auto itr = g_DialogOverrides.begin(); itr != g_DialogOverrides.end();)
			{
				if (itr->first == hwndDlg || IsWindow(itr->first))
				{
					itr++;
					continue;
				}

				itr = g_DialogOverrides.erase(itr);
			}
		}
	}
	g_DialogMutex.unlock();

	return proc(hwndDlg, uMsg, wParam, lParam);
}

HWND WINAPI hk_CreateDialogParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	//	EndDialog MUST NOT be used
	DialogOverrideData *data = new DialogOverrideData;
	data->DialogFunc = lpDialogFunc;
	data->Param = dwInitParam;
	data->IsDialog = false;

	DlgData = data;
	return CreateDialogParamA(hInstance, lpTemplateName, hWndParent, DialogFuncOverride, dwInitParam);
}

INT_PTR WINAPI hk_DialogBoxParamA(HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM dwInitParam)
{
	//	EndDialog MUST be used
	DialogOverrideData *data = new DialogOverrideData;
	data->DialogFunc = lpDialogFunc;
	data->Param = dwInitParam;
	data->IsDialog = true;

	DlgData = data;
	return DialogBoxParamA(hInstance, lpTemplateName, hWndParent, DialogFuncOverride, dwInitParam);
}

BOOL WINAPI hk_EndDialog(HWND hDlg, INT_PTR nResult)
{
	std::lock_guard<std::recursive_mutex> lock(g_DialogMutex);

	//	Fix for the CK calling EndDialog on a CreateDialogParamA window
	auto itr = g_DialogOverrides.find(hDlg);
	if (itr != g_DialogOverrides.end() && !itr->second.IsDialog)
	{
		DestroyWindow(hDlg);
		return TRUE;
	}

	return EndDialog(hDlg, nResult);
}

LRESULT WINAPI hk_SendMessageA(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (hWnd && Msg == WM_DESTROY)
	{
		std::lock_guard<std::recursive_mutex> lock(g_DialogMutex);

		// If this is a dialog, we can't call DestroyWindow on it
		auto itr = g_DialogOverrides.find(hWnd);
		if (itr != g_DialogOverrides.end())
		{
			if (!itr->second.IsDialog)
				DestroyWindow(hWnd);
		}

		return 0;
	}

	return SendMessageA(hWnd, Msg, wParam, lParam);
}

void __stdcall hk_sub_4A1C10(HWND ListViewControl)
{
	((void(__stdcall *)(HWND))(0x004A1C10))(ListViewControl);

	SendMessageA(ListViewControl, LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE_USEHEADER);
	SendMessageA(ListViewControl, LVM_SETCOLUMNWIDTH, 1, LVSCW_AUTOSIZE_USEHEADER);
	SendMessageA(ListViewControl, LVM_SETCOLUMNWIDTH, 2, LVSCW_AUTOSIZE_USEHEADER);
	SendMessageA(ListViewControl, LVM_SETCOLUMNWIDTH, 3, LVSCW_AUTOSIZE_USEHEADER);
	SendMessageA(ListViewControl, LVM_SETCOLUMNWIDTH, 4, LVSCW_AUTOSIZE_USEHEADER);
	SendMessageA(ListViewControl, LVM_SETCOLUMNWIDTH, 5, LVSCW_AUTOSIZE_USEHEADER);
}

//	fix crash with esm as active file - credit to hlp
static __declspec(naked) void hk_addr_42CD31()
{
	__asm
	{
		test ecx, ecx
		jne HahaGary
		xor al, al
		mov dword ptr ss : [esp], 0x0042CD6D
		ret

	HahaGary :
		mov esi, dword ptr ds : [ecx + 0x110]
		mov edx, dword ptr ds : [eax]
		ret
	}
}

//	patch ONAM count - credit to hlp
static __declspec(naked) void hk_addr_4E26BA()
{
	__asm
	{
		mov dword ptr ds : [esi + 0x434], eax
		mov edx, 0x04
		mul edx
		seto cl
		ret
	}
}

//	Fix Rock-It Launcher crash - credit to jazzisparis
TESForm* (*GetFormByID)(UInt32 formID) = (TESForm* (*)(UInt32))0x004F9620;

bool __fastcall GetIsRIL(TESForm *form)
{
	static UInt32 RIL_FormID = 0;
	if (!RIL_FormID)
	{
		RIL_FormID = 0xFFFFFFFF;
		DataHandler *dataHandler = *(DataHandler**)0x00ED3B0C;
		if (dataHandler)
		{
			for (tList<ModInfo>::Iterator infoIter = dataHandler->modList.modInfoList.Begin(); !infoIter.End(); ++infoIter)
			{
				if (_stricmp(infoIter->name, "Fallout3.esm")) continue;
				RIL_FormID = (infoIter->modIndex << 0x18) | 0x434B;
				break;
			}
		}
	}
	return form->refID == RIL_FormID;
}

__declspec(naked) void CheckIsRILHook()
{
	static const UInt32 kRetnAddr = 0x005B8FFD;

	__asm
	{
		mov		ecx, esi
		call	GetIsRIL
		test	al, al
		jz		notRIL
		retn
	notRIL:
		push	edi
		mov		edi, [esp + 0xC]
		jmp		kRetnAddr
	}
}

//	enable/disable spell checker - credit to roy
__declspec(naked) void hk_SpellCheck()
{
	static const UInt32 kRetnAddrYes = 0x0041B613;
	static const UInt32 kRetnAddrNo = 0x0041B61A;

	__asm
	{
		cmp		bEnableSpellChecker, 0
		je		AwwGary
		call	dword ptr ds:[0x00D23548]	//	GetDlgItem
		jmp		kRetnAddrYes
	AwwGary:
		add		esp, 0x0C
		jmp		kRetnAddrNo
	}

}

// make search and replace window stay open unless explicitly close - credit to StewieA
void __declspec(naked) hk_SearchDestroyHook()
{
	static const UINT32 kRetnAddr = 0x0047CE8C;

	_asm
	{
		call dword ptr ds:[0x00D2353C]
		jmp kRetnAddr
	}
}

int hk_inflateInit(z_stream_s *Stream, const char *Version, int Mode)
{
	// Force inflateEnd to error out and skip frees
	Stream->state = nullptr;

	return 0;
}

int hk_inflate(z_stream_s *Stream, int Flush)
{
	size_t outBytes = 0;
	libdeflate_decompressor *decompressor = libdeflate_alloc_decompressor();

	libdeflate_result result = libdeflate_zlib_decompress(decompressor, Stream->next_in, Stream->avail_in, Stream->next_out, Stream->avail_out, &outBytes);
	libdeflate_free_decompressor(decompressor);

	if (result == LIBDEFLATE_SUCCESS)
	{

		Stream->total_in = Stream->avail_in;
		Stream->total_out = (uint32_t)outBytes;

		return 1;
	}

	if (result == LIBDEFLATE_INSUFFICIENT_SPACE)
		return -5;

	return -2;
}

_declspec(naked) void hk_PreviewWindowCheckForeground() {
	static const UInt32 returnAddr = 0x48CACF;

	_asm {
		/* the hWnd was already pushed in esi */
		call dword ptr ds : [0xD2347C] //GetParent

		push ebp
		mov ebp, eax
		call dword ptr ds : [0xD233A0] // GetActiveWindow
		cmp eax, ebp
		pop ebp

		jne skipSetFocus
		push esi
		call dword ptr ds : [0xD234DC] //SetFocus

	skipSetFocus :
		jmp returnAddr
	}
}

char* recompileAllWarning = { "Are you sure you want to recompile every script in every plugin?\nYou should never need to do this." };
_declspec(naked) void RecompileAllWarningScriptHook() {
	static const UInt32 retnAddr = 0x5C498A;
	_asm {
		push 0x104 // change default button to No
		push 0xD2FA78
		push recompileAllWarning
		jmp retnAddr
	}
}
_declspec(naked) void RecompileAllWarningMainHook() {
	static const UInt32 retnAddr = 0x4442D3;
	_asm {
		push 0x104 // change default button to No
		push 0xD2FA78
		push recompileAllWarning
		jmp retnAddr
	}
}

void doKonami(int);

/* small konami easter egg */
BOOL __stdcall hk_LoadESPESMCallback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_NOTIFY && ((LPNMHDR)lParam)->code == LVN_KEYDOWN) {
		doKonami(((LPNMLVKEYDOWN)lParam)->wVKey);
	}
	return ((BOOL(__stdcall *)(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam))(0x432A80))(hDlg, msg, wParam, lParam);
}

BOOL __stdcall hk_SearchAndReplaceCallback(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
	return ((BOOL(__stdcall *)(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam))(0x47C990))(hDlg, msg, wParam, lParam);
}

_declspec(naked) void EndLoadingHook() {
	PlaySound("MouseClick", NULL, SND_ASYNC);
//	PrintCmdTable();
	_asm {
	originalCode:
		add esp, 8
			pop esi
			retn
	}
}

// hooks before movement speed is determined for flycam mode
static int lastFlycamTime = 0;
_declspec(naked) void FlycamMovementSpeedMultiplierHook() {
	_asm
	{
		pushad
	}
	static const UInt32 retnAddr = 0x455D17;
	*(float*)(0xED12C0) = fFlycamNormalMovementSpeed;

	if (GetAsyncKeyState(VK_SHIFT) < 0) {
		*(float*)(0xED12C0) *= fFlycamShiftMovementSpeed;
	}

	if (GetAsyncKeyState(VK_MENU) < 0) {
		*(float*)(0xED12C0) *= fFlycamAltMovementSpeed;
	}

	/* fix flycam speed being dependent on framerate by slowing down movement if framerate is greater than 30fps (33ms/frame)*/
	if (GetTickCount() - lastFlycamTime < 33) {
		*(float*)(0xED12C0) *= (GetTickCount() - lastFlycamTime)/33.0;
	}

	lastFlycamTime = GetTickCount();
	
	_asm 
	{
		popad
	originalCode:
		mov     eax, dword ptr ds:[0xF1FBF4]
		jmp retnAddr
	}
}

// edi       : difference in x pos, 
// esp+0x324 : difference in y pos
// Hook to make flycam rotation speed based on x/y position difference rather than 1.0 for any difference
_declspec(naked) void FlycamRotationSpeedMultiplierHook() {
	static const UInt32 retnAddr = 0x45D3F8;
	_asm {
		mov dword ptr ds : [0x00ED140C], ebx
		
		push edi
		fild dword ptr ss : [esp]
		add esp, 4
		
		fmul fFlycamRotationSpeed
		fstp dword ptr ss : [esp + 0xF0]

		fild dword ptr ss : [esp+0x324]
		fmul fFlycamRotationSpeed
		fstp dword ptr ss : [esp + 0x18]
		jmp retnAddr
	}
}

_declspec(naked) void ReferenceBatchActionDoButtonHook() {
	static const UInt32 retnAddr = 0x411CD4;
	_asm {
		push dword ptr ds : [0xECED38] // replaces a 'push 0' with the selected action
		push 1
		push esi
		jmp retnAddr
	}
}



void __cdecl SaveWindowPositionToINI(HWND hWnd, char* name) {
	((void(__cdecl*)(HWND hWnd, char* Src))(0x43E170))(hWnd, name);
}

/* ideally this would be replaced with a wrapper around the Script Edit callback function */
_declspec(naked) void ScriptEditKeypressHook(HWND hWnd) {
	static const UInt32 skipAddr = 0x5C40EC;
	static const UInt32 saveAddr = 0x5C4961;
	static const UInt32 retnAddr = 0x5C3ED3;
	_asm {
		ja skip
		cmp ebx, 'S'
		jne notSave
		
		cmp byte ptr ds:[esp+0x1F], 0 // check if control pressed
		jz notSave
		
		jmp saveAddr 
		
	skip:
		jmp skipAddr
	notSave:
		jmp retnAddr
	}
}

bool __fastcall ScriptEdit__Save(byte* thiss, void* dummyEDX, HWND hDlg, char a3, char a4) {
	bool saveSuccess = ((bool (__thiscall *)(byte* thiss, HWND hDlg, char a3, char a4))(0x5C2F40))(thiss, hDlg, a3, a4);
	if (saveSuccess) {
		SendDlgItemMessageA(hDlg, 1166, EM_SETMODIFY, 0, 0);
		
		// remove the '*' from the end of the window name
		char windowName[MAX_PATH];
		int len = GetWindowTextA(hDlg, windowName, MAX_PATH)-1;

		if (len > 2 && windowName[len] == '*') {
			windowName[len-1] = '\0'; // strip the '* '
			SetWindowTextA(hDlg, windowName);
		}
	}
	return saveSuccess;
}

float GetRefSpeedMultiplier() {
	float multiplier = 1.0F;
	if (GetAsyncKeyState(VK_SHIFT) < 0) {
		multiplier *= fMovementShiftMultiplier;
	}
	if (GetAsyncKeyState(VK_MENU) < 0) {
		multiplier *= fMovementAltMultiplier;
	}
	return multiplier;
}

char __cdecl hk_DoRenderPan(int a1, int a2, float a3) {
	if (GetAsyncKeyState(VK_MENU) < 0) {
		a3 *= fMovementAltMultiplier;
	}
	return ((char(__cdecl*)(int a1, int a2, float a3))(0x464210))(a1, a2, a3);
}

char __cdecl hk_DoRenderMousewheelScroll(int a1, int a2, float a3) {
	return ((char(__cdecl*)(int a1, int a2, float a3))(0x464210))(a1, a2, a3 * GetRefSpeedMultiplier());
}

double __fastcall hk_CalculateVectorMagnitude(float* vector, void* dummyEDX) {
	return ((double(__thiscall*) (float* thiss))(0x40B3D0))(vector) * GetRefSpeedMultiplier();
}

/* multiply the reference movement speed dependent */
_declspec(naked) void RenderWindowReferenceMovementSpeedHook() {
	static const UInt32 retnAddr = 0x455398;
	_asm {
		pushad
		call GetRefSpeedMultiplier
		fmul
		popad

	originalCode:
		fmul dword ptr ds:[0xECFD1C]
		jmp retnAddr
	}
}

float GetOrthoSpeedMultiplier() {
	if (GetAsyncKeyState(VK_MENU) < 0) {
		return fMovementAltMultiplier;
	}
	return 1.0F;
}

_declspec(naked) void hk_OrthographicZoom() {
	static const UInt32 retnAddr = 0x45F667;
	_asm {
		pushad
		call GetOrthoSpeedMultiplier
		fmul
		popad

	originalCode:
		fmul    dword ptr ds : [0xD2E0D0]
		jmp retnAddr
	}
}

_declspec(naked) void hk_OrthographicZoom2() {
	static const UInt32 retnAddr = 0x4602DE;
	_asm {
		fild dword ptr ss:[esp+0x24]
		fmul dword ptr ds:[eax]

		pushad
		call GetOrthoSpeedMultiplier
		fmul
		popad

		jmp retnAddr 
	}
}

_declspec(naked) void hk_RefCameraRotation() {
	static const UInt32 retnAddr = 0x45F601;
	_asm {
		fld dword ptr [eax]
		
		pushad
		call GetOrthoSpeedMultiplier
		fmul
		popad

		push ecx
		fstp [esp]
		jmp retnAddr
	}
}

bool isFirstInit = true;
_declspec(naked) void hk_LoadFilesInit() {
	static const UInt32 autoLoadAddr = 0x432D7D;
	static const UInt32 noLoadAddr = 0x432E64;

	_asm {
		cmp byte ptr ds:[isFirstInit], 0
		je notFirst
		mov byte ptr ds:[isFirstInit], 0
		jmp autoLoadAddr
	notFirst:
		jmp noLoadAddr
	}
}

/* check if Q or E are held and modify the Z movement speed (stored in esp+0x2C) before it is passed to the view transform */
_declspec(naked) void RenderWindowFlycamPreTransformMovementHook() {
	static const UInt32 retnAddr = 0x455DB1;
	_asm {
		push 'Q'
		call esi
		movzx eax, ax
		test eax, 0x8000
		jz noQ

		fld  dword ptr ss:[esp+0x2C]
		fsub dword ptr ds:[0xED12C0]
		fstp dword ptr ss:[esp+0x2C]
	noQ:
		push 'E'
		call esi
		movzx eax, ax
		test eax, 0x8000
		jz noE

		fld dword ptr ss:[esp + 0x2C]
		fadd dword ptr ds : [0xED12C0]
		fstp dword ptr ss:[esp + 0x2C]
	noE:
		mov esi, dword ptr ds:[0xED116C]
		jmp retnAddr
	}
}

_declspec(naked) void RenderWindowFlycamPostTransformMovementHook() {
	static const UInt32 retnAddr = 0x455DCE;
	_asm {
		push eax
		push VK_LCONTROL
		call dword ptr ds:[0xD234D8] // GetAsyncKeyState
		movzx eax, ax
		test eax, 0x8000
		jz noDown

		pop eax
		fld  dword ptr ss : [eax + 8]
		fsub dword ptr ds : [0xED12C0]
		fstp dword ptr ss : [eax + 8]
		push eax

	noDown:
		push VK_SPACE
		call dword ptr ds : [0xD234D8] // GetAsyncKeyState
		movzx eax, ax
		test eax, 0x8000
		jz noUp

		pop eax
		fld  dword ptr ss : [eax + 8]
		fadd dword ptr ds : [0xED12C0]
		fstp dword ptr ss : [eax + 8]
		push eax

	noUp:
		pop eax
		mov edx, dword ptr ss:[eax]
		mov ecx, dword ptr ss : [eax+4]
		jmp retnAddr
	}
}

LRESULT __stdcall UpdateTimeOfDayScrollbarHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	LRESULT scrollPos = SendMessageA(hWnd, Msg, wParam, lParam);
	SendMessageA(g_trackBarHwnd, TBM_SETPOS, TRUE, scrollPos);
	
	char timeBuf[100];
	sprintf(timeBuf, "%.2f", scrollPos / 4.0F);
	SetWindowTextA(g_timeOfDayTextHwnd, timeBuf);
	
	return scrollPos;
}

void __stdcall UpdateTimeOfDayInputBoxHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	SendMessageA(hWnd, Msg, wParam, lParam);
	SendMessageA(g_trackBarHwnd, TBM_SETPOS, TRUE, lParam);
	
	char timeBuf[100];
	sprintf(timeBuf, "%.2f", lParam / 4.0F);
	SetWindowTextA(g_timeOfDayTextHwnd, timeBuf);
}

_declspec(naked) void LipGenLoopHook() {
	static const UInt32 retnAddr = 0x41EA83;
	static const UInt32 skipAddr = 0x41EE65;
	_asm {
		// ecx contains TESForm
		test byte ptr ss : [ecx + 8], 2 // form->flags & kModified

		je skip

		mov eax, [ecx]
		mov edx, [eax+0xFC]
		jmp retnAddr
		
	skip:
		jmp skipAddr
	}
}

_declspec(naked) void LipGenCountTopicsHook() {
	static const UInt32 retnAddr = 0x41EA35;
	_asm {
		mov esi, dword ptr ss:[eax]
		test esi, esi
		jz done

		// esi contains TESForm
		test byte ptr ss : [esi + 8], 2 // form->flags & kModified

		je done

		inc ecx
	done:
		jmp retnAddr
	}
}

_declspec(naked) void EmbeddedRenderWindowSoundNullCheck() {
	static const UInt32 noSoundFileAddr = 0x89357D;
	static const UInt32 soundFileAddr = 0x8935B5;
	_asm {
		test ecx, ecx
		je noSound
		
		movsx edx, byte ptr ds:[ecx]
		test edx, edx
		je noSound
		
		jmp soundFileAddr

	noSound:
		jmp noSoundFileAddr
	}
}

extern bool GetIsRenderWindowAllowCellLoads();
extern void SetIsShowLightMarkers(bool);
extern bool GetIsShowLightMarkers();
extern void SetFlycamMode(int);
extern int GetFlycamMode();
extern void SetIsShowSoundMarkers(bool);
extern bool GetIsShowSoundMarkers(void);

void __fastcall PreferencesWindowApplyButtonHook(int* thiss, void* dummyEDX, int a2) {
	((int(__thiscall*)(int* thiss, int a2))(0x855B30))(thiss, a2);
	SendMessageA(g_allowCellWindowLoadsButtonHwnd, BM_SETCHECK, GetIsRenderWindowAllowCellLoads(), NULL);
}

BOOL __stdcall RenderWindowCallbackHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_KEYDOWN) {
		switch (wParam) {
		case VK_ESCAPE:
		case VK_LWIN:
			SetFlycamMode(0);
			break;

		case 'I':
			SetIsShowLightMarkers(!GetIsShowLightMarkers());
			break;
/*
		case 'S':
			if (GetFlycamMode() == 0) {
				SetIsShowSoundMarkers(!GetIsShowSoundMarkers());
			}
			break;
			*/
		}
	}
	else if (msg == WM_RBUTTONDOWN) {
		SetFlycamMode(0);
	}
	return ((BOOL(__stdcall*)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam))(0x455AA0))(hWnd, msg, wParam, lParam);
}

extern HWND g_MainHwnd;
void ShowSaveFailureError()
{
	MessageBoxA(g_MainHwnd, "Save failed. Please close any programs with the file open.", "Save Failed", MB_ICONWARNING);
}

_declspec(naked) void SaveFailureHook()
{
	static const UInt32 retnAddr = 0x4E1DBF;
	_asm {
		je noWarn
		pushad
		call ShowSaveFailureError
		popad
	noWarn:
		jmp retnAddr
	}
}

_declspec(naked) void ObjectWindowListFilterUneditedHook()
{
	static const UInt32 skipAddr = 0x439793;
	static const UInt32 retnAddr = 0x43973F;
	_asm {
		// esi contains TESForm
		test byte ptr ss : [esi+8], 2 /// form->flags & kModified
		jne editedForm
		jmp skipAddr

	editedForm:
		mov eax, dword ptr ss : [esp + 0x1C]
		test eax, eax
		jmp retnAddr
	}
}
/*
#define ID_OBJECTWINDOWFILTEREDITED_CHECKBOX 51017
HWND g_objectWindowFilterEditedHwnd;
void ObjectWindow_AddFilterEditedCheckbox(HWND MainWindow, HINSTANCE hInstance) {
	g_objectWindowFilterEditedHwnd = CreateWindowEx(
		NULL,
		"BUTTON",
		"Toggle show only edited",
		BS_PUSHLIKE | BS_CHECKBOX | WS_CHILD | WS_VISIBLE | BS_NOTIFY | BS_BITMAP,
		1160, 4, // x, y
		24, 21, // width, height
		MainWindow,
		(HMENU)ID_OBJECTWINDOWFILTEREDITED_CHECKBOX,
		hInstance,
		NULL
	);
	SendMessageA(g_objectWindowFilterEditedHwnd, BM_SETCHECK, false, NULL);

	HBITMAP hBitmap = (HBITMAP)LoadImage(ZeGaryHaxHandle, MAKEINTRESOURCE(IDB_ALLOW_CELL_LOADS_ICON), IMAGE_BITMAP, NULL, NULL, NULL);
	SendMessageA(g_objectWindowFilterEditedHwnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
}
*/
WNDPROC OldObjectWindowColumnsCallback_WndProc = (WNDPROC)0x449530;
bool __stdcall ObjectWindowColumnsCallback(HWND hWnd, UInt32 msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CREATE)
	{
		const CREATESTRUCT* createInfo = (CREATESTRUCT*)lParam;
//		ObjectWindow_AddFilterEditedCheckbox(hWnd, createInfo->hInstance);
	}
	if (msg == WM_COMMAND)
	{
		const uint32_t param = LOWORD(wParam);
		//		if (param == ID_OBJECTWINDOWFILTEREDITED_CHECKBOX)
		{

		}
	}
	return CallWindowProc(OldObjectWindowColumnsCallback_WndProc, hWnd, msg, wParam, lParam);
}
void __fastcall FastExitHook(volatile LONG** thiss);

_declspec(naked) void CellViewListViewCreateFormIDColumnHook()
{
	static const UInt32 retnAddr = 0x42EFC1;
	_asm
	{
		push 65 // pixels
		push 1
		push LVM_SETCOLUMNWIDTH
		push dword ptr ds : [0xECF548] // g_cellViewRightListHdlg
		call ebx // SendMessageA
		mov ecx, dword ptr ds : [0xECF554]
		jmp retnAddr
	}
}

_declspec(naked) void ObjectWindowListViewColumnSizeHook()
{
	static const UInt32 retnAddr = 0x449660;
	_asm
	{
		push 65 // pixels
		push 2
		push LVM_SETCOLUMNWIDTH
		push edi // g_objectWindowListHdlg
		call ebx // SendMessageA
		
		mov ecx, dword ptr ss : [esp + 0x54]
		push 1
		jmp retnAddr
	}
}

void __cdecl InsertListViewHeaderSetSizeHook(HWND hWnd, WPARAM wParam, int a3, int a4, int a5)
{
	((LRESULT(_cdecl*)(HWND, WPARAM, int, int, int))(0x419F50))(hWnd, wParam, a3, a4, a5);
	SendMessageA(hWnd, LVM_SETCOLUMNWIDTH, wParam, 65);
}

_declspec(naked) void EditLandCheckLandIsNull()
{
	static const UInt32 retnAddr = 0x61CA60;
	_asm
	{
		mov dword ptr ss : [esp + 0x4], ebx
		test eax, eax
		je done
		cmp dword ptr ds : [eax], 0
	done:
		jmp retnAddr
	}
}

_declspec(naked) void FormListCheckNull()
{
	// add check if eax is null before dereferencing it
	static const UInt32 retnAddr = 0x501457;
	_asm
	{
		add esp, 0x4
		test eax, eax
		je done
		mov ecx, dword ptr ds : [eax]
		test ecx, ecx
	done:
		jmp retnAddr
	}
}

void BadFormLoadHook();
void SaveScriptChangedType();

_declspec(naked) void MultipleMasterLoadHook()
{
	static char* MultipleMastersMessage = "Multiple master files selected for load, do you wish to continue? (enable bAllowMultipleMasterLoads to remove this warning)";
	static const UInt32 continueLoadingAddr = 0x4DD394;
	static const UInt32 stopLoadingAddr = 0x4DD2F9;
	_asm
	{
		push MB_TASKMODAL | MB_ICONWARNING | MB_YESNO
		push 0xD41FE4 // "Invalid File Selection"
		push MultipleMastersMessage
		push 0
		call edi // MessageBoxA
		cmp eax, IDYES
		jne stopLoading
		mov byte ptr ds : [0xED3B80], 1 // bAllowMultipleMasterLoads
		jmp continueLoadingAddr
	stopLoading:
		jmp stopLoadingAddr
	}
}

__declspec(naked) void RenderWindowHandlesRefRotationHook()
{
	static const UInt32 retnAddr = 0x4523D0;
	_asm
	{
		call GetRefSpeedMultiplier // no need to push/popad at this location
		fild dword ptr ss : [esp + 0x114]
		fmul
		test byte ptr ds : [0x00ECFCEC] , 0x2
		jmp retnAddr
	}
}

const char* NavmeshConfirmMessage = "Are you sure you want to find cover edges?";
_declspec(naked) void RenderWindowNavMeshConfirmFindCover()
{
	static const UInt32 skipAddr = 0x462C7A;

	_asm
	{
		push MB_TASKMODAL | MB_ICONWARNING | MB_YESNO
		push 0xD2D1F4 // "Navmesh: Find Cover"
		push NavmeshConfirmMessage
		push 0
		call MessageBoxA
		cmp eax, IDNO
		je skip
	doNavmesh:
		push 0
		mov ecx, 0xF073F8
		push 0x456F2E // retnAddr
		mov eax, 0x6F51B0 // Navmesh::FindCoverEdges
		jmp eax
	skip:
		jmp skipAddr
	}
}

_declspec(naked) void MainWindowNavMeshConfirmFindCover()
{
	static const UInt32 skipAddr = 0x44382F;

	_asm
	{
		push MB_TASKMODAL | MB_ICONWARNING | MB_YESNO
		push 0xD2D1F4 // "Navmesh: Find Cover"
		push NavmeshConfirmMessage
		push 0
		call MessageBoxA
		cmp eax, IDNO
		je skip
	doNavmesh:
		push 0
		mov ecx, 0xF073F8
		push 0x44451D // retnAddr
		mov eax, 0x6F51B0 // Navmesh::FindCoverEdges
		jmp eax
	skip:
		jmp skipAddr
	}
}

_declspec(naked) void NavMeshToolbarConfirmFindCover()
{
	static const UInt32 skipAddr = 0x40AC87;

	_asm
	{
		push MB_TASKMODAL | MB_ICONWARNING | MB_YESNO
		push 0xD2D1F4 // "Navmesh: Find Cover"
		push NavmeshConfirmMessage
		push 0
		call MessageBoxA
		cmp eax, IDNO
		je skip
	doNavmesh:
		push 0
		mov ecx, 0xF073F8
		push 0x40AC7B // retnAddr
		mov eax, 0x6F51B0 // Navmesh::FindCoverEdges
		jmp eax
	skip:
		jmp skipAddr
	}
}

void __fastcall InitPreviewWindowBackgroundColor(void* window, void* edx, UInt32 unusedColor)
{
	UInt8 r = g_iPreviewWindowRed;
	UInt8 g = g_iPreviewWindowGreen;
	UInt8 b = g_iPreviewWindowBlue;
	UInt32 color = r + (g << 8) + (b << 16);
	ThisStdCall(0x4793D0, window, color);
}

static double havokAnimationRate = 1000.0;
void SetHavokAnimationSpeed(float speed)
{
	havokAnimationRate = 1000.0 / speed;
}

#define ID_ANIMATIONSPEED_TRACKBAR 1337
#define ID_ANIMATIONSPEED_EDIT 1338
#define ID_ANIMATION_STATIC 1339
BOOL __stdcall HavokPreviewCallback(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {

	if (Message == WM_COMMAND)
	{
		const uint32_t param = LOWORD(wParam);
		switch (param)
		{
		case ID_ANIMATIONSPEED_EDIT:
			if (GetFocus() != (HWND)lParam) break;

			if (HIWORD(wParam) == EN_MAXTEXT) break;

			char text[16];
			int len = SendMessageA((HWND)lParam, EM_GETLINE, 0, (LPARAM)text);
			text[len] = '\0'; // null terminate the line since EM_GETLINE does not

			float speed = atof(text);
			SetHavokAnimationSpeed(speed);
			SendMessageA(GetDlgItem(hWnd, ID_ANIMATIONSPEED_TRACKBAR), TBM_SETPOS, TRUE, speed * 50); // update scrollbar
			return 0;
		}
	} 
	else if (Message == WM_HSCROLL)
	{
		if ((HWND)lParam == GetDlgItem(hWnd, ID_ANIMATIONSPEED_TRACKBAR))
		{
			char timeBuf[100];

			const uint32_t param = LOWORD(wParam);

			if (param == SB_THUMBPOSITION || param == SB_THUMBTRACK || param == SB_ENDSCROLL)
			{
				float speed;
				if (param == SB_ENDSCROLL) speed = SendDlgItemMessageA(hWnd, ID_ANIMATIONSPEED_TRACKBAR, TBM_GETPOS, 0, 0);
				else speed = HIWORD(wParam);

				speed *= 0.02; // convert speed from 0-100 to 0-2

				SetHavokAnimationSpeed(speed);

				sprintf(timeBuf, "%.2f", speed);
				SetWindowTextA(GetDlgItem(hWnd, ID_ANIMATIONSPEED_EDIT), timeBuf);
			}
			return 0;
		}
	}
	else if (Message == WM_INITDIALOG)
	{
		HWND trackbar = GetDlgItem(hWnd, ID_ANIMATIONSPEED_TRACKBAR);
		SendMessageA(trackbar, TBM_SETPOS, true, 50);

		SendMessageA(trackbar, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
		SendMessageA(trackbar, TBM_SETTICFREQ, 10, 0);

		char* defaultTime = "1.00";
		SendDlgItemMessageA(hWnd, ID_ANIMATIONSPEED_EDIT, WM_SETTEXT, NULL, (LPARAM)defaultTime);

		g_iPreviewWindowRed = GetOrCreateINIInt("Preview Window", "iBackgroundRed", 127, filename);
		g_iPreviewWindowGreen = GetOrCreateINIInt("Preview Window", "iBackgroundGreen", 127, filename);
		g_iPreviewWindowBlue = GetOrCreateINIInt("Preview Window", "iBackgroundBlue", 127, filename);
		int landHeight = GetOrCreateINIInt("Preview Window", "iLandHeight", 50, filename);

		SendDlgItemMessageA(hWnd, 2543, TBM_SETPOS, 1u, landHeight);
	}
	else if (Message == WM_DESTROY)
	{
		int red = GetDlgItemInt(hWnd, 1109, 0, 0); 
		int green = GetDlgItemInt(hWnd, 1033, 0, 0);
		int blue = GetDlgItemInt(hWnd, 1111, 0, 0);
		char arr[11];
		WritePrivateProfileString("Preview Window", "iBackgroundRed", _itoa(red, arr, 10), filename);
		WritePrivateProfileString("Preview Window", "iBackgroundGreen", _itoa(green, arr, 10), filename);
		WritePrivateProfileString("Preview Window", "iBackgroundBlue", _itoa(blue, arr, 10), filename);

		int landHeight = SendDlgItemMessageA(hWnd, 2543, TBM_GETPOS, 0, 0);
		WritePrivateProfileString("Preview Window", "iLandHeight", _itoa(landHeight, arr, 10), filename);
	}

	return ((BOOL(__stdcall*)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam))(0x4107F0))(hWnd, Message, wParam, lParam);
}

void MoveDlgItem(HWND hWnd, int ID, int deltaX, int deltaY)
{
	RECT rect;
	POINT point;
	HWND element = GetDlgItem(hWnd, ID);
	GetWindowRect(element, &rect);
	point.x = rect.left;
	point.y = rect.top;
	ScreenToClient(hWnd, &point);
	LONG height = rect.bottom - rect.top;
	LONG width = rect.right - rect.left;
	SetWindowPos(element, NULL, point.x + deltaX, point.y + deltaY, width, height, 0);
}

RECT* previousHavokWindowRect = (RECT*)0xECECBC;
void __cdecl HavokPreviewResize(HWND hWnd)
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	LONG deltaX = clientRect.right - previousHavokWindowRect->right;
	LONG deltaY = clientRect.bottom - previousHavokWindowRect->bottom;
	
	// move the trackbar and edit dialogs
	MoveDlgItem(hWnd, ID_ANIMATIONSPEED_TRACKBAR, deltaX/2, deltaY);
	MoveDlgItem(hWnd, ID_ANIMATIONSPEED_EDIT, deltaX/2, deltaY);
	MoveDlgItem(hWnd, ID_ANIMATION_STATIC, deltaX/2, deltaY);
	InvalidateRect(hWnd, &clientRect, true);
	// call original function
	((void(__cdecl*)(HWND))(0x40F930))(hWnd);
}

void SetupHavokPreviewWindow()
{
	SafeWrite32(0x441162, UInt32(HavokPreviewCallback));
	SafeWrite32(0x444EF7, UInt32(HavokPreviewCallback));
	SafeWrite32(0x44BB7C, UInt32(HavokPreviewCallback));

	SafeWrite32(0x4109E3, UInt32(&havokAnimationRate));
	SafeWrite32(0x410A50, UInt32(&havokAnimationRate));
	SafeWrite32(0x40FCE2, UInt32(&havokAnimationRate));
	SafeWrite32(0x40FD2C, UInt32(&havokAnimationRate));

	WriteRelCall(0x410868, UInt32(HavokPreviewResize));

	// fix preview window RGB labels
	SafeWrite8(0x10E6600, 'G');
	SafeWrite8(0x10E6680, 'B');
	SafeWrite8(0x10E6960, 'R');

	// read background color from ini
	WriteRelCall(0x4100A0, UInt32(InitPreviewWindowBackgroundColor));

	// make pause button not forget particle position (still fails occasionally)
	SafeWrite32(0x410C2A, 0xCCEB006A); // jump over calls setting dword ECECD8

	// skip setting ground height taskbar pos as it's done in the callback instead
	SafeWrite16(0x40FF78, 0x13EB);
}
