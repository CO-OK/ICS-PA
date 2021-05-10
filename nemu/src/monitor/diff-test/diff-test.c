#include "nemu.h"
#include "monitor/monitor.h"
#include <unistd.h>
#include <sys/prctl.h>
#include <signal.h>

#include "protocol.h"
#include <stdlib.h>



bool gdb_connect_qemu(void);
bool gdb_memcpy_to_qemu(uint32_t, void *, int);
bool gdb_getregs(union gdb_regs *);
bool gdb_setregs(union gdb_regs *);
bool gdb_si(void);
void gdb_exit(void);

static bool is_skip_qemu;
static bool is_skip_nemu;

void diff_test_skip_qemu() { is_skip_qemu = true; }
void diff_test_skip_nemu() { is_skip_nemu = true; }

#define regcpy_from_nemu(regs) \
  do { \
    regs.eax = cpu.eax; \
    regs.ecx = cpu.ecx; \
    regs.edx = cpu.edx; \
    regs.ebx = cpu.ebx; \
    regs.esp = cpu.esp; \
    regs.ebp = cpu.ebp; \
    regs.esi = cpu.esi; \
    regs.edi = cpu.edi; \
    regs.eip = cpu.eip; \
    regs.eflags = cpu.EFLAGS_;\
  } while (0)

static uint8_t mbr[] = {
  // start16:
  0xfa,                           // cli
  0x31, 0xc0,                     // xorw   %ax,%ax
  0x8e, 0xd8,                     // movw   %ax,%ds
  0x8e, 0xc0,                     // movw   %ax,%es
  0x8e, 0xd0,                     // movw   %ax,%ss
  0x0f, 0x01, 0x16, 0x44, 0x7c,   // lgdt   gdtdesc
  0x0f, 0x20, 0xc0,               // movl   %cr0,%eax
  0x66, 0x83, 0xc8, 0x01,         // orl    $CR0_PE,%eax
  0x0f, 0x22, 0xc0,               // movl   %eax,%cr0
  0xea, 0x1d, 0x7c, 0x08, 0x00,   // ljmp   $GDT_ENTRY(1),$start32

  // start32:
  0x66, 0xb8, 0x10, 0x00,         // movw   $0x10,%ax
  0x8e, 0xd8,                     // movw   %ax, %ds
  0x8e, 0xc0,                     // movw   %ax, %es
  0x8e, 0xd0,                     // movw   %ax, %ss
  0xeb, 0xfe,                     // jmp    7c27
  0x8d, 0x76, 0x00,               // lea    0x0(%esi),%esi

  // GDT
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xcf, 0x00,
  0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xcf, 0x00,

  // GDT descriptor
  0x17, 0x00, 0x2c, 0x7c, 0x00, 0x00
};

void init_difftest(void) {
  int ppid_before_fork = getpid();
  int pid = fork();
  if (pid == -1) {
    perror("fork");
    panic("fork error");
  }
  else if (pid == 0) {
    // child

    // install a parent death signal in the chlid
    int r = prctl(PR_SET_PDEATHSIG, SIGTERM);
    if (r == -1) {
      perror("prctl error");
      panic("prctl");
    }

    if (getppid() != ppid_before_fork) {
      panic("parent has died!");
    }

    close(STDIN_FILENO);
    execlp("qemu-system-i386", "qemu-system-i386", "-S", "-s", /*"-nographic",*/ NULL);
    //qemu-i386
    //execlp("qemu-system-x86_64", "qemu-system-x86_64", "-S", "-s", "-nographic", NULL);
    perror("exec");
    panic("exec error");
  }
  else {
    // father

    gdb_connect_qemu();
    Log("Connect to QEMU successfully");

    atexit(gdb_exit);

    // put the MBR code to QEMU to enable protected mode
    bool ok = gdb_memcpy_to_qemu(0x7c00, mbr, sizeof(mbr));
    assert(ok == 1);

    union gdb_regs r;
    gdb_getregs(&r);

    // set cs:eip to 0000:7c00
    r.eip = 0x7c00;
    r.cs = 0x0000;
    ok = gdb_setregs(&r);
    assert(ok == 1);

    // execute enough instructions to enter protected mode
    int i;
    for (i = 0; i < 20; i ++) {
      gdb_si();
    }
  }
}

void init_qemu_reg() {
  union gdb_regs r;
  gdb_getregs(&r);
  regcpy_from_nemu(r);
  bool ok = gdb_setregs(&r);
  assert(ok == 1);
}

void difftest_step(uint32_t eip) {
  union gdb_regs r;
  bool diff = false;

  if (is_skip_nemu) {
    is_skip_nemu = false;
    return;
  }

  if (is_skip_qemu) {
    // to skip the checking of an instruction, just copy the reg state to qemu
    gdb_getregs(&r);
    regcpy_from_nemu(r);
    gdb_setregs(&r);
    is_skip_qemu = false;
    return;
  }

  gdb_si();
  gdb_getregs(&r);

  // TODO: Check the registers state with QEMU.
  if(r.eip != cpu.eip)
  {
    diff = true;
    printf("detect diff at eip,\tr.eip=%08X\tcpu.eip=%08X\n",r.eip,cpu.eip);
  }
  if(r.eax != cpu.eax)
  {
    diff = true;
    printf("detect diff at eax,\tr.eax=%08X\tcpu.eax=%08X\n",r.eax,cpu.eax);
  }
  if(r.ebx != cpu.ebx)
  {
    diff = true;
    printf("detect diff at ebx,\tr.ebx=%08X\tcpu.ebx=%08X\n",r.ebx,cpu.ebx);
  }
  if(r.ecx != cpu.ecx)
  {
    diff = true;
    printf("detect diff at ecx,\tr.ecx=%08X\tcpu.ecx=%08X\n",r.ecx,cpu.ecx);
  }
  if(r.edx != cpu.edx)
  {
    diff = true;
    printf("detect diff at edx,\tr.edx=%08X\tcpu.edx=%08X\n",r.edx,cpu.edx);
  }
  if(r.esp != cpu.esp)
  {
    diff = true;
    printf("detect diff at esp,\tr.esp=%08X\tcpu.esp=%08X\n",r.esp,cpu.esp);
  }
  if(r.ebp != cpu.ebp)
  {
    diff = true;
    printf("detect diff at ebp,\tr.ebp=%08X\tcpu.ebp=%08X\n",r.ebp,cpu.ebp);
  }
  if(r.esi != cpu.esi)
  {
    diff = true;
    printf("detect diff at esi,\tr.esi=%08X\tcpu.esi=%08X\n",r.esi,cpu.esi);
  }
  if(r.edi != cpu.edi)
  {
    diff = true;
    printf("detect diff at edi,\tr.edi=%08X\tcpu.edi=%08X\n",r.edi,cpu.edi);
  }
  
  /*else if(r.eflags != cpu.EFLAGS_)
  {
    /*
      只需要比特定的C,Z,S,I,O
    
    //CF
    rtlreg_t CF = r.eflags & eflag_CF;
    rtlreg_t ZF = (r.eflags & eflag_ZF) >> 6;
    rtlreg_t SF = (r.eflags & eflag_SF) >> 7;
    rtlreg_t IF = (r.eflags & eflag_IF) >> 9;
    rtlreg_t OF = (r.eflags & eflag_OF) >> 11;
    if(CF != e_CF)
    {
      diff = true;
      printf("detect diff at eflags_CF,\tr.CF=%d\tcpu.CF=%d\n",CF,e_CF);
    }
    else if(ZF != e_ZF)
    {
      diff = true;
      printf("detect diff at eflags_ZF,\tr.ZF=%d\tcpu.ZF=%d\n",ZF,e_ZF);
    }
    else if(SF != e_SF)
    {
      diff = true;
      printf("detect diff at eflags_SF,\tr.SF=%d\tcpu.SF=%d\n",SF,e_SF);
    }
    else if(IF != e_IF)
    {
      diff = true;
      printf("detect diff at eflags_IF,\tr.IF=%d\tcpu.IF=%d\n",IF,e_IF);
    }
    else if(OF != e_OF)
    {
      diff = true;
      printf("detect diff at eflags_OF,\tr.OF=%d\tcpu.OF=%d\n",OF,e_OF);
    }
    
  }*/
  // Set `diff` as `true` if they are not the same.
  //TODO();
//CF
    rtlreg_t CF = r.eflags & eflag_CF;
    rtlreg_t ZF = (r.eflags & eflag_ZF) >> 6;
    rtlreg_t SF = (r.eflags & eflag_SF) >> 7;
    rtlreg_t IF = (r.eflags & eflag_IF) >> 9;
    rtlreg_t OF = (r.eflags & eflag_OF) >> 11;
    if(CF != e_CF)
    {
      diff = true;
      printf("detect diff at eflags_CF,\tr.CF=%d\tcpu.CF=%d\n",CF,e_CF);
    }
    else if(ZF != e_ZF)
    {
      diff = true;
      printf("detect diff at eflags_ZF,\tr.ZF=%d\tcpu.ZF=%d\n",ZF,e_ZF);
    }
    else if(SF != e_SF)
    {
      diff = true;
      printf("detect diff at eflags_SF,\tr.SF=%d\tcpu.SF=%d\n",SF,e_SF);
    }
    else if(OF != e_OF)
    {
      diff = true;
      printf("detect diff at eflags_OF,\tr.OF=%d\tcpu.OF=%d\n",OF,e_OF);
    }
  if (diff) {
    nemu_state = NEMU_STOP;
  }
}
