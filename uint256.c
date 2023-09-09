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
  UInt256 result;
  // TODO: implement
  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex(UInt256 val) {
  char *hex = NULL;
  // TODO: implement
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
  UInt256 sum;
  // TODO: implement
  uint32_t leftOver = 0;
  for (unsigned int i = 0; i < 8; i++) {
    uint32_t leftElement = left.data[i];
    uint32_t rightElement = right.data[i];
    uint32_t sumOfLeftAndRight = leftElement + rightElement;
    sumOfLeftAndRight = sumOfLeftAndRight + leftOver;
    if (sumOfLeftAndRight < leftElement) {
      leftOver = sumOfLeftAndRight;
    }
    sum.data[i] = sumOfLeftAndRight;
  }
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub(UInt256 left, UInt256 right) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate(UInt256 val) {
  UInt256 result;
  // TODO: implement
  return result;
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
