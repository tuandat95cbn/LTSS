# include <stdio.h>
# include <mpi.h>
const int SERVER_RANK = 0;
const int MESSAGE_TAG = 0;
void do_server_work( int number_of_processes )
{
    const int max_message_length = 256;
    char message[ max_message_length ];
    int src ;
    MPI_Status status ;
    for ( src = 0; src < number_of_processes ; src ++ )
    {
        if ( src != SERVER_RANK )
        {
            MPI_Recv( message , max_message_length , MPI_CHAR ,src , MESSAGE_TAG , MPI_COMM_WORLD ,&status );
            printf ( " Received : %s \n " , message );
        }
    }
}
void do_client_work( int rank )
{
    const int max_message_length = 256;
    char message [ max_message_length ];
    int message_length ;
    message_length =
        sprintf ( message , " Greetings from process %d " , rank );
    message_length ++;
    /* add one for null char */
    MPI_Send ( message , message_length , MPI_CHAR ,
    SERVER_RANK , MESSAGE_TAG , MPI_COMM_WORLD );
}
int main ( int argc , char * argv[] )
{
    int rank , number_of_processes ;
    MPI_Init( &argc , &argv );
    MPI_Comm_size( MPI_COMM_WORLD , &number_of_processes );
    MPI_Comm_rank( MPI_COMM_WORLD , &rank );
    if ( rank == SERVER_RANK )
        do_server_work( number_of_processes );
    else
        do_client_work( rank );
    MPI_Finalize();
    return 0;
}

