/* 20031510 Jingyu MA scyjm2*/
/* This piece of code is using the Allman style,and strictly stick to indention,the comments has been listed on the right hand side*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N 1000           //state the maximum number of the city you can intake

/* The definition of the Graph structure  */
typedef struct{
    int n;               //n is the number of nodes
    int** E;             //E is the adjacent matrix to store the distance between city and city
}Graph;

char name[N][256];       //state a two-dimensional array to store the city names 
char buf[1024];          //store the string we read into the buffer,give a large buffer to make sure anytime it is safe to store
int  goon=1;             //I remember goon is always associated with loops to control when to quit the loop 

void run(Graph* g);      //state the run function in case of using it in other functions


/* The exit function is used when exiting the program,each is specified with a code and print a message */ 
void EXIT(int code)
{
    if(code == 0)                                    //Normal quit is exit with code 0
	{
        exit(0);
    }
    if(code == 1)                                    //If the file doesn't exist or you just cannot open the file,quit with code 1 
	{ 
        perror("Cannot open file."); 
        exit(1);
    }  
    if(code == 2)                                    //If the file is invalid, say, it doesn't match the format,we quit with code 2
	{ 
        printf("Invalid distances file.\n"); 
        exit(2); 
    }
    if(code == 3)                                    //If the memory cannot be allocated,we have to exit with code 3 
	{ 
        perror("Unable to allocate memory."); 
        exit(3); 
    }
	if(code == 4)                                    //If something else happened then quit with exit code 4
	{
		perror("Something unexpected happened.");
		exit(4);
	}
}


/* This function will initialize the graph and nodes */
void initGraph(Graph* g, int n)
{
    g->E = (int**)malloc(n*sizeof(int*));           //Allocate the memory of nodes
    if(g->E==NULL)                                  //if failed to allocate memory quit with code 3
	{ 
        EXIT(3); 
    }
    for(int i = 0; i < n; ++i)                      //Using a for loop to allocate memory for the adjacent matrix 
	{
        g->E[i] = (int*)malloc(n*sizeof(int));      //request the storage space for a line in the matrix
        if(g->E[i]==NULL)                           //if failed to allocate memory exit with code 3 
		{
            EXIT(3); 
        }
        for(int j = 0; j < n; ++j)                  //initiate every element in the matrix to an invalid value,-1
		{
            g->E[i][j] = -1;
        }
    }
    g->n = n;                                       //put the argument n into the Graph structure,give out the node numbers  
}


/* add an edge to the graph from "from" to "to" len is the edge weight */
void addEdge(Graph* g, int from, int to, int len)
{   
    g->E[from][to] = len;                          //put the distance between stations into the 2D array
}


/* readGraph is to read the data from the file */
Graph* readGraph(FILE* fp)
{                             
    Graph* g = (Graph*)malloc(sizeof(Graph));               
    fscanf(fp,"%s",buf);                    //read the first line from the file contains commas and cities                            
	int cityId = 0;                         //give each city an id,each one of them has a number                                       
    int len = strlen(buf), tmp = 0;         //calculate the length of the buffer                     
    buf[len] = ',';                         //put a comma to the end of the buffer                                   	
    for(int i = 1; i <= len; ++i)           //loop to search through the buffer
	{                  
        if(buf[i] == ',')                   //if you meet a comma
		{                           
            name[cityId][tmp] = '\0';       //give the city name an ending "\0"          
            cityId++;                       //add 1 to cityId ,meaning we have read one more city number                                
            tmp=0;                          //reinitialize tmp,prepare for the next loop                                   
        }
		else
		{ 
			name[cityId][tmp++] = buf[i];   //else we put what we read into the name array
		}
    }
    if(cityId==1)                           //if there is only one city then the file is invalid                     
	{
		EXIT(2);
	}
    initGraph(g,cityId);                    //initialize the graph with how many cities you read from the file                        
    for(int i = 0; i < cityId; ++i)
	{                
        fscanf(fp,"%s",buf);                //scan the first line of the file into the buf                       
        int v = 0;                          //v is the weight length of the edge,initialized as 0
        int j, cnt=0;                       //j is the loop variable,cnt is the destination city                               
        len = strlen(buf);                  //still calculate the length of the buffer                     
        buf[len] = ',';                     //put a comma to the end of the buffer                          
        for(j=0;j<=len;++j)                 //loop to search for the comma
		{ 
			if(buf[j]==',')                 //if you find the comma，break out of the for loop
			{
				break;
			}
		}
		buf[j]=0;                           //put a zero to the end of the string
		if(strcmp(buf,name[i]))             //if the city numbers doesn't match the number in the rows then there exit with code 2
		{                 
			EXIT(2);
		}
		
        for(j++;j<=len;++j)                 //loop to construct the graph,construct the nodes 
		{                         
            if(buf[j]==',')                 //each time you find a comma
			{                        
                if(v!=0)                    //if the distance is not 0
				{ 
			      addEdge(g,i,cnt,v);       //add an edge from the current city to the destination city cnt            
				  v=0;                      //reinitialize distance v to 0
				}   
                cnt++;                      //add one to the destination city,see if the next city                               
            }
			else if(buf[j]>='0'&&buf[j]<='9')      //if the input is legal
			{        
                v=v*10+buf[j]-'0';                 //traverse through string,number=(10*previous result + current number)               
            }                                      //and the sum will exactly be the distance between cities
			else                                   //if there is illegal input in the file then exit with code 2
			{
                EXIT(2);
            }
        }
    }
    return g;                                      //return the pointer that point to the graph
}


/* Release is to release the memory used in graph*/
void release(Graph* g)
{
    for(int i = 0; i < g->n; ++i)
	{ 
        free(g->E[i]);
	}
    free(g->E);
}


/* One of the classic algorithm to search for the shortest path is to use Dijkstra,in simple terms,each time you find the vertex
closest to the start point,then you expand from that vertex to get the shortest path from the start point to all other points */ 
void Dijkstra(int S, int T, Graph* g)
{
    int *pre = (int*)malloc(sizeof(int)*(g->n));   //give memory space,pre is the precursor of the shortest path
    if(pre == NULL)
	{ 
        EXIT(3); 
    }
    int *dis = (int*)malloc(sizeof(int)*(g->n));   //give memory space,dis is the distance of the shortest distance from S to T
    if(dis == NULL)
	{ 
        EXIT(3); 
    }
    int *vis = (int*)malloc(sizeof(int)*(g->n));   //give memory space,vis is the nodes traverse state
    if(vis== NULL)
	{ 
        EXIT(3); 
    }
    for(int i = 0; i < g->n; ++i)
	{ 
        pre[i] = -1;                               //initialize all the elements in the pre array to an invalid value,-1 
        dis[i] = -1;                               //initialize all the elements in the dis array to an invalid value,-1
        vis[i] = 0;                                //initialize all the elements in the vis array to 0 
    }
    dis[S] = 0;                                    //state that the distance to start point is 0
    for(int i=0;i<(g->n)-1;i++)
	{
        int x = -1;
        for(int i = 0; i < g->n; ++i)
		{
            if(vis[i])                             //at this node vis is empty,quit the loop and continue with next node
			{
				continue;
			}
            if(dis[i]==-1)                         //at this node dis is an invalid value,quit the loop go on with next node
			{
				continue;
			}				
            if(x==-1)                              //save the node index to x
			{
				x=i;
			}				
            if(dis[i]<dis[x])                      //node i is more closer than node x,update node i to node x
			{
				x=i;
			}
        }
        if(x==-1)                                  //x is an invalid value,quit the loop
		{
			break;
        }     	
        vis[x] = 1; 
        for(int i = 0; i < g->n; ++i)              //loop to find the shortest path and save it 
		{
            if(vis[i])                             //at this node vis is empty quit the loop and continue with next node
			{
				continue;
			}
            if(g->E[x][i]==-1)                     //if there is no possible journey continue the loop 
			{
				continue;
			}
            if(dis[i]==-1)                         //save the first distance to dis[i]
			{
                dis[i]=dis[x]+g->E[x][i];
                pre[i]=x;
            }
            if(dis[x]+g->E[x][i]<dis[i])           //if there is a shorter distance,save this distance to dis[i]
			{
                dis[i]=dis[x]+g->E[x][i];
                pre[i]=x;
            }
        }
    }
	if(dis[T]==-1)                                 //if the distance is an invalid value then print there is no possible journey 
	{
		printf("No possible journey.\n");
		run(g);                                    //start from the beginning
	}	
    printf("From %s\n",name[S]);                   //print the start station
    int path = 0;
    if(pre[T]==S)                                  //if the last station in the precursor is start station then it is a direct
	{
        printf("direct\n"); 
        path = 0;
    }
	else
	{
        printf("via\n"); 
        int n = 0;
        int x = pre[T];
        while(x != S)                              //loop to find the information on stations that went through
		{ 
            vis[n++] = x; 
            x = pre[x]; 
        }
        path = n ;                                                                                       
        for(int i = n-1; i >= 0; --i)              //loop to print the stations that went through
		{ 
            printf("%s\n",name[vis[i]]);                    
        }
    }   
    printf("To %s\n",name[T]);                                //print out the terminal station
    printf("Distance %d km\n",dis[T]);                        //print out the distance 
    printf("Cost %d RMB\n",(int)(1.2*dis[T]+0.5+path*25));    //print out the cost 
    free(vis); 
    free(dis); 
    free(pre);                                                //free all the memory used
}


/* run is used to run the prompt and keep the program going */
void run(Graph* g)
{
    printf("Start station:");
    fgets(buf,sizeof(buf),stdin);               //use fgets to read Start station name from keyboard               
    int len = strlen(buf);
    buf[len-1] = buf[len];                      //if you are using fgets there will be a \n at the end,so we clear it                  
    if(buf[0]=='\0')                            //if you read an enter which is nothing from keyboard,then you have to quit 
	{ 
	    goon=0;                                 //set goon to 0 ,quit the loop  
        release(g);                             //release the memory 
        EXIT(0);                                //exit with code 0
    }
    int S = -1;                                 //set S to be an invalid value, -1
    for(int i = 0; i < g->n; ++i)               //loop to search for the start station
	{ 
        if(strcmp(name[i],buf) == 0)            //if you find the start station in the name array  
		{ 
            S = i;                              //give the current index to S,in other words,we mark the position of S in the array
		}
    }
    if(S == -1)                                 //if the start station doesn't exist then just print the message
	{ 
        printf("No such station.\n"); 
        return; 
    }
    printf("End station:");
    fgets(buf,sizeof(buf),stdin);               //use fgets to read the End station name from the key board
    int T = -1;                                 //initialize Terminal station as a invalid value 
    len = strlen(buf);                          //len = the length of the buf
    buf[len-1] = buf[len];                      //
    for(int i = 0; i < g->n; ++i)               //loop to search for if the the terminal station exists
	{ 
        if(strcmp(name[i],buf) == 0)            //when there is correct match in the name array then strcmp will return 0 
		{ 
            T = i;                              //give the index you found to T, in other words mark the terminal station
		}
	}
    if(T == -1)                                 //if terminal station is a invalid station just print the message
	{ 
        printf("No such station.\n"); 
        return; 
    }   
    if(S == T)                                  //if the start station and the end station are the same then start from the beginning 
	{ 
        printf("No journey, same start and end station.\n"); 
        return; 
    }       
    Dijkstra(S,T,g);                            //run Dijkstra with Start station,Terminal station,and with the graph
}


/* Here is the main function sorry for not declaring function prototype I know it is hard to read through all these*/ 
int main(int argc, char* argv[])
{    
    if(argc != 2)                               //if the input mismatch the input format quit with exit code 4
	{ 
        perror("Invalid command line arguments. Usage: train <disances.txt>"); 
        exit(4); 
    }
    FILE* fp = fopen(argv[1],"r");              //open the distance file with read-only                                     
    if(fp == NULL)
	{ 
        EXIT(1); 
    }
    Graph* g = readGraph(fp);                   //store the thing read into g,a pointer
    while(goon)                                 //keep running the program until the user stop,while goon ==1
	{ 
        run(g);                                 
    }  
    return 0;
}
