#include    <stdio.h>
#include    <mpi.h>
#include    <stdlib.h>
#include    <time.h>
void readData(){
    scanf("%d",&n);
    a=(int*) malloc(n*sizeof(int));
    for(int i=0;i<n;i++){
        scanf("%d",&a[i]);
    }

}
int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
void printArray(int *a,int n,int z){

    printf("*****************\n Rank is %d %d The array is %d number\n",rank,z,n);
    for(int i=0;i<n-1;i++){
        printf("(%d)->",a[i]);
    }
    printf("%d end",a[n-1]);
    printf("\n****************\n");
}
int maim(int argc,char ** argv){
        clock_t begin = clock();

    freopen(argv[1],"r",stdin);
    char cRank[10];
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    sprintf (cRank, "%d_stdout.txt", rank);
    freopen(cRank,"w",stdout);
    if(rank ==0){
        readData();
        nold=n;
        n=n/size;
    }

    double timestart=MPI_Wtime();
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
    localArr=(int *) malloc(n*sizeof(int));
    MPI_Scatter(a,n,MPI_INT,localArr,n,MPI_INT,0,MPI_COMM_WORLD);
    qsort (localArr, n, sizeof(int), compare);
    int sampl[size-1];
    int it=0;
    for(int i=0;i<size-1;i++){
        sampl[i]=localArr[it+=(n/size)];
    }
    printArray(sampl,size-1,1);
}
