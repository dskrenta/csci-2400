/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/* We do not support C11 <threads.h>.  */
/* 
 * bitOr - x|y using only ~ and & 
 *   Example: bitOr(6, 5) = 7
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitOr(int x, int y) {
    // x or y are true if x and y are not both false
    // one of Demorgan's laws
    // converts 0s to 1s, compares, switches again
         
    return ~(~x&~y);
}
/* 
 * evenBits - return word with all even-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int evenBits(void) {
    // 0x55 in binary is 01010101
    // shift left 25
    // shift left 16
    // shift left 8
    // return 32 bit word with all even bits set to 1
    
    int first = 0x55 << 24;
    int second = 0x55 << 16;
    int third = 0x55 << 8;
    int fourth = 0x55;
    
    return first | second | third | fourth;
}
/* 
 * minusOne - return a value of -1 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int minusOne(void) {
    // 0 in binary is 00000000
    // 0 flipped is 11111111
    // signed equals -1
    
    return ~0;
}
/* 
 * allEvenBits - return 1 if all even-numbered bits in word set to 1
 *   Examples allEvenBits(0xFFFFFFFE) = 0, allEvenBits(0x55555555) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allEvenBits(int x) {
    // tear 16 bits off end and compare with x
    // tear 8 bits off end and compare with x
    // tear 4 bits off end and compare with x
    // tear 2 bits off end and compare with x
    // compare x with 1
    
    x = x & (x >> 16);
    x = x & (x >> 8);
    x = x & (x >> 4);
    x = x & (x >> 2);
    
    return x & 1;
}
/* 
 * anyOddBit - return 1 if any odd-numbered bit in word set to 1
 *   Examples anyOddBit(0x5) = 0, anyOddBit(0x7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int anyOddBit(int x) {
    // 0xaa is 10101010 in binary
    // mask to get odd result
    // x & res to check if any odd digits are 1
    // !! to change to 0 or 1 based on any odd digits that are 1
    
    int res = 0xaa;
    res = res | res << 8;
    res = res | res << 16;
    return !!(x & res);
}
/* 
 * byteSwap - swaps the nth byte and the mth byte
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2
 */
int byteSwap(int x, int n, int m) {
    int temp = 0;
    // left shift n and m because between 0 and 3
    n = n << 3;
    m = m << 3;
    // and compare 0xff to right shifted x by n and m and xor x >> m
    // get difference between shifted bits
    // and it to 0xff to fill remaining bits
    temp = 0xff & ((x >> n) ^ (x >> m));
    // move bits in x based on values form temp
    // xor to get bits from temp << n and temp << n into x
    // xor to only get changed bits
    x = x ^ (temp << n);
    x = x ^ (temp << m);
    return x;
}
/* 
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
    // most significant bit of sum
    int temp = (x + y) >> 31;
    // most significant bit of x
    x = x >> 31;
    // most significant bit of y
    y = y >> 31;
    // will return 1 if temp is not 1 when x and y are 0 or temp is 0 when x and y are 1
    return !!(~temp | x | y) & (temp | ~x | ~y);
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    // puts x in 0 or 1 form
    // x is now either all 1s or all 0s
    x = !!x;
    x = ~x+1;
    // returns ~x & z if x & y is 0
    return (x & y) | (~x & z);
}
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
    // get sign identifier
    int sign = 1 << 31;
    // if greater than 0x39 is added from x result will be negative
    int upper = ~(sign | 0x39);
    // if less than 0x30 is added result will be negative
    int lower = ~0x30;
    // add x and check sign bit
    upper = sign & (upper + x) >> 31;
    // add x and check sign bit
    lower = sign & (lower + 1 + x) >> 31;
    // if either is negative, x falls outside the range of ascii digits
    return !(upper | lower);
}
/* 
 * replaceByte(x,n,c) - Replace byte n in x with c
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: replaceByte(0x12345678,1,0xab) = 0x1234ab78
 *   You can assume 0 <= n <= 3 and 0 <= c <= 255
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 3
 */
int replaceByte(int x, int n, int c) {
    // 11111111
    int mask = 0xff;
    // shift by 3 because n between 0 and 3
    int shift = n << 3;
    // shift by 0xff, flip to get 0s
    mask = ~(mask << shift);
    // shift bits of c into nth byte
    c = c << shift;
    // zero out nth byte, replace it with byte from x
    return (x & mask) | c;
}
/* reverseBits - reverse the bits in a 32-bit integer,
              i.e. b0 swaps with b31, b1 with b30, etc
 *  Examples: reverseBits(0x11111111) = 0x88888888
 *            reverseBits(0xdeadbeef) = 0xf77db57b
 *            reverseBits(0x88888888) = 0x11111111
 *            reverseBits(0)  = 0
 *            reverseBits(-1) = -1
 *            reverseBits(0x9) = 0x90000000
 *  Legal ops: ! ~ & ^ | + << >> and unsigned int type
 *  Max ops: 90
 *  Rating: 4
 */
int reverseBits(int x) {
    // unsign x
    unsigned reverseX = x;
    
    // create masks for each reverse
    int mask8 = 0xff | (0xff << 16);
    int mask4 = mask8 ^ (mask8 << 4);
    int mask2 = mask4 ^ (mask4 << 2);
    int mask1 = mask2 ^ (mask2 << 1);
    
    // reverse bits at each step (1, 2, 4, 8, 16)
    reverseX = (reverseX >> 16) | (reverseX << 16);
    reverseX = ((reverseX & ~mask8) >>  8) | ((reverseX & mask8) <<  8);
    reverseX = ((reverseX & ~mask4) >>  4) | ((reverseX & mask4) <<  4);
    reverseX = ((reverseX & ~mask2) >>  2) | ((reverseX & mask2) <<  2);
    reverseX = ((reverseX & ~mask1) >>  1) | ((reverseX & mask1) <<  1);
    
    return reverseX;
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
int satAdd(int x, int y) {
    // sum x and y
    int sum = x + y;
    // get negative overflow
    int lowerOverflow = 0x1 << 31;
    // flips lowerOverflow to get upperOverflow
    int upperOverflow = ~lowerOverflow;
    // mask of overflow if exists
    int overflowMask = ((x ^ sum) & (y ^ sum)) >> 31;
    // determines upper or lower overflow
    int sign = x >> 31;
    // conditional to determine if x and y can be added, if they cannot return the overflow
    return ((sum) & ~overflowMask) | (overflowMask & ((sign & lowerOverflow) | (~sign & upperOverflow)));
}
/*
 * Extra credit
 */
/* 
 * float_abs - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_abs(unsigned uf) {
    // mask sign bit, check if all exponent bits are set to 1
    unsigned absRes = uf & 0x7fffffff;

    // if the frac bits are some value return res
    if ((absRes & 0x7f800000) ^ 0x7f800000) {
        return absRes;
    }
    // return the absolute value
    else {
        if (absRes << 9) {
            return uf;
        }
        else {
            return absRes;
        }
    }
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) {
    // get components of fraction
	unsigned sign = uf >> 31;
	unsigned exp = (uf >> 23) & 0xFF;
	unsigned frac =(uf & 0x7FFFFF);
	unsigned bias = 0x7F;
	unsigned res = frac;
  
    // special cases: NaN and infinity
    if (exp == 0xFF) {
        return 0x80000000u;
    }
  
    // denormalized case and normalized exp less than bias cases
    if (exp < bias) {
        return 0x0;
    }
  
    // normalized cases
    exp -= bias;
  
    // overflow case
    if (exp >= 31) {
        return 0x80000000u;
    }
  
    // get integer result after shift corresponding bits
    if (exp > 22) {
        res = frac << (exp - 23);
    }
    else { 
        res = frac >> (23 - exp);
    }

    // add 1 << exp for normalized case
    res += 1 << exp;
  
    // if the sign is 1 change the sign
    if (sign) {
        res = -res;
    }
  
    return res;
}
/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
    // mask important components
    unsigned fraction = uf & 0x7fffff;
    unsigned exponent = uf & 0x7f800000;
    unsigned sign = uf & 0x80000000;
    unsigned rounder = 0;

    // return argument when NaN or infinity
    if (!((uf & 0x7f800000) ^ 0x7f800000)) {
        return uf;
    }

    // if exponent is greather than 1, substract 1 and divide uf by 2
    if (exponent > 0x800000) {
        return sign + (exponent - 0x800000) + fraction;
    }
    // is exponent is less or equal to 1, shift fraction to right by 1 and divide uf by 2
    // rounder value for case where fraction needs to be rounded to be even occurs when 2 lsb are both set to 1
    else {
        if ((fraction & 0x3) ^ 0x3) {
            rounder = 0;
        }
        else {
            rounder = 1;
        }
        return sign + ((exponent + fraction) >> 1) + rounder;
    }
}
