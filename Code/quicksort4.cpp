#include    <stdio.h>
#include    <mpi.h>
#include    <stdlib.h>
#include    <time.h>
int rank;
int size;
int n;
int *a;
int nold;
int *localArr;
void readData(){
    scanf("%d",&n);
    a=(int*) malloc(n*sizeof(int));
    for(int i=0;i<n;i++){
        scanf("%d",&a[i]);
    }
}
void printArray(int *a,int n,int z){

    printf("*****************\n Rank is %d %d The array is %d number\n",rank,z,n);
    for(int i=0;i<n-1;i++){
        printf("(%d)->",a[i]);
    }
    printf("%d end",a[n-1]);
    printf("\n****************\n");
}
void swap(int *x,int *y){
    int tmp=*x;
    *x=*y;
    *y=tmp;
}
int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}
int main(int argc, char** argv){
    clock_t begin = clock();

    freopen(argv[1],"r",stdin);
    char cRank[10];
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    //sprintf (cRank, "%d_stdout.txt", rank);
    //freopen(cRank,"w",stdout);
    if(rank ==0){
        readData();
        nold=n;
        n=n/size;
    }
    double pstime=0;
    double timestart=MPI_Wtime();
    double timeend =MPI_Wtime();
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
    localArr=(int *) malloc(n*sizeof(int));
    MPI_Scatter(a,n,MPI_INT,localArr,n,MPI_INT,0,MPI_COMM_WORLD);
    int key;
    //MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Request request;
    MPI_Status status;
    for(int step=size/2;step>0;step=step>>1){

        if(rank%(step*2)==0 ){
            srand(time(NULL));
            int r = rand()%n;
            key=localArr[r];
            timeend =MPI_Wtime();
            for(int i=1;i<2*step;i++)
                MPI_Isend(&key,1,MPI_INT,rank+i,2,MPI_COMM_WORLD,&request);
            timeend =MPI_Wtime();
        } else {

            MPI_Recv(&key,1,MPI_INT,(rank/(step*2))*(2*step),2,MPI_COMM_WORLD,&status);
        }
        //printf("key is %d\n",key);
        int e=0;
        for(int i=0;i<n;i++){
            if(localArr[i]<key){
                swap(&localArr[e],&localArr[i]);
                e++;
            }
        }
        int nSend;
        int nRev;

        if((rank/step)%2==0 ){
            nSend=n-e;
            MPI_Isend(&nSend,1,MPI_INT,rank+step,0,MPI_COMM_WORLD,&request);
            MPI_Recv(&nRev,1,MPI_INT,rank+step,0,MPI_COMM_WORLD,&status);
        }else{
            nSend=e;
            MPI_Isend(&nSend,1,MPI_INT,rank-step,0,MPI_COMM_WORLD,&request);
            MPI_Recv(&nRev,1,MPI_INT,rank-step,0,MPI_COMM_WORLD,&status);
        }
        //printf("rank %d send %d rev %d\n",rank,nSend,nRev);
        //printArray(localArr,n,0);
        int *revArr= (int *) malloc((n-nSend+nRev)*sizeof(int));


        if((rank/step)%2==0 ){
            MPI_Isend(localArr+e,nSend,MPI_INT,rank+step,1,MPI_COMM_WORLD,&request);
            MPI_Recv(revArr,nRev,MPI_INT,rank+step,1,MPI_COMM_WORLD,&status);
            //MPI_Wait(&request, &status);
            for(int i=0;i<n-nSend;i++){
                revArr[nRev+i]=localArr[i];
            }
            //printArray(revArr,nRev,3);
        } else {
            MPI_Isend(localArr,e,MPI_INT,rank-step,1,MPI_COMM_WORLD,&request);
            MPI_Recv(revArr,nRev,MPI_INT,rank-step,1,MPI_COMM_WORLD,&status);
            //MPI_Wait(&request, &status);
            for(int i=0;i<n-nSend;i++){
                revArr[nRev+i]=localArr[e+i];
            }
            //printArray(revArr,nRev,3);
        }

        //free(localArr);

        localArr=revArr;

        n=(n-nSend)+nRev;

        //printf("N is %d\n",n);
        //printArray(localArr,n,3);

    }

    qsort (localArr, n, sizeof(int), compare);

    if(rank!=0){

        MPI_Isend(&n,1,MPI_INT,0,4,MPI_COMM_WORLD,&request);
        MPI_Isend(localArr,n,MPI_INT,0,3,MPI_COMM_WORLD,&request);

    } else {
        printf("completed %d 1\n",nold);
        int *Arr=(int *) malloc((nold)*sizeof(int));

        for(int i=0;i<n;i++){
                Arr[i]=localArr[i];
        }

        int t=n;
        for(int i=1;i<size;i++){
            int rev;

            MPI_Recv(&rev,1,MPI_INT,i,4,MPI_COMM_WORLD,&status);
            int *revA=(int *) malloc((rev)*sizeof(int));
            MPI_Status status2;
            MPI_Recv(revA,rev,MPI_INT,i,3,MPI_COMM_WORLD,&status2);
            //printArray(revA,rev,5);
            for(int i=0;i<rev;i++){
                Arr[t+i]=revA[i];
            }
            t=t+rev;
            printf("completed2\n");
            free(revA);
        }
        //printArray(Arr,nold,5);
    }

    timeend =MPI_Wtime();
    if(rank==0){
            printf("Time %f",timeend-timestart);
    }
    MPI_Finalize();
    fclose(stdin);

    //clock_t end = clock();
    //double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    //printf("%f ",time_spent);
    //printf("read Done");
    //fclose(stdout);

}
