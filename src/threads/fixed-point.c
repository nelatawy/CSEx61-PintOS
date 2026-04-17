#include "fixed-point.h"
#include <stdio.h>



int64_t int_to_fixed_p(int val){
    return (val >= 0)?((int64_t)val << F_BITS):-((int64_t)(-val) << F_BITS);
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
    return ((first << F_BITS) / second) ;
    // example :
    // (10101 | 00101) / (10101 | 00101)  (| : dictates the int-fraction seperator) 
    // this would be 00000 | 00001
    // but to make it correct -> 00001 | 00000 we need to shift by the F_BITS post division
    // but here we mustn't do the first/second first then shif because that would be just an integer divison result shifted
    // instead we should shift first then divide
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
            -((-val + (1 << (F_BITS - 1))) >> F_BITS) :
            (val + (1 << (F_BITS - 1))) >> F_BITS;
            //  why 1 << (F_BITS - 1) because we want to add /subtract (2^F_BITS / 2) 
            // that is (2 ^ (F_BITS - 1)) which is 1 <<( F_BITS - 1)
}

/* Test function for fixed-point arithmetic operations */
void test_fixed_point(void) {
    printf("\n=== FIXED-POINT ARITHMETIC TESTS ===\n");
    int pass_count = 0, fail_count = 0;
    
    /* Test 1: Basic Conversions */
    printf("\nTest 1: int_to_fixed_p and fixed_p_to_int\n");
    int x = 5;
    int64_t fx = int_to_fixed_p(x);
    int result = fixed_p_to_int(fx);
    if (result == x) { printf("  5 -> fixed -> 5 [PASS]\n"); pass_count++; } 
    else { printf("  5 -> fixed -> %d [FAIL]\n", result); fail_count++; }
    
    /* Test 2: Zero Conversion */
    printf("\nTest 2: Zero conversion\n");
    int64_t fzero = int_to_fixed_p(0);
    int zero_back = fixed_p_to_int(fzero);
    if (zero_back == 0) { printf("  0 -> fixed -> 0 [PASS]\n"); pass_count++; } 
    else { printf("  0 -> fixed -> %d [FAIL]\n", zero_back); fail_count++; }
    
    /* Test 3: Negative Number Conversion */
    printf("\nTest 3: Negative number conversion\n");
    int neg = -10;
    int64_t fneg = int_to_fixed_p(neg);
    int neg_back = fixed_p_to_int(fneg);
    if (neg_back == neg) { printf("  -10 -> fixed -> -10 [PASS]\n"); pass_count++; } 
    else { printf("  -10 -> fixed -> %d [FAIL]\n", neg_back); fail_count++; }
    
    /* Test 4: Large Number Conversion */
    printf("\nTest 4: Large number conversion\n");
    int large = 1000;
    int64_t flarge = int_to_fixed_p(large);
    int large_back = fixed_p_to_int(flarge);
    if (large_back == large) { printf("  1000 -> fixed -> 1000 [PASS]\n"); pass_count++; } 
    else { printf("  1000 -> fixed -> %d [FAIL]\n", large_back); fail_count++; }
    
    /* Test 5: Fixed-Fixed Addition */
    printf("\nTest 5: add_fixed_fixed (3 + 2 = 5)\n");
    int64_t fa = int_to_fixed_p(3);
    int64_t fb = int_to_fixed_p(2);
    int64_t fsum = add_fixed_fixed(fa, fb);
    int sum_result = fixed_p_to_int(fsum);
    if (sum_result == 5) { printf("  3 + 2 = 5 [PASS]\n"); pass_count++; } 
    else { printf("  3 + 2 = %d [FAIL]\n", sum_result); fail_count++; }
    
    /* Test 6: Fixed-Fixed Subtraction */
    printf("\nTest 6: sub_fixed_fixed (5 - 3 = 2)\n");
    int64_t fdiff = sub_fixed_fixed(fsum, fa);
    int diff_result = fixed_p_to_int(fdiff);
    if (diff_result == 2) { printf("  5 - 3 = 2 [PASS]\n"); pass_count++; } 
    else { printf("  5 - 3 = %d [FAIL]\n", diff_result); fail_count++; }
    
    /* Test 7: Fixed-Fixed Subtraction with Negatives */
    printf("\nTest 7: sub_fixed_fixed (2 - 5 = -3)\n");
    int64_t fdiff_neg = sub_fixed_fixed(fb, fa);
    int diff_neg_result = fixed_p_to_int(fdiff_neg);
    if (diff_neg_result == -1) { printf("  2 - 3 = -1 [PASS]\n"); pass_count++; } 
    else { printf("  2 - 3 = %d [FAIL]\n", diff_neg_result); fail_count++; }
    
    /* Test 8: Fixed-Fixed Multiplication */
    printf("\nTest 8: mult_fixed_fixed (3 * 2 = 6)\n");
    int64_t fmul = mult_fixed_fixed(fa, fb);
    int mul_result = fixed_p_to_int(fmul);
    if (mul_result == 6) { printf("  3 * 2 = 6 [PASS]\n"); pass_count++; } 
    else { printf("  3 * 2 = %d [FAIL]\n", mul_result); fail_count++; }
    
    /* Test 9: Fixed-Fixed Multiplication by Zero */
    printf("\nTest 9: mult_fixed_fixed (5 * 0 = 0)\n");
    int64_t f5 = int_to_fixed_p(5);
    int64_t fmul_zero = mult_fixed_fixed(f5, fzero);
    int mul_zero = fixed_p_to_int(fmul_zero);
    if (mul_zero == 0) { printf("  5 * 0 = 0 [PASS]\n"); pass_count++; } 
    else { printf("  5 * 0 = %d [FAIL]\n", mul_zero); fail_count++; }
    
    /* Test 10: Fixed-Integer Addition */
    printf("\nTest 10: add_fixed_int (3 + 4 = 7)\n");
    int64_t f3 = int_to_fixed_p(3);
    int64_t f3_plus_4 = add_fixed_int(f3, 4);
    int add_int_result = fixed_p_to_int(f3_plus_4);
    if (add_int_result == 7) { printf("  3 + 4 = 7 [PASS]\n"); pass_count++; } 
    else { printf("  3 + 4 = %d [FAIL]\n", add_int_result); fail_count++; }
    
    /* Test 11: Fixed-Integer Subtraction */
    printf("\nTest 11: sub_fixed_int (10 - 3 = 7)\n");
    int64_t f10 = int_to_fixed_p(10);
    int64_t f10_minus_3 = sub_fixed_int(f10, 3);
    int sub_int_result = fixed_p_to_int(f10_minus_3);
    if (sub_int_result == 7) { printf("  10 - 3 = 7 [PASS]\n"); pass_count++; } 
    else { printf("  10 - 3 = %d [FAIL]\n", sub_int_result); fail_count++; }
    
    /* Test 12: Fixed-Integer Multiplication */
    printf("\nTest 12: mult_fixed_int (3 * 2 = 6)\n");
    int64_t fmul_int = mult_fixed_int(f3, 2);
    int mul_int_result = fixed_p_to_int(fmul_int);
    if (mul_int_result == 6) { printf("  3 * 2 = 6 [PASS]\n"); pass_count++; } 
    else { printf("  3 * 2 = %d [FAIL]\n", mul_int_result); fail_count++; }
    
    /* Test 13: Fixed-Integer Division */
    printf("\nTest 13: div_fixed_int (6 / 2 = 3)\n");
    int64_t f6 = int_to_fixed_p(6);
    int64_t fdiv = div_fixed_int(f6, 2);
    int div_result = fixed_p_to_int(fdiv);
    if (div_result == 3) { printf("  6 / 2 = 3 [PASS]\n"); pass_count++; } 
    else { printf("  6 / 2 = %d [FAIL]\n", div_result); fail_count++; }
    
    /* Test 14: Larger Multiplications */
    printf("\nTest 14: mult_fixed_fixed (10 * 5 = 50)\n");
    int64_t f10f = int_to_fixed_p(10);
    int64_t f5f = int_to_fixed_p(5);
    int64_t fmul_large = mult_fixed_fixed(f10f, f5f);
    int mul_large = fixed_p_to_int(fmul_large);
    if (mul_large == 50) { printf("  10 * 5 = 50 [PASS]\n"); pass_count++; } 
    else { printf("  10 * 5 = %d [FAIL]\n", mul_large); fail_count++; }
    
    /* Test 15: Fixed-Fixed Division */
    printf("\nTest 15: div_fixed_fixed (20 / 4 = 5)\n");
    int64_t f20 = int_to_fixed_p(20);
    int64_t f4 = int_to_fixed_p(4);
    int64_t fdiv_fixed = div_fixed_fixed(f20, f4);
    int div_fixed = fixed_p_to_int(fdiv_fixed);
    if (div_fixed == 5) { printf("  20 / 4 = 5 [PASS]\n"); pass_count++; } 
    else { printf("  20 / 4 = %d [FAIL]\n", div_fixed); fail_count++; }
    
    /* Test 16: Negative Multiplication */
    printf("\nTest 16: mult_fixed_fixed (-3 * 2 = -6)\n");
    int64_t fneg3 = int_to_fixed_p(-3);
    int64_t fmul_neg = mult_fixed_fixed(fneg3, fb);
    int mul_neg = fixed_p_to_int(fmul_neg);
    if (mul_neg == -6) { printf("  -3 * 2 = -6 [PASS]\n"); pass_count++; } 
    else { printf("  -3 * 2 = %d [FAIL]\n", mul_neg); fail_count++; }
    
    /* ========== ROUNDING TESTS ========== */
    printf("\n--- ROUNDING TESTS ---\n");
    
    /* Test 17: Round integer (exact, no fraction) */
    printf("\nTest 17: round_fixed (5.0 -> 5)\n");
    int64_t f5_exact = int_to_fixed_p(5);
    int round_exact = round_fixed(f5_exact);
    if (round_exact == 5) { printf("  5.0 rounds to 5 [PASS]\n"); pass_count++; } 
    else { printf("  5.0 rounds to %d [FAIL]\n", round_exact); fail_count++; }
    
    /* Test 18: Round small positive fraction (< 0.5) */
    printf("\nTest 18: round_fixed (5.25 -> 5)\n");
    int64_t f5_quarter = int_to_fixed_p(5) + (1 << (F_BITS - 2));  /* 5 + 0.25 */
    int round_quarter = round_fixed(f5_quarter);
    if (round_quarter == 5) { printf("  5.25 rounds to 5 [PASS]\n"); pass_count++; } 
    else { printf("  5.25 rounds to %d [FAIL]\n", round_quarter); fail_count++; }
    
    /* Test 19: Round large positive fraction (> 0.5) */
    printf("\nTest 19: round_fixed (5.75 -> 6)\n");
    int64_t f5_three_quarter = int_to_fixed_p(5) + (3 * (1 << (F_BITS - 2)));  /* 5 + 0.75 */
    int round_three_quarter = round_fixed(f5_three_quarter);
    if (round_three_quarter == 6) { printf("  5.75 rounds to 6 [PASS]\n"); pass_count++; } 
    else { printf("  5.75 rounds to %d [FAIL]\n", round_three_quarter); fail_count++; }
    
    /* Test 20: Round negative integer (exact) */
    printf("\nTest 20: round_fixed (-5.0 -> -5)\n");
    int64_t fneg5_exact = int_to_fixed_p(-5);
    int round_neg_exact = round_fixed(fneg5_exact);
    if (round_neg_exact == -5) { printf("  -5.0 rounds to -5 [PASS]\n"); pass_count++; } 
    else { printf("  -5.0 rounds to %d [FAIL]\n", round_neg_exact); fail_count++; }
    
    /* Test 21: Round negative fraction (< 0.5) */
    printf("\nTest 21: round_fixed (-5.25 -> -5)\n");
    int64_t fneg5_quarter = int_to_fixed_p(-5) - (1 << (F_BITS - 2));  /* -5 - 0.25 */
    int round_neg_quarter = round_fixed(fneg5_quarter);
    if (round_neg_quarter == -5) { printf("  -5.25 rounds to -5 [PASS]\n"); pass_count++; } 
    else { printf("  -5.25 rounds to %d [FAIL]\n", round_neg_quarter); fail_count++; }
    
    /* Test 22: Round negative fraction (> 0.5) */
    printf("\nTest 22: round_fixed (-5.75 -> -6)\n");
    int64_t fneg5_three_quarter = int_to_fixed_p(-5) - (3 * (1 << (F_BITS - 2)));  /* -5 - 0.75 */
    int round_neg_three_quarter = round_fixed(fneg5_three_quarter);
    if (round_neg_three_quarter == -6) { printf("  -5.75 rounds to -6 [PASS]\n"); pass_count++; } 
    else { printf("  -5.75 rounds to %d [FAIL]\n", round_neg_three_quarter); fail_count++; }
    
    /* Test 23: Round zero with fraction */
    printf("\nTest 23: round_fixed (0.75 -> 1)\n");
    int64_t f0_three_quarter = (3 * (1 << (F_BITS - 2)));  /* 0.75 */
    int round_0_three_quarter = round_fixed(f0_three_quarter);
    if (round_0_three_quarter == 1) { printf("  0.75 rounds to 1 [PASS]\n"); pass_count++; } 
    else { printf("  0.75 rounds to %d [FAIL]\n", round_0_three_quarter); fail_count++; }
    
    /* Test 24: Round small positive near zero */
    printf("\nTest 24: round_fixed (0.25 -> 0)\n");
    int64_t f0_quarter = (1 << (F_BITS - 2));  /* 0.25 */
    int round_0_quarter = round_fixed(f0_quarter);
    if (round_0_quarter == 0) { printf("  0.25 rounds to 0 [PASS]\n"); pass_count++; } 
    else { printf("  0.25 rounds to %d [FAIL]\n", round_0_quarter); fail_count++; }
    

    /* Test 25: custom */
    // printf("\nTest 25: custom (2*0.57 / (2*0.57 + 1))\n");
    // int64_t numer = div_fixed_int(int_to_fixed_p(36),50);  
    // int64_t denom = add_fixed_int(div_fixed_int(int_to_fixed_p(36),50),1);
    // int64_t old_rec = div_fixed_int(int_to_fixed_p(16693),100); 
    // int64_t factor = div_fixed_fixed(numer, denom);
    // printf("Result = %d\n", round_fixed(mult_fixed_int(mult_fixed_fixed(factor, old_rec), 100)));
    

        /* Test 25: custom */
    printf("\nTest 25: custom (2*0.57 / (2*0.57 + 1))\n");
    int64_t fixed_load_avg = div_fixed_int(int_to_fixed_p(70),100);
    int64_t res = div_fixed_int(
        int_to_fixed_p(70*59 + 100)
    ,60);

    int64_t curr_part = int_to_fixed_p(1);

    // int64_t res = 
    //     div_fixed_int(
    //     mult_fixed_int(add_fixed_fixed(prev_part, curr_part),100)
    //     ,60);
    // );
    // int64_t res = add_fixed_fixed(prev_part, curr_part);
    printf("Result = %ld\n", round_fixed(res));
    


    printf("\n=== TEST SUMMARY ===\n");
    printf("Passed: %d\n", pass_count);
    printf("Failed: %d\n", fail_count);
    printf("Total:  %d\n\n", pass_count + fail_count);
}


// int main(){
//     test_fixed_point();
// }