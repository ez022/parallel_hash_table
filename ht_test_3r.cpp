#include <iostream>
#include <cstdlib>  // for std::rand
#include <algorithm>
#include "hash_table.h"  // Assuming the header file is in the same directory
#include "get_time.h"
#include "parallel.h"

//test-hn，parallel; 3r;

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
    int numinsert = 14;
    HashTable<int> myIntHashTable(tablesize);
    HashTable<int> myIntHashTable2(tablesize);
    HashTable<int> myIntHashTable3(tablesize);

    

    //int myInserts[12] = {21, 62, 121,83, 145, 25, 65, 86, 321, 210, 63, 133};
    int myInserts[14] = {55,115,35,15,275,495,235,355,415,175,195,135,75,295};

    int foundIndex[numinsert];

//test inserts
    parlay::timer t;

    parallel_for(0, numinsert, [&](size_t i) {
        myIntHashTable.insert(myInserts[i]);
        std::cout << "=====inserting: " <<myInserts[i]<< t.total_time() << std::endl;

    });    

    t.stop();
    std::cout << "Time for 12 inserts is: " << t.total_time() << std::endl;

//test find;
    parlay::timer t2;

    parallel_for(0, numinsert, [&](size_t i) {
        foundIndex[i]=myIntHashTable.findIndex(myInserts[i]);
    });      

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

     
    
    parallel_for(0, numinsert, [&](size_t i) {
        myIntHashTable2.insert(myInserts[i]);
        std::cout << "=====inserting: " <<myInserts[i]<< t.total_time() << std::endl;

    });    

    parallel_for(0, numinsert, [&](size_t i) {
        myIntHashTable3.insert(myInserts[i]);
        std::cout << "=====inserting: " <<myInserts[i]<< t.total_time() << std::endl;

    });     

    std::cout << "Now prints hash table "  << std::endl;
    std::cout << "--------------------"  << std::endl;

    printElement("ind",slotwidth,separator);

    for (size_t i = 0; i < tablesize; i++) {
        //std::cout << "| " << myInserts[i]; 
        printElement(i,slotwidth,separator);

    } 
     std::cout << std::endl;
    std::cout << "--------------------"  << std::endl;
    printElement("rd1",slotwidth,separator);
    for (size_t i = 0; i < tablesize; i++) {
        //std::cout << "| " << myIntHashTable[i]; 
        printElement(myIntHashTable.getByInd(i),slotwidth,separator);

    } 
     std::cout << std::endl;
    std::cout << "--------------------"  << std::endl;

        printElement("rd2",slotwidth,separator);
    for (size_t i = 0; i < tablesize; i++) {
        //std::cout << "| " << myIntHashTable[i]; 
        printElement(myIntHashTable2.getByInd(i),slotwidth,separator);

    } 
     std::cout << std::endl;
    std::cout << "--------------------"  << std::endl;

  printElement("rd3",slotwidth,separator);
    for (size_t i = 0; i < tablesize; i++) {
        //std::cout << "| " << myIntHashTable[i]; 
        printElement(myIntHashTable3.getByInd(i),slotwidth,separator);

    } 
     std::cout << std::endl;
    std::cout << "--------------------"  << std::endl;

    //test delete

    // myIntHashTable.deleteVal(83);
    // std::cout << "now delete 83"  << std::endl;
    
    myIntHashTable.deleteVal(355);
    std::cout << "now delete 355"  << std::endl;

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
