Name: Yoohyuk Chang, Yongjae Lee


<Contributions>
Yoohyuk Chang: 
Worked on uint256_create_from_u32, uint256_create, and uint256_get_bits functions.
Worked on uint256_rotate_left abd uint256_rotate_right functions.
Worked on writing test functions for uint256_add, uint256_sub, uint256_negate, uint256_rotate_left, uint256_rotate_right, uint256_create

Yongjae Lee:
Worked on uint256_create_from_u32, uint256_create, and uint256_get_bits functions.
Worked on uint256_create_from_hex, uint256_format_as_hex, uint256_add, uint256_sub, uint256_negate.
Worked on writing test functions for uint256_create_from_hex, uint256_format_as_hex, uint256_create_from_u32, uint256_get_bits 


***** Note to CAs *****
In uint256_tests.c file, there is a test function called 'test_uint256_create_from_hex_larger_than_256'. We deliberately commented that out
in the main function of the file because when we run the test on our computer, it says it all passes, but when submitted to gradescope,
it says the unit test does not pass. So we commented out that one test case. Thank you!