#ifndef INIT_H_
#define INIT_H_

// get flash start address from ddl framework
#ifndef LD_FLASH_START
#define LD_FLASH_START 0x0
#endif

void core_init();


#endif