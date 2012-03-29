/* Examine - a tool for memory leak detection on Windows
 *
 * Copyright (C) 2012 Vincent Torri. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <winnt.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <imagehlp.h>

#include "examine_list.h"
#include "examine_stacktrace.h"


LPVOID WINAPI EXM_HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
BOOL WINAPI EXM_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
void *EXM_malloc(size_t size);
void EXM_free(void *memblock);


typedef struct
{
    char *func_name_old;
    PROC  func_proc_old;
    PROC  func_proc_new;
} Exm_Hook_Overload;

/*
 * WARNING
 *
 * Mofidy the value of EXM_HOOK_OVERLOAD_COUNT and
 * EXM_HOOK_OVERLOAD_COUNT when adding other overloaded
 * functions in overloads_instance
 */
#define EXM_HOOK_OVERLOAD_COUNT 2
#define EXM_HOOK_OVERLOAD_COUNT_CRT 4

Exm_Hook_Overload overloads_instance[EXM_HOOK_OVERLOAD_COUNT_CRT] =
{
    {
        "HeapAlloc",
        NULL,
        (PROC)EXM_HeapAlloc
    },
    {
        "HeapFree",
        NULL,
        (PROC)EXM_HeapFree
    },
    {
        "malloc",
        NULL,
        (PROC)EXM_malloc
    },
    {
        "free",
        NULL,
        (PROC)EXM_free
    }
};

typedef struct
{
    char             *filename;
    Exm_List         *modules;
    Exm_Hook_Overload overloads[EXM_HOOK_OVERLOAD_COUNT_CRT];
    char             *crt_name;
    Exm_Sw           *stacktrace;
} Exm_Hook;

typedef LPVOID (WINAPI *exm_heap_alloc_t) (HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes);
typedef BOOL   (WINAPI *exm_heap_free_t)  (HANDLE hHeap, DWORD dwFlags, LPVOID lpMem);
typedef void  *(*exm_malloc_t)            (size_t size);
typedef void   (*exm_free_t)              (void *memblock);

Exm_Hook exm_hook_instance;

LPVOID WINAPI EXM_HeapAlloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes)
{
    exm_heap_alloc_t ha;
    LPVOID data;
    Exm_List *stack;
    Exm_List *iter;
    int i;

    ha = (exm_heap_alloc_t)exm_hook_instance.overloads[0].func_proc_old;
    data = ha(hHeap, dwFlags, dwBytes);

    printf("HeapAlloc !!! %p\n", data);

    stack = exm_sw_frames_get(exm_hook_instance.stacktrace);
    i = 0;
    iter = stack;
    while (iter)
    {
        Exm_Sw_Data *frame;

        frame = (Exm_Sw_Data *)iter->data;
        printf("[%d] %s (%s) %d\n",
               i,
               exm_sw_data_filename_get(frame),
               exm_sw_data_function_get(frame),
               exm_sw_data_line_get(frame));
        i++;
        iter = iter->next;
    }
    exm_list_free(stack, exm_sw_data_free);

    return data;
}

BOOL WINAPI EXM_HeapFree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
    exm_heap_free_t hf;
    BOOL res;

    printf("HeapFree !!! %p\n", hHeap);

    hf = (exm_heap_free_t)exm_hook_instance.overloads[1].func_proc_old;
    res = hf(hHeap, dwFlags, lpMem);

    return res;
}

void *EXM_malloc(size_t size)
{
    exm_malloc_t ma;
    void *data;
    Exm_List *stack;
    Exm_List *iter;
    int i;

    ma = (exm_malloc_t)exm_hook_instance.overloads[2].func_proc_old;
    data = ma(size);

    printf("malloc !!! %p\n", data);

    stack = exm_sw_frames_get(exm_hook_instance.stacktrace);
    i = 0;
    iter = stack;
    while (iter)
    {
        Exm_Sw_Data *frame;

        frame = (Exm_Sw_Data *)iter->data;
        printf("[%d] %s (%s) %d\n",
               i,
               exm_sw_data_filename_get(frame),
               exm_sw_data_function_get(frame),
               exm_sw_data_line_get(frame));
        i++;
        iter = iter->next;
    }
    exm_list_free(stack, exm_sw_data_free);

    return data;
}

void EXM_free(void *memblock)
{
    exm_free_t f;
    Exm_List *stack;
    Exm_List *iter;
    int i;

    printf("free !!! %p\n", memblock);

    stack = exm_sw_frames_get(exm_hook_instance.stacktrace);
    i = 0;
    iter = stack;
    while (iter)
    {
        Exm_Sw_Data *frame;

        frame = (Exm_Sw_Data *)iter->data;
        printf("[%d] %s (%s) %d\n",
               i,
               exm_sw_data_filename_get(frame),
               exm_sw_data_function_get(frame),
               exm_sw_data_line_get(frame));
        i++;
        iter = iter->next;
    }
    exm_list_free(stack, exm_sw_data_free);

    f = (exm_free_t)exm_hook_instance.overloads[3].func_proc_old;
    f(memblock);
}

static char *
_exm_hook_crt_name_get(void)
{
    HANDLE                   hf;
    HANDLE                   hmap;
    BYTE                    *base;
    IMAGE_DOS_HEADER        *dos_headers;
    IMAGE_NT_HEADERS        *nt_headers;
    IMAGE_IMPORT_DESCRIPTOR *import_desc;
    char                    *res = NULL;

    hf = CreateFile(exm_hook_instance.filename, GENERIC_READ, FILE_SHARE_READ,
                    NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf == INVALID_HANDLE_VALUE)
        return NULL;

    hmap = CreateFileMapping(hf, NULL, PAGE_READONLY | SEC_IMAGE, 0, 0, NULL);
    if (!hmap)
        goto close_file;

    base = (BYTE *)MapViewOfFile(hmap, FILE_MAP_READ, 0, 0, 0);
    if (!base)
        goto unmap;

    dos_headers = (IMAGE_DOS_HEADER *)base;
    nt_headers = (IMAGE_NT_HEADERS *)((BYTE *)dos_headers + dos_headers->e_lfanew);
    import_desc = (IMAGE_IMPORT_DESCRIPTOR *)((BYTE *)dos_headers + nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    while (import_desc->Characteristics)
    {
        if(IsBadReadPtr((BYTE *)dos_headers + import_desc->Name,1) == 0)
        {
            char *module_name;

            module_name = (char *)((BYTE *)dos_headers + import_desc->Name);
            printf("Imports from %s\r\n",(BYTE *)dos_headers + import_desc->Name);
            if (lstrcmpi("msvcrt.dll", module_name) == 0)
            {
                printf("msvcrt.dll !!\n");
                res = _strdup(module_name);
                break;
            }
            if (lstrcmpi("msvcr90.dll", module_name) == 0)
            {
                printf("msvcr90.dll !!\n");
                res = _strdup(module_name);
                break;
            }
            if (lstrcmpi("msvcr90d.dll", module_name) == 0)
            {
                printf("msvcr90d.dll !!\n");
                res = _strdup(module_name);
                break;
            }
            import_desc = (IMAGE_IMPORT_DESCRIPTOR *)((BYTE *)import_desc + sizeof(IMAGE_IMPORT_DESCRIPTOR));
        }
        else
            break;
    }

    UnmapViewOfFile(base);
    CloseHandle(hf);

    return res;

  unmap:
    UnmapViewOfFile(base);
  close_file:
    CloseHandle(hf);

    return NULL;
}

int
exm_modules_get(void)
{
    HMODULE      modules[1024];
    DWORD        modules_nbr;
    unsigned int i;

    /* FIXME: use EnumProcessModulesEx for windows >= Vista */
    if (!EnumProcessModules(GetCurrentProcess(), modules, sizeof(modules), &modules_nbr))
        return 0;

    for (i = 0; i < (modules_nbr / sizeof(HMODULE)); i++)
    {
        char   name[MAX_PATH] = "";
        char  *tmp;
        size_t l;
        DWORD  res;

        res = GetModuleFileName(modules[i], name, sizeof(name));
        if (!res)
          return 0;

        /* we skip the filename of the process */
        if (stricmp(name, exm_hook_instance.filename) == 0)
            continue;

        /* we exit the loop if we find the injected DLL */
        tmp = strstr(name, "examine_dll.dll");
        if (tmp && (*(tmp + strlen("examine_dll.dll")) == '\0'))
            break;

        /* what remains is the list of the needed modules */
        l = strlen(name) + 1;
        tmp = malloc(sizeof(char) * l);
        if (!tmp)
            continue;
        memcpy(tmp, name, l);
        exm_hook_instance.modules = exm_list_append(exm_hook_instance.modules, tmp);
    }
    exm_list_print(exm_hook_instance.modules);
    return 1;
}

int
exm_hook_init(void)
{
    HANDLE handle;
    void  *base;
    int    length;

    handle = OpenFileMapping(PAGE_READWRITE, FALSE, "shared_size");
    if (!handle)
        return 0;

    base = MapViewOfFile(handle, FILE_MAP_READ, 0, 0, sizeof(int));
    if (!base)
    {
        CloseHandle(handle);
        return 0;
    }

    CopyMemory(&length, base, sizeof(int));
    UnmapViewOfFile(base);
    CloseHandle(handle);

    handle = OpenFileMapping(PAGE_READWRITE, FALSE, "shared_filename");
    if (!handle)
        return 0;

    base = MapViewOfFile(handle, FILE_MAP_READ, 0, 0, length);
    if (!base)
    {
        CloseHandle(handle);
        return 0;
    }

    exm_hook_instance.filename = malloc(length * sizeof(char));
    if (!exm_hook_instance.filename)
    {
        UnmapViewOfFile(base);
        CloseHandle(handle);
        return 0;
    }

    CopyMemory(exm_hook_instance.filename, base, length);
    UnmapViewOfFile(base);
    CloseHandle(handle);

    printf(" ** filename : %s\n", exm_hook_instance.filename);

    exm_modules_get();

    memcpy(exm_hook_instance.overloads, overloads_instance, sizeof(exm_hook_instance.overloads));

    exm_hook_instance.crt_name = _exm_hook_crt_name_get();

    exm_hook_instance.stacktrace = exm_sw_init();

    return 1;
}

void
exm_hook_shutdown(void)
{
    if (exm_hook_instance.stacktrace)
        free(exm_hook_instance.stacktrace);
    if (exm_hook_instance.filename)
        free(exm_hook_instance.filename);
}

void
_exm_modules_hook_set(HMODULE module, const char *lib_name, PROC old_function_proc, PROC new_function_proc)
{
    PIMAGE_IMPORT_DESCRIPTOR iid;
    PIMAGE_THUNK_DATA        thunk;
    ULONG                    size;

    iid = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(module, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size);
    if (!iid)
        return;

    while (iid->Name)
    {
        PSTR module_name;

        module_name = (PSTR)((PBYTE) module + iid->Name);
        if (_stricmp(module_name, lib_name) == 0)
            break;
        iid++;
    }

    if (!iid->Name)
        return;

    thunk = (PIMAGE_THUNK_DATA)((PBYTE)module + iid->FirstThunk );
    while (thunk->u1.Function)
    {
        PROC *func;

        func = (PROC *)&thunk->u1.Function;
        if (*func == old_function_proc)
        {
            MEMORY_BASIC_INFORMATION mbi;
            DWORD dwOldProtect;

            VirtualQuery(func, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

            if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_READWRITE, &mbi.Protect))
                return;

            *func = *new_function_proc;
            VirtualProtect(mbi.BaseAddress, mbi.RegionSize, mbi.Protect, &dwOldProtect);
            break;
        }
        thunk++;
    }
}

void
_exm_hook_modules_hook(const char *lib_name, int crt)
{
    HMODULE      mods[1024];
    HMODULE      lib_module;
    HMODULE      hook_module = NULL;
    DWORD        res;
    DWORD        mods_nbr;
    unsigned int i;
    unsigned int start;
    unsigned int end;

    if (!crt)
    {
        start = 0;
        end = EXM_HOOK_OVERLOAD_COUNT;
    }
    else
    {
        start = EXM_HOOK_OVERLOAD_COUNT;
        end = EXM_HOOK_OVERLOAD_COUNT_CRT;
    }

    lib_module = LoadLibrary(lib_name);

    for (i = start; i < end; i++)
        exm_hook_instance.overloads[i].func_proc_old = GetProcAddress(lib_module, exm_hook_instance.overloads[i].func_name_old);

    if (!EnumProcessModules(GetCurrentProcess(), mods, sizeof(mods), &mods_nbr))
        return;

    for (i = 0; i < (mods_nbr / sizeof(HMODULE)); i++)
    {
        char name[256] = "";
        char *windir = getenv("WINDIR");
        char buf[256];

        res = GetModuleFileNameEx(GetCurrentProcess(), mods[i], name, sizeof(name));
        if (!res)
            continue;

        snprintf(buf, 255, "%s\\system32\\", windir);

        /* if (strcmp(buf, name) > 0) */
            /* printf(" $$$$ %s\n", name); */

        if (lstrcmp(name, exm_hook_instance.filename) != 0)
            continue;

            printf(" $$$$ %s\n", name);
        hook_module = mods[i];
    }

    if (hook_module)
    {
        for (i = start; i < end; i++)
            _exm_modules_hook_set(hook_module, lib_name,
                                  exm_hook_instance.overloads[i].func_proc_old,
                                  exm_hook_instance.overloads[i].func_proc_new);
    }

    FreeLibrary(lib_module);
}

void
_exm_hook_modules_unhook(const char *lib_name, int crt)
{
    HMODULE      mods[1024];
    HMODULE      hook_module = NULL;
    DWORD        mods_nbr;
    DWORD        res;
    unsigned int i;
    unsigned int start;
    unsigned int end;

    if (!crt)
    {
        start = 0;
        end = EXM_HOOK_OVERLOAD_COUNT;
    }
    else
    {
        start = EXM_HOOK_OVERLOAD_COUNT;
        end = EXM_HOOK_OVERLOAD_COUNT_CRT;
    }

    if (!EnumProcessModules(GetCurrentProcess(), mods, sizeof(mods), &mods_nbr))
        return;

    for (i = 0; i < (mods_nbr / sizeof(HMODULE)); i++)
    {
        char name[256] = "";

        res = GetModuleFileNameEx(GetCurrentProcess(), mods[i], name, sizeof(name));
        if (!res)
            continue;

        if (lstrcmp(name, exm_hook_instance.filename) != 0)
            continue;

        hook_module = mods[i];
    }

    if (hook_module)
    {
        for (i = start; i < end; i++)
            _exm_modules_hook_set(hook_module, lib_name,
                                  exm_hook_instance.overloads[i].func_proc_new,
                                  exm_hook_instance.overloads[i].func_proc_old);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule __UNUSED__, DWORD ulReason, LPVOID lpReserved __UNUSED__)
{
    switch (ulReason)
    {
     case DLL_PROCESS_ATTACH:
         printf (" # process attach\n");
         if (!exm_hook_init())
             return FALSE;

         break;
     case DLL_THREAD_ATTACH:
         printf (" # thread attach begin\n");
         _exm_hook_modules_hook("kernel32.dll", 0);
         if (exm_hook_instance.crt_name)
             _exm_hook_modules_hook(exm_hook_instance.crt_name, 1);
         printf (" # thread attach end\n");
         break;
     case DLL_THREAD_DETACH:
         printf (" # thread detach\n");
         break;
     case DLL_PROCESS_DETACH:
         printf (" # process detach\n");
         _exm_hook_modules_unhook("kernel32.dll", 0);
         if (exm_hook_instance.crt_name)
             _exm_hook_modules_unhook(exm_hook_instance.crt_name, 1);
         exm_hook_shutdown();
         break;
    }

    return TRUE;
}
