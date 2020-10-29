#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
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

struct user_type take_login_input();
void user_tasks(struct user_type,int);
void admin_tasks(struct user_type,int);
int main(){


int fd_client = socket(AF_INET,SOCK_STREAM,0);
if(fd_client==-1){
	perror("socket error");
	return 0;
}


//ip address of server
struct sockaddr_in server;
server.sin_family = AF_INET;
server.sin_port = htons(8080);
inet_pton(AF_INET,"127.0.0.1",&server.sin_addr);

//connect to server
if(connect(fd_client,(struct sockaddr*)&server,sizeof(server))==-1){
	perror("connection failed ");		
	return 0;
}

	printf("Connection established\n");
	//send type,id,passswd
	struct user_type buff = take_login_input();
	send(fd_client,&buff,sizeof(buff),0);
	
	char rbuff[1024];
	recv(fd_client,rbuff,sizeof(rbuff),0);
	printf("Server Message: %s \n",rbuff);
	//printf("\n");
	int status;
	recv(fd_client,&status,sizeof(int),0);
	if(status==0){
		if(strcmp(buff.type,"admin")==0){
			admin_tasks(buff,fd_client);
		}
		else{
			user_tasks(buff,fd_client);	
		}
	}	
	getchar();
	getchar();
	printf("Disconnected\n");
	
return 0;
}

struct user_type take_login_input(){
	struct user_type buff;
	
	//scan type of user		
	while(1){
		printf(" Please Enter 1 for Admin Account || 2 for Normal Account|| 3 for Joint Account \n");
		int type;
		scanf(" %d",&type);
		fflush(stdin);
		fflush(stdout);
		if(type==1){
			char temp[10]="admin";
			memcpy(buff.type,temp,sizeof(temp));
			break;
		}
		else if(type==2){
			char temp[10]="user";
			memcpy(buff.type,temp,sizeof(temp));
			break;		
		}
		else if(type==3){
			char temp[10]="joint";
			memcpy(buff.type,temp,sizeof(temp));
			break;
		}
		else{
			printf("Please Enter valid Input \n");
		}
	}
	//scan userid
	{
		printf("UserId:");
		scanf(" %d",&(buff.user_id));
	}
	//scan password
	{
		printf("Password:");
		char temp[40];
		scanf(" %[^\n]",temp);
		memcpy(buff.passwd,temp,sizeof(temp));
	}

	return buff;

}

void user_tasks(struct user_type buff,int fd_client){
	char rbuff[1024];
	// recv(fd_client,rbuff,sizeof(rbuff),0);
	// printf("%s\n",rbuff);
	while(1){
		bzero(rbuff,1024);
		recv(fd_client,rbuff,sizeof(rbuff),0);
		printf(" %s\n",rbuff);
		int task;
		scanf(" %d",&task);
		//printf("%d",task);
		send(fd_client,&task,sizeof(int),0);
				
		if(task==1){
				bzero(rbuff,1024);
				recv(fd_client,rbuff,sizeof(rbuff),0);
				printf(" %s\n",rbuff);
				int amount;
				scanf(" %d",&amount);
				//printf(" %d \n",amount);
				send(fd_client,&amount,sizeof(int),0);
				bzero(rbuff,1024);
				recv(fd_client,rbuff,sizeof(rbuff),0);
				printf(" %s",rbuff);
				bzero(rbuff,1024);
				recv(fd_client,rbuff,sizeof(rbuff),0);
				printf(" %s\n",rbuff);

			}
			else if(task==2){
				bzero(rbuff,1024);
				recv(fd_client,rbuff,sizeof(rbuff),0);
				printf(" %s\n",rbuff);
				int amount;
				scanf(" %d",&amount);
				//printf(" %d \n",amount);
				send(fd_client,&amount,sizeof(int),0);
				bzero(rbuff,1024);
				recv(fd_client,rbuff,sizeof(rbuff),0);
				printf(" %s ",rbuff);
				bzero(rbuff,1024);
				recv(fd_client,rbuff,sizeof(rbuff),0);
				printf("%s\n",rbuff);
			}
			else if(task==3){
				bzero(rbuff,1024);
				recv(fd_client,rbuff,sizeof(rbuff),0);
				printf("%s ",rbuff);
				bzero(rbuff,1024);
				recv(fd_client,rbuff,sizeof(rbuff),0);
				printf("%s\n",rbuff);
			}
			else if(task==4){
				printf("Enter New password\n");
				char newpass[40];
				scanf(" %[^\n]",newpass);
				struct user_type newpass_struct;
				memcpy(&newpass_struct,&buff,sizeof(buff));
				memcpy(&(newpass_struct.passwd),&newpass,sizeof(newpass));
				send(fd_client,&newpass_struct,sizeof(newpass_struct),0);

				bzero(rbuff,1024);
				recv(fd_client,rbuff,sizeof(rbuff),0);
				printf("%s\n",rbuff);
			}
			else if(task==5){
				struct user_type temp;
				recv(fd_client,&temp,sizeof(temp),0);
				printf("User Id: %d \n",temp.user_id);
				printf("Status: %s \n",temp.status);
				printf("Username %s \n",temp.username);
				printf("Account Type %s \n",temp.type);
				printf("Balance %d \n",temp.amount);
				if(strcmp(temp.type,"joint")==0){
					printf("Joint User Id: %d \n",temp.joint_id);
					printf("Joint Username %s \n",temp.joint_username);
				}
			}
			else if(task==6){
				printf("Thank you for Banking with us!\n");
				return;
			}

			else{
				bzero(rbuff,1024);
				recv(fd_client,rbuff,sizeof(rbuff),0);
				printf("%s\n",rbuff);
			}
		}
}

void admin_tasks(struct user_type buff,int fd_client){
	char rbuff[1024];
	// recv(fd_client,rbuff,sizeof(rbuff),0);
	// printf("%s\n",rbuff);
	while(1){
		bzero(rbuff,1024);
		recv(fd_client,rbuff,sizeof(rbuff),0);
		printf(" %s\n",rbuff);
		int task;
		scanf(" %d",&task);
		send(fd_client,&task,sizeof(int),0);

		if(task==1){
			
			struct user_type newaccount;
			printf("Enter Type of account\n");
			char t1[10];
			scanf(" %[^\n]",t1);
			memcpy(newaccount.type,t1,sizeof(t1));
			char t2[10]="active";
			memcpy(newaccount.status,t2,sizeof(t2));
			printf("Enter Username \n");
			char t3[40];
			scanf(" %[^\n]",t3);
			memcpy(newaccount.username,t3,sizeof(t3));
			if(strcmp(t1,"joint")==0){
				printf("Enter joint username");
				char t[40];
				scanf(" %[^\n]",t);
				memcpy(newaccount.joint_username,t,sizeof(t));

			}
			printf("Enter Password \n");
			char t4[40];
			scanf(" %[^\n]",t4);
			memcpy(newaccount.passwd,t4,sizeof(t4));
			newaccount.amount=100;
			send(fd_client,&newaccount,sizeof(newaccount),0);

			//receive status from server
			bzero(rbuff,1024);
			recv(fd_client,rbuff,sizeof(rbuff),0);
			printf(" %s\n",rbuff);

			recv(fd_client,&newaccount,sizeof(newaccount),0);
			printf("UserID: %d \n",newaccount.user_id);
			if(strcmp(newaccount.type,"joint")==0)
				printf("Joint UserId: %d \n",newaccount.joint_id);
			printf("Password: %s \n",newaccount.passwd);


			
		}
		else if(task==2){
			struct user_type account;
			printf("Enter Type of account\n");
			char t1[10];
			memcpy(account.type,t1,sizeof(t1));
			scanf(" %[^\n]",t1);
			printf("Enter account id\n");
			scanf(" %d",&(account.user_id));
			send(fd_client,&account,sizeof(account),0);

			//receive status from server
			bzero(rbuff,1024);
			recv(fd_client,rbuff,sizeof(rbuff),0);
			printf(" %s\n",rbuff);



		}
		else if(task==3){
			struct user_type account;
			printf("Enter Type of account\n");
			char t1[10];
			scanf(" %[^\n]",t1);
			memcpy(account.type,t1,sizeof(t1));
			printf("Enter user_id\n");
			scanf(" %d",&(account.user_id));
			printf("Enter new values or -1 to skip\n");
			printf("Modify Status \n");
			char t2[10];
			scanf(" %[^\n]",t2);
			memcpy(account.status,t2,sizeof(t2));
			printf("Modify Username \n");
			char t3[40];
			scanf(" %[^\n]",t3);
			memcpy(account.username,t3,sizeof(t3));
			if(strcmp(t1,"joint")==0){
				printf("Modify joint username");
				char t[40];
				scanf(" %[^\n]",t);
				memcpy(account.joint_username,t,sizeof(t));

			}
			printf("Modify Password \n");
			char t4[40];
			scanf(" %[^\n]",t4);
			memcpy(account.passwd,t4,sizeof(t4));
			printf("Modify Balance\n");
			scanf(" %d",&(account.amount));
			send(fd_client,&account,sizeof(account),0);

			//receive status from server
			bzero(rbuff,1024);
			recv(fd_client,rbuff,sizeof(rbuff),0);
			printf(" %s\n",rbuff);

		}
		else if(task==4){
			struct user_type account;
			printf("Enter Type of account\n");
			char t1[10];
			scanf(" %[^\n]",t1);
			memcpy(account.type,t1,sizeof(t1));
			printf("Enter user_id\n");
			scanf(" %d",&(account.user_id));
			send(fd_client,&account,sizeof(account),0);

			struct user_type temp;
			recv(fd_client,&temp,sizeof(temp),0);

			printf("Account Type %s \n",temp.type);
			printf("User Id: %d \n",temp.user_id);
			printf("Account status %s \n",temp.status);
			printf("Username %s \n",temp.username);
			if(strcmp(temp.type,"joint")==0){
				printf("Joint User Id: %d \n",temp.joint_id);
				printf("Joint Username %s \n",temp.joint_username);
			}
			printf("Balance %d \n",temp.amount);




		}
		else if(task==5){
			//receive status from server
			bzero(rbuff,1024);
			recv(fd_client,rbuff,sizeof(rbuff),0);
			printf(" %s\n",rbuff);
			return;

		}
		else {
			//receive status from server
			bzero(rbuff,1024);
			recv(fd_client,rbuff,sizeof(rbuff),0);
			printf(" %s\n",rbuff);
		}
	
	}
}
