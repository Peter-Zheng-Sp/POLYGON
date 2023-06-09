#include"engine.h"
#include"sigScaner.h"
uint8_t* GameBase = NULL;
uint8_t** GName = NULL;
UWorld** GWorld = NULL;

void* _Spawn = NULL;
fn_GetBoneMatrix _GetBoneMatrix = NULL;
fn_LineTraceSingle _LineTraceSingle = NULL;
fn_GetForwardVector _GetForwardVector = NULL;

fn_ProjectWorldLocationToScreen _ProjectWorldLocationToScreen = NULL;

float Width = 0;
float Height = 0;

void  EngineInit(HWND hWnd)
{
	RECT Rect;
	GetClientRect(hWnd, &Rect);
	Width = (float)(Rect.right / 2.f);
	Height = (float)(Rect.bottom / 2.f);

	_Spawn = (void*)sigScanerCode::find(NULL, "40 55 53 56 41 56 41 57 48 8D AC 24 90 FE FF FF 48 81 EC 70 02 00 00");
	GName = (uint8_t**)(uintptr_t)sigScanerCode::GetAbsoluteAddress(sigScanerCode::find(NULL, "8B D9 74 09 48 8D 15 ? ? ? ? EB 16"), 7, 3) + 1;
	GWorld = (UWorld**)sigScanerCode::GetAbsoluteAddress(sigScanerCode::find(NULL, "F6 80 ? ? ? ? 10 0F 84 ? ? ? ? 48 89 05 ? ? ? ? 0F 28 D6"), 16, 12) + 1;
	_GetBoneMatrix = (fn_GetBoneMatrix)sigScanerCode::GetAbsoluteAddress(sigScanerCode::find(NULL, "44 0F 29 A4 24 E0 01 00 00 E8 ? ? ? ? 0F 28 35 ? ? ? ? 66 0F 10 06"), 10, 14);
	_LineTraceSingle = (fn_LineTraceSingle)sigScanerCode::GetAbsoluteAddress(sigScanerCode::find(NULL, "44 88 64 24 20 F2 0F 11 4D E0 E8 ? ? ? ? 48 8B 4C 24 70"), 11, 15) ;
	_ProjectWorldLocationToScreen = (fn_ProjectWorldLocationToScreen)sigScanerCode::GetAbsoluteAddress(sigScanerCode::find(NULL,"F2 0F 11 4C 24 60 48 89 7B 20 E8 ? ? ? ? 4C 8D 5C 24 70"),11,15);
	
}
UWorld* GetWorld()
{
	return *GWorld;
}


string GetName(uint32_t Id)
{
	uint32_t Block = Id >> 16;
	uint32_t Offset = Id & 65535;

	FNameEntry22* Info = (FNameEntry22*)(GName[2 + Block] + 2 * Offset);
	return string(Info->AnsiName, Info->Len);
}

bool WorldToScreen(Vector3 WorldLocation, Vector2& ScreenLocation)
{
	void* PlayerColl = GetWorld()->OwningGameInstance->GetPlayerController();
	return _ProjectWorldLocationToScreen(PlayerColl, WorldLocation, ScreenLocation, false);
}

Vector3 GetBoneLocation(void* Mesh, int32_t BoneIndex)
{
	Vector3 Pos;
	FMatrix Matrix;

	_GetBoneMatrix(Mesh, &Matrix, BoneIndex);

	Pos.X = Matrix._41;
	Pos.Y = Matrix._42;
	Pos.Z = Matrix._43;

	return Pos;
}


bool LineTraceSingle(Vector3 Start, Vector3 End, TArray<AActor*>& ActorsToIgnore)
{
	FHitResult OutHit;
	return _LineTraceSingle(GetWorld(), Start, End, 0, true, ActorsToIgnore, 0, OutHit, true, { 0,0,0,0 }, { 0,0,0,0 }, 0.f);
}

Vector3 GetForwardVector(FRotator InRot)
{
	return _GetForwardVector(InRot);
}