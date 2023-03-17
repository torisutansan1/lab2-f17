#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {

//you write this

int i = 0;
int rc = 0;
struct proc* curproc = myproc();
uint sz = PGROUNDUP(curproc->sz);

acquire(&(shm_table.lock));
for (i = 0; i < 64; i++)
{
  if (shm_table.shm_pages[i].id != id) { continue; }
  else { rc = id; }

  if (rc != 0)
  { 
    mappages(curproc->pgdir, (char*) sz, PGSIZE, V2P(shm_table.shm_pages[rc].frame), PTE_W | PTE_U);
    *pointer = (char*) sz;
    shm_table.shm_pages[rc].refcnt++;
  }
  else
  {
    shm_table.shm_pages[rc].id = id;
    shm_table.shm_pages[rc].frame = kalloc();
    memset(shm_table.shm_pages[rc].frame, 0, PGSIZE);
    mappages(curproc->pgdir, (char*) sz, PGSIZE, V2P(shm_table.shm_pages[rc].frame), PTE_W | PTE_U);
    shm_table.shm_pages[rc].refcnt++;
  }
}
release(&(shm_table.lock));

return 0; //added to remove compiler warning -- you should decide what to return
}


// int shm_close(int id) {
// //you write this too!




// return 0; //added to remove compiler warning -- you should decide what to return
// }

int shm_close(int id){
  int found = 1;
  int j;
  // struct shm_table shmtable;
  for(j = 0; j < 64; j++){
    if(shm_table.shm_pages[j].id == id){
      found = 0;
      shm_table.shm_pages[j].refcnt--;
      if(shm_table.shm_pages[j].refcnt <= 0){
        shm_table.shm_pages[j].frame = 0;
        shm_table.shm_pages[j].refcnt = 0;
        shm_table.shm_pages[j].id = 0;
      }
      else{
        break;
      }
    }
  }
  return found;
}
