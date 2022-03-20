#ifndef _SLIME_H
#define _SLIME_H
#define SLIME_PACKED __attribute__((__packed__))

typedef uint8_t SlimeHandle;

typedef struct SlimeUUID {
	uint8_t data[16];
} SlimeUUID;


inline SlimeUUID UUIDstr(const char* data){
	SlimeUUID tmp = {0};
	if(sscanf(
		data,
		"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		&tmp.data[0], &tmp.data[1], &tmp.data[2], &tmp.data[3],
		&tmp.data[4], &tmp.data[5],
		&tmp.data[6], &tmp.data[7],
		&tmp.data[8], &tmp.data[9],
		&tmp.data[10], &tmp.data[11], &tmp.data[12], &tmp.data[13], &tmp.data[14], &tmp.data[15]
	) != 16){
		printf("Scheisse");
	}
	
	return tmp;
}

template<typename T>
struct SLIME_PACKED SlimeBarePacket {
	uint8_t handle;
	T data;
};

#include "base.slimeh"

#endif
