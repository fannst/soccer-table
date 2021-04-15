/*
	>> Copyright Luke A.C.A. Rieff & Rien Dumore - Project-D ROC Nijmegen
*/

#include <cstdint>

#pragma once

/**************************************************************
 * SPFP System (Layer 2)
 **************************************************************/

typedef enum {
	SPFP_CONTROL_OP_AM_I_ALONE = 0,	/* Raspberry is sad and alone, asks if STM is there */
	SPFP_CONTROL_OP_NOT_ALONE = 1,	/* STM is there, and wants to talk with the raspberry */
	SPFP_CONTROL_OP_RESTART = 2,	/* Restarts the STM */
	SPFP_CONTROL_OP_BEEP = 3		/* Beeps LOL */
} spfp_system_op_t;

typedef struct {
	uint8_t			op;				/* Opcode */
} spfp_system_pkt_t;

/**************************************************************
 * SPFP LRAM (Layer 2)
 **************************************************************/

typedef enum {
	SPFP_LRAM_AXIS1 = 0,
	SPFP_LRAM_AXIS2 = 1,
	SPFP_LRAM_AXIS3 = 2,
	SPFP_LRAM_AXIS4 = 3,
	SPFP_LRAM_AXIS5 = 4
} spfp_lram_axis_t;

typedef enum {
	SPFP_LRAM_MSEL_ROT = 0,
	SPFP_LRAM_MSEL_LINEAR = 1
} spfp_lram_msel_t;

typedef enum {
	SPFP_LRAM_OP_MOVE = 0,
	SPFP_LRAM_OP_HOME = 1
} spfp_lram_op_t;

typedef struct __attribute__ (( packed )) {
	unsigned		axis : 5;		/* Axis Selection */
	unsigned		msel : 2;		/* Motor Select */
	unsigned		hnex : 1;		/* Has Next ? */
	uint16_t		val;			/* ROT (0-360), Linear (0-10000) */
	uint8_t			next[0];
} spfp_lram_move_pkt_t;

typedef struct __attribute__ (( packed )) {
	unsigned		axis : 5;		/* Axis Selection */
	unsigned		msel : 2;		/* Motor Select */
	unsigned		hnex : 1;		/* Has Next */
	uint8_t			next[0];
} spfp_lram_home_pkt_t;

typedef struct __attribute__ (( packed )) {
	uint8_t 		op;			/* Operation Code */
	uint8_t			p[0];		/* The Payload */
} spfp_lram_pkt_t;

/**************************************************************
 * SPFP Class (Layer 1)
 **************************************************************/

typedef enum {
	SPFP_CLASS_SYSTEM = 0,
	SPFP_CLASS_LRAM_CONTROL = 1,
	SPFP_CLASS_UMS = 2
} spfp_class_t;

typedef struct __attribute__ (( packed )) {
	uint8_t			c;				/* Packet Class */
	uint8_t			p[0];			/* Payload */
} spfp_class_pkt_t;

/// Gets the next LRAM home packet, returns nullptr if not exists.
spfp_lram_home_pkt_t *spfp_lram_home_next (spfp_lram_home_pkt_t *pkt) noexcept;
