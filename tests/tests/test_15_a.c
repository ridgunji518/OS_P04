#include "types.h"
#include "user.h"

int main(int argc, char* argv[]) {
    int pid_1 = fork();
    if(pid_1 == 0){
    	argv[0] = "test_15_b";
    	exec("test_15_b", (void*)argv);
    }
    wait();
    int pid_2 = fork();
    if(pid_2 == 0){
    	argv[0] = "test_15_b";
    	exec("test_15_b", (void*)argv);
    }
    wait();
    exit();
}
