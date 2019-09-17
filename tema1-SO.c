#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#define MAX_LINE 1024


void listRecSimple(const char *path)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL)
    {
        perror("Could not open directory");
        return;
    }
    while((entry = readdir(dir)) != NULL)
    {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0)
            {
                printf("%s\n", fullPath);
                if(S_ISDIR(statbuf.st_mode))
                {
                    listRecSimple(fullPath);
                }
            }
        }
    }
    closedir(dir);
}


void listRec(const char *path, char filter[], char val[])
{
    int x;
    char name[100];
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL)
    {
        fprintf(stderr,"ERROR\ninvalid directory path\n");
        return;
    }
    while((entry = readdir(dir)) != NULL)
    {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0)
            {
                // printf("%s\n", fullPath);
                if(strcmp(filter,"size")==0)
                {
                    sscanf(val,"%d",&x);
                    if(S_ISREG(statbuf.st_mode))
                    {
                        if(statbuf.st_size<x)
                        {
                            printf("%s\n", fullPath);
                        }
                    }
                }
                else if(strcmp(filter,"name")==0)
                {
                    strcpy(name,entry->d_name);
                    char name2[100];
                    strcpy(name2,name+(strlen(name)-strlen(val)));
                    if(strcmp(name2, val)==0)
                    {
                        printf("%s\n", fullPath);
                    }
                }
                if(S_ISDIR(statbuf.st_mode))
                {
                    listRec(fullPath, filter, val);
                }
            }
        }
    }
    closedir(dir);//if(ok==0) printf("SUCCES\n");
}


int listDirSimple(const char *path)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL)
    {
        perror("Could not open directory");
        return -1;
    }
    else
    {
        printf("SUCCESS\n");
    }
    while((entry = readdir(dir)) != NULL)
    {
        snprintf(filePath, 512, "%s/%s", path, entry->d_name);
        if(lstat(filePath, &statbuf) == 0 && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            //          if(S_ISREG(statbuf.st_mode)  ) {
            printf("%s\n", filePath);

            //      } //else {
            // printf("[err] ");
            //}
        }
        //  printf("%s\n", entry->d_name);
    }
    closedir(dir);
    return 0;
}

int listDir(const char *path, char filter[], char val[])
{
    int x;
    char name[100];
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];//absPath[512];
    struct stat statbuf;
    //snprintf(absPath, 512, "%s/%s", path, entry->d_name);
    dir = opendir(path);
    if(dir == NULL)
    {
        perror("Could not open directory");
        return -1;
    }
    while((entry = readdir(dir)) != NULL)
    {
        snprintf(filePath, 512, "%s/%s", path, entry->d_name);
        if(lstat(filePath, &statbuf) == 0)
        {
            if(strcmp(filter,"size")==0)
            {
                sscanf(val,"%d",&x);
                if(S_ISREG(statbuf.st_mode))
                {
                    if(statbuf.st_size<x)
                    {
                        printf("%s\n", filePath);
                    }
                }
            }
            else if(strcmp(filter,"name")==0)
            {
                strcpy(name,entry->d_name);
                char name2[100];
                strcpy(name2,name+(strlen(name)-strlen(val)));
                if(strcmp(name2, val)==0)
                {
                    printf("%s\n", filePath);
                }
            }
        }
        else
        {
            printf("[err] ");
        }
    } //if(ok==0) printf("SUCCES\n");
    closedir(dir);
    return 0;
}


void parsare(const char *path)
{

    unsigned version=0,magic=0,header_size=0, no_of_sections=0,sect_size=0,sect_offset=0,sect_type=0;

    char name[9];
    unsigned stype[20],ssize[20];
    char arrname[20][9];
    int fd=-1;
//   off_t size;
    fd=open(path,O_RDONLY);
    if(fd<0)
    {
        perror("could not open input file");
        exit(1);
    }

    read(fd,&magic,4);
    if(magic!=0x314F374C)
    {
        printf("ERROR\nwrong magic\n");
        return;
    }

    read(fd,&header_size,2);

    read(fd,&version,4);
    if(version<66 || version>151)
    {
        printf("ERROR\nwrong version\n");
        return;
    }

    read(fd,&no_of_sections,1);
    if(no_of_sections<3 || no_of_sections>19)
    {
        printf("ERROR\nwrong sect_nr\n");
        return;
    }

    for(int k=0; k<no_of_sections; k++)
    {
        read(fd,&name,8);
        name[9]=0;//printf("char:%s\n",name);
        strcpy(arrname[k],name);//printf("arr:%s\n",arrname[k]);

        read(fd,&sect_type,4);
        stype[k]=sect_type;//printf("%d\n",stype[k]);

        read(fd,&sect_offset,4);
////
        read(fd,&sect_size,4);
        ssize[k]=sect_size;//printf("%d\n",ssize[k]);

        if(sect_type!=19 && sect_type!=85 && sect_type!=59 && sect_type!=33 && sect_type!=90 && sect_type!=49)
        {
            printf("ERROR\nwrong sect_types\n");
            return;
        }
    }
    printf("SUCCESS\n");
    printf("version=%d\n",version);
    printf("nr_sections=%d\n",no_of_sections);

    for(int l=0; l<no_of_sections; l++)
    {
        printf("section%d: %s %d %d\n",l+1,arrname[l],stype[l],ssize[l]);
    }

    close(fd);
    //return 0;
}


void findall(const char *path)
{
    unsigned no=0,sn=0,st=0,so=0,ss=0,magic=0,hs=0,ver=0;

    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;

    dir = opendir(path);
    if(dir == NULL)
    {
        perror("Could not open directory");
        return;
    }
    while((entry = readdir(dir)) != NULL)
    {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            int ok=1;
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0)
            {
                if(S_ISREG(statbuf.st_mode))
                {
                    int fd=-1;
//   off_t size;
                    fd=open(fullPath,O_RDONLY);
                    if(fd<0)
                    {
                        perror("could not open input file");
                        exit(1);
                    }
                    read(fd,&magic,4);if(magic!=0x314F374C){return;}
                    read(fd,&hs,2);
                    read(fd,&ver,4);if(ver<66 || ver>151){return;}
                    read(fd,&no,1);if(no<3 || no>19){return;}//printf("%d\n",no);
                    for(int i=0; i<no; i++)
                    {
                        read(fd,&sn,8);
                        read(fd,&st,4);
if(st!=19 && st!=85 && st!=59 && st!=33 && st!=90 && st!=49){return;}
                        read(fd,&so,4);
                        read(fd,&ss,4);
                        if(ss>1262)
                        {
                            ok=0;
                        }
                    }
                    if(ok==1)
                    {

                        printf("%s\n", fullPath);
                    }
                }
                if(S_ISDIR(statbuf.st_mode))
                {
                    findall(fullPath);
                }
            }
        }
    }
    closedir(dir);
}


int main(int argc, char **argv)
{
    char value[100]="", opt[50]="";
    int pathArg=2,rec=0;
    if(argc>=2)
    {
        if(strcmp(argv[1],"variant")==0)
        {
            printf("10213\n");
        }
        else if(strcmp(argv[1],"list")==0)
        {
            ///
            if(argc==4) //fara recursive
            {
                for(int i=2; i<4; i++)
                {
                    if(strstr(argv[i],"path="))
                    {
                        pathArg=i;
                    }
                    else if(strstr(argv[i],"size_smaller="))
                    {
                        strcat(value,argv[i]+13);
                        strcpy(opt,"size");
                    }
                    else if(strstr(argv[i],"name_ends_with="))
                    {
                        strcat(value,argv[i]+15);
                        strcpy(opt,"name");
                    }
                    else if(strstr(argv[i],"recursive"))
                    {
                        rec=1;
                    }
                }
                char s2[100];
                strcpy(s2,argv[pathArg]+5);
                //   strcat(s,s2);
                printf("SUCCESS\n");
                if(rec==1)
                {
                    listRecSimple(s2);
                }
                else
                {
                    listDir(s2,opt,value);
                }

            }
            else if(argc==5) //cu recursive
            {
                for(int i=2; i<5; i++)
                {
                    if(strstr(argv[i],"path="))
                    {
                        pathArg=i;
                    }
                    else if(strstr(argv[i],"size_smaller="))
                    {
                        strcat(value,argv[i]+13);
                        strcpy(opt,"size");
                    }
                    else if(strstr(argv[i],"name_ends_with="))
                    {
                        strcat(value,argv[i]+15);
                        strcpy(opt,"name");
                    }
                }
                char  s2[100];
                strcpy(s2,argv[pathArg]+5);
                //strcat(s,s2);
                printf("SUCCESS\n");
                listRec(s2,opt,value);
                //printf("%s ",argv[pathArg]);
            }
            else if(argc==3)
            {
                char  s2[100];
                strcpy(s2,argv[pathArg]+5);
                //strcat(s,s2);
                listDirSimple(s2);
            }
            /////


        }
        else if(strcmp(argv[1],"parse")==0 || strcmp(argv[2],"parse")==0)
        {
            if(strcmp(argv[1],"parse")==0)
            {
                char  s2[100];
                strcpy(s2,argv[2]+5);
                parsare(s2);//////////
            }
            else if(strcmp(argv[2],"parse")==0)
            {
                char  s2[100];
                strcpy(s2,argv[1]+5);
                parsare(s2);////////////
            }

        }
        else if(strcmp(argv[1],"findall")==0 || strcmp(argv[2],"findall")==0)
        {
            if(strcmp(argv[1],"findall")==0)
            {
                printf("SUCCESS\n");
                char  s2[100];
                strcpy(s2,argv[2]+5);
                findall(s2);//////////
            }
            else if(strcmp(argv[2],"findall")==0)
            {
                printf("SUCCESS\n");
                char  s2[100];
                strcpy(s2,argv[1]+5);
                findall(s2);////////////
            }

        }
    }
    else
    {
        printf("Dati argumentul/argumentele dorite!\n");
    }
    return 0;
}





