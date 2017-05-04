#include <stdio.h>
#include <time.h>
#include <stdlib.h>

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

int main(int argc,char** argv) {

   // int a[] = {2,5,1,6,3,8,10,11,3,4,21,14,15,16,17};
    int *x = NULL;
    int n_element = atoi(argv[1]);
    FILE *inp = NULL;
    FILE *out = NULL;
    printf("\nSize is %d\n",n_element);
    x = malloc(n_element*sizeof(int));

    if(x == NULL) {
        printf("\n\n Memory Allocation Failed! \n\n");
        exit(EXIT_FAILURE);
    }

    inp = fopen(argv[2],"r");
    if(inp == NULL) {
        printf("\nMemory Allocation Failed !\n");
        exit(EXIT_FAILURE);
    }

    printf("\nInput Data is: %s",argv[2]);
    for(int i=0;i < n_element - 1;i++) {
        int tmp;
        fscanf(inp,"%d",&tmp);
        x[i] = tmp;
    }

    fclose(inp);

    clock_t begin = clock();
//    merge_sort(x,0,n_element-1,n_element);
	quick_sort(x,0,n_element - 1);  
	clock_t end = clock();
    printf("\nTime run is: %f\n",(double)(end - begin)/CLOCKS_PER_SEC);

}
