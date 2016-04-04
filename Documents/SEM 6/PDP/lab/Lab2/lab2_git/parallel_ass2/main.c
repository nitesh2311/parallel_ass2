#include <stdio.h>
#include <stdlib.h>
#include "sudoku.h"
#include <string.h>
#include <omp.h>


#define N 16

// value of the struct array
struct values{
	int* value;
	int len;
	int set;
	
};

// peer id for a square
struct peers{
	int* peer;
	int len;
	
};

//cell of  a square
struct cells{
	int* idnum;
	int len;
};

typedef struct values* val;
typedef struct values valw;
typedef struct peers* per;
typedef struct cells* cell;


per arrstr;

// copy the values array for recursive calls
val copyV(val arr)
{
	val create = (val)malloc(sizeof(struct values)*N*N);
	int i,j;
	
	for(i=0;i<N*N;i++)
	{
		create[i].len=arr[i].len;
		create[i].value=(int*)malloc(sizeof(int)*N);
		for(j=0;j<N;j++)create[i].value[j]=arr[i].value[j];
		create[i].set=arr[i].set;
	}
	return create;
}

// remove the digit from peer which has been allocated
int removeP(int id,int digit,val array)
{
	int i,j;
	int len = arrstr[id].len;
	
	for(i=0;i<len;i++)
	{	
		if(array[arrstr[id].peer[i]].value[digit]!=-1)
		{	array[arrstr[id].peer[i]].value[digit] = -1;
			array[arrstr[id].peer[i]].len--;
		}	
	}
	return 0;
	
}

// set the array if the len of choice is one
int setP(int id,val array)
{	int i,j;
	int len = arrstr[id].len;

	for(i=0;i<len;i++)
	{
		 // check the  peer length of value poss and if set has been prompted or not
		int idn = arrstr[id].peer[i];

		if(array[idn].len==1 && array[idn].set == -1)		{
			for(j=0;j<N;j++)
			{	
				if(array[idn].value[j]!=-1)
				{	array[idn].set = j;
					eliminate(idn,j,array);
				}
			}
		}
	}
}

// eliminating the choice not possible by setting an id
int eliminate(int id,int dig,val arr)
{
	removeP(id,dig,arr);
	setP(id,arr);
}

// correct array bool function
int correct(val array)
{	int i;
	
	for(i=0;i<N*N;i++)
	{
		if(array[i].len==0 && array[i].set== -1)return -1;
	}
	return 0;
}

// id of tuple 
inline int idcr(int i,int j){ return i*N+j; }


// id with a min value of a values struct
int min(val array)
{	int i,j;
	int mini=N+1;
	int save=-1;
	
	for(i=0;i<N*N;i++)
	{
		{	int id=i;
			if(array[id].set==-1)
			{
				if(mini > array[id].len)
				{
					mini=array[id].len;
					save=id;
				}
			}
		}
	}
	return save;
}

// print the value array
void print(val arr,int** out)
{	int i=0;
	//printf("\n");
	for(i=0;i<N*N;i++)
	{
		//printf("%d ",arr[i].set+1);
		out[i/N][i%N]=arr[i].set+1;
		//if(i%N==N-1)printf("\n");
	}
}

// the condition to check if we have solved sudoku
int checksolve(val array)
{	int i;
	
	for(i=0;i<N*N;i++)
	{
		if(array[i].set!=-1 )continue;
		else return 0;
	}
	return 1;
}

// recursive main function which provide values to an id
int search(val array,int c,int** out)
{
	int i;

	if(correct(array)==-1)
	{	
		return -1;
	}

	if(checksolve(array))
	{	printf("solved here :\n");
		print(array,out);
		return 1;
	}
		
	int id=min(array);
	int ans=0;
	c++;

	for(i=0;i<N;i++)
	{

		if(array[id].value[i]!=-1)
		{
			
			val arr = copyV(array); 
			arr[id].set=i;
			eliminate(id,i,arr);
			// create copy of values and set an id	
			ans = search(arr,c,out);

			
			if(ans == 1)return ans;

		}
	}
	
	return ans;
	
}

//chexck whether peer id is repeated or not
int again(int id,int idc, int len)
{	int i=0;
	
	for(i=0;i<len;i++)
	{	if(arrstr[id].peer[i]==idc)return 1;
	}
	return 0;
	
}

void makepeer(int id)
{	int j1,i1,idc,k=0;
	int i=id/N;
	int j=id%N;

	for(j1=0;j1<N;j1++)
	{	if(j1!=j)
		{	idc = idcr(i,j1);
			int check = again(id,idc,k);
			if(check==0){
				arrstr[id].peer[k]=idc;
				k++;
			}
		}
	}
	
	for(i1=0;i1<N;i1++)
	{	if(i1!=i)
		{	idc = idcr(i1,j);
			int check = again(id,idc,k);
			if(check==0)
			{
				arrstr[id].peer[k]=idc;
				k++;
			}
		}
	}
	
	int root = sqrt(N); 
	int sti=(i/root)*root;
	int stj=(j/root)*root;
	int count=0;
	i1=sti;j1=stj;

	while(count<N)
	{	
		idc = idcr(i1,j1);
		int check = again(id,idc,k);
		if(check==0 && idc!= id )
		{	arrstr[id].peer[k]=idc;
			k++;	
		}

		j1++;
		count++;
		if(count%root==0)
		{	i1++;
			j1-=root;
		}
		
	}
	arrstr[id].len=k;
}

void init(val arr,int** out)
{
	int i,j,id;
	
	for(i=0;i<N*N;i++)
	{	
		arr[i].value=(int*)malloc(sizeof(int)*N);
		for(j=0;j<N;j++)arr[i].value[j]=0;
		arr[i].set=-1;
		arr[i].len=N;
	}


	arrstr = (per)malloc(sizeof(struct peers)*N*N);
	for(i=0;i<N*N;i++)
	{
		arrstr[i].peer=(int*)malloc(sizeof(int)*sqrt(N)*N);
		makepeer(i);
	}


	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			
		

			if(out[i][j]!=0)
			{
				id = i*N+j;
				//printf("%d\n",id);
				arr[id].set=out[i][j]-1;
				eliminate(id,out[i][j]-1,arr);

			}
		}
	}
		printf("id :\n");



}

void printP()
{	int i=0,j=0;

	for(i=0;i<N*N;i++)
	{
		for(j=0;j<arrstr[i].len;j++)
		{
			printf("%d ",arrstr[i].peer[j]);
		}
		printf("\n");
	}
}


int** solveSudoku(int ** originalGrid)
{

	val ar;
	ar = (val)malloc(sizeof(valw)*(N*N));
	init(ar,originalGrid);

	printf("start\n");
	search(ar,0,originalGrid);

	return originalGrid;

}

//returns a 2D array from a file containing the Sudoku in space separated format (empty cells are 0)
int ** readInput(char *filename){
	FILE *infile;
	infile = fopen(filename, "r");
	int i, j;

	char dummyline[SIZE+1];
	char dummy;
	int value;
	int **sudokuGrid = (int**)malloc(sizeof(int*)*SIZE);
	for (i=0;i<SIZE;i++){
		sudokuGrid[i] = (int*)malloc(sizeof(int)*SIZE);
		for (j=0;j<SIZE;j++)
			sudokuGrid[i][j] = 0;
	}

	for (i = 0; i < SIZE; i++){
		for (j = 0; j < SIZE; j++){
			/* Checking if number of rows is less than SIZE */
			if (feof(infile)){
				if (i != SIZE){
					printf("The input puzzle has less number of rows than %d. Exiting.\n", SIZE);
					exit(-1);
				}
        		}

        		fscanf(infile, "%d", &value);
        		if(value >= 0 && value <= SIZE)
				sudokuGrid[i][j] = value;
			else{
				printf("%d\n",value);
				printf("The input puzzle is not a grid of numbers (0 <= n <= %d) of size %dx%d. Exiting.\n", SIZE, SIZE, SIZE);
				exit(-1);
			}
		}

		fscanf(infile, "%c", &dummy); /* To remove stray \0 at the end of each line */

		/* Checking if row has more elements than SIZE */
		if (j > SIZE){
			printf("Row %d has more number of elements than %d. Exiting.\n", i+1, SIZE);
			exit(-1);
		}
	}
	return sudokuGrid;
}


/*checks if solution is a valid solution to original 
i.e. all originally filled cells match, and that solution is a valid grid*/
int isValid(int **original, int **solution){
	int valuesSeen[SIZE],i,j,k;
	

	//check all rows
	for (i=0;i<SIZE;i++){
		for (k=0;k<SIZE;k++) valuesSeen[k] = 0;
		for (j=0;j<SIZE;j++){
			if (solution[i][j]==0){ 

					return 0;
			}
			if ((original[i][j])&&(solution[i][j] != original[i][j])){
				return 0;
			}
			int v = solution[i][j];
			if (valuesSeen[v-1]==1){
				
			return 0;
			}
			valuesSeen[v-1] = 1;
		}
	}
	//check all columns
	for (i=0;i<SIZE;i++){
		for (k=0;k<SIZE;k++) valuesSeen[k] = 0;
		for (j=0;j<SIZE;j++){
			int v = solution[j][i];
			if (valuesSeen[v-1]==1){
				return 0;
			}
			valuesSeen[v-1] = 1;
		}
	}

	//check all minigrids
	//check all rows
	for (i=0;i<SIZE;i=i+MINIGRIDSIZE){
		for (j=0;j<SIZE;j=j+MINIGRIDSIZE){
			for (k=0;k<SIZE;k++) valuesSeen[k] = 0;
			int r,c;
			for (r=i;r<i+MINIGRIDSIZE;r++)
				for (c=j;c<j+MINIGRIDSIZE;c++){
					int v = solution[r][c];
					if (valuesSeen[v-1]==1) {
						return 0;
					}
					valuesSeen[v-1] = 1;
				}
		}
	}


	return 1;
}

int main(int argc, char *argv[]){
	if (argc<3){
		printf("Usage: ./sudoku <thread_count> <inputFile>\n");
		exit(0);
	}
	int **originalGrid = readInput(argv[2]);
	int **gridToSolve = readInput(argv[2]);
	int thread_count = atoi(argv[1]);
	if (thread_count<=0){
		printf("Usage: Thread Count should be positive\n");
	}
	omp_set_num_threads(thread_count);
	int i,j;
	printf("************************INPUT GRID***********************\n");
	for (i=0;i<SIZE;i++){
		for (j=0;j<SIZE;j++){
			printf("%d ",originalGrid[i][j]);
		}
		printf("\n");
	}

	double start = omp_get_wtime();
	int **outputGrid = solveSudoku(originalGrid);
	double finish = omp_get_wtime();
	printf("************************OUTPUT GRID***********************\n");
	for (i=0;i<SIZE;i++){
		for (j=0;j<SIZE;j++)
			printf("%d ",outputGrid[i][j]);
		printf("\n");
	}
	printf("*********************************************************\n");
	if (isValid(originalGrid,outputGrid)){
		printf("SOLUTION FOUND\nTIME = %lf\n",(finish-start));
	}
	else{
		printf("NO SOLUTION FOUND\nTIME =%lf\n",(finish-start));
	}
	
}
