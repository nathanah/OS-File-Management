# Phase 1

For phase 1, we initialized our variables: super_block (our superblock), the_fat (which holds our array of FAT blocks), open_files (which holds our array of file descriptors), and our root_dir_array (an array to hold the different files for our root directory).

The most important thing for `mount()` is to make sure that our file descriptors are initiated, so we do not have to go through the trouble of initializing them in open. 

As for `unmount()`, there is a corner case where we do not want to end up unmounting the disk while there are still file descriptors opened (solved via an if statement we added to the bottom of `unmount()`. Also, since we created a separate global array to hold our FAT, we also have to make sure that it is written back to the super_block. We also made sure to free our created memory from malloc as well. 

For `info()`, we simply used for loops to find out how many empty fat blocks and empty files that we have in our root directory, and we used these numbers for our ratios 


# Phase 2

For `create()`, if the root directory has an empty slot, we fill it in with a file name along with its file size along with changing its first block to FAT_EOC (which is 0xFFF). We also checked to make sure that the root directory is not already full.

For `delete()`, there are two important items we have to do. When we call delete, we want to make sure that our file-related file chain in the fat is set back to 0. We also want to make sure a file cannot be deleted until the file itself has no open file descriptors.


# Phase 3

For `open()`, we loop through our root_directory_array in order to find the index that holds the root_index of the file descriptor. We also initialize its offset to 0. 

For `close()`, if we want to close a file descriptor, we set it equal to -1. 

For `stat()` and `lseek()`, an extra corner case we checked was whether or not an open_files root index is -1, because that means that the file descriptor is closed and does not exist for us to use via seek and lseek. 


# Phase 4

For our `read()`, we initially do error checking on the file descriptor. We then take the file descriptor's offset to find which block it is located in and the found block's offset. We then go into our while loop which reads the block and copies that data within the block into a buffer. We then iterate to the next block until our count has been reached or the EOF is reached. 

For `write()`, most of the process is the same, but we have to include adding blocks to the file. We have to initially assign a block for empty files, and we have to assign blocks at the EOF if our data has filled up its last block. 


# Testing for Phase 3-4

We created `thread_fs_write()` in the test file itself in which we can add files as well as write files to the disk. 
More specifically our write function also allows us to write files to a certain offset of other files and ensures that the correct offset is being used.  
Using the tester, we are able to see whether or not the data_blks are allocated correctly and if the size of the files are correct corresponding to what is contained in it.
When `cat()` is called, it ensures that our read is done accordingly as well. 
