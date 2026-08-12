#ifndef EXHAUST_HPP
#define EXHAUST_HPP

/* "Species-Blossom" - a CoreWars evolver.  Copyright (C) 2005 'Varfar' 
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 1, or (at your option) any later
 * version. 
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details. 
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA. 
 */

extern "C" {
	#include "exhaust-1.9.2/insn.h"
	#include "exhaust-1.9.2/exhaust.h"
	#include "exhaust-1.9.2/asm.h"
	#include "exhaust-1.9.2/sim.h"
}

class Mnemonic {
	public:
		static const char* Opcode(const OPCODE opcode);
		static const char* Modifier(const MODIFIER modifier);
		static char AddrMode(const ADDRMODE addrmode);	
};

class OpInfo {
	public:
		static MODIFIER EffectiveModifier(const OPCODE opcode,const MODIFIER modifier);
		static bool Branches(const OPCODE opcode);
		static bool Skips(const OPCODE opcode);
		static bool Continues(const OPCODE opcode);
};

#endif //EXHAUST_HPP


