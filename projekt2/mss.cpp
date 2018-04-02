#include <mpi.h>
#include <cmath>
#include <limits>
#include <fstream>
#include <algorithm>


using namespace std;


int main(int argc, char * argv[])
{
    int numprocs;               //pocet procesoru
    int myid;                   //muj rank
    MPI_Status stat;            //struct- obsahuje kod- source, tag, error

    //MPI INIT
    MPI_Init(&argc,&argv);                          // inicializace MPI 
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);       // zjistíme, kolik procesů běží 
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);           // zjistíme id svého procesu 

    // Argument parsing
    if (argc != 2)
    {
        cout << "1 argument expected";
        return 1;
    }

    // Pre experimenty
    // použíté na zistenie rýchlosti algo.
    double start, end;
    MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
    start = MPI_Wtime();

    // Celkovy pocet cisel, pre fronty/buffer
    int num_count = atoi(argv[1]);
    unsigned buffer_size = ceil(float (num_count)/numprocs);
    // Vstupny subor s cislami
    string number_filename = "numbers";
    unsigned *numbers = new unsigned[buffer_size*numprocs];
    // Buffre
    unsigned *buffer_internal = new unsigned[buffer_size];
    unsigned *buffer_input = new unsigned[buffer_size];

    // Nacitanie bytov zo suboru
    if(myid == 0)
    {
        unsigned index = 0;
        unsigned number;
        fstream fin;
        fin.open(number_filename, ios::in);                   

        while(fin.good())
        {
            number = fin.get();
            // EOF vyskocenie
            if(!fin.good())
            {
                break;
            }
            cout << number << " ";
            if(index >= num_count)
            {
                // cerr << "More numbers in file numbers than expected";
                break;
            }
            numbers[index] = number;
            index++;
        }
        cout << "\n";
        fin.close();
        // HACK ked su pocet cisel a pocet proc nesudelitelne cisla
        for(unsigned i=index;i<(buffer_size*numprocs);i++)
        {
            numbers[i] = numeric_limits<unsigned int>::max();
        }
    }
    // Posleme vsetkym ich cast nezoradenej postupnosti
    MPI_Scatter(numbers, buffer_size, MPI_UNSIGNED ,
                buffer_internal, buffer_size, MPI_UNSIGNED,
                0, MPI_COMM_WORLD);

    // Preprocessing
    sort(buffer_internal, &buffer_internal[buffer_size]);

    //  Merge-splitting sort
    unsigned sorted_condition = ceil(float(numprocs)/2.0);
    unsigned sort_buffer_internal [buffer_size * 2];
    for(unsigned i=0; i<sorted_condition; i++)
    {
        // Parne pocitaju
        // Krok 1
        if((myid % 2 == 0) && (myid + 1 < numprocs))
        {   
            MPI_Recv(buffer_input, buffer_size, MPI_UNSIGNED,
                     myid + 1, 0, MPI_COMM_WORLD, &stat);
            merge(buffer_internal, &buffer_internal[buffer_size],
                  buffer_input, &buffer_input[buffer_size], sort_buffer_internal);
            copy(&sort_buffer_internal[0],
                 &sort_buffer_internal[buffer_size],
                 buffer_internal);
            copy(&sort_buffer_internal[buffer_size],
                 &sort_buffer_internal[buffer_size * 2],
                 buffer_input);
            MPI_Send(buffer_input, buffer_size, MPI_UNSIGNED,
                     myid + 1, 0, MPI_COMM_WORLD);
        }
        else if (myid % 2 != 0)
        {
            MPI_Send(buffer_internal, buffer_size, MPI_UNSIGNED,
                     myid - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buffer_internal, buffer_size, MPI_UNSIGNED,
                     myid - 1, 0, MPI_COMM_WORLD, &stat);
        }

        // Neparne pocitaju
        // Krok 2
        if((myid % 2 == 0) && (myid != 0))
        {
            MPI_Send(buffer_internal, buffer_size, MPI_UNSIGNED,
                     myid - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(buffer_internal, buffer_size, MPI_UNSIGNED,
                     myid - 1, 0, MPI_COMM_WORLD, &stat);
        }
        else if((myid % 2 != 0) && (myid + 1 < numprocs))
        {
            MPI_Recv(buffer_input, buffer_size, MPI_UNSIGNED,
                     myid + 1, 0, MPI_COMM_WORLD, &stat);
            merge(buffer_internal, &buffer_internal[buffer_size],
                  buffer_input, &buffer_input[buffer_size], sort_buffer_internal);
            copy(&sort_buffer_internal[0],
                 &sort_buffer_internal[buffer_size],
                 buffer_internal);
            copy(&sort_buffer_internal[buffer_size],
                 &sort_buffer_internal[buffer_size * 2],
                 buffer_input);
            MPI_Send(buffer_input, buffer_size, MPI_UNSIGNED,
                     myid + 1, 0, MPI_COMM_WORLD);
        }
    }

    // Pozbieraj naspat vysledky a vypis zoradene pole
    MPI_Gather(buffer_internal, buffer_size, MPI_UNSIGNED,
               numbers, buffer_size, MPI_UNSIGNED,
               0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
    end = MPI_Wtime();

    if(myid == 0)
    {
        for(unsigned i=0; i<num_count; i++)
            cout << numbers[i] << endl;
    }

    // Cleanup
    delete[] buffer_internal;
    delete[] buffer_input;
    delete[] numbers;
    MPI_Finalize();

    // Vrati kolko trvalo zoradenie
    // if (myid == 0) 
    // {
    //     cerr << numprocs << endl << num_count << 
    //             endl << end-start << endl;
    // }

    return 0;
}