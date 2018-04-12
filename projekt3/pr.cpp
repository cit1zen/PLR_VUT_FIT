#include <mpi.h>
#include <vector>
#include <math.h>
#include <iostream>


using namespace std;


int main(int argc, char * argv[])
{
    int numprocs;               //pocet procesoru
    int myid;                   //muj rank
    MPI_Status stat;            //struct- obsahuje kod- source, tag, error
    string binary_tree = argv[1];

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
    if (binary_tree.size() == 1)
    {
        if (myid == 0)
            cout << binary_tree << endl;
        MPI_Finalize();
        return 0;
    }


    // Kazdy proc je jedna cesta, musime zistit odkial a kam
    // ukazujeme
    unsigned to = myid/2 + 1;
    unsigned from = myid/4;
    if(myid%2 == 1)
    {
        unsigned dummy = to;
        to = from;
        from = dummy;
    }

    // EULEROVA CESTA
    // Vytvorenie adjacency list
    vector<int> adjacency_list;
    if (2*(to+1) <= binary_tree.size())
    {
        adjacency_list.push_back(to * 4);
        adjacency_list.push_back(to * 4 + 1);
    }
    if (2*(to+1)+1 <= binary_tree.size())
    {
        adjacency_list.push_back(to * 4 + 2);
        adjacency_list.push_back(to * 4 + 3);
    }
    if (to != 0)
    {
        adjacency_list.push_back((to - 1) * 2 + 1);
        adjacency_list.push_back((to - 1) * 2);
    }
    // Etour hladanie
    unsigned reverse;
    if (myid%2 == 0)
    {
        reverse = myid + 1;
    }
    else
    {
        reverse = myid - 1;
    }
    unsigned etour_part;
    for(unsigned i=0;i<adjacency_list.size();i+=2)
    {
        if (adjacency_list[i] == reverse)
        {
            if(i+2<adjacency_list.size())
            {
                etour_part = adjacency_list[i+2];
            }
            else
            {
                etour_part = adjacency_list[0];
            }
        }
    }
    // Rozposli Etour
    int etour [numprocs];
    etour[myid] = etour_part; 
    MPI_Allgather(&etour[myid], 1, MPI_INT,
                  etour, 1, MPI_INT,
                  MPI_COMM_WORLD);

    // Opak k Etour
    // Na vypocet predkov v eulerovej ceste
    int reverse_etour [numprocs];
    MPI_Send(&myid, 1, MPI_INT, etour_part, 0, MPI_COMM_WORLD);
    MPI_Recv(&reverse_etour[myid], 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
             MPI_STATUS_IGNORE);
    MPI_Allgather(&reverse_etour[myid], 1, MPI_INT,
                  reverse_etour, 1, MPI_INT,
                  MPI_COMM_WORLD);

    // Preorder algoritmus
    // Pociatocna vahu
    // Je to dopredna hrana
    int weight = (myid%2 == 0) ? 1 : 0;
    int succ = (etour[myid] == 0) ? -1 : etour[myid];
    int pred = (myid == 0) ? -1 : reverse_etour[myid];
    int requester = 0;
    int buffer[2];
    MPI_Barrier(MPI_COMM_WORLD);
    for (unsigned i=0; i < log2(numprocs); i++)
    {
        // Ak -1, som na konci
        if (succ != -1)
        {
            buffer[0] = myid;
            buffer[1] = pred;
            MPI_Send(&buffer, 2, MPI_INT, succ, 0, MPI_COMM_WORLD);
        }
        MPI_Barrier(MPI_COMM_WORLD);

        // Ak -1; uz sa ma nikto nic nebude pytat
        if (pred != -1)
        {
            MPI_Recv(&buffer, 2, MPI_INT, pred, 0, MPI_COMM_WORLD, &stat);
            requester = buffer[0];
            pred = buffer[1];

            // Sformulujem odpoved na request
            buffer[0] = weight;
            buffer[1] = succ;
            MPI_Send(&buffer, 2, MPI_INT, requester, 0, MPI_COMM_WORLD);
        }
        MPI_Barrier(MPI_COMM_WORLD);

        // Ak -1, som na konci
        if (succ != -1)
        {
            MPI_Recv(&buffer, 2, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &stat);
            weight += buffer[0];
            succ = buffer[1];
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Korekcia
    // Dopredna hrana
    if (myid%2 == 0)
    {
        weight = binary_tree.size() - weight;
    }

    // Zgrupenie vysledkov
    MPI_Barrier(MPI_COMM_WORLD);
    char preorder[numprocs];
    preorder[0] = binary_tree[0];
    if (myid%2 == 0)
    {
        MPI_Send(&to, 1, MPI_INT, weight, 0, MPI_COMM_WORLD);
    }
    if (myid < binary_tree.size() && myid != 0)
    {
        int buffer = 0;
        MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        preorder[myid] = binary_tree[buffer];
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(&preorder[myid], 1, MPI_CHAR,
               preorder, 1, MPI_CHAR,
               0, MPI_COMM_WORLD);


    // Vypis
    if (myid == 0)
    {
        for(int i=0;i<binary_tree.size();i++)
            cout << preorder[i];
         cout << endl;
    }


    MPI_Finalize();
    return 0;
}