
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    /* create temp arrays */
    int L[n1], R[n2];

    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];

    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;
        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);

        merge(arr, l, m, r);
    }
}

void printArray(int A[], int size)
{
    int i;
    for (i=0; i < size; i++)
        printf("%d ", A[i]);
    printf("\n");
}

int main(int argc,char **argv)
{
    /*
    int arr[] = {12, 11, 13, 5, 6, 7};
    int arr_size = sizeof(arr)/sizeof(arr[0]);

    printf("Given array is \n");
    printArray(arr, arr_size);

    mergeSort(arr, 0, arr_size - 1);

    printf("\nSorted array is \n");
    printArray(arr, arr_size);
*/
    int *x = NULL;
    int n_element = atoi(argv[1]);
    FILE *inp = NULL;
    FILE *out = NULL;
    //printf("\nSize is %d\n",n_element);
    x = (int*)malloc(n_element*sizeof(int));
	printf("\nSize is %d\n",n_element);
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
    for(int i=0;i < n_element;i++) {
        int tmp;
        fscanf(inp,"%d",&tmp);
        x[i] = tmp;
    }
	//printf("\nSize is %d\n",n_element);
    fclose(inp);

    clock_t begin = clock();
    mergeSort(x, 0,n_element - 1);
    clock_t end = clock();
    printf("\nTime run is: %f\n",(double)(end - begin)/CLOCKS_PER_SEC);
   // printArray(x,n_element);
    return 0;
}
