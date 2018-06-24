#define MAXINODE 50
#define READ 1
#define WRITE 2
#define MAXFILESIZE 1024
#define REGULAR 1
#define SPETIAL 2
#define START 0
#define CURRENT 1
#define END 2

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>


typedef struct superblock
{
	int totalinodes;
	int freeinodes;
}SUPERBLOCK, *PSUPERBLOCK;


typedef struct inode
{
	char filename[50];
	int inodenumber;
	int filesize;
	int fileactualsize;
	int filetype;
	char* buffer;
	int linkcount;
	int refrencecount;
	int permission;
	struct inode *next;
}INODE, *PINODE, **PPINODE;


typedef struct filetable
{
	int readoffset;
	int writeoffset;
	int count;
	int mode;
	PINODE ptrinode;
}FILETABLE, *PFILETABLE;


typedef struct ufdt
{
	PFILETABLE ptrfiletable;
}UFDT;


UFDT UFDTARR[MAXINODE];
SUPERBLOCK SUPERBLOCKOBJ;
PINODE head = NULL;


void initializesuperblock()
{
	int i = 0;
	while (i<MAXINODE)
	{
		UFDTARR[i].ptrfiletable = NULL;
		i++;
	}
	SUPERBLOCKOBJ.totalinodes = MAXINODE;
	SUPERBLOCKOBJ.freeinodes = MAXINODE;
}


void createDILB()
{
	PINODE newn = NULL;
	PINODE temp = head;
	int i = 1;
	while (i <= MAXINODE)
	{
		newn = (PINODE)malloc(sizeof(INODE));
		newn->linkcount = 0;
		newn->refrencecount = 0;
		newn->filetype = 0;
		newn->filesize = 0;
		newn->buffer = NULL;
		newn->next = NULL;
		newn->inodenumber = i;
		if (temp == NULL)
		{
			head = newn;
			temp = head;
		}
		else
		{
				temp->next=newn;
				temp = temp->next;
		}
		i++;	
	}
}

void ls_file()
{
	PINODE temp = head;

	if (SUPERBLOCKOBJ.freeinodes == MAXINODE)
	{
		printf("ERROR:there are no file\n");
		return;
	}

	printf("\n\nfilename\tinodenumber\tfilesize\tlinkcount\n");
	printf("-----------------------------------------------------\n");
	while (temp != NULL)
	{
		if (temp->filetype != 0)
		{
			printf("%s\t\t%d\t\t%d\t\t%d\n", temp->filename, temp->inodenumber, temp->fileactualsize, temp->linkcount);
		}
		temp = temp->next;
	}
	printf("------------------------------------------------------\n\n");
}


void closeallfile()
{
	int i = 0;
	while (i<50)
	{
		if (UFDTARR[i].ptrfiletable != NULL)
		{
			UFDTARR[i].ptrfiletable->readoffset = 0;
			UFDTARR[i].ptrfiletable->writeoffset = 0;
			(UFDTARR[i].ptrfiletable->ptrinode->refrencecount)--;
		}
		i++;
	}
	printf("\nall files close sucssesfully\n\n");
}


void displayhelp()
{
	printf("\n\n\tls\t:to list out all files\n");
	printf("\tcloseall\t:to cose all opened file\n");	
	printf("\tclear\t:clear the cansole\n");
	printf("\texit\t:terminate file system\n\n");

	printf("\tstat\t:to display the ifo about file using name\n");
	printf("\tfstat\t:to display info about file using file discripter\n");
	printf("\tclose\t:to colse file\n");	
	printf("\trm\t:to delete the file\n\n");
	printf("\tman\t:command discription\n\n");
	printf("\twrite\t:to write contents into file\n");
	printf("\ttruncate\t:to remove all data from file\n\n");

	printf("\tcreate\t:to create the file\n");
	printf("\topen\t:to open the file\n");
	printf("\tread\t:to read the containts from file\n\n");
	
	printf("\tlseek\t:jumping in the file\n\n");
	
}


int stat_file(char* name)
{
	PINODE temp = head;
	//if (name == NULL)
	//	return -1;
	while (temp != NULL)
	{
		if (strcmp(name, temp->filename)==0)
			break;
		temp = temp->next;
	}
	if (temp == NULL)
		return -2;

	printf("\n\n--------statestical info about file-------\n");
	printf("file name: %s\n", temp->filename);
	printf("inode number:%d\n", temp->inodenumber);
	printf("file size:%d\n", temp->filesize);
	printf("file actual size:%d\n", temp->fileactualsize);
	printf("link count:%d\n", temp->linkcount);
	printf("refrence count:%d\n", temp->refrencecount);
	if (temp->permission == 1)
		printf("file permisson:read only\n");
	else if (temp->permission == 2)
		printf("file permisson:write\n");
	else if (temp->permission == 3)
		printf("file permisson:read & write\n");
	printf("----------------------------------------------\n\n");
	return 0;
}


int fstat_file(int fd)
{
	PINODE temp = head;
	if (fd<0)
		return -1;
	if (UFDTARR[fd].ptrfiletable == NULL)
		return -2;

	temp = UFDTARR[fd].ptrfiletable->ptrinode;

	printf("\n\n--------statestical info about file-------\n");
	printf("file name: %s\n", temp->filename);
	printf("inode number:%d\n", temp->inodenumber);
	printf("file size:%d\n", temp->filesize);
	printf("file actual size:%d\n", temp->fileactualsize);
	printf("link count:%d\n", temp->linkcount);
	printf("refrence count:%d\n", temp->refrencecount);
	if (temp->permission == 1)
		printf("file permisson:read only\n");
	else if (temp->permission == 2)
		printf("file permisson:write\n");
	else if (temp->permission == 3)
		printf("file permisson:read & write\n");
	printf("----------------------------------------------\n\n");
	return 0;

}

int GetfdFromName(char *name)
{
	int i = 0;
	while (i<50)
	{
		if (UFDTARR[i].ptrfiletable != NULL)
		{
			if (strcmp((UFDTARR[i].ptrfiletable->ptrinode->filename), name) == 0)
				break;
		}i++;
	}
	if (i == 50)
		return -1;
	else
		return i;
}


int closefilebyname(char* name)
{
	int i = 0;
	i = GetfdFromName(name);
	if (i == -1)
		return -1;
	UFDTARR[i].ptrfiletable->readoffset = 0;
	UFDTARR[i].ptrfiletable->writeoffset = 0;
	(UFDTARR[i].ptrfiletable->ptrinode->refrencecount)--;
	return 0;
}

int rm_file(char* name)
{
	int fd = 0;
	fd = GetfdFromName(name);
	if (fd == -1)
		return -1;
	(UFDTARR[fd].ptrfiletable->ptrinode->linkcount)--;
	if (UFDTARR[fd].ptrfiletable->ptrinode->linkcount == 0)
	{
		UFDTARR[fd].ptrfiletable->ptrinode->filetype = 0;
		free(UFDTARR[fd].ptrfiletable);
	}
	UFDTARR[fd].ptrfiletable = NULL;
	(SUPERBLOCKOBJ.freeinodes)++;
}

void man(char* name)
{

}

int writefile(int fd, char* arr, int isize)
{
	if (((UFDTARR[fd].ptrfiletable->mode) != WRITE) && ((UFDTARR[fd].ptrfiletable->mode) != READ + WRITE))
		return -1;
	if (((UFDTARR[fd].ptrfiletable->ptrinode->permission) != WRITE) && ((UFDTARR[fd].ptrfiletable->ptrinode->permission) != READ + WRITE))
		return -1;
	if ((UFDTARR[fd].ptrfiletable->writeoffset) == MAXFILESIZE)
		return -2;
	if ((UFDTARR[fd].ptrfiletable->ptrinode->filetype) != REGULAR)
		return -3;
	strncpy((UFDTARR[fd].ptrfiletable->ptrinode->buffer) + (UFDTARR[fd].ptrfiletable->writeoffset), arr,isize);
	(UFDTARR[fd].ptrfiletable->writeoffset) = (UFDTARR[fd].ptrfiletable->writeoffset) + isize;
	(UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize) = (UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize) + isize;
	return isize;
}

int truncate_file(char *name)
{
	int fd = 0;
	fd = GetfdFromName(name);
	if (fd == -1)
		return -1;
	memset(UFDTARR[fd].ptrfiletable->ptrinode->buffer, 0, 1024);
	UFDTARR[fd].ptrfiletable->readoffset = 0;
	UFDTARR[fd].ptrfiletable->writeoffset = 0;
	UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize = 0;
}

PINODE getinode(char* name)
{
	PINODE temp = head;
	if (name == NULL)
		return NULL;
	while (temp != NULL)
	{
		if (strcmp(name, temp->filename) == 0)
			break;
		temp = temp->next;
	}
	return temp;
}

int createfile(char* name, int permission)
{
	int i = 0;
	PINODE temp = head;
	if ((name == NULL) || (permission == 0) || (permission>3))
		return -1;
	if (SUPERBLOCKOBJ.freeinodes == 0)
		return -2;
	if (getinode(name) != NULL)
		return -3;
	(SUPERBLOCKOBJ.freeinodes)--;
	while (temp != NULL)
	{
		if (temp->filetype == 0)
			break;
		temp = temp->next;
	}
	while (i<50)
	{
		if (UFDTARR[i].ptrfiletable == NULL)
			break;
		i++;
	}
	UFDTARR[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
	if (UFDTARR[i].ptrfiletable == NULL)
		return -4;

	UFDTARR[i].ptrfiletable->count = 1;
	UFDTARR[i].ptrfiletable->mode = permission;
	UFDTARR[i].ptrfiletable->readoffset = 0;
	UFDTARR[i].ptrfiletable->writeoffset = 0;
	UFDTARR[i].ptrfiletable->ptrinode = temp;
	strcpy(UFDTARR[i].ptrfiletable->ptrinode->filename,name);
	UFDTARR[i].ptrfiletable->ptrinode->filetype = REGULAR;
	UFDTARR[i].ptrfiletable->ptrinode->refrencecount = 1;
	UFDTARR[i].ptrfiletable->ptrinode->linkcount = 1;
	UFDTARR[i].ptrfiletable->ptrinode->filesize = MAXFILESIZE;
	UFDTARR[i].ptrfiletable->ptrinode->fileactualsize = 0;
	UFDTARR[i].ptrfiletable->ptrinode->permission = permission;
	UFDTARR[i].ptrfiletable->ptrinode->buffer=(char*)malloc(MAXFILESIZE);
	memset(UFDTARR[i].ptrfiletable->ptrinode->buffer, 0, 1024);

	return i;
}

int openfile(char* name, int mode)
{
	int i = 0;
	PINODE temp = NULL;
	if (name == NULL || mode <= 0)
		return -1;
	temp = getinode(name);
	if (temp == NULL)
		return -2;
	if (temp->permission<mode)
		return -3;
	while (i<50)
	{
		if (UFDTARR[i].ptrfiletable == NULL)
			break;
		i++;printf("all right %d\n",i);
	}
	UFDTARR[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
	if (UFDTARR[i].ptrfiletable == NULL)
		return -1;
	UFDTARR[i].ptrfiletable->count = 1;
	UFDTARR[i].ptrfiletable->mode = mode;

	if (mode == READ + WRITE)
	{
		UFDTARR[i].ptrfiletable->readoffset = 0;
		UFDTARR[i].ptrfiletable->writeoffset = 0;
	}
	else if (mode == READ)
	{
		UFDTARR[i].ptrfiletable->readoffset = 0;
	}
	else if (mode == WRITE)
	{
		UFDTARR[i].ptrfiletable->writeoffset = 0;
	}
	UFDTARR[i].ptrfiletable->ptrinode = temp;
	(UFDTARR[i].ptrfiletable->ptrinode->refrencecount)++;
	return i;
}

int readfile(int fd, char* arr, int isize)
{

	int read_size = 0;

	if (UFDTARR[fd].ptrfiletable == NULL)
		return -1;
	if (UFDTARR[fd].ptrfiletable->mode != READ && UFDTARR[fd].ptrfiletable->mode != READ + WRITE)
		return -2;
	if ((UFDTARR[fd].ptrfiletable->ptrinode->permission != READ) && (UFDTARR[fd].ptrfiletable->ptrinode->permission != READ + WRITE))
		return -2;
	if (UFDTARR[fd].ptrfiletable->readoffset == UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize)
		return -3;
	if (UFDTARR[fd].ptrfiletable->ptrinode->filetype != REGULAR)
		return -4;

	read_size = (UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize) - (UFDTARR[fd].ptrfiletable->readoffset);
	if (read_size<isize)
	{
		strncpy(arr, (UFDTARR[fd].ptrfiletable->ptrinode->buffer) + (UFDTARR[fd].ptrfiletable->readoffset), read_size);
		UFDTARR[fd].ptrfiletable->readoffset = UFDTARR[fd].ptrfiletable->readoffset + read_size;
	}
	else
	{
		strncpy(arr, (UFDTARR[fd].ptrfiletable->ptrinode->buffer) + (UFDTARR[fd].ptrfiletable->readoffset), isize);
		UFDTARR[fd].ptrfiletable->readoffset = UFDTARR[fd].ptrfiletable->readoffset + isize;
	}
	return isize;
}

int lseekfile(int fd, int size, int form)
{
	if ((fd<0) || (form>2))
		if (UFDTARR[fd].ptrfiletable == NULL)
			if ((UFDTARR[fd].ptrfiletable->mode == READ) || (UFDTARR[fd].ptrfiletable->mode == READ + WRITE))
			{
				if (form == CURRENT)
				{
					if (((UFDTARR[fd].ptrfiletable->readoffset) + size)>(UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize))
						return -1;
					if (((UFDTARR[fd].ptrfiletable->readoffset) + size)<0)
						return -1;
					(UFDTARR[fd].ptrfiletable->readoffset) = (UFDTARR[fd].ptrfiletable->readoffset) + size;
				}
				else if (form == START)
				{
					if (size>(UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize))
						return -1;
					if (size<0)
						return -1;
					(UFDTARR[fd].ptrfiletable->readoffset) = size;
				}
				else if (form == END)
				{
					if ((UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize) + size>MAXFILESIZE)
						return -1;
					if (((UFDTARR[fd].ptrfiletable->readoffset) + size)<0)
						return -1;
					(UFDTARR[fd].ptrfiletable->readoffset) = (UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize) + size;
				}

			}
			else if (UFDTARR[fd].ptrfiletable->mode == WRITE)
			{
				if (form == CURRENT)
				{
					if (((UFDTARR[fd].ptrfiletable->writeoffset) + size)>MAXFILESIZE)
						return -1;
					if (((UFDTARR[fd].ptrfiletable->writeoffset) + size)<0)
						return -1;
					if (((UFDTARR[fd].ptrfiletable->writeoffset) + size)>(UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize))
						(UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize) = (UFDTARR[fd].ptrfiletable->writeoffset) + size;
					(UFDTARR[fd].ptrfiletable->writeoffset) = (UFDTARR[fd].ptrfiletable->writeoffset) + size;
				}
				else if (form == START)
				{
					if (size>MAXFILESIZE)
						return -1;
					if (size<0)
						return -1;
					if (size>(UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize))
						(UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize) = size;
					(UFDTARR[fd].ptrfiletable->writeoffset) = size;
				}
				else if (form == END)
				{
					if ((UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize) + size>MAXFILESIZE)
						return -1;
					if (((UFDTARR[fd].ptrfiletable->writeoffset) + size)<0)
						return -1;
					(UFDTARR[fd].ptrfiletable->writeoffset) = (UFDTARR[fd].ptrfiletable->ptrinode->fileactualsize) + size;
				}
			}
}



int main()
{
	char* ptr = NULL;
	int ret = 0, fd = 0, count = 0;
	char command[4][80], str[80], arr[1024];

	initializesuperblock();
	createDILB();
	while (1)
	{
		//fflush(stdin);
		//strcpy(str,"");
		printf("abhishek@Macintosh:> ");
		fgets(str, 80, stdin);

		count = sscanf(str, "%s%s%s%s", command[0], command[1], command[2], command[3]);

		if (count == 1)
		{
			if (strcmp(command[0], "ls") == 0)
			{
				ls_file();
				continue;
			}
			else if (strcmp(command[0], "closeall") == 0)
			{
				closeallfile();
				continue;
			}
			else if (strcmp(command[0], "clear") == 0)
			{
				system("clear");
				continue;
			}
			else if (strcmp(command[0], "help") == 0)
			{
				displayhelp();
				continue;
			}
			else if (strcmp(command[0], "exit") == 0)
			{
				printf("\nTerminating V.F.S.\n\n");
				break;
			}
			else
			{
				printf("\n ERROR:command not found\n\n");
				continue;
			}
		}
		else if (count == 2)
		{
			if (strcmp(command[0], "stat") == 0)
			{
				ret = stat_file(command[1]);
				if (ret == -1)
					printf("\n ERROR:incorrect parameter\n\n");
				if (ret == -2)
					printf("\n ERORR:there is no such file\n\n");
				continue;
			}
			else if (strcmp(command[0], "fstat") == 0)
			{
				ret = fstat_file(atoi(command[1]));
				if (ret == -1)
					printf("\n ERROR:incorrect parameter\n\n");
				if (ret == -2)
					printf("\n ERORR:there is no such file\n\n");
				continue;
			}
			else if (strcmp(command[0], "close") == 0)
			{
				ret = closefilebyname(command[1]);
				if (ret == -1)
					printf("\n ERORR:there is no such file\n\n");
				continue;
			}
			else if (strcmp(command[0], "rm") == 0)
			{
				ret = rm_file(command[1]);
				if (ret == -1)
					printf("\n ERROR:there is no such file");
				printf("\n file removed successfully");
				continue;
			}
			else if (strcmp(command[0], "man") == 0)
			{
				man(command[1]);
				continue;
			}
			else if (strcmp(command[0], "write") == 0)
			{

				fd = GetfdFromName(command[1]);
				if (fd == -1)
				{
					printf("\n ERROR:incorrect parameter");
					continue;
				}
				printf("\n enter the data:\n");
				scanf("%[^\n]", arr);
				ret = strlen(arr);
				if (ret == 0)
				{
					printf("\n ERROR:incorrect parameter");
					continue;
				}

				ret = writefile(fd, arr, ret);
				if (ret == -1)
					printf("ERROR:permisson denied\n");
				if (ret == -2)
					printf("ERROR:no memory to write\n");
				if (ret == -3)
					printf("ERROR:it is not regular file\n");
			}
			else if (strcmp(command[0], "truncate") == 0)
			{
				ret = truncate_file(command[1]);
				if (ret == -1)
					printf("\n ERORR:incorrect parameter\n");
				continue;
			}
			else
			{
				printf("\n\n ERORR:command not found\n\n");
				continue;
			}

		}
		else if (count == 3)
		{
			if (strcmp(command[0], "create") == 0)
			{
				ret = createfile(command[1], atoi(command[2]));
				if (ret >= 0)
					printf("file is sucsessfully created with file discripter:%d\n", ret);
				if (ret == -1)
					printf("ERROR:incorrect parameter\n");
				if (ret == -2)
					printf("ERROR:there is no inode\n");
				if (ret == -3)
					printf("ERROR:file alrady exist\n");
				if (ret == -4)
					printf("ERROR:memory allocation failuar\n");
				continue;

			}
			else if (strcmp(command[0], "open") == 0)
			{
				ret = openfile(command[1], atoi(command[2]));
				if (ret >= 0)
					printf("file is sucsessfully opened with file discripter:%d\n", ret);
				if (ret == -1)
					printf("ERROR:incorrect parameter\n");
				if (ret == -2)
					printf("ERROR:file not present\n");
				if (ret == -3)
					printf("ERROR:permisson denied\n");
				continue;
			}
			else if (strcmp(command[0], "read") == 0)
			{
				fd = GetfdFromName(command[1]);
				if (fd == -1)
				{
					printf("ERROR:incorrect parameter");
					continue;
				}
				ptr = (char*)malloc(sizeof(atoi(command[2])) + 1);
				if (ptr == NULL)
				{
					printf("ERROR:memory allocation faluar\n");
					continue;
				}
				ret = readfile(fd, ptr, atoi(command[2]));
				if (ret>0)
					write(2, ptr, ret);
				if (ret == 0)
					printf("ERROR:file is empty\n");
				if (ret == -1)
					printf("ERROR:file not exist\n");
				if (ret == -2)
					printf("ERROR:permisson denied\n");
				if (ret == -3)
					printf("ERROR:reach at end of file\n");
				if (ret == -4)
					printf("ERROR:it is not regular file\n");
				continue;
			}
			else
			{
				printf("\nERROR:command not found\n\n");
				continue;
			}
		}
		else if (count == 4)
		{
			if (strcmp(command[0], "lseek") == 0)
			{
				fd = GetfdFromName(command[1]);
				if (fd == -1)
				{
					printf("ERROR:incorrect parameter\n");
					continue;
				}
				ret = (lseekfile(fd, atoi(command[2]), atoi(command[3])));
				if (ret == -1)
				{
					printf("ERROR:unable to perform lseek\n");
				}
			}
			else
			{
				printf("\n Error:command not found\n\n");
				continue;
			}
		}
		else
			continue;
	}
	return 0;
}

