#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

static Header hugeBase;
static Header *hugeFreep;

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

void
vFree(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = hugeFreep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  hugeFreep = p;
}

static Header*
morecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if(p == (char*)-1)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  free((void*)(hp + 1));
  return freep;
}

static Header*
hugeMorecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu < ((1<<22) / sizeof(Header)))
    nu = (1<<22) / sizeof(Header);
  p = sbrk(nu * sizeof(Header));
  if(p == (char*)-1)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  vFree((void*)(hp + 1));
  return hugeFreep;
}

void*
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if(nbytes < (1 << 20)){
    if((prevp = freep) == 0){
      base.s.ptr = freep = prevp = &base;
      base.s.size = 0;
    }
    for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
      if(p->s.size >= nunits){
        if(p->s.size == nunits)
          prevp->s.ptr = p->s.ptr;
        else {
          p->s.size -= nunits;
          p += p->s.size;
          p->s.size = nunits;
        }
        freep = prevp;
        return (void*)(p + 1);
      }
      if(p == freep)
        if((p = morecore(nunits)) == 0)
          return 0;
    }
  }
  else{
    if((prevp = hugeFreep) == 0){
      hugeBase.s.ptr = hugeFreep = prevp = &hugeBase;
      hugeBase.s.size = 0;
    }
    for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
      if(p->s.size >= nunits){
        if(p->s.size == nunits)
          prevp->s.ptr = p->s.ptr;
        else {
          p->s.size -= nunits;
          p += p->s.size;
          p->s.size = nunits;
        }
        hugeFreep = prevp;
        return (void*)(p + 1);
      }
      if(p == hugeFreep)
        if((p = hugeMorecore(nunits)) == 0)
          return 0;
    }
  }
}

void*
vmalloc(uint nbytes, int hugeFlag)
{

  if(hugeFlag != VMALLOC_SIZE_BASE && hugeFlag != VMALLOC_SIZE_HUGE){
    printf("Please pass VMALLOC_SIZE_BASE or VMALLOC_SIZE_HUGE as flag.\n");
    exit();
  }
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if(hugeFlag == VMALLOC_SIZE_BASE){
    if((prevp = freep) == 0){
      base.s.ptr = freep = prevp = &base;
      base.s.size = 0;
    }
    for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
      if(p->s.size >= nunits){
        if(p->s.size == nunits)
          prevp->s.ptr = p->s.ptr;
        else {
          p->s.size -= nunits;
          p += p->s.size;
          p->s.size = nunits;
        }
        freep = prevp;
        return (void*)(p + 1);
      }
      if(p == freep)
        if((p = morecore(nunits)) == 0)
          return 0;
    }
  }
  else{
    if((prevp = hugeFreep) == 0){
      hugeBase.s.ptr = hugeFreep = prevp = &hugeBase;
      hugeBase.s.size = 0;
    }
    for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
      if(p->s.size >= nunits){
        if(p->s.size == nunits)
          prevp->s.ptr = p->s.ptr;
        else {
          p->s.size -= nunits;
          p += p->s.size;
          p->s.size = nunits;
        }
        hugeFreep = prevp;
        return (void*)(p + 1);
      }
      if(p == hugeFreep)
        if((p = hugeMorecore(nunits)) == 0) //need to change morecore
          return 0;
    }
  }
}