#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#define RANK_MASTER 0

FILE *inp;
FILE *out;

void merge(int a[],int L,int M,int R,int size);
void merge_sort(int a[],int L,int R,int size);
void sortCheckers(int size,int a[]);
void mergeParallel(int localeData[],int localeSize,int comm_size,int rank);


int * merge_two(int *v1, int n1, int *v2, int n2)
{
	int i,j,k;
	int * result;
   // printf("n1 is: %d and n2 is: %d",n1,n2);
  //  printf("\nv1 is: ");
   // for(int x = 0;x<n1;x++){
    //    printf("%d ",v1[x]);
    //}
    //printf("\nv2 is: ");
    //for(int x = 0;x < n2;x++) {
     //   printf("%d ",v2[x]);
    //}
	result = (int *)malloc((n1+n2)*sizeof(int));
	i=0; j=0; k=0;
	while(i<n1 && j<n2)
		if(v1[i]<v2[j])
		{
			result[k] = v1[i];
			i++; k++;
		}
		else
		{
			result[k] = v2[j];
			j++; k++;
		}
	if(i==n1)
		while(j<n2)
		{
			result[k] = v2[j];
			j++; k++;
		}
	else
		while(i<n1)
		{
			result[k] = v1[i];
			i++; k++;
		}
	return result;
}

int main(int argc,char **argv) {


    int rank;
    int comm_size;
    int *localeData = NULL;
    int *x = NULL;

    int n_elements = atoi(argv[1]);
    MPI_Status status;
    // Khoi tao moi truong MPI commutication

    MPI_Init(&argc,&argv);

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&comm_size);


    if(rank == RANK_MASTER) {
        printf("Start Agorithm Merge Sort Parallel");
        //printf("\nSize is %d\n",n_elements);
        x = malloc(n_elements*sizeof(int));
        if(x == NULL) {
            printf("\nMemory Allocation Failed! \n");
            exit(EXIT_FAILURE);
        }

        inp = fopen(argv[2],"r");
        if(inp == NULL) {
            printf("\n inp Memory Allocation Failed! \n");
            exit(EXIT_FAILURE);
        }
        printf("\nInput Data is: %s",argv[2]);
        for(int i = 0;i < n_elements;i++) {
            int tmp;
            fscanf(inp,"%d",&tmp);
            x[i] = tmp;
        }
        printf("\nEnd Input !!!");
        fclose(inp);
    }

    double startTimer;
    double stopTimer;

    if(rank == RANK_MASTER) {
        startTimer = MPI_Wtime();
    }

     if(n_elements < comm_size) {
        printf("\n\n SIZE is less than the number of process! \n\n");
        exit(EXIT_FAILURE);
    }

    // Chia array thanh subarray
    int localeSize = n_elements/comm_size;
    if(rank == comm_size - 1) {
        localeSize = n_elements - (comm_size-1)*localeSize;
    }

   // printf("\n\n n_elements is: %d",n_elements);
    //printf("\n\n comm_size is: %d",comm_size);
    //printf("\n\n localeSize is: %d",localeSize);
    // khoi tao cac subarray cho moi bo su li
    localeData = (int*)malloc(localeSize*sizeof(int));
    if(localeData == NULL) {
        printf("\n localeData Memory Allocation Failed!");
        exit(EXIT_FAILURE);
    }
    // SGui moi subarray cho moi bo xu li
   // MPI_Scatter(x,localeSize,MPI_INT,localeData,localeSize,MPI_INT,RANK_MASTER,MPI_COMM_WORLD);
    // Thuc hien merger_sort tren moi bo xu li
    // printf("aaaa");
    if(rank == RANK_MASTER) {
       // printf("aa");
        for(int i = 0;i < localeSize;i++) {
            localeData[i] = x[i];
        }

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
    }else {
        MPI_Recv(localeData,localeSize,MPI_INT,0,0,MPI_COMM_WORLD,&status);
       // printf("\nrank %d is: ",rank);
    }
	//printf("rank %d  localesize : %d\n",rank,localeSize);
    //for(int i = 0;i < localeSize-1;i++) {
	//printf("%d-> ",localeData[i]);
    //}

	//printf("%d end\n",localeData[localeSize-1]);
    merge_sort(localeData,0,localeSize - 1,localeSize);
    // tron subarray da sap xep vao process master
  //  MPI_Gather(localeData,localeSize,MPI_INT,x,localeSize,MPI_INT, RANK_MASTER, MPI_COMM_WORLD);
    //free(localeData);
    /*
    * meger parallel
    *
    */
    int step = 1,m;
    int* other;
    while(step < comm_size) {
        if(rank%(2*step) == 0) {
            if(rank + step < comm_size) {
                MPI_Recv(&m,1,MPI_INT,rank + step,0,MPI_COMM_WORLD,&status);
            //    printf("m is: %d",m);
                other = (int*)malloc(m*sizeof(int));
                MPI_Recv(other,m,MPI_INT,rank + step,0,MPI_COMM_WORLD,&status);
                localeData = merge_two(localeData,localeSize,other,m);
                localeSize = localeSize + m;
            }
        }else {
            int near = rank - step;
            MPI_Send(&localeSize,1,MPI_INT,near,0,MPI_COMM_WORLD);
            MPI_Send(localeData,localeSize,MPI_INT,near,0,MPI_COMM_WORLD);
            break;

        }
        step = step*2;
    }

    if(rank == RANK_MASTER) {
        // Final sorting
       /* printf("\nArray is: ");
        for(int i =0 ;i < n_elements;i++) {
            printf("%d ",localeData[i]);
        }*/
        printf("\n");
      //  merge_sort(x,0,n_elements - 1,n_elements);
        stopTimer = MPI_Wtime();
        out = fopen(argv[3],"w");
        if(out == NULL) {
            printf("\n Out Memory Allocation Failed!");
            exit(EXIT_FAILURE);
        }

        // viet thong tin ra file
        fprintf(out, "Sorted Data: ");
        fprintf(out, "\n\nInput size is: %d",n_elements);
        fprintf(out, "\n\nNumber Processes is: %d",comm_size);
        fprintf(out, "\n\nWall time is: %.4f",stopTimer - startTimer);
        printf("\nWall time is: %.4f",stopTimer - startTimer);
        fprintf(out,"\n\n");

        for(int i = 0;i < n_elements;i++) {
            fprintf(out,"%d ",localeData[i]);
        }

        // dong file out
        fclose(out);
        printf("\n\nCheck Sorted");
        sortCheckers(n_elements,localeData);
        printf("\n\n");
    }

    if(rank == RANK_MASTER) {
        free(x);
    }
    free(localeData);
    MPI_Finalize();
    return EXIT_SUCCESS;
}


void sortCheckers(int size, int a[]) {
    for(int i = 1;i < size;i++) {
        if(a[i-1] > a[i]) {
            printf("\n\n Check failed. array not sorted");
            break;
        }
    }
    printf("\n\nCheck successfully completed. Array Sorted");
}

/*

void mergeParallel(int localeData,int localeSize,int comm_size,int rank){
    if(rank % 2 == 0) {
        if(comm_size % 2 == 0) {
            MPI_Send(&localeSize,1,MPI_INT,rank + 1,0,MPI_COMM_WORLD);
            MPI_Send(localeData,localeSize,MPI_INT,rank + 1,0,MPI_COMM_WORLD);
        }else {
            if(rank != comm_size - 1) {
                MPI_Send(&localeSize,1,MPI_INT,rank + 1,MPI_COMM_WORLD);
                MPI_Send(localeData,localeSize,MPI_INT,rank + 1,0,MPI_COMM_WORLD);
            }
        }
    }else {

    }

}
*/


void merge(int a[],int L,int M,int R,int size) {
    int i = L; // first position of the first list
    int j = M + 1; // first position of the second list a[M + 1, ... R]
    int TA[size];

    for(int k = L; k <= R;k++) {
        if(i > M) { // the first list
            TA[k] = a[j];
            j++;
        }else if(j > R) {
            TA[k] = a[i];
            i++;
        }else {
            if(a[i] < a[j]) {
                TA[k] = a[i];
                i++;
            }else {
                TA[k] = a[j];
                j++;
            }
        }
    }
    for(int k = L;k <= R;k++) {
        a[k] = TA[k];
    }
}


void merge_sort(int a[],int L,int R,int size) {
    if(L < R) {
        int M = (L + R)/2;
        merge_sort(a,L,M,size);
        merge_sort(a,M + 1,R,size);
        merge(a,L,M,R,size);
    }

}




