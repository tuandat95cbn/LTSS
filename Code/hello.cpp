# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <mpi.h>
int generateArr(){
    freopen("input.txt","w",stdout);
    int n=100000;
    srand(time(NULL));
    int r = rand()%10000;
    printf("%d\n",n);
    for(int i=0;i<n;i++){
        printf("%d ",rand()%10000-rand()%10000);
    }
    fclose(stdout);
}
int main ( int argc , char * argv [] )
{
generateArr();
return 0;
}
