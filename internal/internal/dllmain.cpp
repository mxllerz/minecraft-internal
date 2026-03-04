// 3/4/2024 - millerz1337 tarafindan yapilmistir.
// // This file is part of monarch internal, licensed under the MIT License (MIT).
// herhangi biryerde kullanmadan once, bu kodun ne yaptigini ve nasil calistigini anladiginizdan emin olunuz.
// sahiplik bilgilerini silmeyin. İzinsiz satmayın. 
// kendi projenizmiş gibi paylaşmayın.
// ticari amaçla kullanacaksanız önce izin alın.
// izinsiz satış yeniden dağıtım veya sahiplik kaldırma gibi durumlarda gerekeni yaoarim.
// tum haklari millerz1337'ye aittir, izinsiz kullanilamaz, kopyalanamaz, dagitilamaz, satilamaz.
// discorda gelerek canli yardim alabilir soru sorabilirsiniz, istekte bulunabilirsiniz.
// https://discord.gg/YZngJgYwah
// https://discord.gg/YZngJgYwah
// https://discord.gg/YZngJgYwah
// 1337 is everywhere!

#include "pch.h"
#include <Windows.h>
#include "hooks.h"

DWORD WINAPI MainThread(LPVOID lpParam)
{
    InitializeHooks();
    return 0;
}
// millerz1337
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, NULL, 0, NULL);
        if (hThread) CloseHandle(hThread);
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        RemoveHooks();
    }
    return TRUE;
}
