#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define RANK_MASTER 0

FILE* inp;
FILE* out;

void swap(int a[],int x,int y) {
    int tmp = a[x];
    a[x] = a[y];
    a[y] = tmp;
}


int partition(int a[],int L,int R,int indexPivot) {
    int pivot = a[indexPivot];
    swap(a,indexPivot,R);
    int storeindex = L;
    for(int i = L;i <= R - 1;i++) {
        if(a[i] < pivot) {
            swap(a,storeindex,i);
            storeindex++;
        }
    }
    swap(a,storeindex,R);
    return storeindex;
}


void quick_sort(int a[],int L,int R) {
    if(L < R) {
        int index = (L + R)/2;
        index = partition(a,L,R,index);
        if(L < index) {
            quick_sort(a,L,index - 1);
        }
        if(index < R) {
            quick_sort(a,index + 1,R);
        }
    }
}

void MergeSplitLow(int local_A[], int temp_B[], int temp_C[],
        int local_n) {
   int ai, bi, ci;

   ai = 0;
   bi = 0;
   ci = 0;
   while (ci < local_n) {
      if (local_A[ai] <= temp_B[bi]) {
         temp_C[ci] = local_A[ai];
         ci++; ai++;
      } else {
         temp_C[ci] = temp_B[bi];
         ci++; bi++;
      }
   }

   memcpy(local_A, temp_C, local_n*sizeof(int));
}

void MergeSplitHigh(int local_A[], int temp_B[], int temp_C[],
        int local_n) {
   int ai, bi, ci;

   ai = local_n-1;
   bi = local_n-1;
   ci = local_n-1;
   while (ci >= 0) {
      if (local_A[ai] >= temp_B[bi]) {
         temp_C[ci] = local_A[ai];
         ci--; ai--;
      } else {
         temp_C[ci] = temp_B[bi];
         ci--; bi--;
      }
   }

   memcpy(local_A, temp_C, local_n*sizeof(int));
}

void OddEvenIter(int locale_A[],int temp_B[],int temp_C[], int locale_n,int phase,
        int even_partner, int odd_partner, int rank,int p,double &timeS,double &timeR) {

    MPI_Status status;
    if(phase % 2 == 0) { // phase even, odd process  -- rank - 1
        if(even_partner >= 0) {
            double time1 = MPI_Wtime();
            MPI_Sendrecv(locale_A,locale_n,MPI_INT,even_partner, 0 , temp_B,
                locale_n,MPI_INT,even_partner, 0,MPI_COMM_WORLD,&status);
            double time2 = MPI_Wtime();
            timeS += time2 - time1;
            if(rank % 2 !=0) {
                MergeSplitHigh(locale_A,temp_B,temp_C,locale_n);
            }else {
                MergeSplitLow(locale_A,temp_B,temp_C,locale_n);
            }
        }

    }else {
        if(odd_partner >= 0) {
            double time1 = MPI_Wtime();
            MPI_Sendrecv(locale_A,locale_n,MPI_INT,odd_partner, 0 , temp_B,
                locale_n,MPI_INT,odd_partner, 0,MPI_COMM_WORLD,&status);
            double time2 = MPI_Wtime();
            timeS += time2 - time1;
            if(rank % 2 !=0) {
                MergeSplitLow(locale_A,temp_B,temp_C,locale_n);
            }else {
                MergeSplitHigh(locale_A,temp_B,temp_C,locale_n);
            }
        }

    }

}


void Sort(int local_A[],int locale_n,int rank,int p,double &timeS,double &timeR) {

    int  phase;
    int *temp_B, *temp_C;
    int even_partner; // phase chan
    int odd_partner;  // phase le

    // bo nho tam dung de luu tru khi merge-split
    temp_B = (int*) malloc(locale_n*sizeof(int));
    temp_C = (int*) malloc(locale_n*sizeof(int));

    if(rank % 2 !=0) {
        even_partner = rank - 1;
        odd_partner = rank + 1;
        if(odd_partner == p) odd_partner = -1;
    }else {
        even_partner = rank + 1;
        if(even_partner == p) even_partner = -1;
        odd_partner = rank - 1;
    }

    quick_sort(local_A,0,locale_n - 1);

    for(phase = 0;phase < p;phase++) {
        OddEvenIter(local_A,temp_B,temp_C,locale_n,phase,
                                even_partner,odd_partner,rank,p,timeS,timeR);
    }
    free(temp_B);
    free(temp_C);
}


int main(int argc, char** argv) {

    int n_elements;
    int rank;
    int comm_size;
    int *x = NULL;
    int *localeData = NULL;
    double startTimer,endTimer;
    double timeSend = 0,timeRecive = 0;
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    n_elements = atoi(argv[1]);

    if(rank == RANK_MASTER) {
        printf("\nSize is %d\n",n_elements);
        x = (int*)malloc(n_elements*sizeof(int));
        if(x == NULL) {
            printf("\n\n Memory Allocation Failed ! \n\n");
            exit(EXIT_FAILURE);
        }

        inp = fopen(argv[2],"r");
        if(inp == NULL) {
            printf("\n\n inp Memory Allocation Failed ! \n\n");
            exit(EXIT_FAILURE);
        }
        printf("\n\nInput Data is: %s \n\n",argv[2]);
        for(int i = 0;i < n_elements;i++) {
            int tmp;
            fscanf(inp,"%d",&tmp);
            x[i] = tmp;
        }
        printf("\n\n End Input!");
        fclose(inp);
    }


    if(rank == RANK_MASTER) {
        startTimer = MPI_Wtime();
    }

    if(n_elements < comm_size) {
        printf("\n Size is less than the number of process!\n");
        exit(EXIT_FAILURE);
    }

    // chia thanh cac sub array
    int localeSize = n_elements/comm_size;

    if(rank == comm_size - 1) {
        localeSize = n_elements - (comm_size-1)*localeSize;
    }

    localeData = (int*)malloc(localeSize*sizeof(int));

    if(localeData == NULL) {
        printf("\n localeData Memory Allocation Failed!");
        exit(EXIT_FAILURE);
    }

    //MPI_Scatter(x,localeSize,MPI_INT,localeData,localeSize,MPI_INT,RANK_MASTER,MPI_COMM_WORLD);
    if(rank == RANK_MASTER) {
       // printf("aa");
        for(int i = 0;i < localeSize;i++) {
            localeData[i] = x[i];
        }
        double time1 = MPI_Wtime();
        for(int i = 1;i < comm_size;i++) {
         //   printf("bb");
            int index = i*localeSize;
            int *loc = NULL;
            if(i != comm_size - 1) {
                loc = (int*)malloc(localeSize*sizeof(int));
                for(int j =0;j < localeSize;j++) {
                    loc[j] = x[index + j];
                }
                MPI_Send(loc,localeSize,MPI_INT,i,0,MPI_COMM_WORLD);
            }else {
                int size = n_elements - (comm_size - 1)*localeSize;
                loc = (int*)malloc(size*sizeof(int));
                for(int j =0;j < size;j++) {
                    loc[j] = x[index + j];
                }
                MPI_Send(loc,size,MPI_INT,i,0,MPI_COMM_WORLD);
            }
        }
        double time2 = MPI_Wtime();
        timeSend += time2 - time1;
    }else {
        double time1 = MPI_Wtime();
        MPI_Recv(localeData,localeSize,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        double time2 = MPI_Wtime();
        timeRecive += time2 - time1;

       // printf("\nrank %d is: ",rank);
    }

    Sort(localeData,localeSize,rank,comm_size,timeSend,timeRecive);
    double time1 = MPI_Wtime();
    MPI_Gather(localeData,localeSize,MPI_INT,x,localeSize,MPI_INT,RANK_MASTER,MPI_COMM_WORLD);
    double time2 = MPI_Wtime();
    timeRecive += time2 - time1;

    if(rank == RANK_MASTER) {
        endTimer = MPI_Wtime();
    }
    // Time Passing
    if(rank != RANK_MASTER) {
        MPI_Send(&timeSend,1,MPI_DOUBLE,RANK_MASTER,0,MPI_COMM_WORLD);
        MPI_Send(&timeRecive,1,MPI_DOUBLE,RANK_MASTER,0,MPI_COMM_WORLD);
    }else {
        double timePass[comm_size];
        for(int i = 0;i < comm_size;i++) {
            if(i == 0) {
                timePass[i] = timeSend + timeRecive;
            }else {
                timePass[i] = 0;
            }
        }
        for(int i = 1;i < comm_size;i++) {
            double timeS,timeR;
            MPI_Recv(&timeS,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD,&status);
            MPI_Recv(&timeR,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD,&status);
            timePass[i] = timeS + timeR;
        }
        double timeT = 0;
        for(int i = 0;i < comm_size;i++) {
            timeT += timePass[i];
        }
        printf("\n\nTime Passing is: %f\n\n",timeT);
    }

    free(localeData);
    if(rank == RANK_MASTER) {

       // endTimer = MPI_Wtime();
        out = fopen(argv[3],"w");
        if(out == NULL) {
            printf("\n Out Memory Allocation Failed!");
            exit(EXIT_FAILURE);
        }
        // viet thong tin ra file
        fprintf(out, "Sorted Data: ");
        fprintf(out, "\n\nInput size is: %d",n_elements);
        fprintf(out, "\n\nNumber Processes is: %d",comm_size);
        fprintf(out, "\n\nWall time is: %.4f",endTimer - startTimer);
        printf("\nWall time is: %.4f",endTimer - startTimer);
        fprintf(out,"\n\n");

        for(int i = 0;i < n_elements;i++) {
            fprintf(out,"%d ",x[i]);
        }

        // dong file out
        fclose(out);
    }
    MPI_Finalize();
    return EXIT_SUCCESS;
}

