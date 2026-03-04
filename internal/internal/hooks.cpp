// 3/4/2024 - millerz1337 tarafindan yapilmistir.
// herhangi biryerde kullanmadan once, bu kodun ne yaptigini ve nasil calistigini anladiginizdan emin olunuz.
// satildigini gorursem gerekirse islem bile yaparim
// tum haklari millerz1337'ye aittir, izinsiz kullanilamaz, kopyalanamaz, dagitilamaz, satilamaz.
// This file is part of monarch internal, licensed under the MIT License (MIT).
// discorda gelerek canli yardim alabilir soru sorabilirsiniz, istekte bulunabilirsiniz.
// https://discord.gg/YZngJgYwah
// https://discord.gg/YZngJgYwah
// https://discord.gg/YZngJgYwah

#include <Windows.h>
#include <GL/gl.h>
#include <cmath>
#include "detours.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#include "globals.h"
#include "ui.h"
// millerz1337
typedef void (APIENTRY* glRotatef_t)(GLfloat, GLfloat, GLfloat, GLfloat);
typedef void (APIENTRY* glTranslatef_t)(GLfloat, GLfloat, GLfloat);
typedef void (APIENTRY* glScalef_t)(GLfloat, GLfloat, GLfloat);
typedef void (APIENTRY* glPushMatrix_t)(void);
typedef void (APIENTRY* glPopMatrix_t)(void);
typedef void (APIENTRY* glEnable_t)(GLenum);
typedef void (APIENTRY* glDepthRange_t)(GLdouble, GLdouble);
typedef void (APIENTRY* glPolygonMode_t)(GLenum, GLenum);
typedef BOOL (WINAPI* wglSwapBuffers_t)(HDC);
// millerz1337

static glRotatef_t o_glRotatef = nullptr;
static glTranslatef_t o_glTranslatef = nullptr;
static glScalef_t o_glScalef = nullptr;
static glPushMatrix_t o_glPushMatrix = nullptr;
static glPopMatrix_t o_glPopMatrix = nullptr;
static glEnable_t o_glEnable = nullptr;
static glDepthRange_t o_glDepthRange = nullptr;
static glPolygonMode_t o_glPolygonMode = nullptr;
static wglSwapBuffers_t o_wglSwapBuffers = nullptr;


static bool isViewModelActive = false;
static int  matrixStackDepth = 0;
static int  viewmodelMatrixDepth = 0;

static bool IsViewModelPass() {
    return isViewModelActive;
}

// millerz1337
static bool imguiInit = false;
static HWND g_hwnd = nullptr;
static WNDPROC o_WndProc = nullptr;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (show_menu) {
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp);
        return true;
    }
    return CallWindowProc(o_WndProc, hwnd, msg, wp, lp);
}

// millerz1337
void APIENTRY hooked_glPushMatrix() {
    o_glPushMatrix();
    matrixStackDepth++;
}

void APIENTRY hooked_glPopMatrix() {
    if (isViewModelActive && matrixStackDepth == viewmodelMatrixDepth)
        isViewModelActive = false;
    o_glPopMatrix();
    matrixStackDepth--;
    if (matrixStackDepth < 0) matrixStackDepth = 0;
}

void APIENTRY hooked_glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
    
    
    if (isViewModelActive && viewmodel_enabled) {
       
    }
 o_glTranslatef(x, y, z);
}

void APIENTRY hooked_glDepthRange(GLdouble zNear, GLdouble zFar) {
    if (zNear == 0.0 && zFar == 0.1) {
        isViewModelActive = true;
    }
    else if (zNear == 0.0 && zFar == 1.0) {
        isViewModelActive = false;
    }
    o_glDepthRange(zNear, zFar);
}

void APIENTRY hooked_glPolygonMode(GLenum face, GLenum mode) {
    if (xray_enabled) {
       
        o_glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        o_glPolygonMode(face, mode);
    }
}

void APIENTRY hooked_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    // millerz1337
    if (nohurtcam_enabled && angle < 0.0f && x == 0.0f && y == 0.0f && z == 1.0f)
    {
        if ((angle / 14.0f) > -1.0f)
        {
            if (o_glRotatef) o_glRotatef(0.0f, x, y, z);
            return;
        }
    }

    if (isViewModelActive && viewmodel_enabled) {
       
        return;
    }

    if (o_glRotatef) o_glRotatef(angle, x, y, z);
}

void APIENTRY hooked_glScalef(GLfloat x, GLfloat y, GLfloat z) {
    if (isViewModelActive && viewmodel_enabled) {
        
        z *= 0.5f; 
    }
    o_glScalef(x, y, z);
}

void APIENTRY hooked_glEnable(GLenum cap)
{
    if (xray_enabled && cap == GL_DEPTH_TEST) return;
    if (xray_enabled && cap == GL_CULL_FACE) return; // Allow seeing through both sides in Wireframe X-Ray
    if (fullbright_enabled && cap == GL_LIGHTING) return;

   
    if (viewmodel_enabled && isViewModelActive && cap == GL_LIGHTING) return;
    
    if (o_glEnable) o_glEnable(cap);
}

BOOL WINAPI hooked_wglSwapBuffers(HDC hdc) {
    if (!imguiInit) {
        g_hwnd = WindowFromDC(hdc);
        if (g_hwnd) {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui_ImplWin32_Init(g_hwnd);
            ImGui_ImplOpenGL3_Init();
            o_WndProc = (WNDPROC)SetWindowLongPtr(g_hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
            imguiInit = true;
        }
    }

    if (imguiInit) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        RenderUI();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return o_wglSwapBuffers(hdc);
}

void InitializeHooks()
{
    HMODULE hOpenGL = GetModuleHandleA("opengl32.dll");
    if (hOpenGL)
    {
        o_glRotatef = (glRotatef_t)GetProcAddress(hOpenGL, "glRotatef");
        o_glTranslatef = (glTranslatef_t)GetProcAddress(hOpenGL, "glTranslatef");
        o_glScalef = (glScalef_t)GetProcAddress(hOpenGL, "glScalef");
        o_glPushMatrix = (glPushMatrix_t)GetProcAddress(hOpenGL, "glPushMatrix");
        o_glPopMatrix = (glPopMatrix_t)GetProcAddress(hOpenGL, "glPopMatrix");
        o_glEnable = (glEnable_t)GetProcAddress(hOpenGL, "glEnable");
        o_glDepthRange = (glDepthRange_t)GetProcAddress(hOpenGL, "glDepthRange");
        o_glPolygonMode = (glPolygonMode_t)GetProcAddress(hOpenGL, "glPolygonMode");
        o_wglSwapBuffers = (wglSwapBuffers_t)GetProcAddress(hOpenGL, "wglSwapBuffers");

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        
        if (o_glRotatef) DetourAttach(&(PVOID&)o_glRotatef, hooked_glRotatef);
        if (o_glTranslatef) DetourAttach(&(PVOID&)o_glTranslatef, hooked_glTranslatef);
        if (o_glScalef) DetourAttach(&(PVOID&)o_glScalef, hooked_glScalef);
        if (o_glPushMatrix) DetourAttach(&(PVOID&)o_glPushMatrix, hooked_glPushMatrix);
        if (o_glPopMatrix) DetourAttach(&(PVOID&)o_glPopMatrix, hooked_glPopMatrix);
        if (o_glEnable) DetourAttach(&(PVOID&)o_glEnable, hooked_glEnable);
        if (o_glDepthRange) DetourAttach(&(PVOID&)o_glDepthRange, hooked_glDepthRange);
        if (o_glPolygonMode) DetourAttach(&(PVOID&)o_glPolygonMode, hooked_glPolygonMode);
        if (o_wglSwapBuffers) DetourAttach(&(PVOID&)o_wglSwapBuffers, hooked_wglSwapBuffers);
        
        DetourTransactionCommit();
    }
}
// millerz1337
void UpdateAllHooks() {}

void RemoveHooks()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    
    if (o_glRotatef) DetourDetach(&(PVOID&)o_glRotatef, hooked_glRotatef);
    if (o_glTranslatef) DetourDetach(&(PVOID&)o_glTranslatef, hooked_glTranslatef);
    if (o_glScalef) DetourDetach(&(PVOID&)o_glScalef, hooked_glScalef);
    if (o_glPushMatrix) DetourDetach(&(PVOID&)o_glPushMatrix, hooked_glPushMatrix);
    if (o_glPopMatrix) DetourDetach(&(PVOID&)o_glPopMatrix, hooked_glPopMatrix);
    if (o_glEnable) DetourDetach(&(PVOID&)o_glEnable, hooked_glEnable);
    if (o_glDepthRange) DetourDetach(&(PVOID&)o_glDepthRange, hooked_glDepthRange);
    if (o_glPolygonMode) DetourDetach(&(PVOID&)o_glPolygonMode, hooked_glPolygonMode);
    if (o_wglSwapBuffers) DetourDetach(&(PVOID&)o_wglSwapBuffers, hooked_wglSwapBuffers);
    
    DetourTransactionCommit();

    if (imguiInit) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        if (o_WndProc) SetWindowLongPtr(g_hwnd, GWLP_WNDPROC, (LONG_PTR)o_WndProc);
    }
}
