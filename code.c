#include<stdio.h>
#include<string.h>
#include<mpi.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char *argv[])
{
	int rank,size,num,j,l,i,k,clock=0,clockval,dest,source;
	FILE *fp;	
	char buff[100],buff1[100],str[100][100],ack;
	char *ptr,*token;
	char msg[100][100];
	
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Status st;
	
	
	fp=fopen(argv[1],"r"); 
	
	if(rank==0)
	{
		
        fgets(buff,100,fp);
		num= atoi(buff);
		
		printf("There are %d processes in the system\n",num);
		
		if(num!=size-1)
		{
			ack='t';
			printf("You need to enter %d number of processors to run the program\n", num+1 );
			for(i=1;i<size;i++)
			{	
				MPI_Send(&ack,1,MPI_CHAR,i,1,MPI_COMM_WORLD);
				
			}	
			MPI_Finalize();
			return 0;
		}

		while(!feof(fp))
		{
			
			fgets(buff,100,fp);
			strcpy(buff1,buff);
			ptr=strtok(buff, " ");
			i=0;
			
			while(ptr!=NULL)
			{
				strcpy(str[i],&ptr[0]);
				ptr=strtok(NULL," ");
				i++;
			}
			
			
			if(!strncmp(str[0],"exec",4))
			{
				source = atoi(str[1]);
				ack='x';
				MPI_Send(&ack,1,MPI_CHAR,source,1,MPI_COMM_WORLD);
			}
			else if(!strncmp(str[0],"send",4))
			{
				
				token=strtok(buff1, "\"");
				j=0;
				while(token!=NULL)
				{
					strcpy(msg[j],&token[0]);
					
					token=strtok(NULL,"\"");
					j++;
				}
				
				source = atoi(str[1]);
				dest = atoi(str[2]);
				msg[1];
				ack='e';

				MPI_Send(&ack,1,MPI_CHAR,source,1,MPI_COMM_WORLD);
				MPI_Send(&dest,1,MPI_INT,source,2,MPI_COMM_WORLD);
				MPI_Send(msg[1],100,MPI_CHAR,source,3,MPI_COMM_WORLD);		
				
			}
			
			else if(!strncmp(str[0],"end",3))
			{
				ack='n';
				
				for(i=1;i<size;i++)
				{
					MPI_Send(&ack,1,MPI_CHAR,i,1,MPI_COMM_WORLD);

//				}
				
				//Receiving the clock value from all the processes
	//			for(j=1;j<size;j++)
		//		{
					
					MPI_Recv(&clock,1,MPI_INT,i,101,MPI_COMM_WORLD,&st);
					printf( "Logical time at process %d is %d\n", st.MPI_SOURCE, clock);
			
				}
				
				
			}		//end of else if
			
		}//end of while
		
	}//end of if	
	
	if(rank!=0)
	{
		while(1)
		{	
			MPI_Recv(&ack,1,MPI_CHAR,MPI_ANY_SOURCE,1,MPI_COMM_WORLD,&st);
			
			//if you receive acknowledgement as 'x', then we need to start execute command 
			if(ack =='x')
			{
				printf( "Execution event in process %d\n", rank);
				clock++;
				printf( "Logical time at process %d is %d\n", rank, clock);
			}
			//if you receive acknowledgement as 'e', then we need to start send command where we
			//send a message from co-ordinator to a process 
			else if(ack=='e')
			{
				printf( "send event in process %d\n", rank);
				clock++;
				printf( "Logical time at process %d is %d\n", rank, clock);
				MPI_Recv(&dest,1,MPI_INT,MPI_ANY_SOURCE,2,MPI_COMM_WORLD,&st);
				MPI_Recv(msg[1],100,MPI_CHAR,MPI_ANY_SOURCE,3,MPI_COMM_WORLD,&st);
				//printf("Destination Process: %d \n", dest);
				printf("Message sent from process %d to process %d : %s \n",rank,dest,msg[1]);
				ack='z';
				MPI_Send(&ack,1,MPI_CHAR,dest,1,MPI_COMM_WORLD);
				MPI_Send(&clock,1,MPI_INT,dest,25,MPI_COMM_WORLD);
				MPI_Send(msg[1],100,MPI_CHAR,dest,35,MPI_COMM_WORLD);
				
			}
			//sending from one processe to other process
			else if(ack=='z')
			{
				MPI_Recv(&clockval,1,MPI_INT,MPI_ANY_SOURCE,25,MPI_COMM_WORLD,&st);
				
				MPI_Recv(msg[1],100,MPI_CHAR,MPI_ANY_SOURCE,35,MPI_COMM_WORLD,&st);
				printf("Message received from process %d to process %d : %s \n",st.MPI_SOURCE,rank,msg[1]);
				clock++;
				if(clock<=clockval)
				{
					clock=clockval+1;
				}
				printf( "Logical time at process %d is %d\n", rank, clock);
				
			}
			//Below if condition is for ending the processes if the given number of processes in the input file are inappropriate
			else if (ack =='t')
			{
				MPI_Finalize();
				return 0; 
			}	
			else if(ack =='n')
			{
				MPI_Send(&clock,1,MPI_INT,0,101,MPI_COMM_WORLD);
				MPI_Finalize();
				return 0;
				
			}
			
	
	
		}//end of while
		
	}//end of if

	fclose(fp);
	MPI_Finalize();
	return 0;
	
}// end of main
	
	
