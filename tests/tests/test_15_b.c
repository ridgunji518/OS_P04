#include "types.h"
#include "user.h"
int main() {
	int original_page_cnt[2];
	int page_cnt[2];
	if(procpgdirinfo(original_page_cnt) == -1) {
		printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
	}
	printf(1, "Start of program - base pages:%d huge pages:%d\n", original_page_cnt[0], original_page_cnt[1]);
	char *a;
	int hgpg_sz = 4 * 1024 * 1024;
	int max_mem = 0x3E000000 - 0x1E000000 - (64 * hgpg_sz);
	a = (char *) vmalloc(max_mem, VMALLOC_SIZE_HUGE); 
	memset(a, 0, max_mem);
	if(procpgdirinfo(page_cnt) == -1) {
		printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
        }
	printf(1, "XV6_TEST_OUTPUT Max, %dMB - program base pages:%d huge pages:%d\n", max_mem/1048576, page_cnt[0] - original_page_cnt[0], page_cnt[1] - original_page_cnt[1]);
exit();
}
