#include <iostream>
#include <cstddef>

#include <atomic>

#include <algorithm>

#include <cmath>
#include "parallel.h"

using namespace parlay;
//determ;


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
       // return hashToRange2(v);
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
    size_t i = firstIndex(v);
    while (true) {
      T c = htable[i];
      if (c == EMPTY) {
        if (cas(&htable[i], c, v)) return true;
      } else {
        int cmp = cmpVals(v, c);
        if (cmp == 0) {
          if (cas(&htable[i], c, v))
            return true;
        } else if (cmp < 0)
          i = incrementIndex(i);
        else if (cas(&htable[i], c, v)) {
          v = c;
          i = incrementIndex(i);
        }
      }
    }
  }


    T find(T v) {
    size_t h = firstIndex(v);
    T c = htable[h];
    while (true) {
      if (c == EMPTY) return EMPTY;
      int cmp = cmpVals(v, c);
      if (cmp >= 0) {
        if (cmp > 0)
          return EMPTY;
        else
          return c;
      }
      h = incrementIndex(h);
      c = htable[h];
    }
  }

      size_t findIndex(T v) {
    size_t h = firstIndex(v);
    T c = htable[h];
    while (true) {
      if (c == EMPTY) return -1;
      int cmp = cmpVals(v, c);
      if (cmp >= 0) {
        if (cmp > 0)
          return -1;
        else
          return h;
      }
      h = incrementIndex(h);
      c = htable[h];
    }
  }


  bool deleteVal(T v) {
    size_t i = firstIndex(v);
    int cmp;

    size_t j = i;
    T c = htable[j];

    if (c == EMPTY) return true;

    // find first location with priority less or equal to v's priority
    while ((cmp = (c == EMPTY) ? 1 : cmpVals(v, c)) <
           0) {
      j = incrementIndex(j);
      c = htable[j];
    }
    while (true) {
      if (cmp != 0) {
        // v does not match key of c, need to move down one and exit if
        // moving before h(v)
        if (j == i) return true;
        j = decrementIndex(j);
        c = htable[j];
        cmp = (c == EMPTY) ? 1 : cmpVals(v, c);
      } else {  
        size_t jj = incrementIndex(j);
        T x = htable[jj];
        while (x != EMPTY && lessIndex(j, firstIndex(x))) {
          jj = incrementIndex(jj);
          x = htable[jj];
        }
        size_t jjj = decrementIndex(jj);
        while (jjj != j) {
          T y = htable[jjj];
          if (y == EMPTY || !lessIndex(j, y)) {
            x = y;
            jj = jjj;
          }
          jjj = decrementIndex(jjj);
        }

        // try to copy the the replacement element into j
        if (cas(&htable[j], c, x)) {
          // swap was successful
          // if the replacement element was EMPTY, we are done
          if (x == EMPTY) return true;

          v = x;
          j = jj;
          i = firstIndex(v);
        }
        c = htable[j];
        cmp = (c == EMPTY) ? 1 : cmpVals(v, c);
      }
    }
  }


};    
  
