// 20031510 scyjm2 Jingyu Ma
#include<iostream>
#include<fstream>
#include<stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <algorithm>
#include<unistd.h>


using namespace std;

#define M 120//marco define ant number 120

#define MAX_PATH 256 //how much the file length can be in total
//****************************John Ma**********************************************//
int ant=M;//set the ant number to M
int MAXIT=13810;//Set the maximum iteration to 13810 as a respect to the total number of cities in the world
double alpha=1,beta=10,rho=0.1;// Relative importance of alpha residual information relative importance of alpha residual information, relative importance of beta prediction value,rho volatility coefficient         
int Q=2400;//The amount of pheromones that each ant leaves behind when it travels once                             





double start_time=clock();//record program start time

double  rnd(int low,double uper)//Returns a random number between [low, uper]
{
    double p=(rand()/(double)RAND_MAX)*((uper)-(low))+(low);
    return (p);
};


int rnd(int uper)//Returns an integer between [0, uper]
{
    return (rand()%uper);
};













    int main(int argc,char *argv[])
{
    ifstream in(argv[1]); //open argv[1] from commend line
    string s;         //store every line as string in s
    int line_count=0; //store the current line number
    int first=1;      //if first run then check first line missing key word
    while(getline(in,s)){
         if(first){
             string::size_type idx;
                   idx=s.find("city");//if you don't find city then exit
                   if(idx == string::npos ) {
                       cout << "Wrong file format,first line \"city\" missing\n";
                       exit(2);
                   }
             idx=s.find("x");    //if you don't find x then exit
             if(idx == string::npos ) {
                 cout << "Wrong file format,first line \"x\" missing\n";
                 exit(2);
             }

             idx=s.find("y");    //if you don't find y then exit
             if(idx == string::npos ) {
                 cout << "Wrong file format,first line \"y\" missing\n";
                 exit(2);
             }

                   first=0;    //after you have read the first line then change first to zero
         }
        line_count++;         //line_count++;
        //cout<<s.c_str()<<endl;
    }//
    int N=line_count-1;      //N is how much city you have (ignore first line)





    double **tao;       // The pheromone on each side is 1 at the beginning

    tao = new double*[N]; //Initilize tao array with double

    for(int j=0;j<N;j++)
    {
        tao[j] = new double[N];      
    }

    for (int i=0;i<N;i++)
    {
        for (int j=0;j<N;j++)
        {
            tao[i][j]= 1; //The pheromone on each side is 1 at the beginning
        }

    }

    int *bestroute=new int [N];  // Initilize the best route
   
    int **deltatao;
   //Deltatao [N][N] starts with a 0 pheromone increment
        deltatao = new int *[N];
        for( int i=0; i<N; i++ )
        {
            deltatao[i] = new int [N]  ;
        }


        double **Distance; //record the distance

        Distance = new double *[N];
        for( int i=0; i<N; i++ )
        {
            Distance[i] = new double [N]  ;
        }

        //Record the ants as they pass through the city
        int *sequence=new int [N];  //

      //Record whether the city has been to
        int *ever=new int [N];  //

        //Initilize Coordinate matrix 
        double xy[N][2];


        //תTransfer probility
        double *probility=new double [N];  //

      //The ith ant has to travel N cities
        int **route;

        route = new int *[M];
        for( int i=0; i<M; i++ )
        {
            route[i] = new int [N]  ;
        }



        int *name=new int [N];  //store the name of the city in case it is not in a right order!
        char   buffer[MAX_PATH];
    getcwd(buffer, sizeof(buffer));
    strcat(buffer,"/");
    strcat(buffer,argv[1]);
   // cout<<buffer<<endl;
    ifstream infile(buffer,ios::in);;   //put the absolute path into buffer and then read it



	if(!infile)
	{
		cout<<"Can't open file\n";
		exit(1);
	}
	double temp;
	int i,j,k=0;
	double *d;
	d=&xy[0][0];
    string first_line;
    getline(infile,first_line);  //read the first line

    int* p_to_name;
    p_to_name=&name[0];
    while(infile>>temp)  //The data in data.txt is read into memory as an array
	{
		k++;
		if(k%3==1) {
		    *p_to_name=temp;
		    p_to_name++;
            continue;
		}
		*d=temp;
		d++;
	}

	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
		{
			if(i==j) 
				Distance[i][j]=0;
			temp=pow((xy[i][0]-xy[j][0]),2)+pow((xy[i][1]-xy[j][1]),2);//Computing distance matrix
			Distance[i][j]=sqrt(temp);
		}

	infile.close();


	//Initilization	
	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
		{
			tao[i][j]=1;//The pheromone on each side is 1 at the initial time of tao[N][N]
		}
	for(i=0;i<N;i++)
		bestroute[i]=0;
	double routelength=10e9;//Used to record the current optimal path length
	double length[M];//Current path length
	
	for(i=0;i<N;i++)
		for(j=0;j<N;j++)
			deltatao[i][j]=0;//Deltatao [N][N] starts with a 0 pheromone increment
	int t;
	srand((int)time(0));
	int Ant[M];
	for(i=0;i<ant;i++)//Randomly generate the initial city number of each ant
		Ant[i]=rnd(N);
	i=0;
	do{
		j=0;
		do{   
            int p1,Curcity1,Nextcity1;
            double length1=0;         
            int i1;
            for(i1=0; i1 < N; i1++)
            {
                probility[i1]=0;ever[i1]=0;}
            double value1=0;//The initial path length is 0
            p1=0; //set the start city to the first p1 
            for(i1=0; i1 < N; i1++)
                sequence[i1]=0; //Initialize 01234.. N - 1
            sequence[0]=p1;//Let's say the ant started from the city p1
            ever[p1]=1;//Put the departure cities in sequence table ever[p1]=1, indicating that you have been there
            i1=0;
            do{
                Curcity1=sequence[i1];//���ϵ�ǰ���ڳ���
                int i_this,num_this=0;//num��ʾû�߹��ĳ��и���
                int j_this=10000;
                double temp_this=0,sum_this=0;
                for(i_this=0; i_this < N; i_this++)
                {
                    if(ever[i_this] == 0) //û����
                    {
                        temp_this+= pow((1.0 / Distance[Curcity1][i_this]), beta) * pow((tao[Curcity1][i_this]), alpha);//����δ�߹��ĳ���ת�Ƹ���֮��
                        num_this++;
                    }
                }
                for(i_this=0; i_this < N; i_this++)
                {
                    if(ever[i_this] == 0)
                    {
                        if(num_this == 1)
                        {
                            j_this=i_this;
                            break;//ֻʣһ����ʱ
                        }
                        probility[i_this]= pow((1.0 / Distance[Curcity1][i_this]), beta) * pow((tao[Curcity1][i_this]), alpha) / temp_this;//ת�Ƹ���
                    }
                    else probility[i_this]=0;//ȥ���� ת�Ƹ���Ϊ0
                    sum_this+=probility[i_this];
                }

                double rate=rnd(0, sum_this);//Get a random number between [0, sum], and select the next city by roulette
                double choose=0;
                if(num_this != 1)//There's more than one city I haven't been to
                {
                    for(i_this=0; i_this < N; i_this++)
                    {
                        if(ever[i_this] == 0)//Never been to a city before
                        {
                            choose+=probility[i_this];//The transfer probability and choose of the new city
                            if(choose>=rate)//Select the next city by roulette
                            {
                                j_this=i_this;
                                break;
                            }
                        }
                    }
                }
                if(i_this != j_this) //Choose a new city you haven't been to
                {
                    for(i_this=0; i_this < N; i_this++)
                        if(ever[i_this] == 0)
                        {
                            j_this=i_this;
                            break;
                        }
                }
                Nextcity1=j_this;//Select the next city, j_this
                i1++;//Sequence records the city sequence i1
                if(Nextcity1 >= N) break; 
                sequence[i1]=Nextcity1;//Record the collection of cities the ants have visited so far
                ever[Nextcity1]=1;//A sign that the city has been here
                value1+=Distance[Curcity1][Nextcity1];//The distance between the current and the next city
            }while(i1 < N);//The city sequence has not yet reached N and continues to generate new cities
            length1= value1 + Distance[sequence[0]][sequence[N - 1]];//Plus the distance from the last city to the starting point
            length[j]=length1;
			//length[j]=Travel(Ant[j]);
			if(routelength>=length[j])  //The new path is shorter
			{
				routelength=length[j];//The optimal length
				k=j;//The jth ant
				for(t=0;t<N;t++)
					bestroute[t]=sequence[t];//The optimal sequence
			}
			for(t=0;t<N;t++)
				route[j][t]=sequence[t];//Record the path of each ant
			j++;	
		}while(j<ant);//ant=M
		cout<<"Iteration:"<<i<<" "<<"Length:"<<routelength<<endl;//The routelength is the optimal path
		for(j=0;j<ant;j++)
		{
			Ant[j]=route[j][N-1];//Update the initial city number
		
		}
        int i2;
        int j2;
        for (j2=0; j2 < N - 1; j2++)
        {
            deltatao[route[k][j2]][route[k][j2 + 1]]+= Q / length[k];//Q the amount of pheromone that each ant leaves behind when it travels through the path is its own increment in pheromone plus its Q/length increment
            deltatao[route[k][j2 + 1]][route[k][j2]]+= Q / length[k];//Length current path length the K th ant has the same pheromone increment from city J to city J+1 and back
        }
        deltatao[route[k][N-1]][0]+=Q/length[k];  //The number of pheromones that return to the original city is also increased
        deltatao[0][route[k][N-1]]+=Q/length[k];//Ring of ant algorithm
        for (i2=0; i2 < N; i2++) //The ith ant
            for (j2=0; j2 < N; j2++)  //The jth city
            {
                if(rho*deltatao[i2][j2] < 0.3)//Rho volatilization coefficient of pheromone concentration on path i2-j2 
                    tao[i2][j2]= (1 - rho) * tao[i2][j2] + 0.1;  //The pheromone concentration on the path i2-j2 is lower than 0.3 and updated to (1-rho)*tao[I][j]+0.1 volatilization?
                else
                    tao[i2][j2]= (1 - rho) * tao[i2][j2] + rho * deltatao[i2][j2]; //Otherwise, it is updated to (1-rho)*tao[I][j]+rho*deltatao[I][j];Evaporation restrengthening
                deltatao[i2][j2]=0;//release of the incremental
            }//Tao [I][j] is the pheromone value deltatao[I][j] is a pheromone increment
	
		i++;		 //I iteration times
	    if((150<i)&&(i<5000))        //As the number of iterations increases, the rho gradually increases
			if(i%100==0)
                rho=rho+0.1; //To improve!
        double end_time=clock();
        double use_time=(end_time-start_time)/CLOCKS_PER_SEC;
        if(N>1000){
          if(use_time>=1200){
              break;
          }
        }else{
          if(use_time>=600){
              break;
          }
        }
	}while(i<MAXIT);//The city sequence has not yet reached N and continues to generate new cities
    double end_time=clock();
    double use_time=(end_time-start_time)/CLOCKS_PER_SEC;
	cout<<"Best route: "<<endl;

//    cout << endl;
	//show();
	cout<<routelength<<"\t";
        int temp0, i0;
        for (i0 = 0; i0 < N; i0++)
            //{temp0=bestroute[i0];
            //cout<<add[temp0]<<"-->";
        {
            cout << name[bestroute[i0]] << "\t";
        }
        //temp0=bestroute[0];
        //cout<<add[temp0]<<endl;
        cout << name[bestroute[0]]<<endl;
    cout<<"Time used: "<<use_time<<endl;
	
	
//free all the memory 
        for (i=0;i<N;i++)
        {
            delete[] tao[i]; 
        }
        delete[] tao;
        delete [] bestroute;

        for (i=0;i<N;i++)
        {
            delete[] Distance[i]; 
        }
        delete[] Distance;
        delete [] sequence;
        delete [] ever;
        delete [] probility;
    
        for( int i=0; i<M; i++ )
        {
            delete [] route[i];
        }
        delete [] route;
        delete [] name;
        return 0;
}





