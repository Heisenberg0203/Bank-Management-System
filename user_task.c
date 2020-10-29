#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
int withdraw(struct user_type ,int,int);
int deposit(struct user_type ,int);
int balance_enquiry(struct user_type ,int);
int change_passwd(struct user_type,struct user_type,int);
int view_details(struct user_type,int);
int openfile_r(struct user_type );
int openfile_rw(struct user_type );
void closefile(int );

int deposit(struct user_type usr,int client){
	int fd = openfile_rw(usr);
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
  	lock.l_type = F_WRLCK;
  	lock.l_whence = SEEK_SET;
  	lock.l_start = (seek_start)*sizeof(usr);
 	  lock.l_len = sizeof(usr);
    // lock.l_start = 0;
    // lock.l_len = 0;
  	lock.l_pid = getpid();

  	//lock the current record
  	printf("Waiting for the lock %d\n",fd);
  	// printf(" seek: %d\n",seek_start );
   //  printf("pid %d\n",getpid());
    fcntl(fd,F_SETLKW,&lock);
  	printf("lock accessed\n");

  	//get the detials
	char temp[1024]="Enter amount to  Deposit";
	send(client,temp,sizeof(temp),0);
	int amount;
	recv(client,&amount,sizeof(int),0);

  	//update change to proper record
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
  		//deposit the amount
  		buff.amount = buff.amount + amount;
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
  			char temp[1024]="Amount deposited Succesfully, Current Amount is ";
			   send(client,temp,sizeof(temp),0);
  			char amount[1024];
  			sprintf(amount,"%d",buff.amount);
  			send(client,amount,sizeof(amount),0);
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

int withdraw(struct user_type usr,int amount,int client){
	int fd = openfile_rw(usr);
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
  	lock.l_type = F_WRLCK;
  	lock.l_whence = SEEK_SET;
  	lock.l_start = (seek_start)*sizeof(usr);
 	lock.l_len = sizeof(usr);
  	lock.l_pid = getpid();

  	//lock the current record
  	fcntl(fd,F_SETLKW,&lock);

  	//update change to proper record
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
  		//withdraw the amount
  		if(buff.amount<amount){
  			char temp[1024] ="low balance";
  			send(client,temp,sizeof(temp),0);
  			char amount[1024];
  			bzero(amount,1024);
  			send(client,amount,sizeof(amount),0);
  			//unlock
	  		lock.l_type = F_UNLCK;
	  		fcntl(fd,F_SETLKW,&lock);
	  		//close file
	  		closefile(fd);
	  		return -1;//low balance;
  		}
  		buff.amount = buff.amount - amount;
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
  			char temp[1024] ="Amount Withdrawn Succesfully, Current Amount is ";
  			send(client,temp,sizeof(temp),0);
  			char amount[1024];
  			sprintf(amount,"%d",buff.amount);
  			send(client,amount,sizeof(amount),0);
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

int balance_enquiry(struct user_type usr,int client){
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

  	//update change to proper record
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
		//view balance the amount
	
		printf("successfull\n");
		char temp[1024] ="Current Amount is ";
		send(client,temp,sizeof(temp),0);
		char amount[1024];
		sprintf(amount,"%d",buff.amount);
		send(client,amount,sizeof(amount),0);
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

int change_passwd(struct user_type usr, struct user_type new,int client){

	int fd = openfile_rw(usr);
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
  	lock.l_type = F_WRLCK;
  	lock.l_whence = SEEK_SET;
  	lock.l_start = (seek_start)*sizeof(usr);
 	lock.l_len = sizeof(usr);
  	lock.l_pid = getpid();

  	//lock the current record
  	fcntl(fd,F_SETLKW,&lock);

  	//update change to proper record
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
  		//change the password
  		memcpy(buff.passwd,new.passwd,sizeof(new.passwd));

  		//update change to db
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
  			char temp[1024] ="Password Succesfully Changed";
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

int view_details(struct user_type usr,int client){
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

  	//update change to proper record
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


int openfile_r(struct user_type usr){
	int fd=-1;
	if(strcmp((usr).type,"admin")==0)
		fd = open("./db/admin",O_RDONLY);
	else if(strcmp((usr).type,"user")==0)
		fd = open("./db/user",O_RDONLY);
	else if(strcmp((usr).type,"joint")==0)
		fd = open("./db/joint",O_RDONLY);
	printf("read user_task %d\n",fd);
	return fd;
}
int openfile_rw(struct user_type usr){
	int fd=-1;
	if(strcmp((usr).type,"admin")==0)
		fd = open("./db/admin",O_RDWR);
	else if(strcmp((usr).type,"user")==0)
		fd = open("./db/user",O_RDWR);
	else if(strcmp((usr).type,"joint")==0)
		fd = open("./db/joint",O_RDWR);
	printf("read user_task %d\n",fd);
	return fd;
}
void closefile(int fd){
	close(fd);
}