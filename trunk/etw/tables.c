#include "mytypes.h"

UBYTE Table[256]={
2,
3,4,3,0,1,4,1,0,3,4,1,2,5,4,1,2,
1,2,1,0,1,4,1,2,1,2,3,2,5,0,3,2,
3,0,3,4,5,2,5,4,5,2,3,0,1,2,3,0,
5,0,5,0,1,0,1,4,5,2,5,0,3,4,5,0,
3,0,5,2,3,4,1,0,5,4,1,0,5,0,3,0,
1,2,3,2,5,2,5,2,1,0,1,4,1,2,3,0,
3,2,5,0,3,0,5,0,5,4,3,0,3,4,5,0,
1,4,5,3,3,0,1,4,5,4,1,0,5,2,3,4,
5,4,5,2,1,2,5,4,3,4,5,4,5,2,5,2,
1,2,3,2,3,2,5,2,5,4,3,4,1,4,5,0,
3,2,1,2,1,4,5,4,3,2,1,0,1,4,1,4,
5,0,1,4,5,2,5,2,5,4,3,0,1,0,3,4,
5,2,1,4,3,4,1,2,1,2,1,0,3,4,1,0,
1,4,5,4,3,0,3,0,3,4,3,0,5,2,3,0,
5,0,1,4,1,4,3,2,1,2,1,4,3,0,3,0,
1,0,3,2,5,2,5,2,5,2,1,0,3,0,4,
};

WORD cos_table[256] =
{
	128,
	128,
	128,
	128,
	127,
	127,
	127,
	126,
	126,
	125,
	124,
	123,
	122,
	122,
	121,
	119,
	118,
	117,
	116,
	114,
	113,
	111,
	110,
	108,
	106,
	105,
	103,
	101,
	99,
	97,
	95,
	93,
	91,
	88,
	86,
	84,
	81,
	79,
	76,
	74,
	71,
	68,
	66,
	63,
	60,
	58,
	55,
	52,
	49,
	46,
	43,
	40,
	37,
	34,
	31,
	28,
	25,
	22,
	19,
	16,
	13,
	9,
	6,
	3,
	0,
	-3,
	-6,
	-9,
	-12,
	-15,
	-18,
	-21,
	-24,
	-28,
	-31,
	-34,
	-37,
	-40,
	-43,
	-46,
	-48,
	-51,
	-54,
	-57,
	-60,
	-63,
	-65,
	-68,
	-71,
	-73,
	-76,
	-78,
	-81,
	-83,
	-85,
	-88,
	-90,
	-92,
	-94,
	-96,
	-98,
	-100,
	-102,
	-104,
	-106,
	-108,
	-109,
	-111,
	-112,
	-114,
	-115,
	-117,
	-118,
	-119,
	-120,
	-121,
	-122,
	-123,
	-124,
	-124,
	-125,
	-126,
	-126,
	-127,
	-127,
	-127,
	-127,
	-127,
	-128,
	-127,
	-127,
	-127,
	-127,
	-127,
	-126,
	-126,
	-125,
	-124,
	-124,
	-123,
	-122,
	-121,
	-120,
	-119,
	-118,
	-117,
	-115,
	-114,
	-112,
	-111,
	-109,
	-108,
	-106,
	-104,
	-102,
	-100,
	-98,
	-96,
	-94,
	-92,
	-90,
	-88,
	-85,
	-83,
	-81,
	-78,
	-76,
	-73,
	-71,
	-68,
	-65,
	-63,
	-60,
	-57,
	-54,
	-51,
	-48,
	-46,
	-43,
	-40,
	-37,
	-34,
	-31,
	-28,
	-24,
	-21,
	-18,
	-15,
	-12,
	-9,
	-6,
	-3,
	0,
	3,
	6,
	9,
	13,
	16,
	19,
	22,
	25,
	28,
	31,
	34,
	37,
	40,
	43,
	46,
	49,
	52,
	55,
	58,
	60,
	63,
	66,
	68,
	71,
	74,
	76,
	79,
	81,
	84,
	86,
	88,
	91,
	93,
	95,
	97,
	99,
	101,
	103,
	105,
	106,
	108,
	110,
	111,
	113,
	114,
	116,
	117,
	118,
	119,
	121,
	122,
	122,
	123,
	124,
	125,
	126,
	126,
	127,
	127,
	127,
	128,
	128,
	128,
};

WORD sin_table[256] =
{
	0,
	3,
	6,
	9,
	13,
	16,
	19,
	22,
	25,
	28,
	31,
	34,
	37,
	40,
	43,
	46,
	49,
	52,
	55,
	58,
	60,
	63,
	66,
	68,
	71,
	74,
	76,
	79,
	81,
	84,
	86,
	88,
	91,
	93,
	95,
	97,
	99,
	101,
	103,
	105,
	106,
	108,
	110,
	111,
	113,
	114,
	116,
	117,
	118,
	119,
	121,
	122,
	122,
	123,
	124,
	125,
	126,
	126,
	127,
	127,
	127,
	128,
	128,
	128,
	128,
	128,
	128,
	128,
	127,
	127,
	127,
	126,
	126,
	125,
	124,
	123,
	122,
	122,
	121,
	119,
	118,
	117,
	116,
	114,
	113,
	111,
	110,
	108,
	106,
	105,
	103,
	101,
	99,
	97,
	95,
	93,
	91,
	88,
	86,
	84,
	81,
	79,
	76,
	74,
	71,
	68,
	66,
	63,
	60,
	58,
	55,
	52,
	49,
	46,
	43,
	40,
	37,
	34,
	31,
	28,
	25,
	22,
	19,
	16,
	13,
	9,
	6,
	3,
	0,
	-3,
	-6,
	-9,
	-12,
	-15,
	-18,
	-21,
	-24,
	-28,
	-31,
	-34,
	-37,
	-40,
	-43,
	-46,
	-48,
	-51,
	-54,
	-57,
	-60,
	-63,
	-65,
	-68,
	-71,
	-73,
	-76,
	-78,
	-81,
	-83,
	-85,
	-88,
	-90,
	-92,
	-94,
	-96,
	-98,
	-100,
	-102,
	-104,
	-106,
	-108,
	-109,
	-111,
	-112,
	-114,
	-115,
	-117,
	-118,
	-119,
	-120,
	-121,
	-122,
	-123,
	-124,
	-124,
	-125,
	-126,
	-126,
	-127,
	-127,
	-127,
	-127,
	-127,
	-128,
	-127,
	-127,
	-127,
	-127,
	-127,
	-126,
	-126,
	-125,
	-124,
	-124,
	-123,
	-122,
	-121,
	-120,
	-119,
	-118,
	-117,
	-115,
	-114,
	-112,
	-111,
	-109,
	-108,
	-106,
	-104,
	-102,
	-100,
	-98,
	-96,
	-94,
	-92,
	-90,
	-88,
	-85,
	-83,
	-81,
	-78,
	-76,
	-73,
	-71,
	-68,
	-65,
	-63,
	-60,
	-57,
	-54,
	-51,
	-48,
	-46,
	-43,
	-40,
	-37,
	-34,
	-31,
	-28,
	-24,
	-21,
	-18,
	-15,
	-12,
	-9,
	-6,
	-3,
};


// Questa tabella contiene le differenze di direzione tra una dir ed un altra!

BYTE CambioDirezione[8][8] = {
	{ // Dir di partenza: 0
		0, 1, 2, 3, 4, -3, -2, -1, 
	},
	{ // Dir di partenza: 1
		-1, 0, 1, 2, 3, 4, -3, -2, 
	},
	{ // Dir di partenza: 2
		-2, -1, 0, 1, 2, 3, 4, -3, 
	},
	{ // Dir di partenza: 3
		-3, -2, -1, 0, 1, 2, 3, 4, 
	},
	{ // Dir di partenza: 4
		4, -3, -2, -1, 0, 1, 2, 3, 
	},
	{ // Dir di partenza: 5
		3, 4, -3, -2, -1, 0, 1, 2, 
	},
	{ // Dir di partenza: 6
		2, 3, 4, -3, -2, -1, 0, 1, 
	},
	{ // Dir di partenza: 7
		1, 2, 3, 4, -3, -2, -1, 0, 
	},
};