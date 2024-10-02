#include <iostream>
#include <cstdlib>  // for std::rand
#include "hash_table.h"  // Assuming the header file is in the same directory
#include "get_time.h"
//test-hn，sequential;

#include <iomanip>

using Type = int;


template<typename T> void printElement(T t, const int& width, const char separator)
{
    std::cout <<"|" << std::setw(width) << std::setfill(separator) << t;
}

int main() {
    //print
    const char separator    = ' ';
    const int slotwidth    = 3;


    // Create a hashtable with integer keys
    size_t tablesize = 20;
    int numinsert = 12;
    HashTable<int> myIntHashTable(tablesize);

    int myInserts[12] = {21, 62, 121,83, 145, 25, 65, 86, 321, 210, 63, 133};
    int foundIndex[12];

//test inserts
    parlay::timer t;
    for (int i = 0; i < numinsert; i++) {
        myIntHashTable.insert(myInserts[i]);
    }
    t.stop();
    std::cout << "Time for 12 inserts is: " << t.total_time() << std::endl;

//test find;
    parlay::timer t2;
    for (int i = 0; i < numinsert; i++) {
        foundIndex[i]=myIntHashTable.findIndex(myInserts[i]);
    }    
    t2.stop();
    std::cout << "Time for 12 finds is : " << t2.total_time() << std::endl;

    std::cout << "-----inserts----- "  << std::endl;

    for (int i = 0; i < numinsert; i++) {
        //std::cout << "| " << myInserts[i]; 
        printElement(myInserts[i],slotwidth,separator);

    }   
        std::cout << std::endl;
    std::cout << "-----their index----- "  << std::endl;

    for (int i = 0; i < numinsert; i++) {
        //std::cout << "| " << foundIndex[i]; 
        printElement(foundIndex[i],slotwidth,separator);


    }    
     std::cout << std::endl;

    std::cout << "Now prints hash table "  << std::endl;
    std::cout << "--------------------"  << std::endl;

    for (size_t i = 0; i < tablesize; i++) {
        //std::cout << "| " << myInserts[i]; 
        printElement(i,slotwidth,separator);

    } 
     std::cout << std::endl;
    std::cout << "--------------------"  << std::endl;
    for (size_t i = 0; i < tablesize; i++) {
        //std::cout << "| " << myIntHashTable[i]; 
        printElement(myIntHashTable.getByInd(i),slotwidth,separator);

    } 
     std::cout << std::endl;
    std::cout << "--------------------"  << std::endl;

    //test delete




    std::cout << "Now prints hash table "  << std::endl;
    std::cout << "--------------------"  << std::endl;

    for (size_t i = 0; i < tablesize; i++) {
        //std::cout << "| " << myInserts[i]; 
        printElement(i,slotwidth,separator);

    } 
     std::cout << std::endl;
    std::cout << "--------------------"  << std::endl;
    for (size_t i = 0; i < tablesize; i++) {
        //std::cout << "| " << myIntHashTable[i]; 
        printElement(myIntHashTable.getByInd(i),slotwidth,separator);

    } 
     std::cout << std::endl;
    std::cout << "--------------------"  << std::endl;

    
 


    return 0;
}
