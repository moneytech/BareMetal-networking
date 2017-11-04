// =========================================================
// libnet -- A network stack implementation for BareMetal OS
//
// Copyright (C) 2017 Return Infinity -- see LICENSE
// =========================================================

#include <libnet/crc.h>

#include <assert.h>
#include <stdlib.h>

static void test_crc32(void)
{
	char buf[] = "123456789";
	unsigned long int crc32 = libnet_crc32(buf, sizeof(buf));
	assert(crc32 == 0xCBF43926UL);
}

int main(void)
{
	test_crc32();
	return EXIT_SUCCESS;
}
