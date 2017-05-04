#include<stdio.h>
#include <time.h>
#include <stdlib.h>

int n;
int *a;
void readData(){
    scanf("%d",&n);
    a=(int *) malloc(n*sizeof(int));	
    for(int i=0;i<n;i++){
        scanf("%d",&a[i]);
    }
}
void swap(int *x,int *y){
    int tmp=*x;
    *x=*y;
    *y=tmp;
}
void printArray(int *a,int n,int rank){

    printf("*****************\n From %d The array is %d number\n",rank,n);
    for(int i=0;i<n;i++){
        printf("%d ",a[i]);
    }
    printf("\n****************");
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

int main(int argc, char** argv){
    printf("Done");
    clock_t begin = clock();
    freopen(argv[1],"r",stdin);
    readData();
    sortEven(a,n);

    fclose(stdin);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f ",time_spent);
}
