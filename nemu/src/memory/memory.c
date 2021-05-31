#include "nemu.h"
#include "device/mmio.h"
#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  if(is_mmio(addr)==-1)
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  else
    return mmio_read(addr, len, is_mmio(addr));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  if(is_mmio(addr)==-1)
    memcpy(guest_to_host(addr), &data, len);
  else
    mmio_write(addr, len,  data, is_mmio(addr));
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  if((addr & 0xfff) + len > 0x1000)//超过了一个页
  {
    //Log("corss boundary read addr=%08X,len=%d",addr,len);
    int first_total = 0x1000-(addr&0xfff);//第一个页中读取的字节数
    int second_total = len - first_total;//第二个页中读取的字节数
    //Log("fisrt_total=%d,second_total=%d",first_total,second_total);
    uint32_t first_paddr = page_translate(addr);
    uint32_t second_paddr = page_translate(addr+first_total);
    //Log("fisrt addr=%08X,second addr=%08X",first_paddr,second_paddr);
    uint32_t first = paddr_read(first_paddr,first_total);
    uint32_t second = paddr_read(second_paddr,second_total);
    //Log("final addr=%08X",(second<<(first_total*8))| first);
    return (second<<(first_total*8))+ first;
  }
  else
  {
    paddr_t paddr = page_translate(addr) ;
    return paddr_read(paddr, len);
  }
  //return paddr_read(addr,len);
  
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  
  if((addr & 0xfff) + len > 0x1000)//cross boundary
  {
    Log("corss boundary write addr=%08X,len=%d",addr,len);
    int first_total = 0x1000-(addr&0xfff);//第一个页中写的字节数
    int second_total = len - first_total;//第二个页中写的字节数
    //Log("fisrt_total=%d,second_total=%d",first_total,second_total);
    uint32_t first_paddr = page_translate(addr);
    uint32_t second_paddr = page_translate(addr+first_total);
    //Log("fisrt addr=%08X,second addr=%08X",first_paddr,second_paddr);
    paddr_write(first_paddr,first_total,data&(~0u>>((4-first_total)*8)));
    paddr_write(second_paddr,second_total,data>>((4-second_total)*8));
    return;
  }
  else
  {
    paddr_t paddr = page_translate(addr) ;
    paddr_write(paddr, len, data);
  }
  //paddr_write(addr, len, data);
}

paddr_t page_translate(vaddr_t addr){
  paddr_t dir_index = get_dir_index(addr) ;
  paddr_t page_index = get_page_index(addr);
  paddr_t offset = get_page_offset(addr);
  //Log("vaddr=%08X,dir_index=%08X,page_index=%08X,offset=%08X",addr,dir_index,page_index,offset);
  if(get_CR0_PG(cpu.CR0)==1)
  {
    //Log("CR0 PG has set with 1");
    uint32_t page_dir_base = cpu.CR3>>12;
    //Log("CR3=%08X,CR3t=%08X",cpu.CR3,cpu.CR3>>12);
    uint32_t page_dir_entry = paddr_read((page_dir_base << 12) + dir_index*4 , 4);//4byte一个条目，
    //Log("page_dir_entry=%08X",page_dir_entry);
    assert(page_dir_entry & 1);
    page_dir_entry |= 0x2f;//修改page_dir_entry的access位
    paddr_write((page_dir_base << 12) + dir_index*4,4,page_dir_entry);
    page_dir_entry&=0xfffff000;//后12位不用来寻址
    uint32_t page_table_entry = paddr_read(page_dir_entry+ page_index*4, 4);
    //Log("page_table_enrty=%08X",page_table_entry);
    assert(page_table_entry & 1);
    page_table_entry |= 0x2f;//修改page_table_entry的access位
    paddr_write(page_dir_entry+ page_index*4,4,page_table_entry);
    page_table_entry&=0xfffff000;//同上
    //Log("final addr=%08X",page_table_entry + offset);
    return page_table_entry + offset;
  }
  return addr;
}
uint32_t get_dir_index(uint32_t vaddr)
{
  return (vaddr>>22)&0x3ff;
}
uint32_t get_page_index(uint32_t vaddr)
{
  return (vaddr>>12)&0x3ff;
}
uint32_t get_page_offset(uint32_t vaddr)
{
  return vaddr&0xfff;
}
int get_CR0_PG(uint32_t CR0)
{
  //CR0的第一位是PG
  return CR0>>31;
}