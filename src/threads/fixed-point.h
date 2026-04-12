/* 
This header file defines all the fixed-point operation's needed by the priority algorithms using in scheduling
*/
#include <stdint.h>


#define F_BITS 14

// conversions
int64_t int_to_fixed_p(int);

int fixed_p_to_int(int64_t);


// fixed-fixed operations
int64_t add_fixed_fixed(int64_t, int64_t);

int64_t sub_fixed_fixed(int64_t, int64_t);

int64_t mult_fixed_fixed(int64_t, int64_t);

int64_t div_fixed_fixed(int64_t, int64_t);


// fixed-int operations
int64_t add_fixed_int(int64_t, int);

int64_t sub_fixed_int(int64_t, int);

int64_t mult_fixed_int(int64_t, int);

int64_t div_fixed_int(int64_t, int);

int round_fixed(int64_t);
