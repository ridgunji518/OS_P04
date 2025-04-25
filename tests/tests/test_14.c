#include "types.h"
#include "user.h"

int main() {
  int original_page_cnt[2];
  int page_cnt[2];
  if(procpgdirinfo(original_page_cnt) == -1) {
    printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
  }

  //setthp to 1
  //allocate small pages
  //allocate large pages
  //allocate interleaved small/large pages
  
  //Worth 3 points
  if(setthp(1) == -1) {
    printf(1,"XV6_TEST_ERROR Error, setthp returned -1\n");
  }


  printf(1, "Start of program - base pages:%d huge pages:%d\n", original_page_cnt[0], original_page_cnt[1]);
  char * a, *b, *c, *d, *e, *f, *g;
  
  
  a = (char *)malloc(1048575);
  memset(a, 0, 1048575);
  if(procpgdirinfo(page_cnt) == -1) {
    printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
  }
  printf(1, "XV6_TEST_OUTPUT After malloc with THP enabled, 1048575B - program base pages:%d huge pages:%d\n", page_cnt[0] - original_page_cnt[0], page_cnt[1] - original_page_cnt[1]);
  

  b = (char *)malloc(200);
  memset(b, 0, 200);
  if(procpgdirinfo(page_cnt) == -1) {
    printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
  }
  printf(1, "XV6_TEST_OUTPUT After malloc with THP enabled, 200B - program base pages:%d huge pages:%d\n", page_cnt[0] - original_page_cnt[0], page_cnt[1] - original_page_cnt[1]);
  
  c = (char *)malloc(1048576);
  memset(c, 0, 1048576);
  if(procpgdirinfo(page_cnt) == -1) {
    printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
  }
  printf(1, "XV6_TEST_OUTPUT After malloc with THP enabled, 1MB - program base pages:%d huge pages:%d\n", page_cnt[0] - original_page_cnt[0], page_cnt[1] - original_page_cnt[1]);

  d = (char *)malloc((1048576 * 2));
  memset(d, 0, (1048576 * 2));
  if(procpgdirinfo(page_cnt) == -1) {
    printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
  }
  printf(1, "XV6_TEST_OUTPUT After malloc with THP enabled, 2MB - program base pages:%d huge pages:%d\n", page_cnt[0] - original_page_cnt[0], page_cnt[1] - original_page_cnt[1]);

  e = (char *)malloc((1024 * 8));
  memset(e, 0, (1024 * 8));
  if(procpgdirinfo(page_cnt) == -1) {
    printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
  }
  printf(1, "XV6_TEST_OUTPUT After malloc with THP enabled, 8KB - program base pages:%d huge pages:%d\n", page_cnt[0] - original_page_cnt[0], page_cnt[1] - original_page_cnt[1]);

  f = (char *)malloc((1048576 * 1));
  memset(f, 0, (1048576 * 1));
  if(procpgdirinfo(page_cnt) == -1) {
    printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
  }
  printf(1, "XV6_TEST_OUTPUT After malloc with THP enabled, 1MB - program base pages:%d huge pages:%d\n", page_cnt[0] - original_page_cnt[0], page_cnt[1] - original_page_cnt[1]);

  g = (char *)malloc((1048576 * 4));
  memset(g, 0, (1048576 * 4));
  if(procpgdirinfo(page_cnt) == -1) {
    printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
  }
  printf(1, "XV6_TEST_OUTPUT After malloc with THP enabled, 4MB - program base pages:%d huge pages:%d\n", page_cnt[0] - original_page_cnt[0], page_cnt[1] - original_page_cnt[1]);
  
  free(a);
  char *h;
  h = (char *)malloc((1048575 * 1));
  memset(h, 0, (1048575 * 1));
  if(procpgdirinfo(page_cnt) == -1) {
    printf(1,"XV6_TEST_ERROR Error, procpgdirinfo returned -1\n");
  }
  printf(1, "XV6_TEST_OUTPUT After malloc with THP enabled, less than 1MB - program base pages:%d huge pages:%d\n", page_cnt[0] - original_page_cnt[0], page_cnt[1] - original_page_cnt[1]);
  exit();
}
