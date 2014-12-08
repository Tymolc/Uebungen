#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(int argc, int **argv) {
	int sysPID;
	pid_t userPID;

	sysPID = syscall(SYS_getpid);

	userPID = getpid();
	
	if(sysPID == -1 || userPID == -1)
		printf("ERROR\n");
	else {
		printf("\n syscall: %d", sysPID);
		printf(" usermode-API: %d\n", (int)userPID);
	}
	getchar();
        return 0;
}
