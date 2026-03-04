// 3/4/2024 - millerz1337 tarafindan yapilmistir.
// herhangi biryerde kullanmadan once, bu kodun ne yaptigini ve nasil calistigini anladiginizdan emin olunuz.
// satildigini gorursem gerekirse islem bile yaparim
// tum haklari millerz1337'ye aittir, izinsiz kullanilamaz, kopyalanamaz, dagitilamaz, satilamaz.
// This file is part of MonarchSoftware, licensed under the MIT License (MIT).
// discorda gelerek canli yardim alabilir soru sorabilirsiniz, istekte bulunabilirsiniz.
// https://discord.gg/YZngJgYwah
// https://discord.gg/YZngJgYwah
// https://discord.gg/YZngJgYwah

#include <Windows.h>
#include <GL/gl.h>
#include <map>
#include <string>
#include <vector>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#include "globals.h"
#include "ui.h"
#include <string>

// millerz1337
bool fullbright_enabled = false;
bool nohurtcam_enabled = false;
bool xray_enabled = false;
int xray_key = 0;
bool viewmodel_enabled = false;
bool show_menu = true;

// millerz1337
static bool ins_was_down = false;
static bool rctrl_was_down = false;
static bool xray_bind_was_down = false;
static ImFont* f_main = nullptr;

static bool is_binding_xray = false;

#define C_ACCENT         IM_COL32(138, 125, 179, 255) // #8a7db3
#define C_TEXT           IM_COL32(238, 238, 238, 255) // #eeeeee
// millerz1337
const char* GetKeyName(int key) {
    if (key >= 'A' && key <= 'Z') return (new std::string(1, (char)key))->c_str();
    if (key >= '0' && key <= '9') return (new std::string(1, (char)key))->c_str();
    switch (key) {
        case VK_XBUTTON1: return "MB1";
        case VK_XBUTTON2: return "MB2";
        case VK_MBUTTON: return "MB3";
        case VK_SHIFT: return "SHIFT";
        case VK_CONTROL: return "CTRL";
        case VK_MENU: return "ALT";
        case VK_CAPITAL: return "CAPS";
        default: return "None";
    }
}
// millerz1337
static bool CustomCheckbox(const char* label, bool* val, const char* bind_label = nullptr, bool* binding_state = nullptr)
{
    float avail = ImGui::GetContentRegionAvail().x;
    float rowH = 18.f;
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* dl = ImGui::GetWindowDrawList();

    float labelW = ImGui::CalcTextSize(label).x;
    float interactW = avail - (bind_label ? 60.f : 0.f); // Leave space for bind button if needed
    
    ImGui::PushID(label);
    
    // millerz1337
    
    float boxSize = 12.f;
    ImVec2 boxMin(pos.x + avail - boxSize - 2, pos.y + (rowH - boxSize) * 0.5f);
    
    ImGui::SetCursorScreenPos(boxMin);
    bool clicked = ImGui::InvisibleButton("##toggle", ImVec2(boxSize, boxSize));
    if (clicked) *val = !*val;

    ImGui::SetCursorScreenPos(pos);

    // millerz1337
    if (f_main) ImGui::PushFont(f_main);
    ImGui::Text(label);
    if (f_main) ImGui::PopFont();

    // millerz1337
    if (bind_label && binding_state) {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.1f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.2f));
        if (ImGui::Button(bind_label, ImVec2(0, 16))) {
            *binding_state = true;
        }
        ImGui::PopStyleColor(3);
    }

    // millerz1337
    ImVec2 boxMax(boxMin.x + boxSize, boxMin.y + boxSize);

    // millerz1337
    dl->AddRectFilled(boxMin, boxMax, *val ? IM_COL32(138, 125, 179, 255) : IM_COL32(30, 30, 35, 255), 2.f);
    dl->AddRect(boxMin, boxMax, IM_COL32(80, 80, 90, 255), 2.f);

    // millerz1337
    if (*val) {
        dl->AddLine(ImVec2(boxMin.x + 3, boxMin.y + 6), ImVec2(boxMin.x + 5, boxMin.y + 9), IM_COL32(255, 255, 255, 255), 1.5f);
        dl->AddLine(ImVec2(boxMin.x + 5, boxMin.y + 9), ImVec2(boxMin.x + 9, boxMin.y + 3), IM_COL32(255, 255, 255, 255), 1.5f);
    }

    ImGui::PopID();
    // millerz1337
    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + rowH + ImGui::GetStyle().ItemSpacing.y));
    return clicked;
}

void RenderUI() {
    if (!f_main) {
        ImGuiIO& io = ImGui::GetIO();
        f_main = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 15.0f);
        if (!f_main) f_main = io.Fonts->AddFontDefault();
    }

    bool ins_is_down = GetAsyncKeyState(VK_INSERT) & 0x8000;
    bool rctrl_is_down = GetAsyncKeyState(VK_RCONTROL) & 0x8000;
    if ((ins_is_down && !ins_was_down) || (rctrl_is_down && !rctrl_was_down)) show_menu = !show_menu;
    ins_was_down = ins_is_down;
    rctrl_was_down = rctrl_is_down;
    // millerz1337
    if (xray_key != 0 && !is_binding_xray) {
        bool xray_is_down = GetAsyncKeyState(xray_key) & 0x8000;
        if (xray_is_down && !xray_bind_was_down) xray_enabled = !xray_enabled;
        xray_bind_was_down = xray_is_down;
    }

    if (!show_menu) return;

    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 4.f;
    s.FramePadding = ImVec2(4, 1);
    s.ItemSpacing = ImVec2(4, 2);
    s.Colors[ImGuiCol_WindowBg] = ImColor(15, 15, 18, 245);
    // millerz1337
    ImGui::SetNextWindowSize(ImVec2(180, 0), ImGuiCond_Always);
    if (ImGui::Begin("monarch", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
        
        if (f_main) ImGui::PushFont(f_main);
        ImGui::TextColored(ImVec4(0.54f, 0.49f, 0.70f, 1.0f), "millerz1337");
        if (f_main) ImGui::PopFont();
        
        ImGui::Separator();
        ImGui::Spacing();

        char bindLabel[16];
        if (is_binding_xray) {
            strcpy(bindLabel, "[???]");
            for (int i = 8; i < 256; i++) {
                if (GetAsyncKeyState(i) & 0x8000) {
                    if (i == VK_LBUTTON || i == VK_INSERT || i == VK_RCONTROL) continue;
                    xray_key = i;
                    is_binding_xray = false;
                    xray_bind_was_down = true; // Prevent immediate toggle
                    break;
                }
            }
        } else {
            if (xray_key == 0) strcpy(bindLabel, "[BIND]");
            else sprintf(bindLabel, "[%s]", GetKeyName(xray_key));
        }
        // millerz1337
        CustomCheckbox("Fullbright", &fullbright_enabled);
        CustomCheckbox("No HurtCam", &nohurtcam_enabled);
        CustomCheckbox("ViewModel", &viewmodel_enabled);
        CustomCheckbox("X-Ray", &xray_enabled, bindLabel, &is_binding_xray);
        
      
        

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        ImGui::Text("INSERT - RCTRL");
        ImGui::PopStyleColor();
    }
    ImGui::End();
}

void InitializeUI() {}
void CleanupUI() {}
