//COSC 361 Spring 2017
//FUSE Project Template
//Group Name
//Group Member 1 Name
//Group Member 2 Name

#ifndef __cplusplus
#error "You must compile this using C++"
#endif
#include <fuse.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdlib>
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
	return -EIO;
}

//////////////////////////////////////////////////////////////////
//Open a file <path>. This really doesn't have to do anything
//except see if the file exists. If the file does exist, return 0,
//otherwise return -ENOENT
//////////////////////////////////////////////////////////////////
int fs_open(const char *path, struct fuse_file_info *fi)
{
	debugf("fs_open: %s\n", path);
	return -EIO;
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
	return -EIO;
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
	return -EIO;
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
	return -EIO;
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
	return -EIO;
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
	return -EIO;
}

//////////////////////////////////////////////////////////////////
//Change the mode (permissions) of <path> to <mode>
//////////////////////////////////////////////////////////////////
int fs_chmod(const char *path, mode_t mode)
{
	return -EIO;
}

//////////////////////////////////////////////////////////////////
//Change the ownership of <path> to user id <uid> and group id <gid>
//////////////////////////////////////////////////////////////////
int fs_chown(const char *path, uid_t uid, gid_t gid)
{
	debugf("fs_chown: %s\n", path);
	return -EIO;
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
	return -EIO;
}

//////////////////////////////////////////////////////////////////
//Make a directory <path> with the given permissions <mode>. If
//the directory already exists, return -EEXIST. If this function
//succeeds, return 0.
//////////////////////////////////////////////////////////////////
int fs_mkdir(const char *path, mode_t mode)
{
	debugf("fs_mkdir: %s\n", path);
	return -EIO;
}

//////////////////////////////////////////////////////////////////
//Remove a directory. You have to check to see if it is
//empty first. If it isn't, return -ENOTEMPTY, otherwise
//remove the directory and return 0.
//////////////////////////////////////////////////////////////////
int fs_rmdir(const char *path)
{
	debugf("fs_rmdir: %s\n", path);
	return -EIO;
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
	return -EIO;
}

//////////////////////////////////////////////////////////////////
//Rename the file given by <path> to <new_name>
//Both <path> and <new_name> contain the full path. If
//the new_name's path doesn't exist return -ENOENT. If
//you were able to rename the node, then return 0.
//////////////////////////////////////////////////////////////////
int fs_truncate(const char *path, off_t size)
{
	debugf("fs_truncate: %s to size %d\n", path, size);
	return -EIO;
}

//////////////////////////////////////////////////////////////////
//fs_destroy is called when the mountpoint is unmounted
//this should save the hard drive back into <filename>
//////////////////////////////////////////////////////////////////
void fs_destroy(void *ptr)
{
	const char *filename = (const char *)ptr;
	debugf("fs_destroy: %s\n", filename);

	//Save the internal data to the hard drive
	//specified by <filename>
}

//////////////////////////////////////////////////////////////////
//int main()
//DO NOT MODIFY THIS FUNCTION
//////////////////////////////////////////////////////////////////
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
