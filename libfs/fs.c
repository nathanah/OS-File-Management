#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

#define FAT_EOC 0xFFFF

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

typedef struct fd {
    int root_idx;
    size_t offset;
    uint8_t filename[16];
} fd;

//Declare global vars
root_dir *root_dir_array;
superblock super_block;
FAT the_fat = NULL;
fd open_files[FS_OPEN_MAX_COUNT];

int fs_mount(const char *diskname)
{
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
  the_fat = (uint16_t*)malloc(super_block.FAT_blocks*BLOCK_SIZE);
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

  // Initialize fd array
  for (int i = 0; i < FS_OPEN_MAX_COUNT; i++) {
    open_files[i].root_idx = -1;
  }

  return 0;
}

int fs_umount(void)
{
  // Check if any files are open
  for(int i = 0; i < FS_OPEN_MAX_COUNT; i++){
    if(open_files[i].root_idx != -1){
      return -1;
    }
  }
  //Write fat back to disk
  FAT buffer = (FAT)malloc(BLOCK_SIZE);
  for(int i = 1; i < super_block.root_index; i++){
    memcpy(buffer, the_fat + (i-1)*BLOCK_SIZE, BLOCK_SIZE);
    block_write(i, buffer);
  }
  free(buffer);

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

  int fat_empty_blocks = 0;
  int root_empty_files = 0;

  //Find empty blocks in FAT
  for(int i = 1; i < super_block.data_blocks; i++){
      if(the_fat[i] == 0){
        fat_empty_blocks++;
      }
  }

  //Find empty files in root_dir
  for(int i = 0; i < FS_FILE_MAX_COUNT; i++){
      if(root_dir_array[i].filename[0] == '\0'){
        root_empty_files++;
      }
  }

  printf("FS Info:\n");
  printf("total_blk_count=%d\n", super_block.total_blocks);
  printf("fat_blk_count=%d\n", super_block.FAT_blocks);
  printf("rdir_blk=%d\n", super_block.root_index);
  printf("data_blk=%d\n", super_block.data_index);
  printf("data_blk_count=%d\n", super_block.data_blocks);
  printf("fat_free_ratio=%d/%d\n", fat_empty_blocks,super_block.data_blocks);
  printf("rdir_free_ratio=%d/%d\n", root_empty_files,FS_FILE_MAX_COUNT);
  return 0;
}

int fs_create(const char *filename)
{
  //Error checking: Filename is NULL or if it is longer than 16)
  if (filename == NULL || strlen(filename) + 1 > FS_FILENAME_LEN) {
    return -1;
  }

  // Loop into our Root_dir array
  for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {

    //Checks for first file that is empty
    if(root_dir_array[i].filename[0] == '\0') {
      strcpy((char*) root_dir_array[i].filename , filename);
      root_dir_array[i].filesize = 0;
      root_dir_array[i].first_data_index = FAT_EOC;
      return 0;
    }

    //If it is not empty, ensure that our filename does not already exist
    else if (strcmp((char*)root_dir_array[i].filename, filename) == 0) {
      return -1;
    }
  }

  //If our Root_dir is full, error.
  return -1;
}

int fs_delete(const char *filename)
{
  // Check if file is open
  for(int i = 0; i < FS_OPEN_MAX_COUNT; i++){
    if(strcmp((char*)open_files[i].filename, filename) == 0){
      return -1;
    }
  }

  // Loop through Root_dir array
  for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {

    //If file name matches
    if(strcmp((char*) root_dir_array[i].filename, filename) == 0) {

      //Delete from FAT
      uint16_t index = root_dir_array[i].first_data_index;
      while(index != FAT_EOC){
        uint16_t next = the_fat[index];
        the_fat[index] = 0;
        index = next;
      }

      //Delete from root_dir
      root_dir_array[i].filename[0] = '\0';
    }
  }

  return 0;
}

int fs_ls(void)
{
  //Check for underlying virtual disk
  if (block_disk_count() == -1) {
    return -1;
  }

  //Handle LS Print statements
  printf("FS Ls:\n");
  for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
    if (root_dir_array[i].filename[0] != '\0')
      printf("File: %s, size: %d, data_blk: %d\n", root_dir_array[i].filename, root_dir_array[i].filesize, root_dir_array[i].first_data_index);
  }
  return 0;
}

int fs_open(const char *filename)
{
  int fd_index = -1;
  // Initialize fd array
  for (int i = 0; i < FS_OPEN_MAX_COUNT; i++) {
    if(open_files[i].root_idx == -1){
      fd_index = i;
      //open_count++;
    }
  }

  // no empty fd
  if(fd_index == -1){
    return -1;
  }


  int f_index = -1;
  // Loop through Root_dir array to find index
  for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
    //If file name matches
    if(strcmp((char*) root_dir_array[i].filename, filename) == 0) {
      f_index = i;
    }
  }
  // file with filename was not found
  if(f_index == -1){
    return -1;
  }

  open_files[fd_index].root_idx = f_index;
  open_files[fd_index].offset = 0;
  strcpy((char*)open_files[fd_index].filename, filename);

  return fd_index;
}

int fs_close(int fd)
{
  // Checks if fd is in range and if file is not open
  if(fd < 0 || fd > FS_FILE_MAX_COUNT || open_files[fd].root_idx == -1){
    return -1;
  }

  open_files[fd].root_idx = -1;

  return 0;
}

int fs_stat(int fd)
{
  // Checks if fd is in range and if file is not open
  if(fd < 0 || fd > FS_FILE_MAX_COUNT || open_files[fd].root_idx == -1){
    return -1;
  }

  return root_dir_array[open_files[fd].root_idx].filesize;
}

int fs_lseek(int fd, size_t offset)
{
  // Checks if fd is in range and if file is not open
  if(fd < 0 || fd > FS_FILE_MAX_COUNT || open_files[fd].root_idx == -1){
    return -1;
  }
  // Checks if offset is out of bounds
  if(offset >= root_dir_array[open_files[fd].root_idx].filesize){
    return -1;
  }

  open_files[fd].offset = offset;

  return 0;
}

int fs_write(int fd, void *buf, size_t count)
{
  // Checks if fd is in range and if file is not open
  if(fd < 0 || fd > FS_FILE_MAX_COUNT || open_files[fd].root_idx == -1){
    return -1;
  }

  int num_written = 0;
  root_dir *this_file = &root_dir_array[open_files[fd].root_idx];

  int block_idx = this_file->first_data_index;
  int block_offset = open_files[fd].offset;
  int block_num = 0;


  //if no data yet
  if(block_idx == FAT_EOC){
    // look for empty blocks
    for(int i = 1; i < super_block.data_blocks; i++){
      // add new block to chain
      if(the_fat[i] == 0){
        this_file->first_data_index = i;
        block_idx = i;
        the_fat[i] = FAT_EOC;
        break;
      }
      // if no empty blocks in FAT
      if (i == super_block.data_blocks){
        return 0;
      }
    }
  }

  // find block of offset
  while(block_offset >= BLOCK_SIZE){
    if(the_fat[block_idx] == FAT_EOC)
      return 0;
    block_offset -= BLOCK_SIZE;
    block_idx = the_fat[block_idx];
    block_num++;
  }

  // malloc block buffer
  void *block = (void*)malloc(BLOCK_SIZE);

  // copy from blocks while still data to write
  while(num_written < count){
    // Read full block
    block_read(block_idx + super_block.data_index, block);

    //calculate how many bytes to copy
    int end = BLOCK_SIZE - 1;

    int copynum = end - block_offset;
    if(num_written + copynum > count){
      copynum = count - num_written;
    }

    // copy from buffer
    memcpy((void*)(block + block_offset), (void*) (buf + num_written), copynum);
    num_written += copynum;
    block_write(block_idx + super_block.data_index, block);

    // swap to next block
    open_files[fd].offset += copynum;
    block_offset = 0;
    block_num++;

    // test for if last block and needs to create new block
    if(the_fat[block_idx] == FAT_EOC && num_written != count){
      // look for empty blocks
      for(int i = 1; i < super_block.data_blocks; i++){
        // add new block to chain
        if(the_fat[i] == 0){
          the_fat[block_idx] = i;
          the_fat[i] = FAT_EOC;
          break;
        }
        // if no empty blocks in FAT
        if (i == super_block.data_blocks){
          free(block);
          this_file->filesize = open_files[fd].offset;
          return num_written;
        }
      }
    }

    block_idx = the_fat[block_idx];
  }
  free(block);
  if(open_files[fd].offset > this_file->filesize){
    this_file->filesize = open_files[fd].offset;
  }

  return num_written;
}

int fs_read(int fd, void *buf, size_t count)
{
  // Checks if fd is in range and if file is not open
  if(fd < 0 || fd > FS_FILE_MAX_COUNT || open_files[fd].root_idx == -1){
    return -1;
  }

  int num_read = 0;
  root_dir *this_file = &root_dir_array[open_files[fd].root_idx];


  // find block of offset
  int block_idx = this_file->first_data_index;
  int block_offset = open_files[fd].offset;
  int block_num = 0;
  if(block_idx == FAT_EOC){
    return 0;
  }
  while(block_offset >= BLOCK_SIZE){
    if(the_fat[block_idx] == FAT_EOC)
      return 0;
    block_offset -= BLOCK_SIZE;
    block_idx = the_fat[block_idx];
    block_num++;
  }

  // malloc block buffer
  void *block = (void*)malloc(BLOCK_SIZE);

  // copy from blocks while still data to copy
  while(block_idx != FAT_EOC && num_read < count){
    // Read full block
    block_read(block_idx + super_block.data_index, block);

    //calculate how many bytes to copy
    int end = BLOCK_SIZE - 1;
    if(the_fat[block_idx] == FAT_EOC){
      end = this_file->filesize - block_num * BLOCK_SIZE;
    }

    int copynum = end - block_offset;
    if(num_read + copynum < count){
      copynum = count - num_read;
    }

    // copy to buffer
    memcpy((void*) (buf+num_read), (void*)(block + block_offset), copynum);
    num_read += copynum;

    // swap to next block
    open_files[fd].offset += copynum;
    block_offset = 0;
    block_idx = the_fat[block_idx];
    block_num++;
  }
  free(block);

  return num_read;
}
