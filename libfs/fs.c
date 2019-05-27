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
    char signature[8];
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
    uint8_t padding[10];
}__attribute__((__packed__)) root_dir;


//Declare global vars
root_dir *root_dir_array;
superblock super_block;
FAT the_fat = NULL;

int fs_mount(const char *diskname)
{
	/* TODO: Phase 1 */
  //Open disk, ret -1 if no disk to open
  if(block_disk_open(diskname) == -1) {
    return -1;
  }

  //Read block
  block_read (0, (void*) &super_block);

  //Check signature
  char* sigHolder = "ECS150FS";
  if(memcmp((char*) &super_block.signature, sigHolder, 8) != 0) {
    return -1;
  }

  //Check block count
  if (super_block.total_blocks != block_disk_count()) {
    return -1;
  }

  //Create the FAT
  the_fat = (uint16_t*)malloc(super_block.data_blocks*sizeof(uint16_t));
  if (the_fat == NULL) {
    return -1;
  }

  for (int i = 1; i < super_block.root_index; i++) {
    block_read(i, ((void*)the_fat) + BLOCK_SIZE * (i - 1));
  }

  //Create root array + check if disk can be read
  root_dir_array = (root_dir*)malloc(FS_FILE_MAX_COUNT * sizeof(root_dir));
  if (block_read(super_block.root_index, (void*) root_dir_array) == -1) {
    return -1;
  }

  // Set boolean holder to true
  //mounted = true;

  return 0;
}

int fs_umount(void)
{
  /* TODO: Phase 1 */
  //Write fat back to disk
  for(int i = 1; i < super_block.root_index; i++){
    FAT buffer;
    memcpy(buffer, the_fat + (i-1)*BLOCK_SIZE, BLOCK_SIZE);
    block_write(i, buffer);
  }

  //Write root dir back to disk
  block_write(super_block.root_index, root_dir_array);

  //close disk (-1 if no disk opened)
  if(block_disk_close() == -1)
    return -1;

  free(the_fat);
  free(root_dir_array);

  return 0;
}

int fs_info(void)
{
	/* TODO: Phase 1 */
  printf("signature:    %s\n", super_block.signature);
  printf("total_blocks: %d\n", super_block.total_blocks);
  printf("FAT_blocks:   %d\n", super_block.FAT_blocks);
  printf("root_index:   %d\n", super_block.root_index);
  printf("data_index:   %d\n", super_block.data_index);
  printf("data_blocks:  %d\n", super_block.data_blocks);
  return 0;
}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */
  //Helper Variable for checking if the root is full
  int count = 0;

  //Error checking: Filename is Null or if it is longer than 16)
  if (filename == NULL) {
    return -1;
  }
  if (strlen(filename) + 1 > FS_FILENAME_LEN) {
    return -1;
  }

  // Loop into our Root_dir array
  for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {

    //Checks for first file that is empty
    if(root_dir_array[i].filename[0] == '\0') {
      strcpy((char*) root_dir_array.filename , filename);
      root_dir_array[i].filesize = 0;
      root_dir_array[i].first_data_index = FAT_EOC;
    }

    //If it is not empty, ensure that our filename does not already exist
    else {
      if (strcmp((char*)root_dir_array[i].filename, filename) == 0) {
        return -1;
      }
    }
    count++;
  }

  //If our Root_dir is full, error.
  if (count == FS_FILE_MAX_COUNT) {
    return -1;
  }
  return 0;
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
  return 0;
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
  return 0;
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */
  return 0;
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
  return 0;
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
  return 0;
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
  return 0;
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
  return 0;
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
  return 0;
}
