#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
#include<io.h>

#define MAXINODE 50

#define READ 1
#define WRITE 2

#define MAXFILESIZE 2048

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2

typedef struct superblock
{
    int TotalInodes;
    int FreeInode;
}SUPERBLOCK, *PSUPERBLOCK;

typedef struct inode
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int permission; //1  23
    struct inode *next;
}INODE, *PINODE,**PPINODE;

typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode;
    PINODE ptrinode;
}FILETABLE,*PFILETABLE;

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[MAXFILESIZE];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    man
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    char *name
//                              The input given by user at the time of execution
//  Return                 :    NONE
//  Description            :    This function gives manual like description and usage for file command
//                              given by user.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void man(char *name)
{
if (name == NULL)
{
    return;
}

if (strcmp(name,"create")==0)
{
    printf("Description : Used to create new regular file\n");
    printf("Usage : create File_name Permission\n");
}
else if(strcmp(name,"read")==0)
{
    printf("Description : Used to read data from regular file\n");
    printf("Usage : read File_name No_Of_Bytes_To_Read\n");
}
else if(strcmp(name,"write")==0)
{
    printf("Description : Used to write into regular file\n");
    printf("Usage : write File_name\n After this enter the data that we want to write\n");
}
else if(strcmp(name,"ls")==0)
{
    printf("Description : Used to list all information of files\n");
    printf("Usage : ls\n");
}
else if(strcmp(name,"stat")==0)
{
    printf("Description : Used to display information of file\n");
    printf("Usage : stat File_name\n");
}
else if(strcmp(name,"fstat")==0)
{
    printf("Description : Used to display information of file\n");
    printf("Usage : fstat File_Descriptor\n");
}
else if(strcmp(name,"truncate")==0)
{
    printf("Description : Used to remove data from file\n");
    printf("Usage : truncat File_name\n");
}
else if(strcmp(name,"open")==0)
{
    printf("Description : Used to open existing file\n");
    printf("Usage : open File_name mode\n");
}
else if(strcmp(name,"close")==0)
{
    printf("Description : Used to close opened file\n");
    printf("Usage : close File_name\n");
}
else if(strcmp(name,"closeall")==0)
{
    printf("Description : Used to close all opened file\n");
    printf("Usage : closeall\n");
}
else if(strcmp(name,"lseek")==0)
{
    printf("Description : Used to change file offset\n");
    printf("Usage : lseek File_Name ChangeInOffset StartPoint\n");
}
else if(strcmp(name,"rm")==0)
{
    printf("Description : Used to delete the file\n");
    printf("Usage : rm File_name\n");
}
else
{
    printf("ERROR : No manual entry available.\n");
}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    DisplayHelp
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    NONE
//  Return                 :    NONE
//  Description            :    This function gives list of all file command.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DisplayHelp()
{
    printf("ls : To List out all files\n");
    printf("clear : To Clear console\n");
    printf("open : To Open the file\n");
    printf("close : To Close the file\n");
    printf("closeall : To Close all open files\n");
    printf("read : To Read the content from file\n");
    printf("write : To Write the content into file\n");
    printf("exit : To Terminate file system\n");
    printf("stat : To Display information of file using name \n");
    printf("fstat : To Display information of file using file descriptor\n");
    printf("truncate : To Remove all data from file\n");
    printf("rm : To Delete the file\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    GetFDFromName
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    This function takes one parameter as character pointer char *name
//                              The input given by user at the time of execution
//  Return                 :    int
//  Description            :    This function return fd and compare the file name with the UFDTArr file name   
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GetFDFromName(char *name)
{
    int i = 0;
    while (i<50)
    {
        if (UFDTArr[i].ptrfiletable != NULL)
            if (strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name)==0)
                break;
        i++;
    }
    if (i == 50)
    {
        return -1;
    }
    else
    {
        return i;
    }  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    Get_Inode
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    This function takes one parameter as character pointer char *name
//                              The input given by user at the time of execution
//  Return                 :    PINODE(struct *inode) 
//  Description            :    This function return addess of inode in which the given file name is present.   
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PINODE Get_Inode(char *name)
{
    PINODE temp = head;
    int i =0;

    if (name==NULL)
    {
        return NULL;
    }
    
    while (temp != NULL)
    {
        if (strcmp(name,temp->FileName)==0)
        {
            break;
        }
        temp = temp->next;  
    }
    return temp;    
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    CreateDILB
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    NONE
//  Return                 :    Void 
//  Description            :    This function create the iNode using singly linear linkedlist. 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CreateDILB()
{
    int i = 1;
    PINODE newn = NULL;
    PINODE temp = head;

    while (i<= MAXINODE)
    {
        newn = (PINODE)malloc(sizeof(INODE));

        newn->LinkCount = 0;
        newn->ReferenceCount = 0;
        newn->FileType = 0;
        newn->FileSize = 0;

        newn->Buffer = NULL;
        newn->next = NULL;

        newn->InodeNumber = i;

        if (temp == NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp->next = newn;
            temp = temp->next;
        }
        i++;    
    }
    printf("DIBL created successfully\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    InitialiseSuperBlock
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    NONE
//  Return                 :    Void 
//  Description            :    This function initialize superblock object TotalInode and freeInode to maxinode which is 50
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void InitialiseSuperBlock()
{
    int i = 0;
    while (i< MAXINODE)
    {
        UFDTArr[i].ptrfiletable = NULL;
        i++;
    }
    SUPERBLOCKobj.TotalInodes = MAXINODE;
    SUPERBLOCKobj.FreeInode = MAXINODE;    
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    CreateFile
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    char *name, int permission
//                              input given by user and permission in which mode do you want to create file 
//  Return                 :    int
//  Description            :    This function return UFDTArr create file with permission   
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CreateFile(char *name,int permission)
{
    int i = 3;
    PINODE temp = head;

    if ((name == NULL) || (permission == 0) || (permission > 3))
    {
        return -1;
    }

    if (SUPERBLOCKobj.FreeInode == 0)
        return -2;

    if (Get_Inode(name) != NULL)
        return -3;

    (SUPERBLOCKobj.FreeInode)--;
    
    while (temp != NULL)
    {
        if (temp->FileType == 0)
            break;
        temp=temp->next;
    }

    while (i<MAXINODE)
    {
       if (UFDTArr[i].ptrfiletable == NULL)
            break;
       i++;    
    }

    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = permission;
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;   

    UFDTArr[i].ptrfiletable->ptrinode = temp;

    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
    UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
    UFDTArr[i].ptrfiletable->ptrinode->permission = permission;
    UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char*)malloc(MAXFILESIZE);

    return i;   
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    rm_File
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    char *name
//                              input given by user at the time of execution 
//  Return                 :    int
//  Description            :    This function return the delete file or delete file  
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//rm_File("Demo.txt")
int rm_File(char * name)
{
    int fd = 0;

    fd = GetFDFromName(name);
    if(fd == -1)
    {
        return -1;
    }
    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;

    if (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
    {
        UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
        //free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer);
        free(UFDTArr[fd].ptrfiletable);
        return 0;
    }
    UFDTArr[fd].ptrfiletable = NULL;
    (SUPERBLOCKobj.FreeInode)++;
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    ReadFile
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    int fd, char *arr, int isize
//  Return                 :    int
//  Description            :    This function read the data from file 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ReadFile(int fd, char *arr,int isize)
{
    int read_size = 0;

    if(UFDTArr[fd].ptrfiletable == NULL)
        return -1;
    
    if (UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != READ+WRITE)
        return -2;
    
    if (UFDTArr[fd].ptrfiletable->ptrinode->permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->permission != READ+WRITE)
        return -2;
    
    if (UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
        return -3;
    
    if (UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
        return -4;
    
    read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);

    if (read_size < isize)
    {
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+ (UFDTArr[fd].ptrfiletable->readoffset),read_size);
        UFDTArr[fd].ptrfiletable->readoffset = UFDTArr[fd].ptrfiletable->readoffset+read_size;
    }
    else
    {
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+ (UFDTArr[fd].ptrfiletable->readoffset),isize);
        UFDTArr[fd].ptrfiletable->readoffset = (UFDTArr[fd].ptrfiletable->readoffset) + isize;
    }
    return isize;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    WriteFile
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    int fd, char *arr, int isize
//  Return                 :    int
//  Description            :    This function will write the contents into the file.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WriteFile(int fd,char *arr,int isize)
{
    if (((UFDTArr[fd].ptrfiletable->mode) != WRITE) && ((UFDTArr[fd].ptrfiletable->mode) != READ+WRITE))
        return -1;
    
    if (((UFDTArr[fd].ptrfiletable->ptrinode->permission) != WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->permission) != READ+WRITE))
        return -1; 
    
    if((UFDTArr[fd].ptrfiletable->writeoffset)== MAXFILESIZE)
        return -2;

    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType) != REGULAR)
        return -3;  

    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+ (UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);   

    (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + isize;

    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)= (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + isize;

    return isize;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    OpenFile
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    char *name,int mode
//  Return                 :    int
//  Description            :    This function will open the file in given mode(read ,write,read+write) 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int OpenFile(char *name,int mode)
{
    int i =0;
    PINODE temp = NULL;

    if (name == NULL || mode <= 0)
        return -1;

    temp = Get_Inode(name);
    if (temp == NULL)
        return -2;
    
    if (temp->permission < mode)
        return -3;

    while (i<50)
    {
        if (UFDTArr[i].ptrfiletable == NULL)
            break;
        i++;     
    }
    
    UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
    if (UFDTArr[i].ptrfiletable == NULL)
        return -1;
    UFDTArr[i].ptrfiletable->count = 1;
    UFDTArr[i].ptrfiletable->mode = mode;
    if (mode == READ+WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
        UFDTArr[i].ptrfiletable->writeoffset =0;
    }
    else if (mode == READ)
    {
        UFDTArr[i].ptrfiletable->readoffset = 0;
    }
    else if (mode == WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset = 0;
    }
    UFDTArr[i].ptrfiletable->ptrinode =temp;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;

    return i;    
}

void CloseFileByName(int fd)
{
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    (UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    CloseFileByName
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    char *name
//  Return                 :    int
//  Description            :    This function will close the file which is given by user 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CloseFileByName(char *name)
{
    int i = 0;
    i = GetFDFromName(name);

    if (i == -1)
        return -1;
    
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    CloseAllFile
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    NONE
//  Return                 :    void
// is used to change the location of the read/write pointer of a file descriptor.
//  Description            :    This function will close all the existing files. 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CloseAllFile()
{
    int i  = 0;
    while (i<50)
    {
        if (UFDTArr[i].ptrfiletable != NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset = 0;
            UFDTArr[i].ptrfiletable->writeoffset = 0;
            (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
            break;
        }
        i++;
    }  
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    LseekFile
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    int fd,int iSize,int From
//  Return                 :    int
//  Description            :    This function is used to change the location of the read/write pointer of a file descriptor.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int LseekFile(int fd,int size,int from)
{
    if ((fd<0) || (from >2))
        return -1;
    if (UFDTArr[fd].ptrfiletable == NULL)
        return -1;

    if ((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ+WRITE))
    {
        if (from == CURRENT)
        {
            if (((UFDTArr[fd].ptrfiletable->readoffset)+size) > UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
                return -1;
            if (((UFDTArr[fd].ptrfiletable->readoffset)+size) <0)
                return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset) +size;
        }
        else if (from == START)
        {
           if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                return -1;
           if (size < 0)
                return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = size;      
        }
        else if (from == END)
        {
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
                return -1;
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size) < 0)
                return -1;
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+ size;
        }    
    }
    else if(UFDTArr[fd].ptrfiletable->mode == WRITE)
    {
        if (from == CURRENT)
        {
            if (((UFDTArr[fd].ptrfiletable->writeoffset)+size) > MAXFILESIZE)
                return -1;
            if (((UFDTArr[fd].ptrfiletable->writeoffset)+size) <0)
                return -1;
            if (((UFDTArr[fd].ptrfiletable->writeoffset)+size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->writeoffset)+size;
                (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->writeoffset) + size;
        }
        else if (from == START)
        {
           if(size > MAXFILESIZE)
                return -1;
           if (size < 0)
                return -1;
            if(size > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
               (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = size;
            (UFDTArr[fd].ptrfiletable->writeoffset) = size;      
        }
        else if (from == END)
        {
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) + size > MAXFILESIZE)
                return -1;
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+size) < 0)
                return -1;
            (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+ size;
        }      
    }  
    return 1; 
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    ls_file
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    NONE
//  Return                 :    void 
//  Description            :    This function will list out all the existing file
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ls_file()
{
    int i = 0;
    PINODE temp = head;

    if (SUPERBLOCKobj.FreeInode == MAXINODE)
    {
        printf("Error : There are no files\n");
        return;
    }

    printf("\nFile Name\tInode number\tFile size\tLink Count\n");
    printf("-------------------------------------------------------\n");
    while (temp != NULL)
    {
        if (temp -> FileType != 0)
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount);
        }
        temp = temp -> next;       
    }
    printf("-------------------------------------------------------\n");   
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    fstat_file
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    int fd
//  Return                 :    int 
//  Description            :    This function will display all the information from inode of all existing file file
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int fstat_file(int fd)
{
    PINODE temp = head;
    int i  = 0;

    if(fd < 0)
        return -1;
    if(UFDTArr[fd].ptrfiletable == NULL)
        return -2;
    
    temp = UFDTArr[fd].ptrfiletable->ptrinode;

    printf("\n----------Statistical Information about file--------------\n");
    printf("File name : %s\n",temp->FileName);
    printf("Inode Number : %d\n",temp->InodeNumber);
    printf("File Size : %d\n",temp->FileSize);
    printf("Actual File Size : %d\n",temp->FileActualSize);
    printf("Link count : %d\n",temp->LinkCount);
    printf("Reference Count : %d\n",temp->ReferenceCount);

    if (temp->permission == 1)
        printf("File permission : Read only\n");
    else if (temp->permission == 2)
        printf("File permission : Write only\n");
    else if (temp->permission == 3)
        printf("File permission : Read & Write\n");
    printf("-----------------------------------------------------------\n\n");

    return 0;   
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    stat_file
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    char *name
//  Return                 :    int 
//  Description            :    This function will display all the informationfrom inode of the given file
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int stat_file(char *name)
{
    PINODE temp = head;
    int i  = 0;

    if(name == NULL)
        return -1;
    
    while (temp != NULL)
    {
        if(strcmp(name,temp->FileName)== 0)
            break;
        temp = temp->next;
    }
    
    if(temp  == NULL)
        return -2;

    printf("\n----------Statistical Information about file----------------\n");
    printf("File name : %s\n",temp->FileName);
    printf("Inode Number : %d\n",temp->InodeNumber);
    printf("File Size : %d\n",temp->FileSize);
    printf("Actual File Size : %d\n",temp->FileActualSize);
    printf("Link count : %d\n",temp->LinkCount);
    printf("Reference Count : %d\n",temp->ReferenceCount);

    if (temp->permission == 1)
        printf("File permission : Read only\n");
    else if (temp->permission == 2)
        printf("File permission : Write only\n");
    else if (temp->permission == 3)
        printf("File permission : Read & Write\n");
    printf("-------------------------------------------------------------\n\n");

    return 0;   
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Function Name          :    truncate_file
//  Function Date          :    13/07/2024
//  Function Author        :    Dnyaneshwari Jadhav
//  Parameters             :    char *name
//  Return                 :    int 
//  Description            :    This function will remove all the data from existing file
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int truncate_File(char *name)
{
    int fd = GetFDFromName(name);
    if(fd == -1)
        return -1;
    
    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
    UFDTArr[fd].ptrfiletable->readoffset  =0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;

    return 1;
}

int main()
{
    char *ptr = NULL;
    int ret = 0, fd = 0, count = 0;
    char command[4][80], str[80], arr[1024];

    InitialiseSuperBlock();
    CreateDILB();

    while (1)
    {
        fflush(stdin);
        strcpy(str,"");

        printf("\nMarvellous VFS : > ");

        fgets(str,80,stdin);//scanf("%[^'\n']s",str);

        count = sscanf(str,"%s%s%s%s",command[0],command[1],command[2],command[3]);

        if (count == 1)
        {
            if (strcmp(command[0],"ls")==0)
            {
                ls_file();
            }
            else if (strcmp(command[0],"closeall")==0)
            {
                CloseAllFile();
                printf("All files closed successfully\n");
                continue;
            }
            else if (strcmp(command[0],"clear")==0)
            {
                system("clear");//
                continue;
            }
            else if (strcmp(command[0],"help")==0)
            {
                DisplayHelp();
                continue;
            }
            else if (strcmp(command[0],"exit")==0)
            {
                printf("Terminating the Mavrvellous Virtual File System\n");
                break;
            }
            else
            {
                printf("\nERROR : Command not Fount !!! \n");
                continue;
            } 
        }
        else if (count == 2)
        {
            if (strcmp(command[0],"stat")== 0)
            {
                ret = stat_file(command[1]);
                if(ret == -1)
                    printf("ERROR : Incorrect parameters\n");
                if(ret == -2)
                    printf("ERROR : There is no such file\n");
                continue;
            }
            else if (strcmp(command[0],"fstat")== 0)
            {
                ret = fstat_file(atoi(command[1]));
                if(ret == -1)
                    printf("ERROR : Incorrect parameters\n");
                if(ret == -2)
                    printf("ERROR : There is no such file\n");
                continue;
            }
            else if (strcmp(command[0],"close")== 0)
            {
                ret = CloseFileByName(command[1]);
                if(ret == -1)
                    printf("ERROR : There is no such file\n");
                continue;
            }
            else if (strcmp(command[0],"rm")== 0)
            {
                ret = rm_File(command[1]);
                if(ret == -1)
                    printf("ERROR : There is no such file\n");
                continue;
            }
            else if (strcmp(command[0],"man")== 0)
            {
                man(command[1]);
            }
            else if (strcmp(command[0],"write")== 0)
            {
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                    printf("ERROR : Incorrect Parameter\n");
                    continue;
                }
                printf("Enter the data : \n");
                scanf("%[^\n]",arr);

                ret  = strlen(arr);
                if (ret == 0)
                {
                    printf("ERROR : Incorrect parameter\n");
                    continue;
                }
                ret = WriteFile(fd,arr,ret);
                if (ret == -1)
                    printf("ERROR : Permission denied\n");
                if (ret == -2)
                    printf("ERROR : There is no sufficient memory to write\n");
                if (ret == -3)
                    printf("ERROR : It is not regular file\n");   
            }
            else if (strcmp(command[0],"truncate")==0)
            {
                ret = truncate_File(command[1]);
                if (ret == -1)
                    printf("ERROR : Incorrect parameter\n");  
            }
            else
            {
                printf("\nERROR : Command not found !!!\n");
                continue;
            }    
        }
        else if (count == 3) //jar command madhe 3 words asatil ter
        {
            if (strcmp(command[0],"create")==0)
            {
                ret = CreateFile(command[1],atoi(command[2])); //ascii to integer covert karto
                if (ret >= 0)
                    printf("File is successfully created with file descriptor : %d\n",ret);
                if (ret == -1)
                    printf("ERROR : Incorrect parameters\n");
                if (ret == -2)
                    printf("ERROR : There is no inodes\n");
                if (ret == -3)
                    printf("ERROR : File already exists\n");
                if (ret == -4)
                    printf("ERROR : Memory allocation failure\n");
                continue;    
            }
            else if (strcmp(command[0],"open")==0)
            {
                ret = OpenFile(command[1],atoi(command[2]));
                if (ret >= 0)
                    printf("File is successfully opened with file descriptor : %d\n",ret);
                if (ret == -1)
                    printf("ERROR : Incorrect parameters\n");
                if (ret == -2)
                    printf("ERROR : File not present\n");
                if (ret == -3)
                    printf("ERROR : Permission denied\n");
                continue;    
            }
            else if (strcmp(command[0],"read")==0)
            {
                fd = GetFDFromName(command[1]);
                if (fd == -1)
                {
                    printf("Error : Incorrect Parameter\n");
                    continue;
                }
                ptr = (char*)malloc(sizeof(atoi(command[2]))+1);
                if (ptr == NULL)
                {
                    printf("ERROR : Memory allocation failure\n");
                    continue;
                }
                ret = ReadFile(fd,ptr,atoi(command[2]));
                if (ret == -1)
                    printf("ERROR : File not existing\n");
                if (ret == -2)
                    printf("ERROR : Permission denied\n");
                if (ret == -3)
                    printf("ERROR : Reached at end of the file\n");
                if (ret == -4)
                    printf("ERROR : It is not a regular file\n");
                if (ret == 0)
                    printf("ERROR : File empty\n");
                if (ret > 0)
                {
                    write(2,ptr,ret);
                }
                continue;    
            }
            else
            {
                printf("\nERROR : Command not found");
                continue;
            }  
        }
        else if (count == 4)
        {
            if (strcmp(command[0],"lseek")==0)
            {
                fd = GetFDFromName(command[1]);
                if (fd == -1)
                {
                    printf("ERROR : Incorrect parameter\n");
                    continue;
                }
                ret = LseekFile(fd,atoi(command[2]),atoi(command[3]));
                if (ret == -1)
                {
                    printf("ERROR : Unable to perform lseek\n");
                }
            }
            else
            {
                printf("ERROR : Command not found\n");
                continue;
            }   
        }
        else
        {
            printf("ERROR : Command not found\n");
            continue;
        }    
    }
    return 0;
}