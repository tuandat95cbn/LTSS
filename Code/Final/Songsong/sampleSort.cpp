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
MPI_Request request;
MPI_Status status;
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
int main ( int argc , char * argv [] )
{

    freopen(argv[1],"r",stdin);
    char cRank[10];
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    sprintf (cRank, "%d_stdout.txt", rank);
    //freopen(cRank,"w",stdout);
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
    int it=n/(size-1)-1;
    for(int i=0;i<size-1;i++){
        sampl[i]=localArr[it];
        it+=(n/(size-1));
        //printf("%d \n",it);
    }
    int *sampleFull;
    if(rank==0) sampleFull=(int *) malloc(size*(size-1)*sizeof(int));
    fclose(stdin);
    MPI_Gather(sampl,size-1,MPI_INT,sampleFull,size-1,MPI_INT,0,MPI_COMM_WORLD);
    if(rank==0)qsort (sampleFull, size*(size-1), sizeof(int), compare);
    int bucket[size];
    if(rank==0){
        it=size-1;
        for(int i=0;i<size-1;i++){
            bucket[i]=sampleFull[it];
            it+=size;
            //printf("%d \n",it);
        }
    }
    //free(sampl);
    //if(rank==0)
    //free(sampleFull);

    MPI_Bcast(&bucket,size-1,MPI_INT,0,MPI_COMM_WORLD);
    //if(rank==0) printArray(sampleFull,size*(size-1),2);
    bucket[size-1]=100000000;

    //fclose(stdout);

        it=0;
    int o=0;
    int *seft;
    int nSeft=0;
    for(int i=0;i<size;i++){

        while ((localArr[it]<bucket[i])&&(it<n)) it++;
        if(rank==i){
            nSeft=it-o;
            seft=(int *) malloc((it-o)*sizeof(int));
            for(int j=o;j<it;j++)
                seft[j-o]=localArr[j];
        } else{
        int nSend=it-o;
        MPI_Isend(&nSend,1,MPI_INT,i,0,MPI_COMM_WORLD,&request);
        if(nSend!=0)
            MPI_Issend(localArr+o,nSend,MPI_INT,i,2,MPI_COMM_WORLD,&request);
        //printArray(localArr+o,nSend,5);

        }
        o=it;
    }

    int *res=(int *) malloc((10000000)*sizeof(int));
    int nRev=nSeft;
    for(int i=0;i<nSeft;i++){
        res[i]=seft[i];
    }
    for(int i=0;i<size;i++)
        if(rank!=i){
            int nV;
            MPI_Recv(&nV,1,MPI_INT,i,0,MPI_COMM_WORLD,&status);
            //printf("rank %d rev %d",rank,nV);
            int *reArr=(int *) malloc(nV*sizeof(int));
            if(nV!=0)
            MPI_Recv(reArr,nV,MPI_INT,i,2,MPI_COMM_WORLD,&status);
            for(int j=0;j<nV;j++){
                res[nRev+j]=reArr[j];
            }

            nRev+=nV;
            free(reArr);
        }

    /*if(rank==0)
    MPI_Issend(&localArr,1,MPI_INT,1,2,MPI_COMM_WORLD,&request);
    int *reArr=(int *) malloc(2*sizeof(int));
    if(rank==1)
    MPI_Recv(reArr,2,MPI_INT,0,2,MPI_COMM_WORLD,&status);
    printArray(reArr,2,6);*/
    qsort (res, nRev, sizeof(int), compare);
    if(rank!=0){

        MPI_Isend(&nRev,1,MPI_INT,0,4,MPI_COMM_WORLD,&request);
        MPI_Isend(res,nRev,MPI_INT,0,3,MPI_COMM_WORLD,&request);

    } else {
        int *Arr=(int *) malloc((nold)*sizeof(int));

        for(int i=0;i<nRev;i++){
                Arr[i]=res[i];
        }
        int t=nRev;
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
            free(revA);
        }

        //printArray(Arr,nold,5);
    }
    double timeend =MPI_Wtime();
    if(rank==0){
            printf("Time %f",timeend-timestart);
    }

    MPI_Finalize();
    //fclose(stdout);

}
