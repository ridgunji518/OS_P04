# Huge Page Allocation

In this assignment, I modified the memory management subsystem in xv6 to allow for user space allocation of 4MB pages. Your objectives for this assignment are as follows:

* Understand paging in xv6
* Understand the complexities of supporting multiple page sizes

The work of this assignment is to:
* Allow users to manually allocate 4MB pages by implementing a user space allocator, vmalloc, based on umalloc code, and a corresponding function, vfree
* Modify the memory management subsystem in xv6 to transparently use 4 MB huge pages
* Add a system call, setthp to configure transparent huge page management

## Project Overview

### Step 1: Enable huge page allocations

Standard xv6 manages memory in 4 KB pages. For this assignment you will modify xv6 to support user space allocation of huge pages, which are **4 MB**. The first step is to support allocating both 4KB pages and 4MB pages.

By default, xv6 assumes the availability of 224 MB of physical memory - you can verify this by checking the value used to define PHYSTOP in memlayout.h

Huge pages must be allocated from physical addresses in the range 480 MB (0x1E000000) to 992 MB (0x3E000000). We have provided macros, HUGE\_PAGE\_START and HUGE\_PAGE\_END to demarcate the region of physical memory from which huge pages should be allocated.

Please look at the diagrams below - the first represents the layout of the virtual and physical address space for xv6 by default. The next image represents what the layout should look like after you have successfully implemented this project.

Original xv6 address space layout:

![Untitled](images/xv6-original.png)

Layout with huge pages:

![Untitled](images/xv6-huge.png)

Your first task is to add functions to manage physical memory, analogous to kinit1, kinit2, kalloc and kfree in kalloc.c. Create new functions called khugeinit, khugealloc and khugefree to initialize huge pages, allocate physical memory and free pages.

Write a simple huge page allocator for huge pages. Understand how kinit1, kinit2, kalloc and kfree work for base pages, and implement khugealloc and khugefree. You can assume huge pages are only used for data pages - so you won't need to implement khugeinit1 and khugeinit2 - just one khugeinit function to initialize the freelist with huge pages from the physical address space reserved for huge pages.

Follow a similar API to kalloc and kfree for two reasons:
* Easy integration into xv6's memory management code: kalloc does not take any arguments, and when it is called, it simply returns a pointer to a 4kB page. This is then mapped using allocuvm in vm.c. Similarly, kfree takes in a single character pointer to an address, which it then adds to the free list. Both functions are used extensively in vm.c, which you will need to modify as the next step in this project
* kalloc and kfree have few dependecies on xv6 specific code: by following a similar design philosophy for khugealloc and khugefree, you should be able to test khugealloc and khugefree intially using an independent C program. Implement the alloc and free functions, initialize free region(s) in the free list, and then you can test kalloc and kfree.

### Step 2: Integration into the memory management subsystem
Once you have implemented the allocator and are confident in its functionality, it is time to integrate it into xv6. Do not forget to add initialization code similar to kinit and to guard operations in kalloc and kfree with locks.

Your ultimate goal is to add a user function, vmalloc, which allows users to explicitly allocate n bytes from either the base or the huge page heap.
You may base vmalloc of malloc, as defined in umalloc.c. vmalloc will accept two parameters - n, the number of bytes you want to allocate, and flag which will be passed a flag to determine whether we want to allocate memory using huge pages or base pages if memory needs to be allocated. **Also make sure to define vfree, a function like free for vmalloc.**

The flags can take two values, VMALLOC\_SIZE\_BASE and VMALLOC\_SIZE\_HUGE, which need to be defined in user.h. vmalloc will return a pointer to a virtual memory address where n bytes are available for use.

**Pay attention to the original umalloc code. The allocator manages the pool as a single data structure via the use of a pointer, freep and a value, base. Please update vmalloc to maintain two separate data structures: a pool strictly comprising of huge page data, and another for base page data.**

However, before implementing the allocator, you will need to make system level modifications. The userspace allocator uses the sbrk system call to obtain pointers to free data in the heap, which in turn calls growproc, which can call allocuvm and deallocuvm. You will need to modify these functions to be able to control where data is allocated.

On checking growproc and allocuvm, you will observe that you may not need to allocate memory if the user space allocator internally already has enough free space. The system does this with 4kB pages, extend the design to also apply to 4MB pages. For example, you may have allocated a huge page, only used 100kB out of 4MB, and then call vmalloc with the huge page flag for another 100kB, you should be able to determine that you have enough space available for this allocation.

Pay attention to the virtual addresses at which new data is allocated in growproc. Ensure that base pages and huge pages allocations are mapped to different parts of the virtual address space to avoid issues with page alignment.

By default, base page are mapped from virtual addresses 0-PHYSTOP (take a look at the functions malloc (umalloc.c), sbrk (sysproc.), grownproc (proc.c), allocuvm (vm.c) and kalloc (kalloc.c) to understand why.) Similarly, map huge pages starting from the virtual address HUGE\_VA\_OFFSET (defined in proc.h.) We have updated the proc structure to hold an additional parameter, hugesz, for this purpose.

Take a look at vm.c and determine what changes need to be made. While some changes may be obvious (adding cases to use khugealloc and khugefree in addition to kalloc and kfree) think about where else would you need to make changes related to huge pages.

For instance, you might need to update the process for mapping virtual addresses to physical addresses. How do PTEs and PDEs get affected by the addition of huge pages? Do huge pages need page tables and page table entries? **Make sure to mark the PDEs for huge pages with the PTE\_PS flag.**

After integrating the allocator and updating the memory management code, this is what the address space will look like, with annotations on how we expect address mappings from one region to another:

![Untitled](images/allocation.png)

#### Use provided new system calls to monitor the effect of the changes
We have added two new system calls:
* procpgdirinfo, which keeps track of the total number of base and huge pages currently allocated by the process. The system call takes an array of two ints (just like how pipe does) as input and populates the value at indices 0 and 1 with the count of valid base and huge pages respectively.
* printhugpde, which prints out PDEs for huge pages.

### Step 3: Transparent Huge Page Allocation
vmalloc allows users to explicitly allocate base pages and huge pages. However, in the real world, users may not know the best situations to use pages of each type. Thus, many operating systems allow for enabling transparent huge page allocation (THP) where the OS automatically determines whether to allocate memory using huge pages based on heuristics.

For the sake of this assignment, you can assume that the THP system does not affect vmalloc (which is used when you want complete control over allocations.) Instead, it only affects allocations made using malloc. As a reminder, the syntax of malloc is malloc(n).

For cases where n < 1MB, we expect the user library to make the allocation using base pages, and if memory was previously allocated and the free pool contains some data from the base page region of the virtual address space, allocations must be made from there.

When n >= 1MB, we expect the allocation to be made using huge pages from the huge page region, either using memory from the allocator's pool that has to previously been allocated but is free (if available), or by allocating new memory.

In either case, if there is some free memory available in the corresponding pool, but it is not large enough to satisfy the entire allocation of n bytes, allocate a new page of the correct size based on n, i.e. allocate a huge page if n >= 1MB, else allocate a base page.

**Like with vmalloc, please update malloc to maintain two separate data structures when adding THP support: a pool strictly comprising of huge page data, and another for base page data.**

#### Adding a system call to control THP
Add a new system call, setthp(int). It will take an integer value as input; when this value is 0, THP is disabled, else it is enabled. THP should be disabled on boot.

You will also want to add a system call to read the current value of the THP variable to help make policy decisions in user space, i.e. in umalloc.

At the end of the project, this is what the flow for user allocations should look like:

![Untitled](images/final.png)

#### Error messages

If vmalloc is passed a flag that is not VMALLOC_SIZE_BASE or VMALLOC_SIZE_HUGE, it must print "Please pass VMALLOC_SIZE_BASE or VMALLOC_SIZE_HUGE as flag.\n" and then call exit()

If sbrk returns -1, both malloc and vmalloc should return 0. sbrk will return -1 in two cases:
* if the arguments passed are incorrect (this won't happen if you invoke the system call with the correct number and type of arguments)
* if allocuvm/hugeallocuvm is unable to allocate new pages or runs into trouble with mappings. allocuvm initially has two error messages, "allocuvm out of memory\n" and "allocuvm out of memory (2)\n"
    * you also need to add the error messages "hugealloc - allocuvm out of memory\n" and "hugealloc - allocuvm out of memory (2)\n" for similar cases when allocating huge pages.
    * allocuvm calls mappages - mappages may call panic if you try to remap an existing PTE. Have an analogous check when updating mappings for huge pages which will call panic, the message should be "hugepage - remap"

Also add a check to free/vfree - if the pointer passed has an address greater than KERNBASE, please print out the following error message "Error - trying to free a kernel pointer in userspace\n" and exit().

## Suggested Workflow
- Implement and initialize physical memory allocator
- Modifications to memory management logic
- Modifications to sbrk, growproc. Implementing vmalloc.
- Create user programs, test system functionality with system calls provided
- Add THP support and control
- Test the system further! Look for bugs related to page alignment, mapping and address spaces
