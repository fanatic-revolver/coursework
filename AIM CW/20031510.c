//
//  main.c
//  mknapsack
//
//  Created by Bai on 14/03/2020.
//  Copyright © 2019 UNNC. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* global parameters */
int RAND_SEED[] = {1,20,30,40,50,60,70,80,90,100,110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
int NUM_OF_RUNS = 1;
int MAX_TIME = 300;  //max amount of time permited (in sec)
int num_of_problems;
int MAX_LS_ITER = 100;

/* parameters for evlutionary algorithms*/
static int POP_SIZE = 50;   //please modify these parameters according to your problem
int MAX_NUM_OF_GEN = 100000; //max number of generations
int T_SIZE = 4; // tournament size
float CROSSOVER_RATE = 1; 
float MUTATION_RATE = 0.001;




struct solution_struct best_sln;  //global best solution

//return a random number between 0 and 1
float rand_01()
{
    float number;
    number = (float) rand();
    number = number/RAND_MAX;
    //printf("rand01=%f\n", number);
    return number;
}

//return a random nunber ranging from min to max (inclusive)
int rand_int(int min, int max)
{
    int div = max-min+1;
    int val =rand() % div + min;
    //printf("rand_range= %d \n", val);
    return val;
}


typedef struct item_struct{
    int dim; //no. of dimensions
    int* size; //volume of item in all dimensions
    int p;
	double ratio;
	int index;
} *ItemPtr;

typedef struct problem_struct{
    int n; //number of items
    int dim; //number of dimensions
    struct item_struct* items;
    int* capacities;  //knapsack capacities
} *ProblemPtr;

void free_problem(struct problem_struct* prob)
{
    if(prob!=NULL)
    {
        if(prob->capacities !=NULL) free(prob->capacities);
        if(prob->items!=NULL)
        {
            for(int j=0; j<prob->n; j++)
            {
                if(prob->items[j].size != NULL)
                    free(prob->items[j].size);
            }
            free(prob->items);
        }
        free(prob);
    }
}

void init_problem(int n, int dim, struct problem_struct** my_prob)
{
    struct problem_struct* new_prob = malloc(sizeof(struct problem_struct));
    new_prob->n=n; new_prob->dim=dim;
    new_prob->items=malloc(sizeof(struct item_struct)*n);
    for(int j=0; j<n; j++)
        new_prob->items[j].size= malloc(sizeof(int)*dim);
    new_prob->capacities = malloc(sizeof(int)*dim);
    *my_prob = new_prob;
}


//example to create problem instances, actual date should come from file
struct problem_struct** load_problems(char* data_file)
{
    int i,j,k;
    //int num_of_probs;
    FILE* pfile = fopen(data_file, "r");
    if(pfile==NULL)
        {printf("Data file %s does not exist. Please check!\n", data_file); exit(2); }
    fscanf (pfile, "%d", &num_of_problems);
 
    struct problem_struct** my_problems = malloc(sizeof(struct problem_struct*)*num_of_problems);
    for(k=0; k<num_of_problems; k++)
    {
        int n, dim, obj_opt;
        fscanf (pfile, "%d", &n);
        fscanf (pfile, "%d", &dim); fscanf (pfile, "%d", &obj_opt);
        
        init_problem(n, dim, &my_problems[k]);  //allocate data memory
        for(j=0; j<n; j++)
        {
            my_problems[k]->items[j].dim=dim;
            fscanf(pfile, "%d", &(my_problems[k]->items[j].p)); //read profit data
            //printf("item[j].p=%d\n",my_problems[k]->items[j].p);
        }
        for(i=0; i<dim; i++)
        {
            for(j=0; j<n; j++)
            {
                fscanf(pfile, "%d", &(my_problems[k]->items[j].size[i])); //read size data
                //printf("my_problems[%i]->items[%i].size[%i]=%d\n",k,j,i,my_problems[k]->items[j].size[i]);
            }
        }
        for(i=0; i<dim; i++){
            fscanf(pfile, "%d", &(my_problems[k]->capacities[i]));
            //printf("capacities[i]=%d\n",my_problems[k]->capacities[i] );
        }
    }
    fclose(pfile); //close file
    return my_problems;
}

typedef struct solution_struct{
    struct problem_struct* prob; //maintain a shallow copy of the problem data
    float objective;
    int feasibility; //indicate the feasiblity of the solution
    int* x; //chromosome vector
    int* cap_left; //capacity left in all dimensions
} *SolutionPtr;


void free_solution(struct solution_struct* sln)  //free memmory usage
{
    if(sln!=NULL)
    {
        free(sln->x);
        free(sln->cap_left);
        sln->objective=0;
        sln->prob=NULL;
        sln->feasibility=false;
    }
}

//copy a solution from another solution
bool copy_solution(struct solution_struct* dest_sln, struct solution_struct* source_sln)
{
    if(source_sln ==NULL) return false;
    if(dest_sln==NULL)
    {
        dest_sln = malloc(sizeof(struct solution_struct));
    }
    else{
        free(dest_sln->cap_left);
        free(dest_sln->x);
    }
    int n = source_sln->prob->n;
    int m =source_sln->prob->dim;
    dest_sln->x = malloc(sizeof(int)*n);
    dest_sln->cap_left=malloc(sizeof(int)*m);
    for(int i=0; i<m; i++)
        dest_sln->cap_left[i]= source_sln->cap_left[i];
    for(int j=0; j<n; j++)
        dest_sln->x[j] = source_sln->x[j];
    dest_sln->prob= source_sln->prob;
    dest_sln->feasibility=source_sln->feasibility;
    dest_sln->objective=source_sln->objective;
    return true;
}

void evaluate_solution(struct solution_struct* sln)
{
    //evaluate the feasibility and objective of the solution
    sln->objective =0; sln->feasibility = 1;
    struct item_struct* items_p = sln->prob->items;
    
    for(int i=0; i< items_p->dim; i++)
    {
        sln->cap_left[i]=sln->prob->capacities[i];
        for(int j=0; j<sln->prob->n; j++)
        {
            sln->cap_left[i] -= items_p[j].size[i]*sln->x[j];
            if(sln->cap_left[i]<0) {
                sln->feasibility = -1*i; //exceeding capacity
                return;
            }
        }
    }
    if(sln->feasibility>0)
    {
        for(int j=0; j<sln->prob->n; j++)
        {
            sln->objective += sln->x[j] * items_p[j].p;
        }
    }
}

//output a given solution to a file
void output_solution(struct solution_struct* sln, char* out_file)
{
    if(out_file !=NULL){
        FILE* pfile = fopen(out_file, "a"); //append solution data
        fprintf(pfile, "%i\n", (int)sln->objective);
        for(int i=0; i<sln->prob->n; i++)
        {
            fprintf(pfile, "%i ", sln->x[i]);
        }
        fprintf(pfile, "\n");
        /*for(int j=0; j<sln->prob->n; j++)
            fprintf(pfile, "%i ", sln->prob->items[j].p);
        fprintf(pfile, "\n");*/
        fclose(pfile);
    }
    else
        printf("sln.feas=%d, sln.obj=%f\n", sln->feasibility, sln->objective);
}

// dec order
int comp1(const void *item1, const void *item2)
{
    ItemPtr i1 = (ItemPtr)item1;
    ItemPtr i2 = (ItemPtr)item2;

    if(i1->ratio == i2->ratio)
        return 0;
    else if(i1->ratio > i2->ratio)
        return -1;
    else
        return 1;
}

// inc order
int comp2(const void *item1, const void *item2)
{
    ItemPtr i1 = (ItemPtr)item1;
    ItemPtr i2 = (ItemPtr)item2;

    if(i1->ratio == i2->ratio)
        return 0;
    else if(i1->ratio > i2->ratio)
        return 1;
    else
        return -1;
}

// evauluate the solution according to items' index
void evaluate_solution1(struct solution_struct* sln)
{
    //evaluate the feasibility and objective of the solution
    sln->objective =0; sln->feasibility = 1;
    struct item_struct* items_p = sln->prob->items;
    
    for(int i=0; i< items_p->dim; i++)
    {
        sln->cap_left[i]=sln->prob->capacities[i];
        for(int j=0; j<sln->prob->n; j++)
        {   
            int index = items_p[j].index;
            sln->cap_left[i] -= items_p[j].size[i]*sln->x[index];
            if(sln->cap_left[i]<0) {
                sln->feasibility = -1*i; //exceeding capacity
                return;
            }
        }
    }
    if(sln->feasibility>0)
    {
        for(int j=0; j<sln->prob->n; j++)
        {
            sln->objective += sln->x[items_p[j].index] * items_p[j].p;
        }
    }
}

// index order
int comp3(const void *item1, const void *item2)
{
	ItemPtr i1 = (ItemPtr)item1;
    ItemPtr i2 = (ItemPtr)item2;
	return i1->index - i2->index;
}

// modify the solutions that violate the capacity constraints
void feasibility_repair(struct solution_struct* sln)
{   
	evaluate_solution(sln);
	if(sln->feasibility < 1) // only repair when not feasible
	{
		int num = sln->prob->n;
		ItemPtr is = sln->prob->items;
		for(int i = 0;i < num;i++)
		{
			double avgS = 0;
			for(int j = 0;j < sln->prob->dim;j++)
				avgS += (double)is[i].size[j] / sln->prob->capacities[j];
			
			is[i].ratio = is[i].p / avgS;
		}
		
		// make the solution feasible
        qsort(is, num, sizeof(is[0]), &comp2);// dec sort
		for(int i = 0;i < num && (sln->feasibility < 1);i++)
		{
			if(sln->x[is[i].index] == 1)
			{
				sln->x[is[i].index] = 0;
				evaluate_solution1(sln);
			}
		}
        
		// check if too much spare capacity
        qsort(is, num, sizeof(is[0]), &comp1); // inc sort
		for(int i = 0;i < num;i++)
		{
			if(sln->x[is[i].index] == 0)
			{
				sln->x[is[i].index] = 1;
				evaluate_solution1(sln);
                // if cannot put in then take out 
				if(sln->feasibility < 1)
				{
					sln->x[is[i].index] = 0;
					evaluate_solution1(sln);
				}

			}
		}
        qsort(is, num, sizeof(is[0]), &comp3); // restore the order of items
        
	}
}

//check the  feasiblity and obj values of solutons from solution_file.
//return 0 is all correct or the index of the first infeasible problem [1, num_of_problems].
int check_solutions(struct problem_struct** my_problems, char* solution_file)
{
    FILE * pfile= fopen(solution_file, "r");
    if(pfile==NULL)
    {
        printf("Solution file %s does not exist. Please check!\n", solution_file);
        exit(2);
    }
    float val_obj;
    int val;
    fscanf (pfile, "%i", &val);
    if(val != num_of_problems)
    {
        printf("The stated number of solutions does not match the number of problems.\n");
        exit(3);
    }
    struct solution_struct temp_sln;
    int count=0, k=0;
    int n, dim;
    while(fscanf (pfile, "%f", &val_obj)!=EOF && k<num_of_problems)
    {
        //val_obj = val;
        n= my_problems[k]->n;  dim= my_problems[k]->dim;
        temp_sln.x = malloc(sizeof(int)*n);
        temp_sln.cap_left=malloc(sizeof(int)*dim);
        temp_sln.prob = my_problems[k];
        while(fscanf (pfile, "%i", &val)!=EOF)
        {
            if(val<0 || val>1) {fclose(pfile);  return k+1;} //illeagal values
            temp_sln.x[count] = val;
            count++;
            if(count==n)
            {
                evaluate_solution(&temp_sln);
                if(!temp_sln.feasibility || fabs(temp_sln.objective - val_obj)>0.01)
                {
                    fclose(pfile);
                    //printf("feasb=%i, obj= %f, val=%i\n",temp_sln.feasibility, temp_sln.objective, val_obj);
                    //output_solution(&temp_sln, "my_debug.txt");
                    return k+1;  //infeasible soltuion or wrong obj
                }
                else{
                    break;
                }
            }
        }
        count=0; k++;
        
        free(temp_sln.x); free(temp_sln.cap_left);
    }
    fclose(pfile);
    return 0;
}

//update global best solution from sln
void update_best_solution(struct solution_struct* sln)
{
    if(best_sln.objective < sln->objective)
    copy_solution(&best_sln, sln);
}

// pairwise swap first decent local search
void local_search_first_descent(struct solution_struct* sln)
{
    ItemPtr is = sln->prob->items;
	int count = 0, l = 0; // variable - number of iteration and the marker of local optimal
	while(count < MAX_LS_ITER && !l) // loop until max number of iteration reached or local optimal reached
	{
		l = 1;
		for(int i = 0;i < sln->prob->n && l;i++)
			if(sln->x[i] == 1)
				for(int j = 0;j < sln->prob->n && l;j++)          
					if(sln->x[j] == 0)
					{
						// the objective difference between two items
						int d = is[j].p - is[i].p;
						// update the best move
						if(d > 0)
						{
							bool f = true; // if the solution is feasible
							for(int k = 0;k < sln->prob->dim;k++)
								if(sln->cap_left[k] + is[i].size[k] < is[j].size[k])
									f = false;                     
					
							if(f) // if feasible then do swap
							{
								sln->x[i] = 0;
								sln->x[j] = 1;
								evaluate_solution(sln);
								l = 0;
							}  
						}
					}
        count++; 
	}
}

// return a new allocated and initialized solution
SolutionPtr allocSln(ProblemPtr prob)
{
	SolutionPtr sln = malloc(sizeof(*sln));
	sln->objective = 0;
	sln->feasibility = 1;
	sln->prob = prob;
	sln->x = malloc(sizeof(int) * prob->n);
	for(int i = 0;i < prob->n;i++)
	{
		sln->x[i] = 0;
	}
	sln->cap_left = malloc(sizeof(int) * prob->dim);
	for(int i = 0;i < prob->dim;i++)
	{
		sln->cap_left[i] = prob->capacities[i];
	}
	return sln;
}

// random initialization
void popInitialize(SolutionPtr *pop, ProblemPtr prob)
{
	for(int i = 0;i < POP_SIZE;i++)
		pop[i] = allocSln(prob);
	
	for(int i = 0;i < POP_SIZE;i++)
	{
		while(pop[i]->feasibility == 1)
		{	// initialize the individual randomly
			int it = rand_int(0,prob->n-1);
			while(pop[i]->x[it] == 1)
				it = rand_int(0,prob->n-1);
			
			pop[i]->x[it] = 1;
			evaluate_solution(pop[i]);
			if(pop[i]->feasibility < 1)
				pop[i]->x[it] = 0;
			
		}
		evaluate_solution(pop[i]);
	}
}

// Tournament selection
void tour(SolutionPtr *pop, SolutionPtr *mtP)
{
	for(int i = 0;i < POP_SIZE;i++)
	{
		// randomly select individuals
		SolutionPtr tP[T_SIZE];
		
		for(int j = 0;j < T_SIZE;j++)
		{
			int id = rand_int(0, POP_SIZE-1);
			tP[j] = pop[id];
		}
		
		// find best 
		int b = -1;
		float bV = -1;
		for(int j = 0;j < T_SIZE;j++)
			if(tP[j]->objective > bV)
			{
				b = j;
				bV = tP[j]->objective;
			}
		
		// copy into mtP
		copy_solution(mtP[i],tP[b]);
	}
	
}

// UX crossover - randomly copy from either of the two parents
SolutionPtr crossover(SolutionPtr p1, SolutionPtr p2)
{
	SolutionPtr son = allocSln(p1->prob);
	for(int i = 0;i < p1->prob->n;i++)
	{
		if(rand_int(0, 1))
			son->x[i] = p1->x[i]; // copy from first parent
		else
			son->x[i] = p2->x[i]; // copy from second parent
	}
	return son;
}

// flip each bit on chromosome with a small possibilty
void mutation(SolutionPtr sln)
{
	for(int i = 0;i < sln->prob->n;i++)
		if(rand_01() < MUTATION_RATE)
		{
			if(sln->x[i] == 1)
				sln->x[i] = 0;
			else
				sln->x[i] = 1;
		}
}

int compfucSolutionDec(const void *a, const void *b)
{
	SolutionPtr s1 = *(SolutionPtr *)a;
	SolutionPtr s2 = *(SolutionPtr *)b;
	if(s1->objective > s2->objective)
		return -1;
    else if(s1->objective < s2->objective)
		return 1;
    else
		return 0;
	
}

// steady state
void replace(SolutionPtr *pop, SolutionPtr *sons)
{
	qsort(pop, POP_SIZE, sizeof(SolutionPtr), &compfucSolutionDec);
	qsort(sons, POP_SIZE, sizeof(SolutionPtr), &compfucSolutionDec);
	
	// replace
	int g = 4; // gap - the number of replacement
	for(int i = 0;i < g;i++)
		// best children replace worst parents
		copy_solution(pop[POP_SIZE-g+i], sons[i]);
	
}

int memeticAlgo(ProblemPtr prob)
{
	clock_t time_start, time_fin;
    time_start = clock();
    double time_spent=0;
    int iter =0;
	
	// initialize population
	SolutionPtr pop[POP_SIZE];
	popInitialize(pop, prob);
	for(int i = 0;i < prob->n;i++)
		prob->items[i].index = i;
	
	
	while(iter < MAX_NUM_OF_GEN && time_spent < MAX_TIME)
	{   
		// select parents
		SolutionPtr mtP[POP_SIZE];
		for(int i = 0;i < POP_SIZE;i++)
			mtP[i] = allocSln(pop[0]->prob);
		
		tour(pop, mtP); // tounament to select parent

		// reproduce offsprings
		SolutionPtr sons[POP_SIZE];
		int count = 0; 
		while(count < POP_SIZE)
		{  
			int i1 = rand_int(0, POP_SIZE-1);
			int i2 = rand_int(0, POP_SIZE-1);
			while(i1 == i2) 
				i2 = rand_int(0, POP_SIZE-1);
			
			sons[count] = crossover(mtP[i1], mtP[i2]); // crossover - UX crossover
			mutation(sons[count]); // mutation
			feasibility_repair(sons[count]); 

			local_search_first_descent(sons[count]);

			int f = 0; // mark if duplicate
			for(int i = 0;i < POP_SIZE && !f;i++)
				if(sons[count]->objective == pop[i]->objective)
					f = 1;
				
			for(int i = 0;i < count && !f;i++)
				if(sons[count]->objective == sons[i]->objective)
					f = 1;
            
			// if duplicate then regenerate			
			if(f)
			{
				free_solution(sons[count]);
				free(sons[count]);
				continue;
			}
			count++;
		}		
		
		// replace worst individuals with best offsprings
		replace(pop, sons); 
		
		// update the best one in the population
		for(int i = 0;i < POP_SIZE;i++)
			update_best_solution(pop[i]);

		
		for(int i = 0;i < POP_SIZE;i++)
		{
			free_solution(mtP[i]); // BUG - The struct itself not freed in the function!
			free(mtP[i]);
			free_solution(sons[i]);
			free(sons[i]);
		}
		
		iter++; 
        time_fin = clock();
        time_spent = (double)(time_fin-time_start)/CLOCKS_PER_SEC;
	}

	for(int i = 0;i < POP_SIZE;i++)
	{
		free_solution(pop[i]);
		free(pop[i]);
	}
}

int main(int argc, const char * argv[]) {

    printf("Starting the run!\n");
    char data_file[50]={"somefile"}, out_file[50]={}, solution_file[50]={};  //max 50 problem instances per run
    if(argc<3)
    {
        printf("Insufficient arguments. Please use the following options:\n   -s data_file (compulsory)\n   -o out_file (default my_solutions.txt)\n   -c solution_file_to_check\n   -t max_time (in sec)\n");
        return 1;
    }
    else if(argc>9)
    {
        printf("Too many arguments.\n");
        return 2;
    }
    else
    {
        for(int i=1; i<argc; i=i+2)
        {
            if(strcmp(argv[i],"-s")==0)
                strcpy(data_file, argv[i+1]);
            else if(strcmp(argv[i],"-o")==0)
                strcpy(out_file, argv[i+1]);
            else if(strcmp(argv[i],"-c")==0)
                strcpy(solution_file, argv[i+1]);
            else if(strcmp(argv[i],"-t")==0)
                MAX_TIME = atoi(argv[i+1]);
        }
        //printf("data_file= %s, output_file= %s, sln_file=%s, max_time=%d", data_file, out_file, solution_file, MAX_TIME);
    }
    struct problem_struct** my_problems = load_problems(data_file);
    
    if(strlen(solution_file)<=0)
    {
        if(strcmp(out_file,"")==0) strcpy(out_file, "my_solutions.txt"); //default output
        FILE* pfile = fopen(out_file, "w"); //open a new file
        fprintf(pfile, "%d\n", num_of_problems); fclose(pfile);
        for(int k=0; k<num_of_problems; k++)
        { 	
            best_sln.objective=0; best_sln.feasibility=0;
            for(int run=0; run<NUM_OF_RUNS; run++)
            {
				srand(RAND_SEED[run]);
                printf("Running Memetic Algorithm...\n");
                printf("prob%d:run%d\n", k, run+1);
				memeticAlgo(my_problems[k]); 

            }
            output_solution(&best_sln,out_file);   
        }
    }else{
        printf("\n**%d\n", check_solutions(my_problems, solution_file));
       
    }
        
    for(int k=0; k<num_of_problems; k++)
    {
       free_problem(my_problems[k]); //free problem data memory
    }
    free(my_problems); //free problems array
    if(best_sln.x!=NULL && best_sln.cap_left!=NULL){ free(best_sln.cap_left); free(best_sln.x);} //free global
    return 0;
}
