#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "uint256.h"

typedef struct {
  UInt256 zero; // the value equal to 0
  UInt256 one;  // the value equal to 1
  UInt256 max;  // the value equal to (2^256)-1
  UInt256 msb_set; // the value equal to 2^255
  UInt256 rot; // value used to test rotations
} TestObjs;

// Helper functions for implementing tests
void set_all(UInt256 *val, uint32_t wordval);

#define ASSERT_SAME(expected, actual) \
do { \
  ASSERT(expected.data[0] == actual.data[0]); \
  ASSERT(expected.data[1] == actual.data[1]); \
  ASSERT(expected.data[2] == actual.data[2]); \
  ASSERT(expected.data[3] == actual.data[3]); \
  ASSERT(expected.data[4] == actual.data[4]); \
  ASSERT(expected.data[5] == actual.data[5]); \
  ASSERT(expected.data[6] == actual.data[6]); \
  ASSERT(expected.data[7] == actual.data[7]); \
} while (0)

#define INIT_FROM_ARR(val, arr) \
do { \
  for (unsigned i = 0; i < 8; ++i) \
    val.data[i] = arr[i]; \
} while (0)

// Functions to create and cleanup the test fixture object
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// Declarations of test functions
void test_get_bits(TestObjs *objs);
void test_create_from_u32(TestObjs *objs);
void test_create(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_rotate_left(TestObjs *objs);
void test_rotate_right(TestObjs *objs);

// Additional Tests ***
void test_add_zero();
void test_add_addition_overflow();

void test_subtract_zero();
void test_subtract_negatively_overflow();

void test_rotate_left_no_rotation(); // not rotating by setting nbits to 0 in left rotation
void test_rotate_left_multiple_of_32(); // rotating left multiple of 32 (number of bits within one element of an array)
void test_rotate_left_multiple_of_256(); // rotating left multiple of 256 (total number of bits of the whole data)
void test_rotate_left_more_than_256(); // rotating left more than 256
void test_rotate_left_less_than_32(); // rotating left less than 32
void test_rotate_left_between_32_and_256(); // rotating left between 32 and 256

void test_rotate_right_no_rotation(); // not rotating by setting nbits to 0 in right rotation
void test_rotate_right_multiple_of_32(); // rotating right multiple of 32 (number of bits within one element of an array)
void test_rotate_right_multiple_of_256(); // rotating right multiple of 256 (total number of bits of the whole data)
void test_rotate_right_more_than_256(); // rotating right more than 256
void test_rotate_right_less_than_32(); // rotating right less than 32
void test_rotate_right_between_32_and_256(); // rotating right between 32 and 256


int main(int argc, char **argv) {
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_get_bits);
  TEST(test_create_from_u32);
  TEST(test_create);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_add);
  TEST(test_sub);
  TEST(test_negate);
  TEST(test_rotate_left);
  TEST(test_rotate_right);
  
  // Additional Tests ***
  TEST(test_add_zero);
  TEST(test_add_addition_overflow);

  TEST(test_subtract_zero);
  TEST(test_subtract_negatively_overflow);
  
  TEST(test_rotate_left_no_rotation);
  TEST(test_rotate_left_multiple_of_32);
  TEST(test_rotate_left_multiple_of_256);
  TEST(test_rotate_left_more_than_256);
  TEST(test_rotate_left_less_than_32);
  TEST(test_rotate_left_between_32_and_256);

  TEST(test_rotate_right_no_rotation);
  TEST(test_rotate_right_multiple_of_32);
  TEST(test_rotate_right_multiple_of_256);
  TEST(test_rotate_right_more_than_256);
  TEST(test_rotate_right_less_than_32);
  TEST(test_rotate_right_between_32_and_256);

  TEST_FINI();
}

// Set all of the "words" of a UInt256 to a specific initial value
void set_all(UInt256 *val, uint32_t wordval) {
  for (unsigned i = 0; i < 8; ++i) {
    val->data[i] = wordval;
  }
}

TestObjs *setup(void) {
  TestObjs *objs = (TestObjs *) malloc(sizeof(TestObjs));

  // initialize several UInt256 values "manually"
  set_all(&objs->zero, 0);
  set_all(&objs->one, 0);
  objs->one.data[0] = 1U;
  set_all(&objs->max, 0xFFFFFFFFU);

  // create a value with only the most-significant bit set
  uint32_t msb_set_data[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x80000000U };
  INIT_FROM_ARR(objs->msb_set, msb_set_data);

  // value with nonzero values in least significant and most significant words
  // Note that the hex representation of this value is
  //   CD000000 00000000 00000000 00000000 00000000 00000000 00000000 000000AB
  uint32_t rot_data[8] = { 0x000000ABU, 0U, 0U, 0U, 0U, 0U, 0U, 0xCD000000U };
  INIT_FROM_ARR(objs->rot, rot_data);

  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_get_bits(TestObjs *objs) {
  ASSERT(0U == uint256_get_bits(objs->zero, 0));
  ASSERT(0U == uint256_get_bits(objs->zero, 1));
  ASSERT(0U == uint256_get_bits(objs->zero, 2));
  ASSERT(0U == uint256_get_bits(objs->zero, 3));
  ASSERT(0U == uint256_get_bits(objs->zero, 4));
  ASSERT(0U == uint256_get_bits(objs->zero, 5));
  ASSERT(0U == uint256_get_bits(objs->zero, 6));
  ASSERT(0U == uint256_get_bits(objs->zero, 7));

  ASSERT(1U == uint256_get_bits(objs->one, 0));
  ASSERT(0U == uint256_get_bits(objs->one, 1));
  ASSERT(0U == uint256_get_bits(objs->one, 2));
  ASSERT(0U == uint256_get_bits(objs->one, 3));
  ASSERT(0U == uint256_get_bits(objs->one, 4));
  ASSERT(0U == uint256_get_bits(objs->one, 5));
  ASSERT(0U == uint256_get_bits(objs->one, 6));
  ASSERT(0U == uint256_get_bits(objs->one, 7));

  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 0));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 1));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 2));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 3));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 4));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 5));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 6));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 7));
}

void test_create_from_u32(TestObjs *objs) {
  UInt256 zero = uint256_create_from_u32(0U);
  UInt256 one = uint256_create_from_u32(1U);

  ASSERT_SAME(objs->zero, zero);
  ASSERT_SAME(objs->one, one);
}

void test_create(TestObjs *objs) {
  (void) objs;

  uint32_t data1[8] = { 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U };
  UInt256 val1 = uint256_create(data1);
  ASSERT(1U == val1.data[0]);
  ASSERT(2U == val1.data[1]);
  ASSERT(3U == val1.data[2]);
  ASSERT(4U == val1.data[3]);
  ASSERT(5U == val1.data[4]);
  ASSERT(6U == val1.data[5]);
  ASSERT(7U == val1.data[6]);
  ASSERT(8U == val1.data[7]);
}

void test_create_from_hex(TestObjs *objs) {
  UInt256 zero = uint256_create_from_hex("0");
  ASSERT_SAME(objs->zero, zero);

  UInt256 one = uint256_create_from_hex("1");
  ASSERT_SAME(objs->one, one);
  

  UInt256 max = uint256_create_from_hex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  ASSERT_SAME(objs->max, max);
}

void test_format_as_hex(TestObjs *objs) {
  char *s;

  s = uint256_format_as_hex(objs->zero);
  ASSERT(0 == strcmp("0", s));
  free(s);

  s = uint256_format_as_hex(objs->one);
  ASSERT(0 == strcmp("1", s));
  free(s);

  s = uint256_format_as_hex(objs->max);
  ASSERT(0 == strcmp("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s));
  free(s);
}

void test_add(TestObjs *objs) {
  UInt256 result;

  result = uint256_add(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_add(objs->zero, objs->one);
  ASSERT_SAME(objs->one, result);

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR(two, two_data);
  result = uint256_add(objs->one, objs->one);
  ASSERT_SAME(two, result);

  result = uint256_add(objs->max, objs->one);
  ASSERT_SAME(objs->zero, result);
}

void test_sub(TestObjs *objs) {
  UInt256 result;

  result = uint256_sub(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->one, objs->one);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->zero, objs->one);
  ASSERT_SAME(objs->max, result);
}

void test_negate(TestObjs *objs) {
  UInt256 result;

  result = uint256_negate(objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_negate(objs->one);
  ASSERT_SAME(objs->max, result);

  result = uint256_negate(objs->max);
  ASSERT_SAME(objs->one, result);
}

void test_rotate_left(TestObjs *objs) {
  UInt256 result;

  // rotating the value with just the most significant bit set
  // one position to the left should result in the value equal to 1
  // (i.e., the value with only the least significant bit set)
  result = uint256_rotate_left(objs->msb_set, 1);
  ASSERT_SAME(objs->one, result);

  // after rotating the "rot" value left by 4 bits, the resulting value should be
  //   D0000000 00000000 00000000 00000000 00000000 00000000 00000000 00000ABC
  result = uint256_rotate_left(objs->rot, 4);
  ASSERT(0x00000ABCU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xD0000000U == result.data[7]);
}

void test_rotate_right(TestObjs *objs) {
  UInt256 result;

  // rotating 1 right by 1 position should result in a value with just
  // the most-significant bit set
  result = uint256_rotate_right(objs->one, 1);
  ASSERT_SAME(objs->msb_set, result);

  // after rotating the "rot" value right by 4 bits, the resulting value should be
  //   BCD00000 00000000 00000000 00000000 00000000 00000000 00000000 0000000A
  result = uint256_rotate_right(objs->rot, 4);
  ASSERT(0x0000000AU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xBCD00000U == result.data[7]);
}





/*
 * Additional Test cases written by Yoohyuk Chang and Yongjae Lee
 */

// Test function to add 0 to a value
void test_add_zero() {
  UInt256 test = {0};
  test.data[0] = 0x00000011U;
  test.data[1] = 0x00000022U;
  test.data[2] = 0x00000033U;
  test.data[3] = 0x00000044U;
  test.data[4] = 0x00000055U;
  test.data[5] = 0x00000066U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000088U;

  UInt256 zero = {0}; // UInt256 representing zero

  UInt256 result = uint256_add(test, zero);

  ASSERT(0x00000011U == result.data[0]);
  ASSERT(0x00000022U == result.data[1]);
  ASSERT(0x00000033U == result.data[2]);
  ASSERT(0x00000044U == result.data[3]);
  ASSERT(0x00000055U == result.data[4]);
  ASSERT(0x00000066U == result.data[5]);
  ASSERT(0x00000077U == result.data[6]);
  ASSERT(0x00000088U == result.data[7]);
}

// Test function to cause an addition to overflow
void test_add_addition_overflow() {
  UInt256 test = {0};
  test.data[0] = 0xFFFFFFFFU;
  test.data[1] = 0xFFFFFFFFU;
  test.data[2] = 0xFFFFFFFFU;
  test.data[3] = 0xFFFFFFFFU;
  test.data[4] = 0xFFFFFFFFU;
  test.data[5] = 0xFFFFFFFFU;
  test.data[6] = 0xFFFFFFFFU;
  test.data[7] = 0xFFFFFFFFU;

  UInt256 overflow_adder = {0};
  overflow_adder.data[0] = 0x00000001U;

  UInt256 result = uint256_add(test, overflow_adder);

  ASSERT(0x00000000U == result.data[0]); // Overflowed
  ASSERT(0x00000000U == result.data[1]); // Carried over
  ASSERT(0x00000000U == result.data[2]); // Carried over
  ASSERT(0x00000000U == result.data[3]); // Carried over
  ASSERT(0x00000000U == result.data[4]); // Carried over
  ASSERT(0x00000000U == result.data[5]); // Carried over
  ASSERT(0x00000000U == result.data[6]); // Carried over
  ASSERT(0x00000000U == result.data[7]); // Carried over
}

// Test function to subtract 0 from a value
void test_subtract_zero() {
  UInt256 test = {0};
  test.data[0] = 0x00000011U;
  test.data[1] = 0x00000022U;
  test.data[2] = 0x00000033U;
  test.data[3] = 0x00000044U;
  test.data[4] = 0x00000055U;
  test.data[5] = 0x00000066U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000088U;

  UInt256 zero = {0};  // A UInt256 representing zero

  UInt256 result = uint256_sub(test, zero); 

  ASSERT(0x00000011U == result.data[0]);
  ASSERT(0x00000022U == result.data[1]);
  ASSERT(0x00000033U == result.data[2]);
  ASSERT(0x00000044U == result.data[3]);
  ASSERT(0x00000055U == result.data[4]);
  ASSERT(0x00000066U == result.data[5]);
  ASSERT(0x00000077U == result.data[6]);
  ASSERT(0x00000088U == result.data[7]);
}

// Test function to cause a negative overflow (underflow)
void test_subtract_negatively_overflow() {
  UInt256 test = {0};
  test.data[0] = 0x00000000U;
  test.data[1] = 0x00000000U;
  test.data[2] = 0x00000000U;
  test.data[3] = 0x00000000U;
  test.data[4] = 0x00000000U;
  test.data[5] = 0x00000000U;
  test.data[6] = 0x00000000U;
  test.data[7] = 0x00000000U;

  UInt256 negative_overflow_subtracter = {0};
  negative_overflow_subtracter.data[0] = 0x00000001U;

  UInt256 result = uint256_sub(test, negative_overflow_subtracter);

  ASSERT(0xFFFFFFFFU == result.data[0]); // Underflowed
  ASSERT(0xFFFFFFFFU == result.data[1]); 
  ASSERT(0xFFFFFFFFU == result.data[2]); 
  ASSERT(0xFFFFFFFFU == result.data[3]); 
  ASSERT(0xFFFFFFFFU == result.data[4]); 
  ASSERT(0xFFFFFFFFU == result.data[5]); 
  ASSERT(0xFFFFFFFFU == result.data[6]); 
  ASSERT(0xFFFFFFFFU == result.data[7]); 
}

// not rotating by setting nbits to 0 in left rotation
void test_rotate_left_no_rotation() {
  UInt256 test = {0};
  test.data[0] = 0x00000011U;
  test.data[1] = 0x00000022U;
  test.data[2] = 0x00000033U;
  test.data[3] = 0x00000044U;
  test.data[4] = 0x00000055U;
  test.data[5] = 0x00000066U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000088U;

  UInt256 result = uint256_rotate_left(test, 0);

  ASSERT(0x00000011U == result.data[0]);
  ASSERT(0x00000022U == result.data[1]);
  ASSERT(0x00000033U == result.data[2]);
  ASSERT(0x00000044U == result.data[3]);
  ASSERT(0x00000055U == result.data[4]);
  ASSERT(0x00000066U == result.data[5]);
  ASSERT(0x00000077U == result.data[6]);
  ASSERT(0x00000088U == result.data[7]);
}

// rotating left multiple of 32 (number of bits within one element of an array)
void test_rotate_left_multiple_of_32() {
  UInt256 test = {0};
  test.data[0] = 0x000000DDU;
  test.data[1] = 0x000000CCU;
  test.data[2] = 0x000000BBU;
  test.data[3] = 0x000000AAU;
  test.data[4] = 0x00000099U;
  test.data[5] = 0x00000088U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000066U;

  UInt256 result = uint256_rotate_left(test, 32);

  ASSERT(0x00000066U == result.data[0]);
  ASSERT(0x000000DDU == result.data[1]);
  ASSERT(0x000000CCU == result.data[2]);
  ASSERT(0x000000BBU == result.data[3]);
  ASSERT(0x000000AAU == result.data[4]);
  ASSERT(0x00000099U == result.data[5]);
  ASSERT(0x00000088U == result.data[6]);
  ASSERT(0x00000077U == result.data[7]);
}

// rotating left multiple of 256 (total number of bits of the whole data)
void test_rotate_left_multiple_of_256() {
  UInt256 test = {0};
  test.data[0] = 0x000000DDU;
  test.data[1] = 0x000000CCU;
  test.data[2] = 0x000000BBU;
  test.data[3] = 0x000000AAU;
  test.data[4] = 0x00000099U;
  test.data[5] = 0x00000088U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000066U;

  UInt256 result = uint256_rotate_left(test, 256);

  ASSERT(0x000000DDU == result.data[0]);
  ASSERT(0x000000CCU == result.data[1]);
  ASSERT(0x000000BBU == result.data[2]);
  ASSERT(0x000000AAU == result.data[3]);
  ASSERT(0x00000099U == result.data[4]);
  ASSERT(0x00000088U == result.data[5]);
  ASSERT(0x00000077U == result.data[6]);
  ASSERT(0x00000066U == result.data[7]);
}

// rotating left more than 256
void test_rotate_left_more_than_256() {
  UInt256 test = {0};
  test.data[0] = 0x000000DDU;
  test.data[1] = 0x000000CCU;
  test.data[2] = 0x000000BBU;
  test.data[3] = 0x000000AAU;
  test.data[4] = 0x00000099U;
  test.data[5] = 0x00000088U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000066U;

  UInt256 result = uint256_rotate_left(test, 260); // 4 more than 256

  ASSERT(0x00000DD0U == result.data[0]);
  ASSERT(0x00000CC0U == result.data[1]);
  ASSERT(0x00000BB0U == result.data[2]);
  ASSERT(0x00000AA0U == result.data[3]);
  ASSERT(0x00000990U == result.data[4]);
  ASSERT(0x00000880U == result.data[5]);
  ASSERT(0x00000770U == result.data[6]);
  ASSERT(0x00000660U == result.data[7]);
}

// rotating left less than 32
void test_rotate_left_less_than_32() {
  UInt256 test = {0};
  test.data[0] = 0x000000DDU;
  test.data[1] = 0x000000CCU;
  test.data[2] = 0x000000BBU;
  test.data[3] = 0x000000AAU;
  test.data[4] = 0x00000099U;
  test.data[5] = 0x00000088U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000066U;

  UInt256 result = uint256_rotate_left(test, 4);

  ASSERT(0x00000DD0U == result.data[0]);
  ASSERT(0x00000CC0U == result.data[1]);
  ASSERT(0x00000BB0U == result.data[2]);
  ASSERT(0x00000AA0U == result.data[3]);
  ASSERT(0x00000990U == result.data[4]);
  ASSERT(0x00000880U == result.data[5]);
  ASSERT(0x00000770U == result.data[6]);
  ASSERT(0x00000660U == result.data[7]);
}

// rotating left between 32 and 256
void test_rotate_left_between_32_and_256() {
  UInt256 test = {0};
  test.data[0] = 0x000000DDU;
  test.data[1] = 0x000000CCU;
  test.data[2] = 0x000000BBU;
  test.data[3] = 0x000000AAU;
  test.data[4] = 0x00000099U;
  test.data[5] = 0x00000088U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000066U;

  UInt256 result = uint256_rotate_left(test, 36); // 32 + 4

  ASSERT(0x00000660U == result.data[0]);
  ASSERT(0x00000DD0U == result.data[1]);
  ASSERT(0x00000CC0U == result.data[2]);
  ASSERT(0x00000BB0U == result.data[3]);
  ASSERT(0x00000AA0U == result.data[4]);
  ASSERT(0x00000990U == result.data[5]);
  ASSERT(0x00000880U == result.data[6]);
  ASSERT(0x00000770U == result.data[7]);  // Notice the last bit
}

// not rotating by setting nbits to 0 in right rotation
void test_rotate_right_no_rotation() {
  UInt256 test = {0};
  test.data[0] = 0x00000011U;
  test.data[1] = 0x00000022U;
  test.data[2] = 0x00000033U;
  test.data[3] = 0x00000044U;
  test.data[4] = 0x00000055U;
  test.data[5] = 0x00000066U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000088U;

  UInt256 result = uint256_rotate_right(test, 0);

  ASSERT(0x00000011U == result.data[0]);
  ASSERT(0x00000022U == result.data[1]);
  ASSERT(0x00000033U == result.data[2]);
  ASSERT(0x00000044U == result.data[3]);
  ASSERT(0x00000055U == result.data[4]);
  ASSERT(0x00000066U == result.data[5]);
  ASSERT(0x00000077U == result.data[6]);
  ASSERT(0x00000088U == result.data[7]);
}

// rotating right multiple of 32 (number of bits within one element of an array)
void test_rotate_right_multiple_of_32() {
  UInt256 test = {0};
  test.data[0] = 0x000000DDU;
  test.data[1] = 0x000000CCU;
  test.data[2] = 0x000000BBU;
  test.data[3] = 0x000000AAU;
  test.data[4] = 0x00000099U;
  test.data[5] = 0x00000088U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000066U;

  UInt256 result = uint256_rotate_right(test, 32);

  ASSERT(0x000000CCU == result.data[0]);
  ASSERT(0x000000BBU == result.data[1]);
  ASSERT(0x000000AAU == result.data[2]);
  ASSERT(0x00000099U == result.data[3]);
  ASSERT(0x00000088U == result.data[4]);
  ASSERT(0x00000077U == result.data[5]);
  ASSERT(0x00000066U == result.data[6]);
  ASSERT(0x000000DDU == result.data[7]);
}

// rotating right multiple of 256 (total number of bits of the whole data)
void test_rotate_right_multiple_of_256() {
  UInt256 test = {0};
  test.data[0] = 0x000000DDU;
  test.data[1] = 0x000000CCU;
  test.data[2] = 0x000000BBU;
  test.data[3] = 0x000000AAU;
  test.data[4] = 0x00000099U;
  test.data[5] = 0x00000088U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000066U;

  UInt256 result = uint256_rotate_right(test, 256);

  ASSERT(0x000000DDU == result.data[0]);
  ASSERT(0x000000CCU == result.data[1]);
  ASSERT(0x000000BBU == result.data[2]);
  ASSERT(0x000000AAU == result.data[3]);
  ASSERT(0x00000099U == result.data[4]);
  ASSERT(0x00000088U == result.data[5]);
  ASSERT(0x00000077U == result.data[6]);
  ASSERT(0x00000066U == result.data[7]);
}

// rotating right more than 256
void test_rotate_right_more_than_256() {
  UInt256 test = {0};
  test.data[0] = 0x000000DDU;
  test.data[1] = 0x000000CCU;
  test.data[2] = 0x000000BBU;
  test.data[3] = 0x000000AAU;
  test.data[4] = 0x00000099U;
  test.data[5] = 0x00000088U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000066U;

  UInt256 result = uint256_rotate_right(test, 260); // 4 more than 256

  ASSERT(0xC000000DU == result.data[0]);
  ASSERT(0xB000000CU == result.data[1]);
  ASSERT(0xA000000BU == result.data[2]);
  ASSERT(0x9000000AU == result.data[3]);
  ASSERT(0x80000009U == result.data[4]);
  ASSERT(0x70000008U == result.data[5]);
  ASSERT(0x60000007U == result.data[6]);
  ASSERT(0xD0000006U == result.data[7]);
}

// rotating right less than 32
void test_rotate_right_less_than_32() {
  UInt256 test = {0};
  test.data[0] = 0x000000DDU;
  test.data[1] = 0x000000CCU;
  test.data[2] = 0x000000BBU;
  test.data[3] = 0x000000AAU;
  test.data[4] = 0x00000099U;
  test.data[5] = 0x00000088U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000066U;

  UInt256 result = uint256_rotate_right(test, 4);

  ASSERT(0xC000000DU == result.data[0]);
  ASSERT(0xB000000CU == result.data[1]);
  ASSERT(0xA000000BU == result.data[2]);
  ASSERT(0x9000000AU == result.data[3]);
  ASSERT(0x80000009U == result.data[4]);
  ASSERT(0x70000008U == result.data[5]);
  ASSERT(0x60000007U == result.data[6]);
  ASSERT(0xD0000006U == result.data[7]);
}

// rotating right between 32 and 256
void test_rotate_right_between_32_and_256() {
  UInt256 test = {0};
  test.data[0] = 0x000000DDU;
  test.data[1] = 0x000000CCU;
  test.data[2] = 0x000000BBU;
  test.data[3] = 0x000000AAU;
  test.data[4] = 0x00000099U;
  test.data[5] = 0x00000088U;
  test.data[6] = 0x00000077U;
  test.data[7] = 0x00000066U;

  UInt256 result = uint256_rotate_right(test, 36); // 32 + 4

  ASSERT(0xB000000CU == result.data[0]);
  ASSERT(0xA000000BU == result.data[1]);
  ASSERT(0x9000000AU == result.data[2]);
  ASSERT(0x80000009U == result.data[3]);
  ASSERT(0x70000008U == result.data[4]);
  ASSERT(0x60000007U == result.data[5]);
  ASSERT(0xD0000006U == result.data[6]);
  ASSERT(0xC000000DU == result.data[7]);
}

