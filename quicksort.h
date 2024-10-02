#include <algorithm>
//k1；；


#include <cmath>
#include "parallel.h"
#include <iostream>

using namespace parlay;

using Type = long long;

const int scanG = 1e3;


inline uint64_t hash64_2(uint64_t u) {
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


inline size_t randInRange(size_t range, uint64_t seed){
  uint64_t hashValue = hash64_2(seed);
  size_t r= hashValue % range;
  return r;
}  

template <class T>
T par_reduce(T *A, size_t n) {


    size_t threshold=1000;  
        
    if (n==0){
      return 0;
    }
    else if (n==1){
      return A[0];

    }
    else if (n<threshold){
      T ans=0;

      for (size_t i =0;i<n;i++){
        ans+=A[i];
      }
      return ans;
    }
    T v1, v2;
    auto f1 = [&]() { v1 = par_reduce(A, n / 2); };
    auto f2 = [&]() { v2 = par_reduce(A + n / 2, n - n / 2); };
    par_do(f1, f2);
    return v1 + v2;
  
}

template <class T>
T reduce(T *A, size_t n) {
  if (n == 0) {
    return 0;
  } 
  T ans=0;
  for (size_t i =0;i<n;i++){
    ans+=A[i];

  }
  return ans;

}


/**
 * Inplace block scan DIRECTLY without filter
 */


template <typename T>
T scan(T *A,size_t n) {
  ////std::cout << "scan - loc1 n is "<<n << std::endl;

  int kgoal =scanG;
  int chsz= (n/kgoal); //chunk size;
 // int mod1 = n%chsz;
  //int k = (n-mod1)/chsz+1;
  int modnk=n % kgoal;
  int k = n/chsz;
    //std::cout << "scan - loc1.5 n , k "<<n <<" "<<k<< std::endl;

  if (modnk!=0){ //this is the case where the last chunk will have less elements
    chsz +=1;
    k=n/chsz+1;
  }
  
  int lastsize=n-(k-1)*chsz;
  ////std::cout << "scan - loc2 n is "<<n << std::endl;

  Type *S = (Type *)malloc(k * sizeof(Type));
    ////std::cout << "scan - loc3 n is "<<n << std::endl;

  parallel_for(0, k-1, [&](size_t i) {
   // ////std::cout << "scan - loc2  " << std::endl;
      ////std::cout << "scan - loc4 n is "<<n << std::endl;

    S[i]=reduce(A+i*chsz, chsz);
          ////std::cout << "scan - loc5 n is "<<n << std::endl;

  });
          ////std::cout << "scan - loc6 n is "<<n << std::endl;
          ////std::cout << "scan - loc6 chsz,lastsize "<<chsz<<" and "<<lastsize << std::endl;
          ////std::cout << "scan - k: "<<k<< std::endl;
        //  ////std::cout << "scan - mod1: "<<mod1<< std::endl;

  S[k-1]=reduce(A+(k-1)*chsz, lastsize);
          ////std::cout << "scan - loc7 n is "<<n << std::endl;

  parallel_for(0, k, [&](int i) {
    T total=reduce(S,i);
    int calcsz=chsz;
    if (i==k-1){
      calcsz=lastsize;
    }
    for (int j=0; j<calcsz;j++){
      T tmp=A[i*chsz+j];
      A[i*chsz+j]=total;
      total+=tmp;
    }
  });



  T res=reduce(S,k);
  free(S);
   ////std::cout << "finish scan--- " << ", and n is "<<n << std::endl;  

  return res;
}


template <class T>
T seq_scan(T *A, size_t n) {
   ////std::cout << "seq scan--- " << ", and n is "<<n << std::endl;  

  T total = 0;
  for (size_t i = 0; i < n; i++) {
    T tmp = A[i];
    A[i] = total;
    total += tmp;
  }
  return total;
}
template <typename T>
T scan_caller(T *A,size_t n) {
  T res;
  if (n>10*scanG){
    res=scan(A,n);
  }
  else{
    res=seq_scan(A,n);
  }

  return res;
  };



template <class T>
void par_partition(T *A, T *B,size_t n, size_t*  fp_s,size_t*  fp_g, size_t*  fp_e ,
size_t& piv_s, size_t& piv_e){
  //fill flag;


  // size_t rand_ind=randInRange(n,112); 
  // T pivotval=A[rand_ind];
  

  
  size_t num_rand = 100;
  size_t piv_canind[num_rand];
  T piv_can[num_rand];
  for (size_t i=0;i<num_rand;i++){
    piv_canind[i] = randInRange(n,i*7);
    piv_can[i]=A[piv_canind[i]];

  }

  std::sort(piv_can, piv_can + num_rand);


  T pivotval=piv_can[num_rand/2];

  //std::cout << "partition--- " << ", and n is "<<n <<" pivot "<<pivotval<< std::endl;  


  parallel_for(0, n, [&](size_t i) {
    if (A[i]<pivotval){
      fp_s[i] =1;
      fp_g[i]=0; 
      fp_e[i]=0;   
    }
    else if(A[i]>pivotval){
      fp_s[i] =0;
      fp_g[i] =1;   
      fp_e[i]=0;
    }
    else{
      fp_s[i] =0;
      fp_g[i] =0;  
      fp_e[i]=1;
    }
  });  


  T e_reduce = par_reduce(fp_e,n);  

  int s_lastflag = 0;
  int e_lastflag = 0;

  //uint64_t g_lastflag = 0;

  if (A[n-1]<pivotval){

    s_lastflag = 1;

  //  g_lastflag = 0;

  }
  else if (A[n-1]==pivotval){
    e_lastflag = 1;
   // g_lastflag = 1;


  }

  ////std::cout << "partition - loc1 " << std::endl;


  size_t s_num = scan_caller(fp_s,n);


   scan_caller(fp_g,n);

  //    //std::cout << "done scan, print out "  << std::endl;
    
  //    //std::cout << "fp_s[]: "  ;
  //  for (size_t i=0;i<n;i++){
  //    //std::cout <<" , "<<fp_s[i]  ;
  //   }  
  //    //std::cout << std::endl  ;

  //    //std::cout << "fp_g[]: "  ;
  //  for (size_t i=0;i<n;i++){
  //    //std::cout <<" , "<<fp_g[i]  ;
  //   }  
  //    //std::cout << std::endl  ;

  //      //std::cout << "fp_e[]: "  ;
  //  for (size_t i=0;i<n;i++){
  //    //std::cout <<" , "<<fp_e[i]  ;
  //   }  
  //    //std::cout << std::endl  ;



  size_t piv_index=s_num;  //this is start;

  //fill those left of the pivot;
  parallel_for(0, n-1, [&](size_t i) {
    if (fp_s[i+1]!=fp_s[i]){
      B[fp_s[i]]=A[i];
    }
    });
  if (s_lastflag ==1){

    B[s_num-1]=A[n-1];
  }  


  //fill the pivot and equal;
  B[piv_index] = pivotval;
  parallel_for(0, e_reduce, [&](size_t i) {
      B[piv_index+i]=pivotval;

    });


  //fill those right of the pivot;
  parallel_for(0, n-1, [&](size_t i) {
    if (fp_g[i+1]!=fp_g[i]){
      B[fp_g[i]+s_num+e_reduce]=A[i];

    }
    });  
  //fill left last index;
  if (s_lastflag==0&&e_lastflag==0){
    B[n-1]=A[n-1];

  }

  //std::cout << "partition - after parallel for" << std::endl;
         //std::cout << "B[]: "  ;
   //for (size_t i=0;i<n;i++){
     //std::cout <<" , "<<B[i]  ;
   // }  
     //std::cout << std::endl  ; 

  piv_s = piv_index;
  piv_e = piv_index+e_reduce-1;
  return;



}

template <class T>
void qsortr(T *A, T *B, size_t n,size_t* fp_s,size_t*  fp_g, size_t*  fp_e) {
  /*
  if (n==1){
   // //std::cout << "qsort returns n is " << n<< std::endl;  

    return;
  }
  if (n==0){
   // //std::cout << "qsort returns n is " << n<< std::endl;  

    return;
  }*/
  size_t threshold = 1e6;
  if(n<threshold){
      std::sort(A, A + n);
      return;
  }
  size_t t_s=0;
  size_t t_e=0;



  par_partition(A,B, n, fp_s,fp_g, fp_e,t_s,t_e);

  parallel_for(0, n, [&](size_t j) {
    A[j]=B[j];

  });

  // for(size_t j=0;j<n;j++){
  //      A[j]=B[j];
  // }


  auto f1 = [&]() { qsortr(A,B, t_s, fp_s,fp_g, fp_e); };
  auto f2 = [&]() { qsortr(A+t_e+1,B+t_e+1, n-t_e-1,fp_s+t_e+1,fp_g+t_e+1, fp_e+t_e+1); };
  par_do(f1, f2);
}

//will call qsortr; allocate arrays which we will use later;
template <class T>
void quicksort(T *A, size_t n) {

  Type* B = (Type*)malloc(n * sizeof(Type));

     size_t* fp_s = (size_t*)malloc(n * sizeof(size_t));
  size_t* fp_g = (size_t*)malloc(n * sizeof(size_t));
   size_t* fp_e = (size_t*)malloc(n * sizeof(size_t));



  qsortr(A, B, n, fp_s, fp_g,fp_e);


  free(fp_s);
  free(fp_g);
  free(fp_e);
  free(B);

}

