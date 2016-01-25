
#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include<fcntl.h>
#include<string.h>
#include <dirent.h>
#include <utime.h>


//usage
usage(){

	printf("Usage : ./fileClone [-f] source destination \n");
}

//check if directory
int isDir(const char * fileName){


struct stat st;

if(stat(fileName,&st)==0){


       if(S_ISDIR(st.st_mode))
	{

           return 1;
	}

}

return 0;

}


//check if file exists

int fileExists(const char * fileName){

struct stat st;
if(stat(fileName,&st)==0){

return 1;
}

return 0;

}


//change file attributes


void fileAttrib(const char * src,const char * dest){

	struct stat st;
	lstat(src,&st);
        int i=chown(dest,st.st_uid,st.st_gid);
	if(i==-1){
		perror("chown error");
		exit(1);
	}
        i=chmod(dest,st.st_mode);
	if(i==-1){
		perror("chmod error");
		exit(1);
	}
        struct utimbuf puttime;
        puttime.modtime = st.st_mtime;
        puttime.actime = st.st_atime;
        i=utime(dest, &puttime); 
	if(i==-1){
		perror("Modification time error");
		exit(1);
	}

}


//http://stackoverflow.com/a/2180788
int fileClone(const char * src,const char * dest){

   int fd_src, fd_dest;
   char buf[4096];
   ssize_t nread;
   int errno;

   fd_src = open(src, O_RDONLY);


    if (fd_src < 0){
        
	//perror("source file does not exists\n");
        perror(src);
	exit(1);

   }

    fd_dest = open(dest, O_WRONLY | O_CREAT|O_TRUNC, 0666);

    if (fd_dest < 0){

	close(fd_src);
        perror(dest);
	exit(1);
    }

	while (nread = read(fd_src, buf, sizeof buf), nread > 0){
	
		char *out_ptr = buf;
        	ssize_t nwritten;

		do {
            		nwritten = write(fd_dest, out_ptr, nread);

            		if (nwritten >= 0)
            		{
                		nread -= nwritten;
                		out_ptr += nwritten;
            		}
            		else if (errno != EINTR)
            		{
				close(fd_src);
				if(fd_dest >=0)
				close(fd_dest);
				perror(dest);
				exit(1);
           		}
        	} while (nread > 0);
	

	}


 if (nread == 0)
 {
        if (close(fd_dest) < 0)
        {
            fd_dest = -1;
	    close(fd_src);

	    if(fd_dest >=0)
	    close(fd_dest);
            
        }
        close(fd_src);

 }

}

//main function

int main(int argc,char * argv[]){


int opt,f=0;
char * source;
char * dest;
char *pch;
char *fileName;
int isDirectory=0;


if(argc<3)
{

        usage();
	return 0;
}


while ((opt=getopt(argc,argv,"f"))!=-1)
{

	switch(opt)
	{

	case 'f':
    		f=1;
    		break;
 
	case '?':
		usage();
		return 0;
		break;	

	}

}

argv += optind;

if( ! *argv){

    usage();
    return 0;

}
  

source=*argv;
argv+=1;

if( ! *argv){

    usage();
    return 0;

}


dest=*argv;


isDirectory=isDir(dest);


if(isDirectory){


strcpy(fileName,source);
pch = strtok (fileName,"/");
  

while (pch != NULL)
  {
    fileName=pch;
    pch = strtok (NULL, "/");
  }

if(dest[strlen(dest) -1] != '/')

{

 dest=strcat(dest,"/");

}

dest=strcat(dest,fileName);

}

if(fileExists(dest) && (f==0)){

printf("provide -f option to overide the destination file \n");

return 1;

}


fileClone(source,dest);
fileAttrib(source,dest);



}




