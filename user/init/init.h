#ifndef INIT_H__
#define INIT_H__

// get flash start address from ddl framework
#ifndef LD_FLASH_START
#define LD_FLASH_START 0x0
#endif

void core_init();


#endif