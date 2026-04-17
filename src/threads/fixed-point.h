/* 
This header file defines all the fixed-point operation's needed by the priority algorithms using in scheduling
*/
#include <stdint.h>


#define F_BITS 14



#define int_to_fixed_p(val) ((val >= 0)?((int64_t)val << F_BITS):-((int64_t)(-val) << F_BITS))
#define fixed_p_to_int(val) (val >> F_BITS)
#define round_fixed(val) ((val < 0)? -((-val + (1 << (F_BITS - 1))) >> F_BITS) :(val + (1 << (F_BITS - 1))) >> F_BITS)


// --------
// fixed-fixed operations
//---------

// add-sub can be totally ignore but they were only added for completeness 
// and also  to avoid using add,sub in a way diff from that of the mult,div
#define add_fixed_fixed(first, second) (first + second)
#define sub_fixed_fixed(first, second) (first - second)

#define mult_fixed_fixed(first, second) ((first * second) >> F_BITS)
//     // example : 
//     // (011 | 10) * (011 | 10) --> 3.5 * 3.5 = 12.25
//     // it wil be 1100|0100 because on multiplying 2 binary numbers 
//     // we place the fraction point at a distance equal to the sum of distances from lsb in both numbers
//     // that it it takes 2 * F_BITS so we need to shift it back (right) by F_BITS to make it consistent again

#define div_fixed_fixed(first, second) ((first << F_BITS) / second)
//     // example :
//     // (10101 | 00101) / (10101 | 00101)  (| : dictates the int-fraction seperator) 
//     // this would be 00000 | 00001
//     // but to make it correct -> 00001 | 00000 we need to shift by the F_BITS post division
//     // but here we mustn't do the first/second first then shift because that would be just an integer divison result shifted
//     // instead we should shift first then divide



// // --------
// // fixed-int operations
// // --------
#define add_fixed_int(first, second) (first + ((int64_t)second << F_BITS))
#define sub_fixed_int(first, second) (first - ((int64_t)second << F_BITS))
#define mult_fixed_int(first, second) (first * second)
#define div_fixed_int(first, second) (first / second)
