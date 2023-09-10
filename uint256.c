/*
 * A simple C library implementation operations on a 256-bit unsigned integer data type
 * CSF Assignment 1 - Milestone 1
 * Yoohyuk Chang
 * ychang82@jhu.edu
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32(uint32_t val) {
  UInt256 result = {0};
  result.data[0] = val;
  return result;
}

// Create a UInt256 value from an array of NWORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 7 is the most significant.
UInt256 uint256_create(const uint32_t data[8]) {
  UInt256 result = {0};
  for (int i = 0; i < 8; i++) {
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex(const char *hex) {
  UInt256 result = {0};
  if (strlen(hex) > 64) {
    char newStr[65];
    for (int i = 0; i < 64; i++) {
      newStr[i] = hex[strlen(hex) - 64 + i];
    }
    newStr[64] = '\0';
    hex = newStr;
  }
  else if (strlen(hex) < 8) {
    result.data[0] = strtoul(hex , NULL, 16);
    return result;
  }

  int len = strlen(hex);
  int uIntIndex = 0;
  
    int remainder = len % 8;
  if (remainder != 0) {
    int lastIndex = len / 8;
    char shortStr[remainder + 1];
    for (int i = 0; i < remainder; i++) {
      shortStr[i] = hex[i];
    }
    result.data[lastIndex] = strtoul(shortStr, NULL, 16);
  }
  for(int i = 0; i < len - remainder; i += 8){
    char factStr[9] = {0};
    for (int j = 0; j < 8; j++) {
    factStr[7 - j] = hex[len - 1 - j - i];
    //printf("%c", hex[len - 1 - j - i]);
    }
    factStr[8] = '\0';  
    //printf("factory String: %s %lu \n", factStr, strlen(factStr));
    result.data[uIntIndex] = strtoul(factStr , NULL, 16);
    uIntIndex++;
  }
  return result;
}


// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = NULL;
  int startIndex;
  for (int i = 7; i >= 0; i--) {
    if (val.data[i] != 0) {
      startIndex = i;
      break;
    }
    if (i == 0 && val.data[i] == 0) {
        hex = malloc(2);  // 1 for '0' and 1 for '\0'
        sprintf(hex, "0");

        return hex;
    }
  }
  int size = (startIndex + 1) * 8 + 1;
  hex = malloc(size);

  char* currentPos = hex;

  for (int i = startIndex; i >= 0; i--) {
    if (i == startIndex) {
            currentPos += sprintf(currentPos, "%x", val.data[i]);  // format without leading 0s for the first non-zero value
        } else {
            currentPos += sprintf(currentPos, "%08x", val.data[i]);  // format with leading 0s for the rest
        }
  }
  return hex;
}


// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 7 is the most
// significant 32 bits.
uint32_t uint256_get_bits(UInt256 val, unsigned index) {
  uint32_t bits;
  bits = val.data[index];
  return bits;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add(UInt256 left, UInt256 right) {
  UInt256 result = {0};  // Initialize all elements to zero
    uint32_t carry = 0;    // Start with no carry

    for (int i = 0; i < 8; i++) {
        uint32_t sumWithoutCarry = left.data[i] + right.data[i];
uint32_t sum = sumWithoutCarry + carry;
if (sumWithoutCarry < left.data[i] || sum < sumWithoutCarry) {
    carry = 1;
} else {
    carry = 0;
}
        result.data[i] = sum;  // Store only the lower 32 bits
    }
    return result;

}

 //int main(){
//   //   char *hex = "A2B3C4D5E6F7890ABCDEF0123456789";
//   // UInt256 result = uint256_create_from_hex(hex);
//   //  for(int i =0; i <8; i++) {
//   //    printf("%u \n", result.data[i]);
//   //  }
//   //  char * str = uint256_format_as_hex(result);
//   // printf("%s", str);
//      UInt256 a = {{1, 0, 0, 0, 0, 0, 0, 0}};
//      UInt256 b = {{1, 0, 0, 0, 0, 0, 0, 0}};
//      UInt256 sum = uint256_sub(a,b);

//     for (int i = 7; i >= 0; i--) {
//         printf("%u ", sum.data[i]);
//      }
  
//  }

  // for (int i = 7; i >= 0; i--) {
  //       printf("%u ", right.data[i]);
  //    }
  // for (int i = 0; i < 8; i++) {
  //   uint32_t sum = left.data[i] + right.data[i]; 
  //   result.data[i] = sum;  
  // }
// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  right = uint256_negate(right);

  result = uint256_add(left, right);
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  for (int i = 0; i < 8; i++) {
        val.data[i] = ~val.data[i];
    }

    // Step 2: Add 1
    uint64_t carry = 1;
    for (int i = 0; i < 8 && carry; i++) {
        uint64_t resultWithCarry = (uint64_t)val.data[i] + carry;
        val.data[i] = (uint32_t)resultWithCarry;  // Take the least significant 32 bits
        carry = resultWithCarry >> 32;            // Take the carry (if any)
    }

    return val;
}

// Return the result of rotating every bit in val nbits to
// the left.  Any bits shifted past the most significant bit
// should be shifted back into the least significant bits.
UInt256 uint256_rotate_left(UInt256 val, unsigned nbits) {
  UInt256 result = {0};

  unsigned int numRotationsOfOneFullBlock = nbits / 32;
  unsigned int numRotationsWithinBlock = nbits % 32;

  for (unsigned int i = 0; i < 8; i++) {
    unsigned int newIndex = (numRotationsOfOneFullBlock + i) % 8;
    result.data[newIndex] = val.data[i];
  }

  UInt256 tempShifted = {0};
  UInt256 tempTrancated = {0};
  for (unsigned int i = 0; i < 8; i++) {
    // temporarily stores the shifted data. It does not include the trancated value yet.
    tempShifted.data[i] = result.data[i] << numRotationsWithinBlock;
    // temporarily stores the trancated values.
    tempTrancated.data[i] = result.data[i] >> (32 - numRotationsWithinBlock); 
  }

  for (unsigned int i = 0; i < 8; i++) {
    unsigned int nextIndex = (i + 1) % 8;
    result.data[nextIndex]  = tempShifted.data[nextIndex] | tempTrancated.data[i];
  }

  return result;
}

// Return the result of rotating every bit in val nbits to
// the right. Any bits shifted past the least significant bit
// should be shifted back into the most significant bits.
UInt256 uint256_rotate_right(UInt256 val, unsigned nbits) {
  UInt256 result = {0};

  unsigned int numRotationsOfOneFullBlock = nbits / 32;
  unsigned int numRotationsWithinBlock = nbits % 32;

  numRotationsOfOneFullBlock = numRotationsOfOneFullBlock % 8;

  for (int i = 7; i >= 0; i--) {
    int newIndex = (i - numRotationsOfOneFullBlock + 8) % 8;
    result.data[newIndex] = val.data[i];
  }

  UInt256 tempShifted = {0};
  UInt256 tempTrancated = {0};
  for (int i = 7; i >= 0; i--) {
    // temporarily stores the shifted data. It does not include the trancated value yet.
    tempShifted.data[i] = result.data[i] >> numRotationsWithinBlock;
    // temporarily stores the trancated values
    tempTrancated.data[i] = result.data[i] << (32 - numRotationsWithinBlock); 
  }

  for (int i = 7; i >= 0; i--) {
    int nextIndex = (i - 1 + 8) % 8;
    result.data[nextIndex]  = tempShifted.data[nextIndex] | tempTrancated.data[i];
  }

  return result;
}
