#include "fixed-point.h"
#include <stdio.h>



int64_t int_to_fixed_p(int val){
    return val << F_BITS;
}

int fixed_p_to_int(int64_t val){
    return val >> F_BITS;
    // transfers the value from our (assumption of leaving F_BITS for fractions) 
    // to regular ints by positioning the integer part where it should've been placed in an int
}


// --------
// fixed-fixed operations
//---------

// add-sub can be totally ignore but they were only added for completeness 
// and also  to avoid using add,sub in a way diff from that of the mult,div
int64_t add_fixed_fixed(int64_t first, int64_t second){
    return first + second;
}

int64_t sub_fixed_fixed(int64_t first, int64_t second){
    return first - second;
}

int64_t mult_fixed_fixed(int64_t first, int64_t second){
    return (first * second) >> F_BITS;
    // example : 
    // (011 | 10) * (011 | 10) --> 3.5 * 3.5 = 12.25
    // it wil be 1100|0100 because on multiplying 2 binary numbers 
    // we place the fraction point at a distance equal to the sum of distances from lsb in both numbers
    // that it it takes 2 * F_BITS so we need to shift it back (right) by F_BITS to make it consistent again
}

int64_t div_fixed_fixed(int64_t first, int64_t second){
    return (first / second) << F_BITS;
    // example :
    // (10101 | 00101) / (10101 | 00101)  (| : dictates the int-fraction seperator) 
    // this would be 00000 | 00001
    // but to make it correct -> 00001 | 00000 we need to shift by the F_BITS post division
}


// --------
// fixed-int operations
// --------
int64_t add_fixed_int(int64_t first, int second){
    return first + ((int64_t)second << F_BITS);
}


// to get (fixed-point - int) you can use this function and multiple by a -1
int64_t sub_fixed_int(int64_t first, int second){
    return first - ((int64_t)second << F_BITS);
}


int64_t mult_fixed_int(int64_t first, int second){
    return first * second;
}

int64_t div_fixed_int(int64_t first, int second){
    return first / second;
}

int round_fixed(int64_t val){
    return  (val < 0)? 
            (val - (1 << (F_BITS - 1))>> F_BITS) :
            (val + (1 << (F_BITS - 1))>> F_BITS);
            //  why 1 << (F_BITS - 1) because we want to add /subtract (2^F_BITS / 2) 
            // that is (2 ^ (F_BITS - 1)) which is 1 <<( F_BITS - 1)
}


//  TESTING
int main() {
    



}