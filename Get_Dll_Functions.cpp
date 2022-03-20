
#include<Windows.h>
#include <stdio.h>
#include<string>

using namespace std;
HANDLE Process = GetStdHandle(STD_OUTPUT_HANDLE);

namespace Colorama
{
    void set(int fb, bool INTENSITY)
    {
        SetConsoleTextAttribute(Process, fb | (INTENSITY ? FOREGROUND_INTENSITY : 0));
    }
    void clear()
    {
        SetConsoleTextAttribute(Process, 7);
    };
}

void ViewExportFunctionOfDlls(string name)
{
   HANDLE handledll = LoadLibraryA(name.c_str());
   DWORD entry0 = (DWORD)handledll;

   if(handledll == NULL || handledll == INVALID_HANDLE_VALUE   ){ Colorama::set(FOREGROUND_RED,true);  printf("\n\tInvalid Handle (LoadLibraryA returned NULL)\n\n");Colorama::clear();return;};
    
    PIMAGE_DOS_HEADER dos_header = (PIMAGE_DOS_HEADER)handledll;
    PIMAGE_NT_HEADERS nt_header = (PIMAGE_NT_HEADERS)(entry0+dos_header->e_lfanew);
    
    DWORD Export_Directory_RVA = (DWORD)(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    PIMAGE_SECTION_HEADER section_header = (PIMAGE_SECTION_HEADER)(IMAGE_FIRST_SECTION(nt_header)),section_export_directory;

    do
    {
        if(Export_Directory_RVA > section_header->VirtualAddress && Export_Directory_RVA < (section_header->VirtualAddress+section_header->Misc.VirtualSize))
        {
           section_export_directory=section_header;
           break;
        }
    }while(*((section_header++)->Name));
    
    PIMAGE_EXPORT_DIRECTORY export_directory = (PIMAGE_EXPORT_DIRECTORY)(entry0+Export_Directory_RVA);
    printf("\n\tDll Name : %s [ RVA : 0x%x | Address : %x ]\n\n", (entry0 + export_directory->Name),export_directory->Name,(entry0+export_directory->Name)) ;
    DWORD* name_table = (DWORD*)(entry0+export_directory->AddressOfNames);
    DWORD* adress_table = (DWORD*)(entry0+export_directory->AddressOfFunctions);
    
    // Export by name, the NumberOfFunctions & NumberOfFunctions are symetric
    if(export_directory->NumberOfFunctions == export_directory->NumberOfFunctions)
    {
       for(int i=0; i<export_directory->NumberOfNames;i++)
    { 
        printf("\n\t\t----------------\n\n\t\t[+] Function Name : %s\n\t\t| |\n\t\t[+] function RVA  : 0x%x\n\t\t| |\n\t\t[+] function Address: 0x%x\n", (entry0+*name_table), adress_table[i],(entry0+adress_table[i]) );
        name_table++;
        adress_table++;
    } 
    printf("\n\t\t----------------\n\n");
    }
}

int main(int argc, char* argv[])
{
    Colorama::set(FOREGROUND_RED | FOREGROUND_GREEN, true);
    ViewExportFunctionOfDlls("msvcrt.dll");
    system("pause");
    return 0;
};

