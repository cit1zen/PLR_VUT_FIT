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

    // Preorder algoritmus
    // Pociatocna vahu
    // Je to dopredna hrana
    unsigned weight = (myid%2 == 0) ? 1 : 0;
    unsigned succ = etour[myid];
    unsigned buffer[2];
    for (unsigned i=0; i < log10(numprocs); i++)
    {
        for(unsigned y=0;x<adjacency_list[x].size();y++)

    }
    cout << "MYID " << myid << " SCORE " << weight << endl;

    MPI_Send(data, count, datatype, i, 0, communicator);

    // if (myid == 0)
    // {
    //     for(unsigned x=0;x<adjacency_list.size();x++)
    //     {
    //         for(unsigned y=0;x<adjacency_list[x].size();y++)
    //         {
    //             cout << adjacency_list[x][y];
    //         }
    //         cout << endl;
    //     }
    //     // cout << argv[1] << endl;
    // }

    // cout << "MYID " << myid << " FROM " << binary_tree[from] << " TO " << binary_tree[to] << " ETOUR " << etour_part << endl;
    // if (myid == 0)
    // {
    //     // cout << reverse << endl;
    //     // cout << "MYID " << myid << " FROM " << binary_tree[from] << " TO " << binary_tree[to] << " ETOUR " << etour_part << endl;
    //     // for(int i=0;i<numprocs;i++)
    //     //     cout << etour[i] << endl;
    // }


    MPI_Finalize();

    return 0;
}