/*2013CS10209_2013CS10243*/
#include <stdio.h>
#include <stdlib.h>
#include "sudoku.h"
#include <string.h>
#include <omp.h>

#define N SIZE

int EMP;

// value of the struct array
struct values{
  int* value;
  int len;
  int set;
  int interset;
  
};




int calans=0;

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


typedef struct {
   val* list;
   int list_sz;
   int list_alloc;
   int top;
}  stack_struct;

typedef stack_struct* my_stack_t;

//---------------------------------------------------------------------------------------
/*------------------------------------------------------------------
 * Stack function
*/ 
int Empty_stack(my_stack_t avail)
{
   if(avail->list_sz == 0 )return 1;
   return 0;

}

val Pop(my_stack_t avail)
{

   val freed = avail->list[avail->top];
   avail->top--;
   avail->list_sz--;
   return freed;

}

void Push(my_stack_t avail,val array)
{  
   
   avail->top++;
   avail->list[avail->top]=array;
   avail->list_sz++;

}



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
    { array[arrstr[id].peer[i]].value[digit] = -1;
      array[arrstr[id].peer[i]].len--;
    } 
  }
  return 0;
  
}

// set the array if the len of choice is one
int setP(int id,val array)
{ int i,j;
  int len = arrstr[id].len;

  for(i=0;i<len;i++)
  {
     // check the  peer length of value poss and if set has been prompted or not
    int idn = arrstr[id].peer[i];

    if(array[idn].len==1 && array[idn].set == -1)   {
      for(j=0;j<N;j++)
      { 
        if(array[idn].value[j]!=-1)
        { array[idn].set = j;
          eliminate2(idn,j,array);
        }
      }
    }
  }
}

int lonerangeP(int id,val array)
{ int dig[N];
  int leng[N],i,j,idc;

  for(i=0;i<N;i++)
  {
    dig[i]=-1;
    leng[i]=0;
  }
  //printf("aniket1\n");
  //rows
  int i1=id/N,j1=id%N;;
  for(i=0;i<N;i++)
  { if(i!=j1)
    { idc=idcr(i1,i);
      if(array[idc].set==-1)
      { 
        for(j=0;j<N;j++)
        {
          if(array[idc].value[j]!=-1)
          { if(leng[j]==0)
            { dig[j]=idc;
              leng[j]++;
            }
            else leng[j]++;
          }
        }
      }
      else if(array[idc].set!=-1)
      {
        leng[array[idc].set]=N+1;
      }
    }
  }

  for(i=0;i<N;i++)
  {
    if(leng[i]==1)
    {
      array[dig[i]].set=i;
      
      eliminate(dig[i],i,array);
    }
  }

  for(i=0;i<N;i++)
  { dig[i]=-1;
    leng[i]=0;
  }
  //cols
  for(i=0;i<N;i++)
  { if(i!=i1)
    { idc=idcr(i,j1);
      if(array[idc].set==-1)
      { 
        for(j=0;j<N;j++)
        {
          if(array[idc].value[j]!=-1)
          { if(leng[j]==0)
            { dig[j]=idc;
              leng[j]++;
            }
            else leng[j]++;
          }
        }
      }
      else if(array[idc].set!=-1)
      {
        leng[array[idc].set]=N+1;
      }
    }
  }
  for(i=0;i<N;i++)
  {
    if(leng[i]==1)
    {
      array[dig[i]].set=i;
      eliminate(dig[i],i,array);
    }
  }
  for(i=0;i<N;i++)
  { dig[i]=-1;
    leng[i]=0;
  }
  //squ
  int root=sqrt(N);
  int sti=(i1/root)*root;int stj=(j1/root)*root,count=0;
  i1=sti;j1=stj;
  while(count<N)
  { idc=idcr(i1,j1);
    if(idc!=id)
    { if(array[idc].set==-1)
      { for(j=0;j<N;j++)
        {
          if(array[idc].value[j]!=-1)
          {
            if(leng[j]==0)
            { leng[j]++;
              dig[j]=idc;
            }
            else leng[j]++;
          }
        }
      }
      else
      {
        leng[array[idc].set]=N+1;
      }
    }
    j1++;count++;
    if(count%root==0)
    { j1-=root;
      i1++;
    }
  }

  for(i=0;i<N;i++)
  {
    if(leng[i]==1)
    { array[dig[i]].set=i;
      eliminate(dig[i],i,array);
    }
  }
}

int lonerange(val arr)
{   
  int dig[N],len[N],i,j,k,id;



  for(i=0;i<N;i++)
  { 
    for(j=0;j<N;j++)
    { dig[j]=0;len[j]=0;  
    }
    for(j=0;j<N;j++)
    {
      id=idcr(i,j);
      if(arr[id].set==-1)
      { for(k=0;k<N;k++)
        { if(arr[id].value[k]!=-1)
          { if(len[k]==0)
            { dig[k]=id;
              len[k]++;
            }
            else len[k]++;

          }
        }
      }
      else
      {
        len[arr[id].set]=N+1;
      }
    }
    for(j=0;j<N;j++)
    {
      if(len[j]==1)
      { arr[dig[j]].set=j;
        eliminate2(dig[j],j,arr);

      }
    }
  }
  //rows

  for(i=0;i<N;i++)
  {
    for(j=0;j<N;j++)
    { dig[j]=0;len[j]=0;  
    }
    for(j=0;j<N;j++)
    {
      id=idcr(j,i);
      if(arr[id].set==-1)
      { for(k=0;k<N;k++)
        { if(arr[id].value[k]!=-1)
          { if(len[k]==0)
            { dig[k]=id;
              len[k]++;
            }
            else len[k]++;

          }
        }
      }
      else
      {
        len[arr[id].set]=N+1;
      }
    }
    for(j=0;j<N;j++)
    {
      if(len[j]==1)
      { arr[dig[j]].set=j;
        eliminate2(dig[j],j,arr);

      }
    }
  }
  //columns
  int root=sqrt(N),i1,j1,idc;
  for(i=0;i<N;i++)
  {
    for(j=0;j<N;j++)
    { dig[j]=0;len[j]=0;  
    }
    int sti=(i/root)*root,stj=(i%root)*root,count=0;
    i1=sti;j1=stj;
    while(count<N)
    { idc=idcr(i1,j1);
      
        if(arr[idc].set==-1)
        { for(j=0;j<N;j++)
          {
            if(arr[idc].value[j]!=-1)
            {
              if(len[j]==0)
              { len[j]++;
                dig[j]=idc;
              }
              else len[j]++;
            }
          }
        }
        else
        {
          len[arr[idc].set]=N+1;
        }
      
      j1++;count++;
      if(count%root==0)
      { j1-=root;
        i1++;
      }
    }
    for(j=0;j<N;j++)
    {
      if(len[j]==1)
      { arr[dig[j]].set=j;
        eliminate2(dig[j],j,arr);

      }
    }

  }
  //squares


}

// eliminating the choice not possible by setting an id
int eliminate(int id,int dig,val arr)
{
  removeP(id,dig,arr);
  setP(id,arr);
  //lonerangeP(id,arr);
 // lonerange(arr);
}


int eliminate2(int id,int dig,val arr)
{
  removeP(id,dig,arr);
  setP(id,arr);
  //lonerangeP(id,arr);

}

// correct array bool function
int correct(val array)
{ int i;
  
  for(i=0;i<N*N;i++)
  {
    if(array[i].len==0 && array[i].set== -1)return -1;
    if(array[i].set!=-1)
    { 
    }
  }
  return 0;
}

// id of tuple 
inline int idcr(int i,int j){ return i*N+j; }


// id with a min value of a values struct
int min(val array)
{ int i,j;
  int mini=N+1;
  int save=-1;
  
  for(i=0;i<N*N;i++)
  {
    { int id=i;
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
{ int i=0;
  //printf("\n");
  #pragma omp critical
  { //  printf("only one thread and then whole func exit\n");
    for(i=0;i<N*N;i++)
      {
      out[i/N][i%N]=arr[i].set+1;
      }
      calans=1;
  }

}

// the condition to check if we have solved sudoku
int checksolve(val array)
{ int i;
  
  for(i=0;i<N*N;i++)
  {
    if(array[i].set!=-1 )continue;
    else return 0;
  }
  return 1;
}

//intialise val values
void initval(val arr)
{ int i;

  for(i=0;i<N*N;i++)
  {
    arr[i].value = (int*)malloc(sizeof(int)*N);
    arr[i].set = -1;
    arr[i].len = N;
  }
}



// save copy
void savecopy(val src, val dest)
{ int i,j;

  for(i=0;i<N*N;i++)
  {
    for(j=0;j<N;j++)dest[i].value[j]=src[i].value[j];
    dest[i].set = src[i].set;
    dest[i].len = src[i].len;
      
  }
}

int idgl = -1;
int digg = -1;
// recursive main function which provide values to an id
int search(val array,int** out)
{
  int i;

  if(calans)
    return calans;

  


  if(correct(array)==-1)
  { 
    return 0;
  }


  lonerange(array);

  if(checksolve(array))
  { //printf("solved here :\n");
    print(array,out);
    return 1;
  }
    

  int id=min(array);
  int ans=0;

  // create a new val
  val newval;
  newval = (val)malloc(sizeof(valw)*(N*N));
  initval(newval);

  for(i=0;i<N;i++)
  {

    if( array[id].value[i]!=-1)
    {
      
      savecopy(array,newval);
      //val arr = copyV(array); 
      //arr[id].set=i;
      //eliminate(id,i,arr);
      // create copy of values and set an id  
      
      array[id].set=i;
      eliminate(id,i,array);
      ans = search(array,out);
      if(ans == 1)return ans;

      savecopy(newval,array);
  
    }
  }

  for(i=0;i<N*N;i++)
  {
    free(newval[i].value);
  }
  free(newval);

  return ans;
  
}

int len=0;
int index1=-1;
int digit1=-1;


int dfs_allot(val array,int k,my_stack_t stack,int * sidep,int** out)
{
  int i;

  if(correct(array)==-1)
  {
    return 0;
  }

  //lonerange(array);

  if(k==EMP)
  { 
    if(correct(array)!=-1)
    { 
      Push(stack,copyV(array));
      len++;

    }

    return 0;
  }
  
  if(checksolve(array))
  {
    

    printf("hii\n");
    print(array,out);
    return 1;
  }


  int id=sidep[k];
  int ans=0;
  
  // create a new val
  val newval;
  newval = (val)malloc(sizeof(valw)*(N*N));
  initval(newval);

  for(i=0;i<N;i++)
  {

    if(array[id].value[i]!=-1)
    {
      
      savecopy(array,newval);
//      newval[id].set=i;
//      index1=id;
//      digit1=i;
      newval[id].set=i;
      eliminate(id,i,newval);
      ans = dfs_allot(newval,k+1,stack,sidep,out);
      if(ans==1)return 1;
      

      

    }
  }

  free(newval);


  return 0;
  
}

// num of possible configs
int poss(val arr)
{
}

struct sortid{
  int id;
  int len;
};

//put them in array
void createposs(val arr,int* savep)
{ 
  int len=0,i,k=0,j;
  int mark[N*N];
  for(i=0;i<N*N;i++)mark[i]=0;

  for(i=0;i<EMP;i++)
  { int mini=N+1;
    int minid=-1;
    for(j=0;j<N*N;j++)
    {
      if(arr[j].set==-1 && mark[j]==0)
      { 
        if(mini > arr[j].len)
        {
          mini=arr[j].len; minid =j;
        }
      }
    }
    if(minid != -1)
    {
      savep[i]=minid;
      mark[minid]=1;
    }
  }

}

// free the allocated memory
void freeval(val arr)
{ int i=0;

  for(i=0;i<N*N;i++)
  {
    free(arr[i].value);
  }
  free(arr);
}



int distribute(val arr,int** out)
{ 
  if(N>=49)EMP=3;   
  else if(N>=36)EMP=4;
  else if(N>=25)EMP=4;
  else if(N>=16)EMP=7;
  else EMP=6;

  int* sidep = (int*)malloc(sizeof(int)*EMP);


  createposs(arr,sidep);

  my_stack_t stack = malloc(sizeof(my_stack_t));
  
  stack->list = (val*)malloc(sizeof(val)*15000);
  stack->top=-1;
  stack->list_sz=0;
  int ansd = dfs_allot(copyV(arr),0,stack,sidep,out);
  
  if(ansd==1)return 1;

  
  int i=0,ans=0,ken=1;
int maxt = (10>omp_get_num_threads())?10:omp_get_num_threads();
maxt=4;
printf("%d max \n",maxt);
  #pragma omp parallel for shared(calans) num_threads(maxt)
  for(i=0;i<len;i++)
  {
    if(calans==0)
    {
      calans |= search(stack->list[i],out);
      
    }

    else if(calans==1)
    {
      continue;
    }
  }

  return calans;

}

//check whether peer id is repeated or not
int again(int id,int idc, int len)
{ int i=0;
  
  for(i=0;i<len;i++)
  { if(arrstr[id].peer[i]==idc)return 1;
  }
  return 0;
  
}

void makepeer(int id)
{ int j1,i1,idc,k=0;
  int i=id/N;
  int j=id%N;
  //corresponding column
  for(j1=0;j1<N;j1++)
  { if(j1!=j)
    { idc = idcr(i,j1);
      int check = again(id,idc,k);
      if(check==0){
        arrstr[id].peer[k]=idc;
        k++;
      }
    }
  }
  //corresponding
  for(i1=0;i1<N;i1++)
  { if(i1!=i)
    { idc = idcr(i1,j);
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
    { arrstr[id].peer[k]=idc;
      k++;  
    }

    j1++;
    count++;
    if(count%root==0)
    { i1++;
      j1-=root;
    }
    
  }
  arrstr[id].len=k;
}

int init(val arr,int** out)
{
  int i,j,id;
  
  for(i=0;i<N*N;i++)
  { 
    arr[i].value=(int*)malloc(sizeof(int)*N);
    for(j=0;j<N;j++)arr[i].value[j]=0;
    arr[i].set=-1;
    arr[i].len=N;
  }

  double fornitesh_start=omp_get_wtime();


  arrstr = (per)malloc(sizeof(struct peers)*N*N);
  for(i=0;i<N*N;i++)
  {
    arrstr[i].peer=(int*)malloc(sizeof(int)*3*N);
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
  lonerange(arr);
  
//  printf("nitesh : %lf\n",fornitesh_end-fornitesh_start);

  if(correct(arr)==-1)
  {
    printf("wrong in init:\n");
    return -1;
  }

  if(checksolve(arr))
  {
    printf("solve here intially: \n");
    print(arr,out);
    double end = omp_get_wtime();
  printf("id :%lf\n",end-fornitesh_start);
  
    return 1;
  }



}

void printP()
{ int i=0,j=0;

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
double st=omp_get_wtime();
  val ar;
  ar = (val)malloc(sizeof(valw)*(N*N));
  int ansc = init(ar,originalGrid);

  if(ansc==-1)return originalGrid;
double end=omp_get_wtime();

  if(ansc==1)return originalGrid;

  distribute(ar,originalGrid);

  printf("start\n");
//  search(ar,originalGrid);


  return originalGrid;

}