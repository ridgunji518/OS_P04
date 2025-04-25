#include "types.h"
#include "user.h"

int main() {
	void *huge_va = (void*)0x1E000000; 
	void *a = (void*)malloc(128);

	int difference = (int) huge_va - (int) a;

	printf(1,"XV6_TEST_OUTPUT ");
	void *b = malloc(difference + 5);
	if(b == 0){
		printf(1, "XV6_TEST_OUTPUT malloc fails correctly\n");
	}
	exit();	
}
