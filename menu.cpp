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
    ImGui::Begin(u8"����_С����");
    ImGui::SameLine();
    ImGui::Text(u8"����ʱ�䣺2022-11-01");
    ImGui::SameLine(125);//135
    GetNowTime();
    ImGui::Text(u8"��ǰʱ�䣺%4d-%02d-%02d %02d:%02d:%02d", nowsystime.year, nowsystime.month, nowsystime.day, nowsystime.hour, nowsystime.min, nowsystime.sec);
    ImGui::SameLine(305.0f);
    if (ImGui::BeginTabItem("Player"))
    {
        ImGui::Checkbox(u8"����::����ɢ", &Options.NoDiff);
        ImGui::Checkbox(u8"����::�޺���", &Options.NoRecoil);
        ImGui::Checkbox(u8"����::ȫ�Զ�", &Options.FullAuto);
        ImGui::Checkbox(u8"����::�����ӵ�", &Options.MaxAmmo);
        ImGui::Checkbox(u8"����::��������", &Options.MaxStamina);
        ImGui::Checkbox(u8"�ӵ�::�ӵ�˲��", &Options.MuzzleVelocityMultiplier);
        ImGui::SliderFloat(u8"����::�����ٶ�", &Options.Mobility, 0.f, 10.f);

        ImGui::EndTabItem();
    }
}

void Esp()
{
    if (ImGui::BeginTabItem("Esp"))
    {

        ImGui::Checkbox(u8"2D �״�", &Options.DrawRadar);
        ImGui::Checkbox(u8"2D ����", &Options.DrawBox2D);
        ImGui::Checkbox(u8"3D ����", &Options.DrawBox3D);
        ImGui::Checkbox(u8"���鷶Χ", &Options.DrawRange);
        ImGui::Checkbox(u8"�������", &Options.DrawBone);
        ImGui::Checkbox(u8"����Ѫ��", &Options.DrawBlood);
        ImGui::Checkbox(u8"���ﳯ��", &Options.DrawLosLine);
        ImGui::Checkbox(u8"��������", &Options.DrawSnapLine);

        ImGui::EndTabItem();
    }
}

void Aimbot()
{
    if (ImGui::BeginTabItem("Aimbot"))
    {

        ImGui::Checkbox(u8"��������", &Options.OpenAimbot);
        ImGui::Checkbox(u8"�ڴ�����", &Options.MemoryAimbot);
        ImGui::Checkbox(u8"��Ĭ����", &Options.SilentAimbot);
        ImGui::Checkbox(u8"�ӵ�׷��", &Options.bulletTrack);
        ImGui::SliderFloat(u8"��Ĭ��Χ", &Options.SilentRange, 0.f, 960.f);

        ImGui::EndTabItem();
    }
}

void SetStyle()
{
    if (ImGui::BeginTabItem(u8"Style"))
    {
        static int �˵����ѡ�� = 0;
        ImGui::Text(u8"�˵����");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::Combo("##output_type6", &�˵����ѡ��, u8"����ɫ\0����ɫ\0��͸��\0ȫ͸��\0");
        switch (�˵����ѡ��)
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
    ImGui::Text(u8"�����ٶ�: %.1f FPS", ImGui::GetIO().Framerate);
    ImGui::SameLine();
    ImGui::Text(u8"��������ʱ�䣺%4d-%02d-%02d %02d:%02d:%02d", runtime.year, runtime.month, runtime.day, runtime.hour, runtime.min, runtime.sec);
    ImGui::EndTabItem();
}

