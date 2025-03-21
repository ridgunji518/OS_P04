Name: Samarth Boranna
CS Login: boranna
WISC ID: 9084843391
Email: boranna@wisc.edu
Status: all tests pass

Name: Rahul Idgunji
CS Login: idgunji
WISC ID: 9084853713
Email: idgunji@wisc.edu
Status: all tests pass

Part 1 Changes:
- kalloc.c
  - Added allocator functions for huge pages

Part 2 Changes:
- main.c
  -  khugeinit(P2V(HUGE_PAGE_START), P2V(HUGE_PAGE_END));
- proc.c
  - changes to growproc and fork functions
- vm.c
  - changes to copyuvm, freeuvm, deallocuvm, allocuvm, kmap, and mappages
- sysproc.c
  - changes to sbrk

Part 3 Changes:
- syscall.h
- defs.h
- user.h
- sysproc.c
- usys.S
- syscall.c
- proc.c
- umalloc.c
  - added vmalloc, vfree, hugeMorecore functions
