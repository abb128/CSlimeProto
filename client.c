#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "slime.h"


static int sockfd;
static struct sockaddr_in servaddr;

#define MAX_LEN 128
static uint8_t buffer[MAX_LEN];

void x_send(uint8_t *data, uint64_t len){
	sendto(sockfd, data, len, MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
}

int x_recv(){
	uint32_t len; //???
	int n = recvfrom(sockfd, buffer, MAX_LEN, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
	buffer[n] = '\0';
	return n;
}


void x_init(){
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(6970);
	servaddr.sin_addr.s_addr = INADDR_ANY;
}


void x_finish(){
	close(sockfd);
}


void send_handshake_init(){
	SlimeHandshakeInit init_h = {0};
	init_h.magic_uuid  = SLIME_HANDSHAKE_INIT_UUID;
	init_h.magic_int   = SLIME_HANDSHAKE_INIT_INT;
	init_h.magic_short = SLIME_HANDSHAKE_INIT_SHORT;
	init_h.magic_byte  = SLIME_HANDSHAKE_INIT_BYTE;
	
	SlimeBarePacket<SlimeHandshakeInit> handshakePacket = {SLIME_HANDLE_HANDSHAKE_INIT, init_h};
	
	x_send((uint8_t *)(&handshakePacket), sizeof(handshakePacket));
	printf("Sent handshake init packet\n");
}

void send_handshake(){
	SlimeHandshake handshake = {0};
	handshake.protocol_version = 1;
	handshake.firmware = UUIDstr("badbadba-dbad-baba-dada-badbadbadbad");
	handshake.hardware = UUIDstr("12341234-1234-4321-aebf-000011112222");
	handshake.hw_uid   = UUIDstr("00000000-1111-2222-3333-444411113333");
	handshake.reliability = SLIME_RELIABILITY_ORDERING;
	
	SlimeBarePacket<SlimeHandshake> handshakePacket = {SLIME_HANDLE_HANDSHAKE, handshake};
	
	x_send((uint8_t *)(&handshakePacket), sizeof(handshakePacket));
	printf("Sent handshake packet\n");
}

void recv_handshake_init_r(){
	x_recv();
	
	SlimeBarePacket<SlimeHandshakeInitResponse> *response = (SlimeBarePacket<SlimeHandshakeInitResponse>*)buffer;
	if(response->handle != SLIME_HANDLE_HANDSHAKE_INIT_R){
		goto error;
	}
	
	if(response->data.magic != SLIME_HANDSHAKE_RESPONSE_MAGIC){
		goto error;
	}
	
	return;
	
error:
	printf("Fail!!!!\n");
	exit(EXIT_FAILURE);
}

void recv_handshake(){
	x_recv();
	SlimeBarePacket<SlimeHandshakeResponse> *response = (SlimeBarePacket<SlimeHandshakeResponse>*)buffer;
	if(response->handle == SLIME_HANDLE_HANDSHAKE_R){
		printf("Handshook successfully :)\n");
		return;
	}
	
	printf("Fail handshake response!!\n");
	exit(EXIT_FAILURE);
}

int main(){
	x_init();
	
	send_handshake_init();
	recv_handshake_init_r();
	
	send_handshake();
	recv_handshake();
	
	x_finish();
	
	return 0;
}
