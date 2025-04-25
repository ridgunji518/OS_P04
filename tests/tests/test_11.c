#include "types.h"
#include "user.h"

int main() {
  int original_page_cnt[2];
  if(procpgdirinfo(original_page_cnt) == -1) {
    printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
  }

  printf(1, "Start of program - base pages:%d huge pages:%d\n", original_page_cnt[0], original_page_cnt[1]);

  char *a;
 
  printf(1, "XV6_TEST_OUTPUT ");
  a = (char *) vmalloc(200, 45); 
  memset(a, 0, 200);
  printf(1, "XV6_TEST_ERROR You should have exited before\n"); 
  exit();
} 

  
