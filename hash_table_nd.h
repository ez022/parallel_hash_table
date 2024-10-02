#include <iostream>
#include <cstddef>

#include <atomic>

#include <algorithm>

#include <cmath>
#include "parallel.h"

using namespace parlay;


#define EMPTY -1
//#define TOMB -2
//hn2;


// from numerical recipes
inline uint64_t hash64(uint64_t u) {
  uint64_t v = u * 3935559000370003845ul + 2691343689449507681ul;
  v ^= v >> 21;
  v ^= v << 37;
  v ^= v >> 4;
  v *= 4768777513237032717ul;
  v ^= v << 20;
  v ^= v >> 41;
  v ^= v << 5;
  return v;
}

template <typename T>
class HashTable {
private:
    T* htable;
    size_t m;

//utility functions
    size_t hash(T v) { 
        return static_cast<size_t>(hash64(v)); 
    }

    size_t hashToRange1(size_t h){
        return hash(h) % m;
    }

    size_t hashToRange2(size_t h){
        return h % m;
    }    

    size_t firstIndex(T v){
        return hashToRange1(v);
    }

    size_t incrementIndex(size_t h) { 
        return (h + 1 == m) ? 0 : h + 1; 
        //if next one is m, go back to [0];otherwise go to next one;
    }

    size_t decrementIndex(size_t h) {
         return (h == 0) ? m - 1 : h - 1; 
         //if already at the start, go to end of table;
    }

    bool cas(T* p, T o, T n) {
//type needs to work with std::atomic
//do not use any type that the standard library chooses to lock)
    return std::atomic_compare_exchange_strong_explicit(
      reinterpret_cast<std::atomic<T>*>(p), &o, n, std::memory_order_relaxed, std::memory_order_relaxed);
    }

    int cmpVals(T v, T b) {
         return (v > b) ? 1 : ((v == b) ? 0 : -1); 
    }

    bool lessIndex(size_t a, size_t b) {
    return (a < b) ? (2 * (b - a) < m) : (2 * (a - b) > m);
  }



public:

    HashTable(size_t tablesize) : m(tablesize) {
        htable = new T[m];

        // Initialize each element with the value -2
        for (size_t i = 0; i < m; ++i) {
            htable[i] = T(EMPTY); // Assuming T can be constructed from an int
        }
    }

    ~HashTable() {
        delete[] htable;
    }

    T getByInd(size_t ind) {
      return htable[ind];

    }
  
    bool insert(T v) {
      //std::cout << "----inserting : " << v << std::endl;

      size_t i = firstIndex(v);
      while (true) {
        T c = htable[i]; 
        if (cas(&htable[i], EMPTY, v)) return true;
        else if (c==v) return true;

        i = incrementIndex(i);
          
      }
    }

    T find(T v) {
          //find to return the value if found;
          //EMPTY if not found
        size_t h = firstIndex(v);
        T c = htable[h];
        while (true) {
          if (c == EMPTY) return EMPTY;
          else if (c==v) return c;
          h = incrementIndex(h);
          c = htable[h];
        }
    }


    size_t findIndex(T v) {
      //findIndex to return the index of value; -1 if not found;
        size_t h = firstIndex(v);
        T c = htable[h];
        while (true) {
          if (c == EMPTY) return -1;
          else if (c==v) return h;
          h = incrementIndex(h);
          c = htable[h];
        }
    }

    bool deleteVal(T v) {
       // std::cout << "---- delete : " << v<< std::endl;

        size_t i = firstIndex(v); //i is first index;
        size_t j = i;
        int cmp;
        T c = htable[j];
        //std::cout << "---- c is : " << c<< std::endl;


        if (c==EMPTY) {
        return true; //it's already gone;
           // std::cout << "====no need to delete : " << j << std::endl;

        }

        //firt find where v is 
       // std::cout << "---- c empty is : " << c==EMPTY<< std::endl;


        while((cmp=(c == EMPTY) ? 0:cmpVals(v,c))!=0){
          //std::cout << "---- c empty is : " <<( c==EMPTY)<< std::endl;

          //std::cout << "---- j is : " << j<<", v "<<v<<", c"<<c<< std::endl;

          j = incrementIndex(j);
          c = htable[j];          
        }
        //next find replacement;
        while (true) {

            if (cmp != 0) { 
                if (j == i) return true; 
                j = decrementIndex(j);
                c = htable[j];
                //moving backward
                cmp = (c == EMPTY) ? 1 : cmpVals(v, c);
            } else {  // found v at location j 
               // std::cout << "---- v is : " << v<< std::endl;

                size_t jj = incrementIndex(j);
                T x = htable[jj];
                //the replacement will be either empty, or one that hashes before j
                while (x != EMPTY && lessIndex(j, firstIndex(x))) {
                  jj = incrementIndex(jj);
                  x = htable[jj];
                }
               // std::cout << "---- jj is : " << jj<< std::endl;

                size_t jjj = decrementIndex(jj);
                while (jjj != j) {
                  T y = htable[jjj];
                  if (y == EMPTY || !lessIndex(j, firstIndex(y))) {
                      x = y;
                      jj = jjj;
                  }
                  jjj = decrementIndex(jjj);
                }
                  // std::cout << "---- j is : " << j<< std::endl;


                // try to copy the the replacement element into j
                if (cas(&htable[j], c, x)) {
                  // swap was successful
                  // if the replacement element was empty, we are done
                  if (x == EMPTY) {
                   // std::cout << "----swapped : " << j << std::endl;

                    return true;
                  //otherwise we keep doing until it's empty;
                  }
                  v = x;
                  j = jj;
                  i = firstIndex(v);
                }
                c = htable[j];
                cmp = (c == EMPTY) ? 1 :cmpVals(v, c);

            }
        }
    }
};    
  
