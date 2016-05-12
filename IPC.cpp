/*
    CS Minor Programming Assignment
    Created by: Salil Mamodiya
    Roll No   : 120102053

Description: The code first take No. of the node input in tree. After it takes
Adjacency matrix from the user. Using BFS traversal it calculates the Minimum 
Spanning Tree. This information is assigned globally.So it can be accessed by the
every child.
After this it takes the data to be sorted from the user. and pass the information to 
treeprocess. which creates child process recursively.
In the treeprocess it creates the child process according to Minimum spanning tree.
Each parent divides the data in the child equally and pass the data using PIPE. 
At the leaf node no further process is spawned and data are sorted. The code using 
Bubble sort to sort the data. After this child process again pass the data to its parent 
using pipe. Parent process Merge the sorted data as soon as it receives from its child
and pass complete Merged data to its parent. Finally Root node receives all the data.
The code finally print all the sorted data.
*/


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
#include <queue>
#include <algorithm>
#define MAX_DATA 1000
using namespace std;

int child[1000][1000];
// child have the minimum spanning tree after the BFS

int Data[MAX_DATA];
// Data variable stores the data that are to be sorted.

int Numnode;
//Numnode are the No of Nodes in our Adjacency matrix

/*

Function Merge:
the function reuturns the merged output in array.
int a[]: First Input Array
int m : Size of First Array
int b[]: Second Input Array
int n: Size of Second Array

This function takes two arrays as input and
stores the resultant array in c.
*/
int* Merge(int a[], int m, int b[], int n) {
  int i, j, k;
  int merged[m+n];
  j = k = 0;
  for (i = 0; i < m + n;) {
    if (j < m && k < n) {
      if (a[j] < b[k]) {
        merged[i] = a[j];
        j++; 
      }
      else {
        merged[i] = b[k];
        k++;

      }
      i++;
    }
    else if (j == m) {
      for (; i < m + n;) {
        merged[i] = b[k];
        k++;
        i++;
      }
    }
    else {
      for (; i < m + n;) {
        merged[i] = a[j];
        j++;
        i++;
      }
    }
  }

return merged;
}

/*
processtree() function : creates a recursive binary tree of processes.
Arguments:
start:      start is the staring index of the global data for that process.
end:        end index of the global data we are passing in the process.
parent data:Data that the parent have.
parentlenth:parent data length.
node:       node is mapped from our global node index for a process.
childindex: childindex is the index of a the child the parent is going to fork. 
            for every parent this index start from 0.
*/

int* processtree(int start, int end, int f, int* parentdata, int parentlength,int node,int childindex)
{
  	int B[MAX_DATA],swap;
  	int *M;
        int *childprocess[100];
        int parentreceive[100][MAX_DATA];
  	int count=0, count2=0;
  	int length;
        length=end-start+1;
  	int val;

  	//Pipe variable declaration for the Inter process communication.
  	int fd[2],fd1[2];
  	pipe(fd);
  	pipe(fd1);
        
        //Creating the child process using fork() function
        pid_t kid = fork();
        
      

        // fork return -1 if the it is unable to spawn process.
  	if (kid == -1) 
    	printf("Child process cannot be created\n");

       // for returns 0 if we are in the child process.
       else if (kid == 0) 
  	{

	    	// In child process
	    	// Close fd[1] to receive data from parent process
		int childcnt=0;
                // calculating the no of the next child for that child process.
                for(int p=0;p<Numnode;++p)
		    if(child[node][p]==1) 
		       ++childcnt;

	    	close(fd[1]);
	    	count=0;
	    	//Receiving Data to be sorted by this process from parent
	    	for(int j=0;j<parentlength;++j)
		{
			//Reading data from the parent.
			read(fd[0],&val,sizeof(val));

			//Storing required Data in array B. child will not take all the data from the parent.
			if(j>=start && j<=end)
			{
				B[count] = val;
				count++;
			}
		}

		printf("Data to be sorted by this process: \n");
		
		for(int j=0;j<length;j++){printf("%d ", B[j]);}
		
		printf("\n");
		
		//Closing the pipe fd[0] after child have received data from parent completely
		
		close(fd[0]);
		printf("Process ID:%d Parent:%d \n", getpid(), getppid());
                //getpid() return the process id of the child 
                // getppid() reutrn the process id of the parent.
	    	
		/// if the child has it's child then we recursively creates that child process. other we will sort the data received from the parent.

	    
                // our Process have child.
                if(childcnt>0) 
	    	{
		    //temp stores the count of the children have been created by that process. it will count up to childcnt. 
		    int temp=0;
		    for(int p=0;p<Numnode;p++)
		    {
		        if(child[node][p]==1)
		        {
		            //Create child
                             if(temp!=childcnt-1)
		             childprocess[temp]=processtree(start+temp*(length/childcnt),start+(temp+1)*(length/childcnt)-1,1,B,parentlength,p,temp);
                             else
                             childprocess[temp]=processtree(start+temp*(length/childcnt),end,1,B,parentlength,p,temp);
                             
                             /// every child returns sorted data to it's parent. and parent will merge these data.
                             
                             //merging
		            if(temp==1)
		              M = Merge(childprocess[0],length/childcnt,childprocess[1],2*(length/childcnt)-length/childcnt);

		            else if(temp>1 && temp!=childcnt-1)

		              M = Merge(M,temp*length/childcnt,childprocess[temp],(temp+1)*(length/childcnt)-temp*(length/childcnt));
                            else
                              M = Merge(M,temp*length/childcnt,childprocess[temp],end-temp*(length/childcnt)+1);
		              
		              temp++;
		        }
		    }    
                        // After we have merged the data we will write data to parent.
                        // data will be transmitted to parent using the pipe.
	      		
                    //Close fd1[0] for data transfer to parent
	      	    close(fd1[0]);
	      		
                    // transmitting data to parent.
		    for(int j=0;j<length;++j)
		    {
		    	val = M[j];
		    	write(fd1[1],&val,sizeof(val));
		    }
		    //Close fd1[1] after data transfer to parent completed
		    close(fd1[1]);
		}

                // when process doesn't have child it will be a leaf node. so we have to sort the Data.
		else
		{
			/*
				This process is a leaf node in the tree.
				Sorting of data is done using bubble sort algorithm.
			*/
                   ///// bubble sorting algorithm
		   for (int c=0;c<(length-1);c++){
                   for (int d=0;d<(length-c-1);d++){  
                         if (B[d] > B[d+1]) 
                       {       swap = B[d];
                               B[d]   = B[d+1];
                               B[d+1] = swap;
                        }
	            }}


                    // After the data has been sorted data is to be passed to the parent.
		    //Close fd1[0] for data transfer to parent
	      	    close(fd1[0]);
				//Write data to parent
		    for(int j=0;j<length;++j)
		    {
		    	val=B[j];
		    	write(fd1[1],&val,sizeof(val));
		    }
		    //Close fd1[1] after data transfer to parent completed
		    close(fd1[1]);
		}
		// Child process completed
                // it will pass exit status to parent.
		exit(EXIT_SUCCESS);
	}
          
        // kid return some positive value if we are in the parent process.
         else //Parent
        {
    	
	    	// Close fd[0] to transfer data to child
		close(fd[0]);
		for(int j=0;j<parentlength;j++)
		{
		    val = parentdata[j];
		    write(fd[1], &val, sizeof(val));
		}

		close(fd[1]);
        
               // receiving data from the child after child has sorted the data.
               close(fd1[1]);
		for(int j=0;j<length;j++)
		{
		    read(fd1[0], &val, sizeof(val)); 
		    parentreceive[childindex][j]=val;
                }
		    
		close(fd1[0]);
		
         
	     
		pid_t pid;
		int status;
                // here the parent will be put on hold untill the child completes.
		pid = waitpid(kid, &status, 0);
        
        
                for(int j=0;j<length;j++) 
                {
                printf("%d ",parentreceive[childindex][j]);
                }
                printf("\n");
        
                printf("Child=%d & Parent %d \n\n",pid,getpid());
                return parentreceive[childindex];
        
	}


 
}

/* 
main function:
main function first take the adjacency matrix in the 'graph' variable. 
then creates bfs using that adjcency matrix. Bfs is stored in the
'child' (Global variable) then it takes the input that to be sorted in 
in the data(Global variable) and pass it to the 'processtree' function 
for sorting. 
*/ 


int main()
{
int i,j,length;
int *sorteddata;
cout<<"Enter no. of Nodes: "; 
cin>>Numnode; 

/// implementation of BFS
int graph[Numnode][Numnode]; 
int visited[Numnode]; 
queue <int> bfsqueue; 

int rootnode; 
cout<<"Enter the Graph :"<<"\n"; 

for (int i=0;i<Numnode;i++) 
   for (int j=0;j<Numnode;j++) 
   { 
     cin>>graph[i][j]; 
     child[i][j]=0;
   } 
cout<<"Enter the Root Node: "; 
cin>>rootnode; 

for (int i=0;i<Numnode;i++)  
visited[i] =0; 

visited[rootnode] =1; 
 
int nodedone =1; 
for (int i=rootnode;nodedone<Numnode;)
  { 
     for (int j=0;j<Numnode;j++) 
        if (visited[j]==0 && graph[i][j]==1) 
          {
           visited[j]=1; 
           child[i][j]=1;
           bfsqueue.push(j);
           nodedone++;
           } 
          i = bfsqueue.front(); 
          bfsqueue.pop(); 
  } 

// print MSP
for (int i=0;i<Numnode;i++) 
  {
     for(int j=0;j<Numnode;j++) 
           cout<<child[i][j]; 
           cout<<endl;
  } 
//BFS completes
    

// taking the input array for sorting.
printf("Length of the input Array you want to sort: ");
scanf("%d",&length);
printf("Enter data ");
for(int i=0;i<length;i++)
{
 scanf("%d ",&Data[i]);
}

sorteddata = processtree(0,length-1,0,Data,length,0,0);


cout<<"\n"<<"Sorted Data: " ;
for(int j=0;j<length;j++) 
printf("%d ",sorteddata[j]);

printf("\n");

return 0;
}
