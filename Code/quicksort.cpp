#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int n;
int a[1000000],*localArr,*reciveArr;
int nold;
MPI_Status status;
int rank;
void readData(){
    scanf("%d",&n);
    for(int i=0;i<n;i++){
        scanf("%d",&a[i]);
    }
}
void printArray(int *a,int n,int z){

    printf("*****************\n Rank is %d %d The array is %d number\n",rank,z,n);
    for(int i=0;i<n-1;i++){
        printf("%d ->",a[i]);
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
int  main(int argc, char** argv){
    clock_t begin = clock();
    freopen("input.txt","r",stdin);
    //freopen("output.txt","w",stdout);
    double passingTime=0;
    double calTime=0;
    double allTime=0;

    MPI_Init(&argc,&argv);
    int numpro;
    int size;
    int *reArr;

    reciveArr= (int *)malloc(1000000*sizeof(int));
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank ( MPI_COMM_WORLD , & rank );
    if ( rank == 0 ){
        readData();
        int subSize=n/size +((n%size==0)?0:1);
        printf("sub size is %d %d %d",subSize,n,size);
        int *subArr= (int *)malloc(subSize*sizeof(int));
        int c=0;
        //printf("done read data\n");
        for(int i=0;i<size-1;i++){
            int nS=0;
            //printf("split data\n");
            while(nS<subSize){
                subArr[nS]=a[c];
                nS++;
                c++;
            }

            MPI_Send(&nS,1,MPI_INT,i+1,0,MPI_COMM_WORLD);
            MPI_Send(subArr,nS,MPI_INT,i+1,0,MPI_COMM_WORLD);
        }
        localArr= (int *)calloc(subSize,sizeof(int));
        int nS=0;
        printf("c is %d\n",c);
        while(c<n){
                localArr[nS]=a[c];
                nS++;
                c++;
        }
        nold=n;
        n=nS;
        printArray(localArr,n,1);
    } else {
        MPI_Recv(&n,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        localArr= (int *)calloc(n,sizeof(int));
        MPI_Recv(localArr,n,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        printArray(localArr,n,1);
    }
    //printf("Done \n");
    int key;
    //MPI_Barrier(MPI_COMM_WORLD);
    if(rank==0){
        srand(time(NULL));
        int r = rand()%nold;
        key=a[r];
    }
        MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //} else {
      //  printf("rank is %d start recive key",rank);
        //MPI_Recv(&key,1,MPI_INT,0,1,MPI_COMM_WORLD,&status);
    //printf("rank is %d key is %d ",rank,key);
    //}
    //printf("Done2 \n");
    int step=size/2;
    //for(step>0;step=step>>1){
        int e=0;
        //printf("local %d\n",rank );
        for(int i=0;i<n;i++){
            //printf("local %d\n",rank);
            if(localArr[i]<key){
                swap(&localArr[e],&localArr[i]);
                e++;
            }
        }
        //printArray(localArr,e,"partion");
        MPI_Request req;
        if( (rank / step) % 2 == 0){
            int nSend=n-e;
            MPI_Send(&nSend,1,MPI_INT,rank+step,step,MPI_COMM_WORLD);
            int nRe;
            MPI_Recv(&nRe,1,MPI_INT,rank+step,step,MPI_COMM_WORLD,&status);
            printf(" resive %d from %d \n",nRe,rank+step);

            if(nSend>0){
                //printArray(localArr,n,"send to rank+step");
                int  *subArr= (int *)calloc(nSend,sizeof(int));
                for(int i=0;i<nSend;i++) localArr[i]=subArr[i];
                MPI_Isend(subArr,nSend,MPI_INT,rank+step,step,MPI_COMM_WORLD,&req);
            }
            //printf("Rank %d send done\n",rank);
            reArr= (int *)calloc(100,sizeof(int));
            //printf("malloc  %d rank %d\n",nRe+e,rank);
            int x=nRe;
            if(nRe>0){
                printf("Resive %d in rank %d \n",nRe,rank);
                MPI_Irecv(&reArr,x,MPI_INT,rank+step,step,MPI_COMM_WORLD,&req);
                MPI_Wait(&req, &status);
                int number_amount;
                MPI_Get_count(&status, MPI_INT, &number_amount);

                // Print off the amount of numbers, and also print additional
                // information in the status object
                printf("1 received %d numbers from 0. Message source = %d, "
           "tag = %d\n",
           number_amount, status.MPI_SOURCE, status.MPI_TAG);
                printf(" resive2 %d in rank %d \n",nRe,rank);
                printArray(reArr,x,2);
            }
            //printf("Rank %d resive done in parter %d \n",rank,step);
            for(int i=0;i<e;i++){
                //printf("reArr %d ",reArr[nRe-1]);
                //printf("local[%d]= %d in rank %d\n",i,localArr[i],rank);
                reArr[nRe+i]=localArr[i];
            }
            printf("Rank %d make new done in partner %d\n ",rank,step);
            free((int *) localArr);
            localArr=reArr;
            n=nRe+e;
            printArray(localArr,n,3);
        } else {
            int nSend=e;
            MPI_Send(&nSend,1,MPI_INT,rank-step,step,MPI_COMM_WORLD);
            int nRe;
            MPI_Recv(&nRe,1,MPI_INT,rank-step,step,MPI_COMM_WORLD,&status);
            printf("Resive %d from %d \n",nRe,rank-step);
            if(nSend>0){
                //printArray(localArr,n,"send to rank-step");
                int  *subArr= (int *)calloc(nSend,sizeof(int));
                for(int i=0;i<nSend;i++) localArr[i]=subArr[i];
                MPI_Isend(&localArr,nSend,MPI_INT,rank-step,step,MPI_COMM_WORLD,&req);
                //MPI_Wait(&req);
            }
            printf("Rank %d send done\n",rank);
            reArr= (int *)calloc(100,sizeof(int));
            int reArr[10000];
            //printf("malloc  %d rank%d\n",nRe+n-e,rank);
            int x=nRe;
            if(nRe>0){
                printf(" resive %d in rank %d \n",nRe,rank);
                MPI_Irecv(&reArr,x,MPI_INT,rank-step,step,MPI_COMM_WORLD,&req);
                int number_amount;
                MPI_Wait(&req, &status);
                MPI_Get_count(&status, MPI_INT, &number_amount);

                // Print off the amount of numbers, and also print additional
                // information in the status object
                printf("1 received %d numbers from 0. Message source = %d, "
           "tag = %d\n",
           number_amount, status.MPI_SOURCE, status.MPI_TAG);
                printf(" resive2 %d in rank %d \n",nRe,rank);
                printArray(reArr,x,2);
            }
            //printf("Rank %d resive done in parter %d \n",rank,step);
            for(int i=0;i<n-e;i++){
                //printf("reArr %d ",reArr[nRe-1]);
                //printf("local[%d]= %d in rank %d",i,localArr[i],rank);
                reArr[nRe+i]=localArr[e+i];
            }
            printf("Rank %d make new done in partner %d\n ",rank,step);
            free((int *) localArr);
            localArr=reArr;
            n=nRe+n-e;
            printArray(localArr,n,3);
        }
    //}
    MPI_Finalize ();
    fclose(stdin);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f ",time_spent);
}

