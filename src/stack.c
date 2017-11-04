// =========================================================
// libnet -- A network stack implementation for BareMetal OS
//
// Copyright (C) 2017 Return Infinity -- see LICENSE
// =========================================================

#include <libnet/stack.h>

void libnet_stack_init(struct libnet_stack *stack)
{
	for (unsigned long long int i = 0; i < LIBNET_PROTOCOL_MAX; i++)
		libnet_protocol_init(&stack->protocol_array[i]);

	stack->protocol_count = 0;
}