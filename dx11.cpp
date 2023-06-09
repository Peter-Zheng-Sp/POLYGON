#include "dx11.h"
#include "engine.h"
#include "render.h"
#include "menu.h"
#include "aimbot.h"
#include "hook.h"
//白色
static ImColor c_白色 = ImColor(255, 255, 255, 255);
//黑色
static ImColor c_黑色 = ImColor(0, 0, 0, 255);
//红色
static ImColor c_红色 = ImColor(255, 0, 0, 255);
//绿色
static ImColor c_绿色 = ImColor(0, 255, 0, 255);
//蓝色
static ImColor c_蓝色 = ImColor(0, 0, 255, 255);
//金黄色
static ImColor c_金黄色 = ImColor(255, 215, 0, 255);
//桔黄
static ImColor c_桔黄 = ImColor(255, 128, 0, 255);
//靛青
static ImColor c_靛青 = ImColor(8, 46, 84, 255);
//碧绿色
static ImColor c_碧绿色 = ImColor(127, 255, 212, 255);
//青绿色
static ImColor c_青绿色 = ImColor(64, 224, 208, 255);
//天蓝
static ImColor c_天蓝 = ImColor(135, 208, 235, 255);
//浅灰蓝色
static ImColor c_浅灰蓝色 = ImColor(176, 224, 230, 255);
//深粉色
static ImColor c_深粉色 = ImColor(255, 20, 147, 255);
//淡粉色
static ImColor c_淡粉色 = ImColor(255, 174, 185, 255);
typedef LRESULT(WINAPI* pWndProc)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
typedef HRESULT(WINAPI* pPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef HRESULT(WINAPI* pResize)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);


static ID3D11Device* g_pDevice = nullptr;//游戏的D3D指针
static ID3D11DeviceContext* g_pDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_RenderTargetView = nullptr;

pPresent g_Present = nullptr;//游戏的原函数地址
pWndProc g_WndProc = nullptr;
pResize g_Resize = nullptr;


DWORD64* SwapVTable = nullptr;//交换链虚表

HWND g_hWnd = 0;


void GetDx11Ptr(IDXGISwapChain* pSwapChain);//函数声明
HRESULT WINAPI Initialize(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

void WriteVtb(int Idx, void* Address)
{
    DWORD Protect;
    VirtualProtect(SwapVTable, 1024, PAGE_EXECUTE_READWRITE, &Protect);
    SwapVTable[Idx] = (DWORD64)Address;
    VirtualProtect(SwapVTable, 1024, Protect, &Protect);
}

HRESULT WINAPI hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (Options.OpenMenu)
    {
        ImGui::Begin(u8"InSert显示及隐藏");

        ImGui::SetWindowSize(ImVec2(310, 330), ImGuiCond_Once);

        if (ImGui::BeginTabBar("tab1"))
        {
            Player();
            Esp();
            Aimbot();
            SetStyle();
            ImGui::EndTabBar();
        }

        ImGui::End();
    }


    UWorld* World = GetWorld();
    if (World && World->GameState)
    {
        TArray<APlayerState*>& Actors = World->GameState->PlayerArray;

        for (size_t i = 0; i < Actors.Count; i++)
        {
            APlayerState* PlayerState = Actors.Data[i];
            if (!PlayerState || !PlayerState->PawnPrivate)
                continue;

            AActor* Actor = PlayerState->PawnPrivate;
            AActor* Player = World->OwningGameInstance->GetPlayer();
            if (!Player || !(Player->RootComponent) || !(Player->Mesh) || Player->PlayerState->Team == PlayerState->Team)
                continue;
          //  printf("%f \n", Player->RootComponent->ModelYaw);
            if (!(Actor->HealthStatsComponent->bIsAlive))
                continue;


            bool IsVisible = Actor->IsVisible(Player, BoneFNames::Head);
            ImColor Color = IsVisible ? ImColor(255, 255, 0) : ImColor(255, 0, 0);

            if (!IsVisible || Options.bulletTrack)
            {
                Aimbot::GetInstance().Push(Player, Actor);
            }

            //初始化渲染对象
            Render RenderObject(Player, Actor);

            //绘制骨骼
            if (Options.DrawBone)
                RenderObject.DrawBone(Color);

            //绘制2D自适应方框
            if (Options.DrawBox2D)
                RenderObject.DrawBox2D(Color);

            if (Options.DrawBox3D)
                RenderObject.DrawBox3D(Color);

            if (Options.DrawSnapLine)
                RenderObject.DrawSnapLine(Color);

            if (Options.DrawLosLine)
                RenderObject.DrawLosLine(Color);

            if (Options.DrawBlood)
                RenderObject.DrawBlood(ImColor(0, 255, 0));

            if (Options.DrawRange)
                ImGui::GetForegroundDrawList()->AddCircle({ Width,Height }, Options.SilentRange, ImColor(255, 255, 255));

            AItem_Weapon_General* Weapon = Player->PlayerState->InventoryComponent->CurrentWeapon;
            if (Options.NoDiff)
            {
                Weapon->AccuracyHip = 99999.f;
                Weapon->AccuracySight = 99999.f;
                Weapon->SpreadShot = 0.f;
            }

            if (Options.MaxAmmo)
            {
                Weapon->MaxMagazineAmmo = 99999;
            }

            if (Options.NoRecoil)
            {
                Weapon->WeaponUpRecoil = 0.f;
                Weapon->WeaponRecoilLift = 0.f;
                Weapon->WeaponBackwardRecoil = 0.f;
                Weapon->WeaponRecoilAlphaPerShot = 0.f;
            }

            if (Options.FullAuto)
            {
                Weapon->WeaponShootingType = EWeaponShootingType::AUTO;
            }

            if (Options.MaxStamina)
            {
                Player->HealthStatsComponent->Stamina = 100.f;
            }

            if (Options.MuzzleVelocityMultiplier)
            {
                Weapon->MuzzleVelocityMultiplier = 10.f;
            }

            Weapon->Mobility = Options.Mobility;

        }

        Aimbot::GetInstance().Clear(); //求出了一个真正离我们准星最近的敌人

        if (Options.OpenAimbot && Options.MemoryAimbot && GetAsyncKeyState(VK_CAPITAL))
        {
            Aimbot::GetInstance().MemoryAimbot();
        }
    }
    ImGui::Render();
    g_pDeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return g_Present(pSwapChain, SyncInterval, Flags);
}

HRESULT WINAPI hkResize(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    if (g_pDevice != nullptr)//加锁,来防止未调用ReImGui函数之前再次执行导致的崩溃
    {
        g_pDevice->Release();
        g_pDevice = nullptr;
        g_RenderTargetView->Release();

        ImGui_ImplDX11_Shutdown();

        //SwapVTable[8] = (DWORD64)Initialize;

        WriteVtb(8, Initialize);
    }

    return g_Resize(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI hkWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch (msg)
    {
    case WM_KEYUP:
        if (wParam == VK_INSERT)
            Options.OpenMenu = !Options.OpenMenu;
        break;
    }


    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
   
    return CallWindowProcA(g_WndProc, hWnd, msg, wParam, lParam);
}
void UnHook()
{

    WriteVtb(8, g_Present);
    WriteVtb(13, g_Resize);
    SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)g_WndProc);      //如果不做这一步处理会崩溃游戏 把窗口属性还原

    if (g_RenderTargetView) { g_RenderTargetView->Release(); }
    if (g_pSwapChain) { g_pSwapChain->Release(); }
    if (g_pDeviceContext) { g_pDeviceContext->Release(); }
    if (g_pDevice) { g_pDevice->Release(); }

    //SwapVTable[8] = (DWORD64)g_Present;    //把虚表跳还回去


    //SwapVTable[13] = (DWORD64)g_Resize;    //把虚表跳还回去

    //ImGui_ImplDX11_Shutdown();
    //ImGui_ImplWin32_Shutdown();
    //ImGui::DestroyContext();
    printf("Dll 卸载成功\n");
    FreeConsole();
    //这里设置了字体必须先卸载内绘的钩子在卸载ImGui 否则报错 !


}
HRESULT WINAPI Initialize(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    static bool Is = false;
   // RectInit(g_hWnd);
    if (!Is)
    {
        Is = true;
        g_WndProc = (WNDPROC)SetWindowLongPtrA(g_hWnd, GWLP_WNDPROC, (LONG_PTR)hkWndProc);//接管消息窗口

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

        io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simhei.ttf", 17.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());

        ImGui_ImplWin32_Init(g_hWnd);
        //RectInit(g_hWnd);
       // EngineInit(g_hWnd); //初始化操作
        EngineInit(g_hWnd);
        HookInitialize();
    }

    GetDx11Ptr(pSwapChain);
    ImGui_ImplDX11_Init(g_pDevice, g_pDeviceContext);

    //SwapVTable[8] = (DWORD64)hkPresent;//初始化完成之后,虚表hook让其跳到真正hook函数中
    WriteVtb(8, hkPresent);
    printf("HookPresent成功\n");
    return g_Present(pSwapChain, SyncInterval, Flags);
}

void GetDx11Ptr(IDXGISwapChain* pSwapChain)
{
    g_pSwapChain = pSwapChain;//取得交换链指针
    g_pSwapChain->GetDevice(__uuidof(g_pDevice), (void**)&g_pDevice);//取得设备指针
    g_pDevice->GetImmediateContext(&g_pDeviceContext);//取得上下文指针

    ID3D11Texture2D* renderTarget = nullptr;
    g_pSwapChain->GetBuffer(0, __uuidof(renderTarget), (void**)&renderTarget);
    g_pDevice->CreateRenderTargetView(renderTarget, nullptr, &g_RenderTargetView);
    printf("成功取到指针 并且释放。\n");
    renderTarget->Release();//以上为获取必要四个指针
}

bool DX11Hook()
{
    g_hWnd = FindWindowA("UnityWndClass", NULL);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, NULL, &featureLevel, NULL) != S_OK)
        return false;

    SwapVTable = (*(DWORD64**)g_pSwapChain);

    DWORD Protect;
    VirtualProtect(SwapVTable, 1024, PAGE_EXECUTE_READWRITE, &Protect);

    g_Present = (pPresent)SwapVTable[8];
    WriteVtb(8, Initialize);
    printf("保护权限写入成功。\n");
    printf("Initialize成功\n");
    //SwapVTable[8] = (DWORD_PTR)Initialize;
    g_Resize = (pResize)SwapVTable[13];
    //SwapVTable[13] = (DWORD_PTR)hkResize;
    WriteVtb(13, hkResize);
    printf("HookResize成功\n");
    g_pSwapChain->Release();

    return true;
}