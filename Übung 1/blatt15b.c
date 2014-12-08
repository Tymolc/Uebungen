# include <windows.h>
# include <winternl.h>
# include <stdio.h>
# include <conio.h>

__declspec(noinline)
__declspec(naked)
NTSTATUS CallNtQueryInformationProcess(IN HANDLE ProcessHandle,
            IN PROCESSINFOCLASS ProcessInformationClass,
            OUT PVOID ProcessInformation,
            IN ULONG ProcessInformationLength,
            OUT PULONG ReturnLength) 
{
        __asm {
            mov eax, 0x00a1 // Windows Server 2003 SP1
            mov edx, 0x7FFE0300
            call dword ptr [edx]

            ret
    }
}

int main(int argc, char** argv) {

	PROCESS_BASIC_INFORMATION output;
	unsigned long buffer = sizeof(output);
	unsigned long outputsize = 0;

	DWORD userPID = GetCurrentProcessId();
	NTSTATUS kernelspaceStatus = CallNtQueryInformationProcess(GetCurrentProcess(), 0, &output, buffer, &outputsize);
	DWORD sysPID = output.UniqueProcessId;
	
	printf("syscall: %i usermode-API: %i\n", sysPID, userPID);

    _getch();
}
