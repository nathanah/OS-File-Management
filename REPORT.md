#Phase 1

For phase 1, we initialized our variables: super_block (our superblock), the_fat (which holds our array of FAT blocks), open_files (which holds our array of file descriptors), and our root_dir_array (an array to hold the different files for our root directory).

The most important thing for `mount()` was to ensure that we created a fat for each block corresponding to each root directory entry that the files will be pointing into.

As for `unmount()`, there is a corner case where we do not want to end up unmounting the disk while there are still file descriptors opened (solved via an if statement we added to the bottom of `unmount()`. Also, since we created a separate global array to hold our FAT, we also have to make sure that it is written back to the super_block. We also made sure to free our created memory from malloc as well. 

For `info()`, we simply used for loops to find out how many empty fat blocks and empty files that we have in our root directory, and we used these numbers for our ratios 


#Phase 2

For `create()`, if the root directory has an empty slot, we fill it in with a file name along with its file size along with changing its first block to FAT_EOC (which is 0xFFF). 



#Phase 3


#Phase 4
