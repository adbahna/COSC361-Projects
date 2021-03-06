//COSC 361 Spring 2017
//FUSE Project Template
//Dgreps
//Michael Goin
//Alex Bahna

#ifndef __cplusplus
#error "You must compile this using C++"
#endif
#include <fuse.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fs.h>

using namespace std;

//Use debugf() and NOT printf() for your messages.
//Uncomment #define DEBUG in block.h if you want messages to show

//Here is a list of error codes you can return for
//the fs_xxx() functions
//
//EPERM          1      /* Operation not permitted */
//ENOENT         2      /* No such file or directory */
//ESRCH          3      /* No such process */
//EINTR          4      /* Interrupted system call */
//EIO            5      /* I/O error */
//ENXIO          6      /* No such device or address */
//ENOMEM        12      /* Out of memory */
//EACCES        13      /* Permission denied */
//EFAULT        14      /* Bad address */
//EBUSY         16      /* Device or resource busy */
//EEXIST        17      /* File exists */
//ENOTDIR       20      /* Not a directory */
//EISDIR        21      /* Is a directory */
//EINVAL        22      /* Invalid argument */
//ENFILE        23      /* File table overflow */
//EMFILE        24      /* Too many open files */
//EFBIG         27      /* File too large */
//ENOSPC        28      /* No space left on device */
//ESPIPE        29      /* Illegal seek */
//EROFS         30      /* Read-only file system */
//EMLINK        31      /* Too many links */
//EPIPE         32      /* Broken pipe */
//ENOTEMPTY     36      /* Directory not empty */
//ENAMETOOLONG  40      /* The name given is too long */

//Use debugf and NOT printf() to make your
//debug outputs. Do not modify this function.
#if defined(DEBUG)
int debugf(const char *fmt, ...)
{
    int bytes = 0;
    va_list args;
    va_start(args, fmt);
    bytes = vfprintf(stderr, fmt, args);
    va_end(args);
    return bytes;
}
#else
int debugf(const char *fmt, ...)
{
    return 0;
}
#endif

BLOCK_HEADER* header;
map<string,NODE*> nodes;
map<int,BLOCK*> blocks;

//////////////////////////////////////////////////////////////////
//
// START HERE W/ fs_drive()
//
//////////////////////////////////////////////////////////////////
//Read the hard drive file specified by dname
//into memory. You may have to use globals to store
//the nodes and / or blocks.
//Return 0 if you read the hard drive successfully (good MAGIC, etc).
//If anything fails, return the proper error code (-EWHATEVER)
//Right now this returns -EIO, so you'll get an Input/Output error
//if you try to run this program without programming fs_drive.
//////////////////////////////////////////////////////////////////
int fs_drive(const char *dname)
{
    debugf("fs_drive: %s\n", dname);

    FILE* hdfd = fopen(dname, "r");

    header = (BLOCK_HEADER*)malloc(sizeof(BLOCK_HEADER));
    // read in block header
    fread(header, sizeof(BLOCK_HEADER), 1, hdfd);

    // check for incorrect magic
    for (int i = 0; i < 8; i++) {
        if (header->magic[i] != MAGIC[i])
            return -EINVAL;
    }

    // read in nodes
    for (unsigned int i = 0; i < header->nodes; i++) {
        // read node
        NODE* n = (NODE*)malloc(sizeof(NODE));
        fread(n, ONDISK_NODE_SIZE, 1, hdfd);
        n->blocks = NULL;

        if (n->size > 0) {
            // read in block offsets
            n->blocks = (uint64_t*)malloc(sizeof(uint64_t)*((n->size/header->block_size)+1));
            fread(n->blocks, sizeof(uint64_t),((n->size/header->block_size)+1), hdfd);
        }

        nodes.insert(make_pair(n->name,n));
    }

    // read in blocks
    for (unsigned int i = 0; i < header->blocks; i++) {
        BLOCK* b = (BLOCK*)malloc(sizeof(BLOCK));
        b->data = (char*)malloc(sizeof(char)*header->block_size);
        fread(b->data, sizeof(char), header->block_size, hdfd);
        blocks.insert(make_pair(i,b));
        debugf("%s\n\n", b->data);
    }

    for (map<string,NODE*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
        debugf("NODE: %s\n", it->first.c_str());
    }

    fclose(hdfd);

    return 0;
}

void coalesce_blocks()
{
    debugf("coalesce_blocks\n");

    int count = 0;
    map<int,int> block_offsets;
    // make a map between block offsets and contiguous offsets
    for (map<string,NODE*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
        if (it->second->size > 0) {
            for (unsigned int i = 0; i < (it->second->size/header->block_size)+1; i++) {
                block_offsets.insert(make_pair(it->second->blocks[i],count));
                it->second->blocks[i] = count;
                count++;
            }
        }
    }

    map<int,BLOCK*> blocks_copy = blocks;
    blocks.clear();
    // fills blocks with contiguous blocks
    for (map<int,int>::iterator it = block_offsets.begin(); it != block_offsets.end(); it++) {
        blocks.insert(make_pair(it->second,blocks_copy[it->first]));
    }
}

//////////////////////////////////////////////////////////////////
//Open a file <path>. This really doesn't have to do anything
//except see if the file exists. If the file does exist, return 0,
//otherwise return -ENOENT
//////////////////////////////////////////////////////////////////
int fs_open(const char *path, struct fuse_file_info *fi)
{
    debugf("fs_open: %s\n", path);

    if (nodes.find(path) == nodes.end())
        return -ENOENT;
    return 0;
}

//////////////////////////////////////////////////////////////////
//Create a file <path>. Create a new file and give it the mode
//given by <mode> OR'd with S_IFREG (regular file). If the name
//given by <path> is too long, return -ENAMETOOLONG. As with
//fs_write, if we're a read only file system
//(fi->flags & O_RDONLY), then return -EROFS.
//Otherwise, return 0 if all succeeds.
//////////////////////////////////////////////////////////////////
int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    debugf("fs_create: %s\n", path);

    if (strlen(path) > NAME_SIZE)
        return -ENAMETOOLONG;
    if (fi->flags & O_RDONLY)
        return -EROFS;
    // if the file already exists, leave
    if (nodes.find(path) != nodes.end())
        return 0;

    NODE* n = (NODE*)malloc(sizeof(NODE));
    strcpy(n->name,path);
    n->mode = mode | S_IFREG;
    n->ctime = time(NULL);
    n->atime = time(NULL);
    n->mtime = time(NULL);
    n->uid = getuid();
    n->gid = getgid();
    n->size = 0;
    n->blocks = NULL;

    nodes.insert(make_pair(path,n));

    return 0;
}

//////////////////////////////////////////////////////////////////
//Read a file <path>. You will be reading from the block and
//writing into <buf>, this buffer has a size of <size>. You will
//need to start the reading at the offset given by <offset>.
//////////////////////////////////////////////////////////////////
int fs_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    debugf("fs_read: %s\n", path);

    map <string, NODE *>::iterator it = nodes.find(path);
    if (it == nodes.end())
        return -ENOENT;

    // make sure we don't read more than the file has
    size = min(size,it->second->size);

    int block_offset = 0, byte_offset = 0;
    if (offset != 0) {
        block_offset = offset/header->block_size;
        byte_offset = offset%header->block_size;
    }

    uint64_t* start_block = it->second->blocks+block_offset;

    int count = 0, i = 0;
    size_t bytes_written = 0;
    // if there is a byte offset, we need to read just a section of the first block
    if (byte_offset != 0) {
        count = min(size-bytes_written,header->block_size-byte_offset);
        memcpy(buf+bytes_written, blocks[(*start_block)]->data+byte_offset, count);
        bytes_written += count;
        i++;
    }

    // keep reading blocks until we read size bytes
    while (bytes_written < size) {
        count = min(size-bytes_written,header->block_size);
        memcpy(buf+bytes_written, blocks[*(start_block+i)]->data, count);
        bytes_written += count;
        i++;
    }

    return bytes_written;
}

//////////////////////////////////////////////////////////////////
//Write a file <path>. If the file doesn't exist, it is first
//created with fs_create. You need to write the data given by
//<data> and size <size> into this file block. You will also need
//to write data starting at <offset> in your file. If there is not
//enough space, return -ENOSPC. Finally, if we're a read only file
//system (fi->flags & O_RDONLY), then return -EROFS
//If all works, return the number of bytes written.
//////////////////////////////////////////////////////////////////
int fs_write(const char *path, const char *data, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
    debugf("fs_write: %s\n", path);

    if (fi->flags & O_RDONLY)
        return -EROFS;
    map <string, NODE *>::iterator it = nodes.find(path);
    if (it == nodes.end())
        return -ENOENT;
    if (S_ISDIR(it->second->mode))
        return -EISDIR;

    // if there isn't enough space on the hard drive, quit
    if (((((size+offset-it->second->size)/header->block_size)+1+blocks.size())*header->block_size) > MAX_DRIVE_SIZE) {
        return -ENOSPC;
    }

    // if we need to allocate more space for the node
    if (((size+offset)/header->block_size) > (it->second->size/header->block_size)) {
        // allocate the new block offsets in the node
        it->second->blocks = (uint64_t*)realloc(it->second->blocks, sizeof(uint64_t)*(((size+offset)/header->block_size)+10));

        map<int,BLOCK*>::reverse_iterator rit;
        // find free block offsets we can use and allocate the blocks
        for (unsigned int i = (it->second->size/header->block_size)+1; i < ((size+offset)/header->block_size)+10; i++) {
            rit = blocks.rbegin();
            BLOCK* b = (BLOCK*)malloc(sizeof(BLOCK));
            b->data = (char*)malloc(sizeof(char)*header->block_size);

            it->second->blocks[i] = rit->first+1;
            blocks.insert(make_pair(rit->first+1,b));
        }

        it->second->size = size+offset;
    }

    off_t block_offset = 0, byte_offset = 0;
    if (offset != 0) {
        block_offset = offset/header->block_size;
        byte_offset = offset%header->block_size;
    }

    uint64_t* start_block = it->second->blocks+block_offset;

    int count = 0, i = 0;
    size_t bytes_written = 0;
    // if there is a byte offset, we need to read just a section of the first block
    if (byte_offset != 0) {
        count = min(size-bytes_written,header->block_size-byte_offset);
        memcpy(blocks[(*start_block)]->data+byte_offset, data+bytes_written, count);
        bytes_written += count;
        i++;
    }

    // keep reading blocks until we read size bytes
    while (bytes_written < size) {
        count = min(size-bytes_written,header->block_size);
        memcpy(blocks[*(start_block+i)]->data, data+bytes_written, count);
        bytes_written += count;
        i++;
    }

    return bytes_written;
}

//////////////////////////////////////////////////////////////////
//Get the attributes of file <path>. A static structure is passed
//to <s>, so you just have to fill the individual elements of s:
//s->st_mode = node->mode
//s->st_atime = node->atime
//s->st_uid = node->uid
//s->st_gid = node->gid
// ...
//Most of the names match 1-to-1, except the stat structure
//prefixes all fields with an st_*
//Please see stat for more information on the structure. Not
//all fields will be filled by your filesystem.
//////////////////////////////////////////////////////////////////
int fs_getattr(const char *path, struct stat *s)
{
    debugf("fs_getattr: %s\n", path);

    map <string, NODE *>::iterator it = nodes.find(path);
    if (it == nodes.end())
        return -ENOENT;

    s->st_mode = it->second->mode;
    s->st_nlink = 1;
    s->st_ctime = it->second->ctime;
    s->st_atime = it->second->atime;
    s->st_mtime = it->second->mtime;
    s->st_uid = it->second->uid;
    s->st_gid = it->second->gid;
    s->st_size = it->second->size;

    return 0;
}

//////////////////////////////////////////////////////////////////
//Read a directory <path>. This uses the function <filler> to
//write what directories and/or files are presented during an ls
//(list files).
//
//filler(buf, "somefile", 0, 0);
//
//You will see somefile when you do an ls
//(assuming it passes fs_getattr)
//////////////////////////////////////////////////////////////////
int fs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
{
    debugf("fs_readdir: %s\n", path);

    string path_s, s;
    string::size_type pos;
    map<string,NODE*>::iterator it = nodes.find(path);

    if (it == nodes.end() || (it->second->mode ^ (S_IFDIR | it->second->mode)) != 0)
        return -ENOTDIR;

    path_s = path;
    if (path_s == "/")
        path_s = "";

    for (it = nodes.begin(); it != nodes.end(); it++) {
        pos = it->first.rfind("/");

        // if the file/dir is in the folder given to us, buffer it
        if (it->first != "/" && path_s != it->first && path_s == it->first.substr(0,pos)) {
            filler(buf, it->first.substr(pos+1,string::npos).c_str(), 0, 0);
            debugf("FILLED: %s\n",it->first.substr(pos+1).c_str());
        }
    }

    //filler(buf, <name of file/directory>, 0, 0)
    filler(buf, ".", 0, 0);
    filler(buf, "..", 0, 0);

    //You MUST make sure that there is no front slashes in the name (second parameter to filler)
    //Otherwise, this will FAIL.

    return 0;
}

//////////////////////////////////////////////////////////////////
//Open a directory <path>. This is analagous to fs_open in that
//it just checks to see if the directory exists. If it does,
//return 0, otherwise return -ENOENT
//////////////////////////////////////////////////////////////////
int fs_opendir(const char *path, struct fuse_file_info *fi)
{
    debugf("fs_opendir: %s\n", path);

    map <string, NODE *>::iterator it = nodes.find(path);

    if (it == nodes.end() || (it->second->mode ^ (it->second->mode | S_IFDIR)) != 0)
        return -ENOENT;

    return 0;
}

//////////////////////////////////////////////////////////////////
//Change the mode (permissions) of <path> to <mode>
//////////////////////////////////////////////////////////////////
int fs_chmod(const char *path, mode_t mode)
{
    debugf("fs_chmod: %s\n", path);

    map<string,NODE*>::iterator it = nodes.find(path);
    if (it == nodes.end())
        return -ENOENT;

    it->second->mode = mode;

    return 0;
}

//////////////////////////////////////////////////////////////////
//Change the ownership of <path> to user id <uid> and group id <gid>
//////////////////////////////////////////////////////////////////
int fs_chown(const char *path, uid_t uid, gid_t gid)
{
    debugf("fs_chown: %s\n", path);

    map<string,NODE*>::iterator it = nodes.find(path);
    if (it == nodes.end())
        return -ENOENT;

    it->second->uid = uid;
    it->second->gid = gid;

    return 0;
}

//////////////////////////////////////////////////////////////////
//Unlink a file <path>. This function should return -EISDIR if a
//directory is given to <path> (do not unlink directories).
//Furthermore, you will not need to check O_RDONLY as this will
//be handled by the operating system.
//Otherwise, delete the file <path> and return 0.
//////////////////////////////////////////////////////////////////
int fs_unlink(const char *path)
{
    debugf("fs_unlink: %s\n", path);

    NODE * n;
    map <std::string, NODE *>::iterator it;
    it = nodes.find(path);

    if (it == nodes.end()) return -ENOENT;
    if (S_ISDIR(it->second->mode)) return -EISDIR;

    n = it->second;

    if (n->size > 0) {
        for (unsigned int i = 0; i < ((n->size / header->block_size) + 1); i++)
        {
            map<int,BLOCK*>::iterator bt = blocks.find(n->blocks[i]);
            free(bt->second->data);
            free(bt->second);
            blocks.erase(bt);
        }
    }

    free(n->blocks);
    free(n);
    nodes.erase(it);

    return 0;
}

//////////////////////////////////////////////////////////////////
//Mke a directory <path> with the given permissions <mode>. If
//the directory already exists, return -EEXIST. If this function
//succeeds, return 0.
//////////////////////////////////////////////////////////////////
int fs_mkdir(const char *path, mode_t mode)
{
    debugf("fs_mkdir: %s\n", path);

    if (strlen(path) > NAME_SIZE)
        return -ENAMETOOLONG;
    // if the file already exits, leave
    if (nodes.find(path) != nodes.end())
        return -EEXIST;

    NODE* n = (NODE*)malloc(sizeof(NODE));
    strcpy(n->name,path);
    n->mode = mode | S_IFDIR;
    n->ctime = time(NULL);
    n->atime = time(NULL);
    n->mtime = time(NULL);
    n->uid = getuid();
    n->gid = getgid();
    n->size = 0;
    n->blocks = NULL;

    nodes.insert(make_pair(path,n));

    return 0;
}

//////////////////////////////////////////////////////////////////
//Remove a directory. You have to check to see if it is
//empty first. If it isn't, return -ENOTEMPTY, otherwise
//remove the directory and return 0.
//////////////////////////////////////////////////////////////////
int fs_rmdir(const char *path)
{
    debugf("fs_rmdir: %s\n", path);

    char buf[NAME_SIZE];
    map<string,NODE*>::iterator it;

    sprintf(buf,"%s/",path);
    for (it = nodes.begin(); it != nodes.end(); it++) {
        // if any file has the directory as a substring in it's path,
        // then it must be inside of the directory and it isn't empty
        if (it->first.find(buf) != string::npos)
            return -ENOTEMPTY;
    }

    nodes.erase(path);

    return 0;
}

//////////////////////////////////////////////////////////////////
//Rename the file given by <path> to <new_name>
//Both <path> and <new_name> contain the full path. If
//the new_name's path doesn't exist return -ENOENT. If
//you were able to rename the node, then return 0.
//////////////////////////////////////////////////////////////////
int fs_rename(const char *path, const char *new_name)
{
    debugf("fs_rename: %s -> %s\n", path, new_name);

    map<string,NODE*>::iterator it = nodes.find(path), new_it;
    if (it == nodes.end())
        return -ENOENT;

    string new_s = new_name;
    size_t pos = new_s.rfind("/");
    if (pos != 0) {
        new_it = nodes.find(new_s.substr(0,pos));
        if (new_it == nodes.end())
            return -ENOENT;
    }

    NODE* n = it->second;
    nodes.erase(it);
    strcpy(n->name,new_name);
    nodes.insert(make_pair(n->name,n));

    return 0;
}

//////////////////////////////////////////////////////////////////
//Truncate the given file path to the given size
//Return 0 if successful, error code otherwise
//////////////////////////////////////////////////////////////////
int fs_truncate(const char *path, off_t size)
{
    debugf("fs_truncate: %s to size %d\n", path, size);

    map<string,NODE*>::iterator it = nodes.find(path);
    if (it == nodes.end())
        return -ENOENT;

    uint64_t* new_blocks = (uint64_t*)malloc(sizeof(uint64_t)*(size/header->block_size)+1);
    memcpy(new_blocks,it->second->blocks,sizeof(uint64_t)*(size/header->block_size)+1);
    free(it->second->blocks);
    it->second->size = size;
    it->second->blocks = new_blocks;

    return 0;
}

//////////////////////////////////////////////////////////////////
//fs_destroy is called when the mountpoint is unmounted
//this should save the hard drive back into <filename>
//////////////////////////////////////////////////////////////////
void fs_destroy(void *ptr)
{
    const char *filename = (const char *)ptr;
    debugf("fs_destroy: %s\n", filename);

    FILE* hdfd = fopen(filename, "w");

    // makes all the blocks contiguous so we can write to hard drive
    coalesce_blocks();

    // write block header
    header->nodes = nodes.size();
    header->blocks = blocks.size();
    fwrite(header,sizeof(BLOCK_HEADER),1,hdfd);

    // write nodes
    for (map<string,NODE*>::iterator it = nodes.begin(); it != nodes.end(); it++) {
        // read node
        fwrite(it->second, ONDISK_NODE_SIZE, 1, hdfd);

        if (it->second->size > 0) {
            // write block offsets
            fwrite(it->second->blocks, sizeof(uint64_t),((it->second->size/header->block_size)+1), hdfd);
        }
    }

    // write blocks
    for (map<int,BLOCK*>::iterator it = blocks.begin(); it != blocks.end(); it++) {
        fwrite(it->second->data, sizeof(char), header->block_size, hdfd);
    }

    fclose(hdfd);
}

//////////////////////////////////////////////////////////////////
//int main()
//DO NOT MODIFY THIS FUNCTION
////i//////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    fuse_operations *fops;
    char *evars[] = { "./fs", "-f", "mnt", NULL };
    int ret;

    if ((ret = fs_drive(HARD_DRIVE)) != 0) {
        debugf("Error reading hard drive: %s\n", strerror(-ret));
        return ret;
    }
    //FUSE operations
    fops = (struct fuse_operations *) calloc(1, sizeof(struct fuse_operations));
    fops->getattr = fs_getattr;
    fops->readdir = fs_readdir;
    fops->opendir = fs_opendir;
    fops->open = fs_open;
    fops->read = fs_read;
    fops->write = fs_write;
    fops->create = fs_create;
    fops->chmod = fs_chmod;
    fops->chown = fs_chown;
    fops->unlink = fs_unlink;
    fops->mkdir = fs_mkdir;
    fops->rmdir = fs_rmdir;
    fops->rename = fs_rename;
    fops->truncate = fs_truncate;
    fops->destroy = fs_destroy;

    debugf("Press CONTROL-C to quit\n\n");

    return fuse_main(sizeof(evars) / sizeof(evars[0]) - 1, evars, fops,
            (void *)HARD_DRIVE);
}
