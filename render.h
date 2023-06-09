#ifndef RENDER_H
#define RENDER_H

#include "base.h"
#include "engine.h"
#include "ImGui/imgui.h"

class Render
{
public:
	Render(AActor* LocalPlayer, AActor* Actor);
	void DrawBone(ImColor Color);
	//绘制骨骼
	void DrawPartBone(int32_t start, int32_t end, ImColor Color);
	
	void DrawBox2D(ImColor Color);

	void DrawBox3D(ImColor Color);



	void DrawBlood(ImColor Color);

	void DrawLosLine(ImColor Color);

	void DrawSnapLine(ImColor Color);
private:

	int FindLeftPoint();

	int FindRightPoint();

	int FindHighestPoint();

	int FindLowestPoint();

	AActor* Actor = NULL;
	AActor* LocalPlayer = NULL;
	bool BoneIs[BoneFNames::Max];  //骨骼是否在一个屏幕内
	Vector3 BonePos[BoneFNames::Max];
	Vector2f BonePoint[BoneFNames::Max];
};
#endif // !RENDER_H
