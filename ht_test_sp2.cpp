#include <iostream>
#include <cstdlib>  // for std::rand
#include <algorithm>
#include "hash_table.h"  // Assuming the header file is in the same directory
#include "get_time.h"
#include "parallel.h"

//test-hn，speed; sp2;

#include <iomanip>


using Type = int;


template<typename T> void printElement(T t, const int& width, const char separator)
{
    std::cout <<"|" << std::setw(width) << std::setfill(separator) << t;
}

int main(int argc, char* argv[]) {
  // size_t defaultsize = 1e8;
  int num_rounds = 3;
  int random_seed = 1;
  // if (argc >= 2) {
  //   defaultsize = atoll(argv[1]);
  // }
  if (argc >= 3) {
    num_rounds = atoi(argv[2]);
  }
  if (argc >= 4) {
    random_seed = atoi(argv[3]);
  }
    // //print
    // const char separator    = ' ';
    // const int slotwidth    = 3;

    // Create a hashtable with integer keys
    //size_t tablesize = 1e9;
    size_t tablesize = 268435456; //this is 2^28
     //size_t tablesize =536870912; //this is 2^29;

    int numinsert = 1e8;
    HashTable<int> myIntHashTable(tablesize);
    HashTable<int> myIntHashTable1(tablesize);

    HashTable<int> myIntHashTable2(tablesize);
    HashTable<int> myIntHashTable3(tablesize);

  Type* A = (Type*)malloc(tablesize * sizeof(Type));
  Type* B = (Type*)malloc(tablesize * sizeof(Type)); //used for random find;
  Type* C = (Type*)malloc(tablesize * sizeof(Type)); //used for random delete;


  double tt_insert = 0;
  double tt_find=0;
  double tt_delete=0;
  for (int i = 0; i <= num_rounds; i++) {
    double t_insert=0;
    double t_find=0;
    double t_delete=0;
    // Generate random arrays
    parallel_for(0, numinsert,
            [&](size_t j) { A[j] = hash64(j * random_seed) % numinsert; });

   parallel_for(0, numinsert,
            [&](size_t j) { B[j]= hash64(j * (45)) % numinsert; });

   parallel_for(0, numinsert,
            [&](size_t j) { C[j]= hash64(j * (123)) % numinsert; });


  //double total_time = 0;
        parlay::timer t;
    parallel_for(0, numinsert, [&](size_t i) {
        myIntHashTable.insert(A[i]);
        //std::cout << "=====inserting: " <<myInserts[i]<< t.total_time() << std::endl;
    });
    t.stop();
    t_insert=t.total_time();

            parlay::timer t2;
    parallel_for(0, numinsert, [&](size_t i) {
        myIntHashTable.find(A[B[i]]);
        //std::cout << "=====inserting: " <<myInserts[i]<< t.total_time() << std::endl;
    });
    t2.stop();
    t_find=t2.total_time();

            parlay::timer t3;
    parallel_for(0, numinsert, [&](size_t i) {
        myIntHashTable.deleteVal(A[C[i]]);
        //std::cout << "=====inserting: " <<myInserts[i]<< t.total_time() << std::endl;
    });
    t3.stop();
    t_delete=t3.total_time();

    // add for ht2, ht3;

        parallel_for(0, numinsert, [&](size_t i) {
        myIntHashTable1.insert(A[i]);
        //std::cout << "=====inserting: " <<myInserts[i]<< t.total_time() << std::endl;
    });

        parallel_for(0, numinsert, [&](size_t i) {
        myIntHashTable2.insert(A[i]);
        //std::cout << "=====inserting: " <<myInserts[i]<< t.total_time() << std::endl;
    });
        parallel_for(0, numinsert, [&](size_t i) {
        myIntHashTable3.insert(A[i]);
        //std::cout << "=====inserting: " <<myInserts[i]<< t.total_time() << std::endl;
    });

        std::cout << "Checking if three hashtables are the same\n";

        parallel_for(0, numinsert, [&](size_t j) {
      if (!((myIntHashTable1.getByInd(j) == myIntHashTable2.getByInd(j))&&
            (myIntHashTable2.getByInd(j) == myIntHashTable3.getByInd(j)))
      ) {
        std::cout << "The three tables are not the same\n";
        std::cout << "j: " <<j << std::endl;

        exit(0);
      }
    });
        std::cout << "===>Passed test!\n";



    if (i == 0) {
      std::cout << "Warmup round running time: " << "insert: "<<t_insert << ", find: "<<t_find <<", delete: "<<t_delete << std::endl;
    } else {
      std::cout << "Round " << i << " running time: "<< "insert: "<<t_insert << ", find: "<<t_find <<", delete: "<<t_delete 
                << std::endl;
         tt_insert+=t_insert;
         tt_find+=t_find;
         tt_delete+=t_delete;         
    }
  }
  std::cout << "Average running time: "<< "insert: "<<tt_insert/num_rounds << ", find: "<<tt_find/num_rounds <<", delete: "<<tt_delete/num_rounds<< std::endl;

    free(A);
    free(B);
    free(C);

    return 0;
}
