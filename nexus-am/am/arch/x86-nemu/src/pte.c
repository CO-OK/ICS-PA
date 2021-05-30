#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {
  //功能是将虚拟地址空间 p 中的虚拟地址 va 映射到物理地址 pa
  uint32_t* ptr = (uint32_t*)p->ptr;
  uint32_t shift = (uintptr_t)va >> 22;
  uintptr_t tr = ptr[shift];
  if (tr == kpdirs[shift]) 
  {
      PTE *uptable = (PTE*)(palloc_f());
      ptr[shift] = (uintptr_t)uptable | PTE_P;
  }
  tr = ptr[shift];
  shift = (((uintptr_t)va) & 0x003ff000) >> 12;
  uint32_t* pgr = (uint32_t*)(tr & 0xfffff000);
  pgr[shift] = (uintptr_t)pa | PTE_P;
  //return 0;
  /*uint32_t* page_dir_base = (uint32_t*)p->ptr;
  uint32_t page_dir_index = (uintptr_t)va >> 22;
  uintptr_t page_table_base = *(page_dir_base+page_dir_index);
  if(!(page_table_base&PTE_P))//不存在二级页表
  {
    *(page_dir_base+page_dir_index) = (uintptr_t)palloc_f() | PTE_P;
  }
  page_table_base = *(page_dir_base+page_dir_index);
  uint32_t page_table_index = (((uintptr_t)va) & 0x003ff000) >> 12;
  *((uint32_t*)(page_table_base & 0xfffff000)+page_table_index)=(uintptr_t)pa | PTE_P;
  return 0;*/
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {
  return NULL;
}
