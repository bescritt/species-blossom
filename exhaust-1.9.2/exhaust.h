#ifndef EXHAUST_H
#define EXHAUST_H
/*  exhaust.h:  Global constants, structures, and types
 * $Id: exhaust.h,v 1.4 2002/05/13 03:05:33 rowan Exp $
 */

/* This file is part of `exhaust', a memory array redcode simulator.
 * Copyright (C) 2002 M Joonas Pihlaja
 * Public Domain.
 */


/* global debug level */
#ifndef DEBUG
#define DEBUG 0
#endif


#include <stdint.h>

/*
 * Global constants
 *
 */

#define MAXLENGTH 100
  /* max length of warrior */


/*
 * Global types
 *
 */

/* misc. integral types */
typedef unsigned char  u8_t;
typedef unsigned short u16_t;
typedef uint32_t u32_t;
typedef int32_t  s32_t;

  /* Choose an appropriate field_t width.  In a field_t variable
   * we hold unsigned integers in 0..CORESIZE-1
   */
#define FIELD_T_WIDTH 16

#if FIELD_T_WIDTH <= 8
  typedef u8_t field_t;
#elif FIELD_T_WIDTH <= 16
  typedef u16_t field_t;
#else
  typedef u32_t field_t;
#endif

  /*
   * Instructions in core:
   */
  typedef struct insn_st {
    u16_t in;                   /* flags, opcode, modifier, a- and b-modes */
    field_t a, b;               /* a-value, b-value */
    #ifdef __cplusplus
    OPCODE opcode() const { return (OPCODE)((in >> opPOS) & opMASK); } 
    MODIFIER modifier() const { return (MODIFIER)((in >> moPOS) & moMASK); }
	 unsigned opcode_modifier() const { return (in >> moPOS); }
    ADDRMODE addrmode_a() const { return (ADDRMODE)((in >> maPOS) & mMASK); }
    ADDRMODE addrmode_b() const { return (ADDRMODE)((in >> mbPOS) & mMASK); }
    void set_opcode(const OPCODE op) { in = OP(op,modifier(),addrmode_a(),addrmode_b()); }
    void set_modifier(const MODIFIER mod) { in = OP(opcode(),mod,addrmode_a(),addrmode_b()); }
	 void set_opcode_modifier(const unsigned opmod) { in = (opmod << moPOS) | (addrmode_b() << mbPOS) | addrmode_b(); }
    void set_addrmode_a(const ADDRMODE a) { in = OP(opcode(),modifier(),a,addrmode_b()); }
    void set_addrmode_b(const ADDRMODE b) { in = OP(opcode(),modifier(),addrmode_a(),b); }
    unsigned flags() const { return (in >> flPOS); }
    bool has_flags() const { return (in > iMASK); }
    bool has_no_flags() const { return !has_flags(); }
    void invalidate() {
    	in = ~0;
    	a = ~0;
    	b = ~0;
    }
    bool valid(const field_t coresize = 0,const bool allow_flags=false) const { // if coresize is 0, bounds of operands will not be checked
			if(	(opcode() >= OPCODE_LAST) ||
				(modifier() >= MODIFIER_LAST) ||
				(addrmode_a() >= ADDRMODE_LAST) ||
				(addrmode_b() >= ADDRMODE_LAST))
				return false;
			if(0 < coresize) { // set?
				if(	(coresize <= a) ||
					(coresize <= b))
					return false;
			}
			if(!allow_flags) {
				if(has_flags())
					return false;
			}
			return true; // if here, then ok
    }
		void set_clear() { in = OP(DAT,mF,DIRECT,DIRECT); a = 0; b = 0; }
		bool is_clear() const { // is this default core?
			return 
				(	(DAT == opcode()) &&
					(mF == modifier()) &&
					(0 == a) &&
					(0 == b));
		}
    #endif
  } insn_t;


  /*
   * Warrior struct
   */
  typedef struct warrior_st {
    insn_t code[ MAXLENGTH ];   /* code of warrior */
    unsigned int len;		/* length of -"- */
    unsigned int start;		/* start relative to first insn */

    int have_pin;		/* does warrior have pin? */
    u32_t pin;			/* pin of warrior or garbage. */

    /* info fields -- these aren't automatically set or used */
    char *name;
    int no;                     /* warrior no. */
  } warrior_t;


#endif /* EXHAUST_H */
