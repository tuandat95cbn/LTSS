#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int n;
int a[1000000],*localArr,*reciveArr;
int nold;
MPI_Status status;
int oddArr[1000000],evenArr[1000000];
void readData(){
    scanf("%d",&n);
    for(int i=0;i<n;i++){
        scanf("%d",&a[i]);
    }
}
void printArray(int *a,int n,char* s){

    printf("*****************\n %s The array is %d number\n",s,n);
    for(int i=0;i<n;i++){
        printf("%d ",a[i]);
    }
    printf("\n****************");
}
void swap(int *x,int *y){
    int tmp=*x;
    *x=*y;
    *y=tmp;
}
void sortEven(int *a,int n){
    int j=0;
    while(1){
        int xd=0;
        int xd2=0;
        for(int i=0;i<n-1;i+=2){
            if(a[i]>a[i+1]){
                swap(&(a[i]),&(a[i+1]));
                xd=1;
            }
        }
        for(int i=1;i<n-1;i+=2){
            if(a[i]>a[i+1]){
                swap(&(a[i]),&(a[i+1]));
                xd2=1;
            }
        }

        if((xd==0)&&(xd2==0)) break;
        j++;
    }
}
int *mergeArr(int *a,int *b,int m,int n){
    int i=0;
    int j=0;
    int c=0;
    int *res=(int *)malloc(nold*sizeof(int));;
    while(1){
        if(i>=m){
            while(j<n) {
            res[c]=b[j];
            c++;
            j++;
            }

            break;
        }
        if(j>=n){
            while(i<m) {
            res[c]=a[i];
            c++;
            i++;
            }
            break;
        }
        if(a[i]<b[j]) {
            res[c]=a[i];
            c++;
            i++;
        } else {
            res[c]=b[j];
            c++;
            j++;
        }

    }
    return res;
}
int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
int  main(int argc, char** argv){
    clock_t begin = clock();
    freopen("input.txt","r",stdin);
    //freopen("output.txt","w",stdout);
    double passingTime=0;
    double calTime=0;
    double allTime=0;

    MPI_Init(&argc,&argv);
    int numpro,rank;
    localArr= (int *)malloc(1000000*sizeof(int));
    reciveArr= (int *)malloc(1000000*sizeof(int));
    MPI_Comm_rank ( MPI_COMM_WORLD , & rank );
    if ( rank == 0 ){
        readData();
        for(int i=0;i<n-1;i+=2){
            oddArr[i/2]=a[i];
            evenArr[i/2]=a[i+1];
        }
        nold=n;
        n=n/2;

        MPI_Send(&n,1,MPI_INT,1,0,MPI_COMM_WORLD);
        MPI_Send(evenArr,n,MPI_INT,1,0,MPI_COMM_WORLD);
        localArr=oddArr;
        if(nold %2==1){
            n=n+1;
            localArr[n-1]=a[nold-1];
        }
    } else {
        MPI_Recv(&n,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        //printf("%d ",n);
        MPI_Recv(localArr,n,MPI_INT,0,0,MPI_COMM_WORLD,&status);

        //char mess[20];
        //sprintf ( mess , " %d " , rank );
    }
    //printArray(localArr,n,"input is: \n");
    sortEven(localArr,n);
    //printArray(localArr,n,"sorted done \n");
    if(rank==1) {
        MPI_Send(localArr,n,MPI_INT,0,0,MPI_COMM_WORLD);
    } else {
        MPI_Recv(reciveArr,n,MPI_INT,1,0,MPI_COMM_WORLD,&status);
        int *b=mergeArr(localArr,reciveArr,(nold%2==0)?nold/2:nold/2+1,n);
        /*printArray(b,nold,"done");
        qsort (a, nold, sizeof(int), compare);
        int u=0;
        for (int i=0; i<nold; i++)
            if(a[i]!=b[i]) u++;
        printArray(a,nold,"qsort");
        printf("%d ",u);*/
    }
    MPI_Finalize ();
    //free(localArr);
    //free(reciveArr);
    fclose(stdin);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f ",time_spent);
}
