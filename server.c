#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<netinet/in.h>
#include<pthread.h>
#include "login.c"
#include "user_task.c"
#include "admin_task.c"
void * connection_handler(void *);
void handle_usertask(struct user_type,int);
void handle_admintask(struct user_type,int);

int main(){

int fd_server = socket(AF_INET,SOCK_STREAM,0);
if(fd_server==-1){
	perror("socket error");
	return 0;
}

int opt=1;
//if address/port is already in use,reuse it
int opt_ret = setsockopt(fd_server,SOL_SOCKET,SO_REUSEADDR|SO_REUSEPORT,&opt,sizeof(opt));

if(opt_ret==-1){
	perror("setsockopt error");
	return 0;
}

//ip address
struct sockaddr_in sin;
sin.sin_family = AF_INET;
sin.sin_addr.s_addr = INADDR_ANY;
sin.sin_port = htons(8080);
int addrlen = sizeof(sin);
//bind server to above ip address and port
if(bind(fd_server, (struct sockaddr *) &sin,sizeof(sin))<0){
	perror("bind");
	return 0;
}

//continously listen to new connections
printf("Server succesfully created and is listening\n");


if(listen(fd_server,5)==-1){
	perror("Listen ");
	return 0;
}

// pthread_t tid[5];
// static int i=0;
while(1){
	// accept new connections if any
	
	int newclient=accept(fd_server,(struct sockaddr *) &sin,(socklen_t *) &addrlen);
	if(newclient==-1){
		perror("accept");
		return 0;
	}
	// if(pthread_create(&tid[i++],NULL,connection_handler,(void*) &newclient)!=0){
	// 	perror("could not create thread");
	// 	return 0;
	// }
	if(fork()==0){
		connection_handler((void*)&newclient);
		return 0;
	}

	//pthread_join(tid[i++],NULL);

}
return 0;
}

void *connection_handler(void * socket_addr){
	
		int client = *((int *) socket_addr);	
		struct user_type readbuff;
		recv(client,&readbuff.type,sizeof(readbuff),0);
		// printf("read succesfully\n");
		// printf("%s\n",readbuff.type);
		// printf("%d\n",readbuff.user_id);
		// printf("%s\n",readbuff.passwd);
		int ret =login(readbuff,client);
		printf(" %d\n",ret);
		send(client,&ret,sizeof(int),0);
		if(ret==0){
			if(strcmp(readbuff.type,"admin")==0){
				handle_admintask(readbuff,client);
			}
			
			else{
				handle_usertask(readbuff,client);
			}	
		}
		close(client);
		pthread_exit(NULL);

}

void handle_usertask(struct user_type readbuff, int client){
	while(1){
		char start[1024]="Enter 1 for Withdraw | Enter 2 for Deposit | Enter 3 for Balance enquiry | Enter 4 for Password change | Enter 5 for View Details | Enter 6 to Exit";
		send(client,start,sizeof(start),0);
		int task;
		recv(client,&task,sizeof(int),0);
		//printf("%d\n",task );
		if(task==1){
			char temp[1024]="Enter amount to Withdraw";
			send(client,temp,sizeof(temp),0);
			int amount;
			recv(client,&amount,sizeof(int),0);
			withdraw(readbuff,amount,client);
		}
		else if(task==2){
			deposit(readbuff,client);
		}
		else if(task==3){
			balance_enquiry(readbuff,client);
		}
		else if(task==4){
			struct user_type newbuff;
			recv(client,&newbuff,sizeof(newbuff),0);
			change_passwd(readbuff,newbuff,client);
		}
		else if(task==5){
			view_details(readbuff,client);
		}
		else if(task==6){
			return;
		}

		else{
			char temp[1024] ="Please Enter Valid Input";
			send(client,temp,sizeof(temp),0);
  		
		}


	}
}

void handle_admintask(struct user_type readbuff, int client){
	while(1)
	{
		char start[1024]="Enter 1 to add account | Enter 2 for Delete | Enter 3 to Modify | Enter 4 to search | Enter 5 to exit";
		send(client,start,sizeof(start),0);
		int task;
		recv(client,&task,sizeof(int),0);
		//printf("%d\n",task );
		if(task==1){
			add(client);
		}
		else if(task==2){
			delete(client);
		}
		else if(task==3){
			modify(client);
		}
		else if(task==4){
			search(client);
		}
		else if(task==5){
			char temp[1024]="Logging out....";
			send(client,temp,sizeof(temp),0);
			return;
		}
		else {
			char temp[1024]="Invalid Option";
			send(client,temp,sizeof(temp),0);
		}

	}


}

