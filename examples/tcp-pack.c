// =========================================================
// libnet -- A network stack implementation for BareMetal OS
//
// Copyright (C) 2017 Return Infinity -- see LICENSE
// =========================================================

#include <libnet/buffer.h>
#include <libnet/mutator.h>
#include <libnet/stack.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int mutate_ethernet(const void *mutator_data,
                           struct libnet_ethernet *ethernet)
{
	(void) mutator_data;

	ethernet->type = LIBNET_ETHERTYPE_IPV6;

	int err = 0;
	err |= libnet_ethernet_set_source(ethernet, "00:11:22:33:44:55", 17);
	err |= libnet_ethernet_set_destination(ethernet, "33:44:55:66:77:88", 17);
	return err;
}

const char ipv6_source[] = "2001:0000:3238:DFE1:0063:0000:0000:FEFB";

const char ipv6_destination[] = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";

static int mutate_ipv6(const void *mutator_data,
                       struct libnet_ipv6 *ipv6)
{
	(void) mutator_data;

	int err = 0;
	err |= libnet_ipv6_set_source(ipv6, ipv6_source, sizeof(ipv6_source) - 1);
	err |= libnet_ipv6_set_destination(ipv6, ipv6_destination, sizeof(ipv6_destination) - 1);
	return err;
}

static int mutate_tcp(const void *mutator_data,
                      struct libnet_tcp *tcp)
{
	(void) mutator_data;

	int err = 0;
	err |= libnet_tcp_set_source(tcp, "20", 2);
	err |= libnet_tcp_set_destination(tcp, "80", 2);
	return err;
}

static int export_buffer(const struct libnet_buffer *buffer)
{
	FILE *file = fopen("tcp-packet.bin", "wb");
	if (file == NULL)
		return -1;

	fwrite(buffer->data, 1, buffer->size, file);

	fclose(file);

	return 0;
}

int main(void)
{
	struct libnet_stack stack;

	libnet_stack_init(&stack);

	int err = 0;
	err |= libnet_stack_push_ethernet(&stack);
	err |= libnet_stack_push_ipv6(&stack);
	err |= libnet_stack_push_tcp(&stack);
	if (err != 0)
	{
		fprintf(stderr, "Failed to construct network stack.\n");
		libnet_stack_done(&stack);
		return EXIT_FAILURE;
	}

	struct libnet_mutator mutator;
	libnet_mutator_init(&mutator);
	mutator.mutate_ethernet = mutate_ethernet;
	mutator.mutate_ipv6 = mutate_ipv6;
	mutator.mutate_tcp = mutate_tcp;

	err = libnet_stack_mutate(&stack, &mutator);
	if (err != 0)
	{
		fprintf(stderr, "Failed to mutate network stack.\n");
		libnet_stack_done(&stack);
		return EXIT_FAILURE;
	}

	char msgbuf[128];
	memset(msgbuf, 0, sizeof(msgbuf));
	strcat(msgbuf, "Hello, world!");

	struct libnet_buffer buffer;
	buffer.data = msgbuf;
	buffer.size = 32;
	buffer.reserved = sizeof(msgbuf);

	err = libnet_stack_pack(&stack, &buffer);
	if (err != 0)
	{
		fprintf(stderr, "Failed to pack message.\n");
		libnet_stack_done(&stack);
		return EXIT_FAILURE;
	}

	export_buffer(&buffer);

	libnet_stack_done(&stack);

	return EXIT_SUCCESS;
}