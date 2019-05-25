#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

/* TODO: Phase 1 */
//Structures
typedef struct superblock {
    uint64_t signature;
    uint16_t total_blocks;
    uint16_t root_index;
    uint16_t data_index;
    uint16_t data_blocks;
    uint8_t FAT_blocks;
    uint8_t padding [4079];
}__attribute__((__packed__)) superblock;

typedef uint16_t* FAT;

typedef struct root_dir {
    uint8_t filename[16];
    uint32_t filesize;
    uint16_t first_data_index;
    uint8_t padding[10]
}__attribute__((__packed__)) root_dir;


//Declare global vars
typedef root *root_dir;
root_dir root_dir_array;


int fs_mount(const char *diskname)
{
	/* TODO: Phase 1 */
  if(block_disk_open(diskname) == -1)
    return -1;


  return 0;
}

int fs_umount(void)
{
	/* TODO: Phase 1 */
  if(block_disk_close() == -1)
    return -1;


  return 0;
}

int fs_info(void)
{
	/* TODO: Phase 1 */
}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}
