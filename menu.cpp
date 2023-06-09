#include "menu.h"
struct systime
{
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
};

_Options Options;
time_t rawtime;
struct tm* timeinfo;
systime nowsystime, runtime, checktime, uestochecktime;
void GetNowTime()
{
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    nowsystime.year = 1900 + timeinfo->tm_year;
    nowsystime.month = 1 + timeinfo->tm_mon;
    nowsystime.day = timeinfo->tm_mday;
    nowsystime.hour = timeinfo->tm_hour;;
    nowsystime.min = timeinfo->tm_min;
    nowsystime.sec = timeinfo->tm_sec;;
}
void Player()
{
    ImGui::Begin(u8"我是_小菜鸟");
    ImGui::SameLine();
    ImGui::Text(u8"更新时间：2022-11-01");
    ImGui::SameLine(125);//135
    GetNowTime();
    ImGui::Text(u8"当前时间：%4d-%02d-%02d %02d:%02d:%02d", nowsystime.year, nowsystime.month, nowsystime.day, nowsystime.hour, nowsystime.min, nowsystime.sec);
    ImGui::SameLine(305.0f);
    if (ImGui::BeginTabItem("Player"))
    {
        ImGui::Checkbox(u8"武器::无扩散", &Options.NoDiff);
        ImGui::Checkbox(u8"武器::无后座", &Options.NoRecoil);
        ImGui::Checkbox(u8"武器::全自动", &Options.FullAuto);
        ImGui::Checkbox(u8"武器::无限子弹", &Options.MaxAmmo);
        ImGui::Checkbox(u8"人物::无限耐力", &Options.MaxStamina);
        ImGui::Checkbox(u8"子弹::子弹瞬击", &Options.MuzzleVelocityMultiplier);
        ImGui::SliderFloat(u8"人物::人物速度", &Options.Mobility, 0.f, 10.f);

        ImGui::EndTabItem();
    }
}

void Esp()
{
    if (ImGui::BeginTabItem("Esp"))
    {

        ImGui::Checkbox(u8"2D 雷达", &Options.DrawRadar);
        ImGui::Checkbox(u8"2D 方框", &Options.DrawBox2D);
        ImGui::Checkbox(u8"3D 方框", &Options.DrawBox3D);
        ImGui::Checkbox(u8"自瞄范围", &Options.DrawRange);
        ImGui::Checkbox(u8"人物骨骼", &Options.DrawBone);
        ImGui::Checkbox(u8"人物血条", &Options.DrawBlood);
        ImGui::Checkbox(u8"人物朝向", &Options.DrawLosLine);
        ImGui::Checkbox(u8"人物连线", &Options.DrawSnapLine);

        ImGui::EndTabItem();
    }
}

void Aimbot()
{
    if (ImGui::BeginTabItem("Aimbot"))
    {

        ImGui::Checkbox(u8"开启自瞄", &Options.OpenAimbot);
        ImGui::Checkbox(u8"内存自瞄", &Options.MemoryAimbot);
        ImGui::Checkbox(u8"静默自瞄", &Options.SilentAimbot);
        ImGui::Checkbox(u8"子弹追踪", &Options.bulletTrack);
        ImGui::SliderFloat(u8"静默范围", &Options.SilentRange, 0.f, 960.f);

        ImGui::EndTabItem();
    }
}

void SetStyle()
{
    if (ImGui::BeginTabItem(u8"Style"))
    {
        static int 菜单风格选择 = 0;
        ImGui::Text(u8"菜单风格：");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::Combo("##output_type6", &菜单风格选择, u8"亮白色\0经典色\0半透明\0全透明\0");
        switch (菜单风格选择)
        {
        case 0:
            ImGui::StyleColorsLight();
            break;
        case 1:
            ImGui::StyleColorsDark();
            break;
        case 2:
            ImGui::StyleColorsClassic();
            break;
        case 3:
            ImGui::My_Style_FullyTransparent();
            break;
        default:
            break;
        }
    }
    ImGui::Text(u8"绘制速度: %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::SameLine();
    ImGui::Text(u8"辅助启动时间：%4d-%02d-%02d %02d:%02d:%02d", runtime.year, runtime.month, runtime.day, runtime.hour, runtime.min, runtime.sec);
    ImGui::EndTabItem();
}

