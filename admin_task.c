#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
int add(int);
int delete(int);
int modify(int);
int search(int);

int add(int client){
	struct user_type buff;
	//receive new account details
	recv(client,&buff,sizeof(buff),0);
	int fd = openfile_rw(buff);
	
	//lock the file for making change
	struct flock lock;
  	lock.l_type = F_WRLCK;
  	lock.l_whence = SEEK_SET;
  	lock.l_start = 0;
 	lock.l_len = 0;
  	lock.l_pid = getpid();

  	fcntl(fd,F_SETLKW,&lock);
  	struct user_type prev;
  	//lseek to last record
  	if(lseek(fd,-sizeof(prev),SEEK_END)==-1)
  	{
  		char temp[1024] ="server error";
  		send(client,temp,sizeof(temp),0);
  		//unlock
  		lock.l_type = F_UNLCK;
  		fcntl(fd,F_SETLKW,&lock);
  		//close file
  		closefile(fd);
  		return -1;//error in seek;
  	}
  	//read prev record
  	if(read(fd,&prev,sizeof(prev))!=0){
  		//create new record and writre
  		if(strcmp(buff.type,"joint")==0){
  			buff.user_id = prev.user_id+2;
  			buff.joint_id = prev.joint_id+2;	
  		}
  		else{

  			buff.user_id = prev.user_id+1;
  		}
  		// update write // pointer is already on correct position
	  	if(write(fd,&buff,sizeof(buff))<=0)
	  		{
	  			char temp[1024] ="server error";
	  			send(client,temp,sizeof(temp),0);
	  			//unlock
		  		lock.l_type = F_UNLCK;
		  		fcntl(fd,F_SETLKW,&lock);
		  		//close file
		  		closefile(fd);
	  			return -1;//error in write;
	  		}
		else
	  		{
	  			printf("successfull\n");
	  			char temp[1024]="Account Successfully Created";
				send(client,temp,sizeof(temp),0);
				send(client,&buff,sizeof(buff),0);
	  			//unlock
	  			lock.l_type = F_UNLCK;
		  		fcntl(fd,F_SETLKW,&lock);
		  		//close file
		  		closefile(fd);
	  			return 0;//successfull;
	  		}

  	}
  	else{
  		char temp[1024] ="server error";
		send(client,temp,sizeof(temp),0);
  		//unlock
  		lock.l_type = F_UNLCK;
  		fcntl(fd,F_SETLKW,&lock);
  		//close file
  		closefile(fd);
  		return -1;//error in read;
  	}
  	
}

int delete(int client){
	struct user_type usr;
	//receive new account details
	recv(client,&usr,sizeof(usr),0);
	int fd = openfile_rw(usr);
		//seek start
	int seek_start;
	if(strcmp(usr.type,"joint")==0){
		seek_start = usr.user_id/2;
	}
	else{
		seek_start = usr.user_id;
	}
	//lock the file for making change
	struct flock lock;
  	lock.l_type = F_WRLCK;
  	lock.l_whence = SEEK_SET;
  	lock.l_start = (seek_start)*sizeof(usr);
 	lock.l_len = sizeof(usr);
  	lock.l_pid = getpid();


  	fcntl(fd,F_SETLKW,&lock);

  	//lseek to appropriate position
  	if(lseek(fd,seek_start*sizeof(usr),SEEK_SET)==-1)
  	{
  		char temp[1024] ="server error";
  		send(client,temp,sizeof(temp),0);
  		//unlock
  		lock.l_type = F_UNLCK;
  		fcntl(fd,F_SETLKW,&lock);
  		//close file
  		closefile(fd);
  		return -1;//error in seek;
  	}

  	struct user_type buff;
  	if(read(fd,&buff,sizeof(buff))!=0){
  		//change the status to inactive
  		char status[10] = "inactive";
  		memcpy(buff.status,status,sizeof(status));
		if(lseek(fd,-1*sizeof(usr),SEEK_CUR)==-1)
  		{	
	  		char temp[1024] ="server error";
  			send(client,temp,sizeof(temp),0);
	  		//unlock
	  		lock.l_type = F_UNLCK;
	  		fcntl(fd,F_SETLKW,&lock);
	  		//close file
	  		closefile(fd);
  			return -1;//error in seek;
  		} 
  		if(write(fd,&buff,sizeof(buff))<=0)
  		{
  			char temp[1024] ="server error";
  			send(client,temp,sizeof(temp),0);
  			//unlock
	  		lock.l_type = F_UNLCK;
	  		fcntl(fd,F_SETLKW,&lock);
	  		//close file
	  		closefile(fd);
  			return -1;//error in write;
  		}
  		else{
  			printf("successfull\n");
  			char temp[1024]="Account Deleted Successfully ";
			send(client,temp,sizeof(temp),0);
  			//unlock
  			lock.l_type = F_UNLCK;
	  		fcntl(fd,F_SETLKW,&lock);
	  		//close file
	  		closefile(fd);
  			return 0;//successfull;
  		}		
  	}
  	else{
  		char temp[1024] ="server error";
		send(client,temp,sizeof(temp),0);
  		//unlock
  		lock.l_type = F_UNLCK;
  		fcntl(fd,F_SETLKW,&lock);
  		//close file
  		closefile(fd);
  		return -1;//error in read;
  	}

}

int modify(int client){

	struct user_type usr;
	//receive new account details
	recv(client,&usr,sizeof(usr),0);
	int fd = openfile_rw(usr);
		//seek start
	int seek_start;
	if(strcmp(usr.type,"joint")==0){
		seek_start = usr.user_id/2;
	}
	else{
		seek_start = usr.user_id;
	}
	//lock the file for making change
	struct flock lock;
  	lock.l_type = F_WRLCK;
  	lock.l_whence = SEEK_SET;
  	lock.l_start = (seek_start)*sizeof(usr);
 	lock.l_len = sizeof(usr);
  	lock.l_pid = getpid();


  	fcntl(fd,F_SETLKW,&lock);

  	//lseek to appropriate position
  	if(lseek(fd,seek_start*sizeof(usr),SEEK_SET)==-1)
  	{
  		char temp[1024] ="server error";
  		send(client,temp,sizeof(temp),0);
  		//unlock
  		lock.l_type = F_UNLCK;
  		fcntl(fd,F_SETLKW,&lock);
  		//close file
  		closefile(fd);
  		return -1;//error in seek;
  	}

  	struct user_type buff;
  	if(read(fd,&buff,sizeof(buff))!=0){
		if(lseek(fd,-1*sizeof(usr),SEEK_CUR)==-1)
  		{	
	  		char temp[1024] ="server error";
  			send(client,temp,sizeof(temp),0);
	  		//unlock
	  		lock.l_type = F_UNLCK;
	  		fcntl(fd,F_SETLKW,&lock);
	  		//close file
	  		closefile(fd);
  			return -1;//error in seek;
  		}
  		//modify changes
  		if(strcmp(usr.status,"-1")!=0){
  			memcpy(buff.status,usr.status,sizeof(usr.status));
  		}
  		if(strcmp(usr.username,"-1")!=0){
  			memcpy(buff.username,usr.username,sizeof(usr.username));
  		}
  		if(strcmp(usr.passwd,"-1")!=0){
  			memcpy(buff.passwd,usr.passwd,sizeof(usr.passwd));
  		}
  		if(strcmp(usr.type,"joint")==0&&strcmp(usr.joint_username,"-1")!=0){
  			memcpy(buff.joint_username,usr.joint_username,sizeof(usr.joint_username));
  		}
  		if(usr.amount!=-1){
  			buff.amount=usr.amount;
  		} 
  		//update changes
  		if(write(fd,&buff,sizeof(buff))<=0)
  		{
  			char temp[1024] ="server error";
  			send(client,temp,sizeof(temp),0);
  			//unlock
	  		lock.l_type = F_UNLCK;
	  		fcntl(fd,F_SETLKW,&lock);
	  		//close file
	  		closefile(fd);
  			return -1;//error in write;
  		}
  		else{
  			printf("successfull\n");
  			char temp[1024]="Account Updated Successfully ";
			send(client,temp,sizeof(temp),0);
  			//unlock
  			lock.l_type = F_UNLCK;
	  		fcntl(fd,F_SETLKW,&lock);
	  		//close file
	  		closefile(fd);
  			return 0;//successfull;
  		}		
  	}
  	else{
  		char temp[1024] ="server error";
		send(client,temp,sizeof(temp),0);
  		//unlock
  		lock.l_type = F_UNLCK;
  		fcntl(fd,F_SETLKW,&lock);
  		//close file
  		closefile(fd);
  		return -1;//error in read;
  	}


}

int search(int client){
	struct user_type usr;
	recv(client,&usr,sizeof(usr),0);
	int fd = openfile_r(usr);
	if(fd==-1){
		return -1;
	}
	//seek start
	int seek_start;
	if(strcmp(usr.type,"joint")==0){
		seek_start = usr.user_id/2;
	}
	else{
		seek_start = usr.user_id;
	}
	//lock the record
	struct flock lock;
  	lock.l_type = F_RDLCK;
  	lock.l_whence = SEEK_SET;
  	lock.l_start = seek_start*sizeof(usr);
 	lock.l_len = sizeof(usr);
  	lock.l_pid = getpid();

  	//lock the current record
  	fcntl(fd,F_SETLKW,&lock);

  	//update seek to proper record
  	if(lseek(fd,seek_start*sizeof(usr),SEEK_SET)==-1)
  	{
  		char temp[1024] ="server error";
		send(client,temp,sizeof(temp),0);
  		//unlock
  		lock.l_type = F_UNLCK;
  		fcntl(fd,F_SETLKW,&lock);
  		//close file
  		closefile(fd);
  		return -1;//error in seek;
  	}

  	struct user_type buff;
  	if(read(fd,&buff,sizeof(buff))!=0){
		//view details
	
		printf("successfull\n");
		// send details to client
		send(client,&buff,sizeof(buff),0);
		//unlock
		lock.l_type = F_UNLCK;
		fcntl(fd,F_SETLKW,&lock);
		//close file
		closefile(fd);
		return 0;//successfull;	
  	}
  	else{
  		char temp[1024] ="server error";
		send(client,temp,sizeof(temp),0);
  		//unlock
  		lock.l_type = F_UNLCK;
  		fcntl(fd,F_SETLKW,&lock);
  		//close file
  		closefile(fd);
  		return -1;//error in read;
  	}

}