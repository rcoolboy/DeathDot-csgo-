// DeathDot.cpp : Defines the entry point for the application.

#include "stdafx.h"

//bClient 29C90000
DWORD bClient, bEngine, LocalBase, mTeam;	// Defaults
#pragma region OFFSETS
DWORD dwClientState = 0x58BCFC;		// dwClientState
DWORD dwViewAngles = 0x4D10;		// dwViewAngles
DWORD LocalPlayer = 0xCBD6B4;		// dwLocalPlayer
DWORD oFlags = 0x104;				// m_fFlags
DWORD forceJump = 0x5170DB0;		// dwForceJump
DWORD forceAttack = 0x30FF2A0;		// dwForceAttack
DWORD dwMouseEnable = 0xCC3200;		// dwMouseEnable
DWORD bSpotted = 0x93D;				// m_bSpotted
DWORD bSpottedMask = 0x980;			// m_bSpottedByMask
DWORD EntityList = 0x4CCDBFC;		// dwEntityList
DWORD iTeam = 0xF4;					// m_iTeamNum
DWORD oDormant = 0xED;				// m_bDormant
DWORD iHealth = 0x100;				// m_iHealth
DWORD bIsDefusing = 0x3914;			// m_bIsDefusing 
DWORD mMoveType = 0x25C;			// m_MoveType
DWORD mVecVelocity = 0x114;			// m_vecVelocity
DWORD glowObject = 0x520DA28;		// dwGlowObjectManager
DWORD glowIndex = 0xA3F8;			// m_iGlowIndex
DWORD iCrosshairId = 0xB394;		// m_iCrosshairId
DWORD flFlashDuration = 0xA3E0;		// m_flFlashDuration
DWORD flFlashMaxAlpha = 0xA3DC;		// m_flFlashMaxAlpha
DWORD BoneMatrix = 0x26A8;			// m_dwBoneMatrix
//new
DWORD m_Local = 0x2FBC;
DWORD aipAngles = 0x302C;			// m_aimPunchAngle  0x4D10 / 0x302C
DWORD vpAngle = 0x3020;				// m_viewPunchAngle 0x302C
DWORD pSens = 0xCC309C;				// dwSensitivity
DWORD modelAmbient = 0x58ED1C;		// model_ambient_min
DWORD vecOrigin = 0x138;			// m_vecOrigin
DWORD vecViewOffset = 0x108;		// m_vecViewOffset
DWORD dwbSendPackets = 0xD20EA;		// dwbSendPackets - for lag
DWORD m_clrRender = 0x70;			// m_clrRender - for chams
#pragma endregion
CMemoryManager* _mm;

struct Vector
{
	float x, y, z;
};
struct Vector3
{
	float x, y, z;
};

#pragma region ESP
class ESP {

	private:
		bool isWall = false;
		bool isRadar = false;
		bool isCham = false;
		bool isRunning;

	struct GlowBase
	{
		float r;
		float g;
		float b;
		float a;
		uint8_t unk1[16];
		bool m_bRenderWhenOccluded;
		bool m_bRenderWhenUnoccluded;
		bool m_bFullBloom;
		uint8_t unk2[10];
	};
	/*struct PlayerStruct {
		int Team;
		bool Ignore;
		bool Dormant;
		bool Spotted;
		int	Health;
		int	GlowIndex;
		int	Base;
		Vector Pos;
		float Angle[3];
	};*/
	struct BoneBase {
		byte padding[12];
		float x;
		byte padding2[12];
		float y;
		byte padding3[12];
		float z;
	};
	struct cham_s
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};
	//PlayerStruct Player[33][3];
	DWORD brightness = 255;
	cham_s clr;

	void glowPlayer(DWORD client, GlowBase entity, DWORD entityadr, int Health) {
		entity.r = 1.f -(float)(Health / 100.f);
		entity.g = (float)(Health / 100.f);
		entity.b = 0.f;
		entity.a = 1.f;
		entity.m_bRenderWhenOccluded = true;
		entity.m_bRenderWhenUnoccluded = false;
		entity.m_bFullBloom = false;
		_mm->w(entityadr + 0x4, entity);
	}
	
	void Glows() {
		DWORD GlowObject;
		_mm->r<DWORD>(bClient + glowObject, GlowObject);
		_mm->r<DWORD>(bClient + LocalPlayer, LocalBase);
		_mm->r<DWORD>(LocalBase + iTeam, mTeam);
		for (int i = 1; i < 65; i++) {
				DWORD player;
			_mm->r<DWORD>(bClient + EntityList + ((i - 1) * 0x10), player);
		if (player == 0 && player == LocalBase) continue;
				bool pDormant;
			_mm->r<bool>(player + oDormant, pDormant);
		if (pDormant) continue;
				DWORD pTeam;
			_mm->r<DWORD>(player + iTeam, pTeam);
		if (mTeam == pTeam) continue;
				bool GlowIndex = false;
			_mm->r<bool>((player + glowIndex), GlowIndex);
				GlowBase entity;
				DWORD HP;
			_mm->r<GlowBase>(GlowObject + ((GlowIndex) * 0x38) + 0x4, entity);
				DWORD entityadr = GlowObject + ((GlowIndex) * 0x38);
			_mm->r<DWORD>((player + iHealth), HP);
			glowPlayer(bClient, entity, entityadr, HP);
		}
	}
	void Radar() {
		for (int i = 1; i < 65; i++) {
			DWORD playerRadar;
			_mm->r<DWORD>(bClient + EntityList + ((i - 1) * 0x10), playerRadar);
			_mm->w(playerRadar + bSpotted, 1);
		}
	}
	void Chams(int x) {
		for (int i = 0; i < 65; i++) {
			DWORD player, pTeam,mTeam; 
			_mm->r<DWORD>(bClient + EntityList + ((i - 1) * 0x10), player);
			DWORD Health;
			bool bDormant;
			_mm->r<DWORD>((player + iHealth), Health);
			_mm->r<bool>(player + oDormant, bDormant);
			if (bDormant) continue;
			if (Health <= 0) continue;
			_mm->r<DWORD>(player + iTeam, pTeam);
			_mm->r<DWORD>(LocalBase + iTeam, mTeam);
			if (x == 0) {
				clr.r = pTeam == mTeam ? 0 : 255;
				clr.g = pTeam == mTeam ? 255 : 255;
				clr.b = pTeam == mTeam ? 0 : 0;
				clr.a = 255;
			}
			else {
				clr.r = 255;
				clr.g = 255;
				clr.b = 255;
				clr.a = 255;
			}
			_mm->w(player + m_clrRender, clr);
		}
		auto thisPtr = (int)(bEngine + modelAmbient - 0x2c);
		auto xored = *(DWORD*)&brightness ^ thisPtr;
		_mm->w(bEngine + modelAmbient, xored);
	}

public:
	ESP(int which = 0) {

		if (which == 1) {
			this->isWall = true;
		}
		if (which == 2) {
			this->isRadar = true;
		}
		if (which == 3) {
			this->isCham = true;
		}
	}
	bool State() {
		return this->isRunning;
	}
	bool StateGlow() {
		return this->isWall;
	}
	bool StateRadar() {
		return this->isRadar;
	}
	bool StateCham() {
		return this->isCham;
	}

	void StartChams() {
		this->isRunning = true;
		while (this->isRunning) {
			if (GetAsyncKeyState(VK_F7) & 0x8000) {
				this->isCham = !this->isCham;
				if (this->isCham) {
					Chams(0);
				}
				else 
				{
					Chams(1);//off chams
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
	void StartRadar() {
		this->isRunning = true;
		while (this->isRunning) {
			if (GetAsyncKeyState(VK_F5) & 0x8000) {
				this->isRadar = !this->isRadar;
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
			if (this->isRadar) {
				Radar();
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		}
	}

	void StartGlow() {
		this->isRunning = true;
		while (this->isRunning) {
			if (GetAsyncKeyState(VK_F6) & 0x8000) {
				this->isWall = !this->isWall;
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
			if (this->isWall) {
				Glows();
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			else 
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		}
	}

	void Stop() {
		this->isRunning = false;
	}
};
#pragma endregion
#pragma region Features
class Feauters {
private:
	bool isRunning;
	bool isFeauters;
	bool isFlash = false;
	bool isFakeLag = false;
	bool isRecoil;
public:
	Feauters(bool isFeauters = false) {
		this->isFeauters = isFeauters;//on off in thread feauters
	}
	void ChangeState(int x, bool q) {
		if (x == 1)
			this->isRecoil = q;
	}
	bool State(int x) {//fornow
		if (x == 1)
			return this->isRecoil;
		else if (x == 2)
			return this->isFakeLag;
		else if (x == 3)
			return this->isFlash;
		else
			return this->isFeauters;
	}
	void StartFakeLag() {
		this->isRunning = true;
		while (this->isRunning) {
			if (isFeauters) {
				if (GetAsyncKeyState(VK_F10) & 0x8000) {
					this->isFakeLag = !this->isFakeLag;
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
				}
				if (GetAsyncKeyState(VK_LMENU) & 0x8000 && this->isFakeLag)
				{
					//niepowinno dzialac
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
					_mm->w(bEngine + dwbSendPackets, 0);
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
					_mm->w(bEngine + dwbSendPackets, 1);
				}
				else {
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
				}
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
		}
	}

	void StartFlash() {
		this->isRunning = true;
		bool reversed = false;
		while (this->isRunning) {
			if (isFeauters) {
				if (GetAsyncKeyState(VK_F9) & 0x8000) {
					this->isFlash = !this->isFlash;
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
				}
				if (this->isFlash)
				{
						if(reversed == true)
							reversed = false;
					_mm->w(bClient + flFlashMaxAlpha, 0.5f);//lower flash to see there is flash on you
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
				}
				else 
				{
					if(reversed == true)
						_mm->w(bClient + flFlashMaxAlpha, 1.f);
				}
				
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
		}
	}
	float flAngleNormalize(float angle)
	{
		while (angle < -180)    angle += 360;
		while (angle > 180)    angle -= 360;
		return angle;
	}
	Vector ClampAngle(Vector angles) {
		angles.y = flAngleNormalize(angles.y);
		angles.x = flAngleNormalize(angles.x);
		if (angles.x > 89.0f)
			angles.x = 89.0f;
		if (angles.x < -89.0f)
			angles.x = -89.0f;

		angles.z = 0;
		return angles;

	}
	void StartRecoil() {
		this->isRunning = true;
		int ShootingState = false;
		int SleepTime = 2;
		clock_t LastKeyPress = 0;
		Vector OldAimPunch;
		Vector NewViewAngles = { 0, 0, 0 };
		Vector CurrentViewAngles = { 0, 0, 0 };
		Vector Aimangles = { 0, 0, 0 };
		Vector AimPunch = { 0, 0, 0 };
		while (this->isRunning) {
			
			if (GetAsyncKeyState(VK_F9) & 0x8000)
			{
				this->isRecoil = !this->isRecoil;
				std::this_thread::sleep_for(std::chrono::milliseconds(150));
			}

			if (this->isRecoil)
			{
				_mm->r(bClient + forceAttack, ShootingState);
				if (ShootingState != 5)
				{
					OldAimPunch.x = OldAimPunch.y = OldAimPunch.z = 0;
					continue;
				} else {

					AimPunch = _mm->rr<Vector>(LocalBase + aipAngles);
					Aimangles = _mm->rr<Vector>(LocalBase + vpAngle);
					AimPunch.x = AimPunch.x;
					AimPunch.y = AimPunch.y;
					AimPunch.z = 0;
					Aimangles.x = Aimangles.x;
					Aimangles.y = Aimangles.y;
					Aimangles.z = 0;
					//_mm->rw<Vector>(LocalBase + aipAngles, AimPunch);
					//_mm->rw<Vector>(LocalBase + vpAngle, Aimangles);
					/*INPUT Input = { 0 };
					Input.type = INPUT_MOUSE;
					Input.mi.dwFlags = MOUSEEVENTF_MOVE;
					Input.mi.dx = -AimPunch.y;
					Input.mi.dy = -AimPunch.x;
					SendInput(1, &Input, sizeof(INPUT));*/
						auto C_State = _mm->rr<DWORD>(bEngine + dwClientState);
					_mm->r(C_State + dwViewAngles, CurrentViewAngles);
					std::wcout << CurrentViewAngles.x << " v " << CurrentViewAngles.z << std::endl;
					NewViewAngles.x = ((CurrentViewAngles.x) - AimPunch.x);
					NewViewAngles.y = ((CurrentViewAngles.y) - AimPunch.y);
					NewViewAngles = ClampAngle(NewViewAngles);
					OldAimPunch = AimPunch;
					_mm->rw<Vector>(C_State + dwViewAngles, NewViewAngles);
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}
	
	
	}
	void Stop() {
		this->isRunning = false;
	}
};
#pragma endregion
#pragma region Bhop
class BHOP {
private:
	bool isBhop;
	bool isRunning;
public:
	BHOP(bool isBhop = false) {
		this->isBhop = isBhop;
	}
	bool State() {
		return this->isBhop;
	}
	void Start() {
		this->isRunning = true;
		while (this->isRunning) {
			if (GetAsyncKeyState(VK_F8) & 0x8000) {
				this->isBhop = !this->isBhop;
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
			if (GetAsyncKeyState(VK_SPACE) & 0x8000 && this->isBhop)
			{
				BYTE mouseEnable = 0;
				_mm->r<DWORD>(bClient + LocalPlayer, LocalBase);
				_mm->r<BYTE>(bClient + dwMouseEnable, mouseEnable);

				if (mouseEnable == 72)
					continue;

				Vector velocity;
				_mm->r<Vector>(LocalBase + mVecVelocity, velocity);

				if (sqrtf(velocity.x * velocity.x + velocity.y * velocity.y) < 1.f)
					continue;

				DWORD movetype = 0;
				_mm->r<DWORD>(LocalBase + mMoveType, movetype);

				if (movetype == 8 || movetype == 9) // MOVETYPE_NOCLIP OR MOVETYPE_LADDER
					continue;

				BYTE fFlags = 0;
				_mm->r<BYTE>(LocalBase + oFlags, fFlags);

				if (fFlags & (1 << 0)) // Check for FL_ONGROUND
					_mm->w(bClient + forceJump, 6); // Will force jump for 1 tick only
				
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
	}
	void Stop() {
		this->isRunning = false;
	}
};
#pragma endregion

ESP Esp;
ESP EspGlow;
ESP EspRadar;
ESP EspChams;
//Feauters Recoil;
//Feauters FakeLag;
BHOP Bhop;

int main()
{
	_setmode(_fileno(stdout), _O_U16TEXT);
	HWND console = GetConsoleWindow();
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);//for colors
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions
	MoveWindow(console, r.left, r.top, 270, 250, TRUE);
	SetConsoleTitle(_T("iksuDeathDot"));
	SetConsoleTextAttribute(hConsole, 10);
	std::wcout << "- Initializing";
	while (!FindWindow(NULL, "Counter-Strike: Global Offensive"))
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::wcout << ".";
	try {
		_mm = new CMemoryManager("csgo.exe");
	}
	catch (...) {
		SetConsoleTextAttribute(hConsole, 12);
		std::wcout << "- Not found exe" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		return 0;
	}
	SetConsoleTextAttribute(hConsole, 10);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::wcout << ".";
	while ((!_mm->GrabModule("client_panorama.dll") || !_mm->GrabModule("engine.dll"))) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
	}
	for (auto m : _mm->GetModules())
	{
		if (!strcmp(m.szModule, "client_panorama.dll"))
		{
			bClient = reinterpret_cast<DWORD>(m.modBaseAddr);
			break;
		}
	}
	for (auto m : _mm->GetModules())
	{
		if (!strcmp(m.szModule, "engine.dll"))
		{
			bEngine = reinterpret_cast<DWORD>(m.modBaseAddr);
			break;
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	std::wcout << "." << std::endl;
	std::wcout << "- Initialization finished";
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	
	system("cls");
	Beep(1000, 100);
	// initialization
	EspRadar = ESP(2);//rads
	EspGlow = ESP(1);//esp
	//Recoil = Feauters(true);
	Bhop = BHOP(false);//bhop
	// made thread segregation
	std::thread tGlow(&ESP::StartGlow, &EspGlow);
	std::thread tRadar(&ESP::StartRadar, &EspRadar);
	std::thread tChams(&ESP::StartChams, &EspChams);
	//std::thread tRecoil(&Feauters::StartRecoil, &Recoil);
	std::thread tBhop(&BHOP::Start, &Bhop);
	//Drawing menu		
		SetConsoleTextAttribute(hConsole, 2);
	std::wcout << L" ╚══╗[ iksuDeathDot ]╔══╝" << std::endl;
	std::wcout << L"╔═══════════════╤═══════╗" << std::endl;
	//std::wcout << L"║ F9 - Recoil   │   ";
	//	SetConsoleTextAttribute(hConsole, 3);
	//std::wcout << Recoil.State(1);
	//	SetConsoleTextAttribute(hConsole, 2);
	//std::wcout << L"   ║" << std::endl;
	std::wcout << L"║ F5 - Radar    │   ";
		SetConsoleTextAttribute(hConsole, 3);
	std::wcout << EspRadar.StateRadar();
		SetConsoleTextAttribute(hConsole, 2);
	std::wcout << L"   ║" << std::endl;
	std::wcout << L"║ F6 - Glow     │   ";
		SetConsoleTextAttribute(hConsole, 3);
	std::wcout << EspGlow.StateGlow();
		SetConsoleTextAttribute(hConsole, 2);
	std::wcout << L"   ║" << std::endl;
	std::wcout << L"║ F7 - Chams    │   ";
		SetConsoleTextAttribute(hConsole, 3);
	std::wcout << EspChams.StateCham();
		SetConsoleTextAttribute(hConsole, 2);
	std::wcout << L"   ║" << std::endl;
	std::wcout << L"║ F8 - Bhop     │   ";
	SetConsoleTextAttribute(hConsole, 3);
	std::wcout << Bhop.State();
	SetConsoleTextAttribute(hConsole, 2);
	std::wcout << L"   ║" << std::endl;
	/*std::wcout << L"║ F9 - Flash    │   ";
		SetConsoleTextAttribute(hConsole, 3);
	std::wcout << NoFlash.State(1);
		SetConsoleTextAttribute(hConsole, 2);
	std::wcout << L"   ║" << std::endl;
	std::wcout << L"║ F10 - F_LAG   │   ";
		SetConsoleTextAttribute(hConsole, 3);
	std::wcout << FakeLag.State(2);
		SetConsoleTextAttribute(hConsole, 2);
	std::wcout << L"   ║" << std::endl;*/
	std::wcout << L"╚═══════════════╧═══════╝" << std::endl;
//	std::wcout << bClient +  << std::endl;



	while (true)
	{
		//refresh states of settings
		if ((GetAsyncKeyState(VK_F5) & 0x8000) ||
			(GetAsyncKeyState(VK_F6) & 0x8000) ||
			(GetAsyncKeyState(VK_F7) & 0x8000) ||
			(GetAsyncKeyState(VK_F8) & 0x8000))
		{
			system("cls");
			SetConsoleTextAttribute(hConsole, 2);
			std::wcout << L" ╚══╗[ iksuDeathDot ]╔══╝" << std::endl;
			std::wcout << L"╔═══════════════╤═══════╗" << std::endl;
			//std::wcout << L"║ F9 - Recoil   │   ";
			//SetConsoleTextAttribute(hConsole, 3);
			//std::wcout << Recoil.State(1);
			//SetConsoleTextAttribute(hConsole, 2);
			//std::wcout << L"   ║" << std::endl;
			std::wcout << L"║ F5 - Radar    │   ";
			SetConsoleTextAttribute(hConsole, 3);
			std::wcout << EspRadar.StateRadar();
			SetConsoleTextAttribute(hConsole, 2);
			std::wcout << L"   ║" << std::endl;
			std::wcout << L"║ F6 - Glow     │   ";
			SetConsoleTextAttribute(hConsole, 3);
			std::wcout << EspGlow.StateGlow();
			SetConsoleTextAttribute(hConsole, 2);
			std::wcout << L"   ║" << std::endl;
			std::wcout << L"║ F7 - Chams    │   ";
			SetConsoleTextAttribute(hConsole, 3);
			std::wcout << EspChams.StateCham();
			SetConsoleTextAttribute(hConsole, 2);
			std::wcout << L"   ║" << std::endl;
			std::wcout << L"║ F8 - Bhop     │   ";
			SetConsoleTextAttribute(hConsole, 3);
			std::wcout << Bhop.State();
			SetConsoleTextAttribute(hConsole, 2);
			std::wcout << L"   ║" << std::endl;
			std::wcout << L"╚═══════════════╧═══════╝" << std::endl;
		}
		if (GetAsyncKeyState(VK_DELETE) & 0x8000)
		{
			std::wcout << "> Closing!" << std::endl;
			Beep(700, 50);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			Beep(700, 50);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			Beep(700, 250);
			Esp.Stop();
			EspRadar.Stop();
			EspGlow.Stop();
			EspChams.Stop();
			Bhop.Stop();
			tGlow.join();
			tRadar.join();
			tChams.join();
			tBhop.join();
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			break;
		}
		//Vector AimPunch;
		//float punch1,punch2,punch3, punch4;
		//_mm->r(bClient + LocalPlayer + 0x2FBC + 0x302C, punch1);
		//_mm->r(bClient + LocalPlayer + 0x302C, punch2);
		//_mm->r(bClient + LocalPlayer + 0x2FBC + 0x3020, punch3);
		//_mm->r(bClient + LocalPlayer + 0x3020, punch4);
		//std::wcout << bClient + LocalPlayer + 0x2FBC + 0x302C << " | " << punch2 << " | " << punch3 << " | " << punch4 << " | " << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		//system("cls");
	}
	return 0;
}