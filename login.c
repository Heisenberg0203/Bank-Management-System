#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
struct user_type
{
	char type[10];
	char status[10];
	int user_id;
	int joint_id;
	char passwd[40];
	char username[40];
	char joint_username[40];
	int amount;
	
};

int login(struct user_type,int);
int validate(char *,struct user_type ,int);

int login(struct user_type usr,int client){
	printf("login\n");
	if(strcmp((usr).type,"admin")==0)
		return validate("./db/admin",usr,client);
	else if(strcmp((usr).type,"user")==0)
		return validate("./db/user",usr,client);
	else if(strcmp((usr).type,"joint")==0)
		return validate("./db/joint",usr,client);
	else
	{
		char temp[1024] ="Incorrect User Type";
		send(client,temp,sizeof(temp),0);
  		
		return -1;
	}
}


int validate(char *filename,struct user_type usr,int client){
	printf("validate %s\n",filename);
	
	//open file in readonly mode
	int fd = open(filename,O_RDONLY);
	printf("%d\n",fd );
  int seek_start;
  if(strcmp(usr.type,"joint")==0){
    seek_start = usr.user_id/2;
  }
  else{
    seek_start = usr.user_id;
  }
	// implement read lock
	struct flock lock;
  	lock.l_type = F_RDLCK;
  	lock.l_whence = SEEK_SET;
  	lock.l_start = (seek_start)*sizeof(usr);
 	lock.l_len = sizeof(usr);
  	lock.l_pid = getpid();

  	//lock the current record
  	fcntl(fd,F_SETLKW,&lock);

  	printf("lock done\n");
	//move the pointer to required location
		if(lseek(fd,(seek_start)*sizeof(usr),SEEK_SET)==-1)
  	{
  		printf("error in seek\n");
          //unlock read lock
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    //close file
    close(fd);
  		return -1;//error in seek;
  	}
	
  	printf("lseek done\n");
  	//read the contents into buffer
  	struct user_type buff;
  	if(read(fd,&buff,sizeof(buff))!=0){
  		printf("read done\n");
  		//check if account is active

  		if(strcmp(buff.status,"inactive")==0)
  		{
  			char temp[1024] ="This account is not active";
			send(client,temp,sizeof(temp),0);
          //unlock read lock
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    //close file
    close(fd);
  		
  			return -1;

		}

  		if((usr).user_id==(buff).user_id&&(strcmp((usr).passwd,(buff).passwd)==0)){
  				char temp[1024]="Sucessfully login";
  				send(client,temp,sizeof(temp),0);
              //unlock read lock
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    //close file
    close(fd);
  				return 0;	
  		}
  		//check for joint account
  		else if((usr).user_id==(buff).joint_id&&(strcmp((usr).passwd,(buff).passwd)==0)){
  				char temp[1024]="Sucessfully login";
  				send(client,temp,sizeof(temp),0);
              //unlock read lock
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
    //close file
    close(fd);
  				return 0;

  		}
  		else{
  			char temp[1024] ="Incorrect Username or password";
			 send(client,temp,sizeof(temp),0);
          //unlock read lock
        lock.l_type = F_UNLCK;
        fcntl(fd,F_SETLKW,&lock);
        //close file
        close(fd);
  		
  			return -1; // incoorect user details
  		}

  	}
  	else{
  		char temp[1024] ="No such user exists";
		  send(client,temp,sizeof(temp),0);
          //unlock read lock
      lock.l_type = F_UNLCK;
      fcntl(fd,F_SETLKW,&lock);
      //close file
      close(fd);
  		
  		return -1; // no record found;
  	}

  	//unlock read lock
  	lock.l_type = F_UNLCK;
  	fcntl(fd,F_SETLKW,&lock);
  	//close file
  	close(fd);
}

