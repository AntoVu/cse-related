#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>

extern uint32_t countZeros32(uint32_t x);

int main() {
		uint32_t a = 0x12345678;
		uint32_t b = 0b11111111111111101111011101111011;
		uint32_t c = 0b00000000000000000000000000000001;
		
		uint32_t a_result, b_result, c_result;
		
		a_result = countZeros32(a);
		b_result = countZeros32(b);
		c_result = countZeros32(c);
		
		printf("a_result = %"PRId32"\n", a_result);
		printf("b_result = %"PRId32"\n", b_result);
		printf("c_result = %"PRId32"\n", c_result);
		
		return EXIT_SUCCESS;
}
