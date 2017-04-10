#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
#include <fs.h>

using namespace std;

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " <block device file>\n";
		return -1;
	}

	ofstream fout(argv[1], ios::binary);
	if (!fout) {
		cout << "Unable to open file '" << argv[1] << "'\n";
		return -2;
	}

	BLOCK_HEADER bh;
	NODE n;
	char data[1024], data2[1024], data3[1024];
	uint64_t list;

	strncpy(bh.magic, MAGIC, 8);
	bh.block_size = 1024;
	bh.blocks = 3;
	bh.nodes = 6;

	fout.write((char*)&bh, sizeof(bh));

	memset(data, 0, sizeof(data));
	memset(data2, 0, sizeof(data2));
	memset(data3, 0, sizeof(data3));
	strcpy(data, "Hello World!\r\n\r\n\r\nThis is a test file!\n\n\n");
	strcpy(data2, "Not in my neighborhood!\n");
	strcpy(data3, "I am a file that is stuffed into a subdir!\n");

	strcpy(n.name, "/");
	n.id = 0;
	n.mode = S_IFDIR | 0755;
	n.ctime = time(NULL);
	n.atime = n.ctime;
	n.mtime = n.ctime;
	n.uid = getuid();
	n.gid = getgid();
	n.size = 0;
	fout.write((char*)&n, ONDISK_NODE_SIZE);

	strcpy(n.name, "/README.txt");
	n.id = 1;
	n.mode = S_IFREG | 0644;
	n.size = strlen(data);
	fout.write((char*)&n, ONDISK_NODE_SIZE);
	list = 0;
	fout.write((char*)&list, sizeof(list));

	n.id = 2;
	n.mode = S_IFREG | 0644;
	n.size = strlen(data2);
	strcpy(n.name, "/README.too");
	fout.write((char*)&n, ONDISK_NODE_SIZE);
	list = 1;
	fout.write((char*)&list, sizeof(list));

	n.id = 3;
	n.mode = S_IFDIR | 0755;
	n.size = 0;
	strcpy(n.name, "/some_subdir");
	fout.write((char*)&n, ONDISK_NODE_SIZE);

	n.id = 4;
	n.mode = S_IFDIR | 0755;
	n.size = 0;
	strcpy(n.name, "/some_subdir/subdir_2");
	fout.write((char*)&n, ONDISK_NODE_SIZE);

	n.id = 5;
	n.mode = S_IFREG | 0644;
	n.size = strlen(data3);
	strcpy(n.name, "/some_subdir/subdir_2/data");
	fout.write((char*)&n, ONDISK_NODE_SIZE);
	list = 2;
	fout.write((char*)&list, sizeof(list));

	fout.write(data, 1024);
	fout.write(data2, 1024);
	fout.write(data3, 1024);

	fout.close();

	return 0;
}
