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

#include "exhaust.hpp"
#include "blossom_utils.hpp"

//************* Mnemonic *************\\

const char *Mnemonic::Opcode(const OPCODE opcode) {
	BLOSSOM_ASSERT((0 <= opcode) && (opcode < OPCODE_LAST));
	static const char* S_MNEMONIC_OPCODE[OPCODE_LAST] = {
		"DAT","SPL","MOV","DJN","ADD","JMZ","SUB","SEQ","SNE","SLT","JMN","JMP","NOP","MUL","MOD","DIV","LDP","STP" };
	return S_MNEMONIC_OPCODE[opcode];
}

const char *Mnemonic::Modifier(const MODIFIER modifier) {
	static const char* S_MNEMONIC_MODIFIER[MODIFIER_LAST] = {
		"F","A","B","AB","BA","X","I"};
	BLOSSOM_ASSERT((0 <= modifier) && (modifier < MODIFIER_LAST));
	return S_MNEMONIC_MODIFIER[modifier];
}

char Mnemonic::AddrMode(const ADDRMODE addrmode) {
	static const char S_MNEMONIC_ADDRMODE[ADDRMODE_LAST] = {
		'$','#','@','<','>','*','{','}'};
	BLOSSOM_ASSERT((0 <= addrmode) && (addrmode < ADDRMODE_LAST));
	return S_MNEMONIC_ADDRMODE[addrmode];
}

//************* OpInfo *************\\

MODIFIER OpInfo::EffectiveModifier(OPCODE opcode,MODIFIER modifier) {
	static const struct {
		unsigned num;
 		MODIFIER lookup[MODIFIER_LAST];
	} EFFECTIVE_MODIFIER_LOOKUP[OPCODE_LAST] = {
		{1, { mF, mF, mF,mF, mF,  mF, mF}},	// * DAT				/* must be 0 */
		{1, { mF, mF, mF,mF, mF,  mF, mF}},	// * SPL,
		{7, { mF, mA, mB,mAB,mBA, mX, mI}},	// * MOV,
		{3, { mF, mA, mB,mB, mA,  mF, mF}},	// * DJN,
		{6, { mF, mA, mB,mAB,mBA, mX, mF}},	// * ADD,
		{3, { mF, mA, mB,mB, mA,  mF, mF}},	// * JMZ,
		{6, { mF, mA, mB,mAB,mBA, mX, mF}},	// * SUB,
		{7, { mF, mA, mB,mAB,mBA, mX, mI}},	// * SEQ,
		{7, { mF, mA, mB,mAB,mBA, mX, mI}},	// * SNE,
		{6, { mF, mA, mB,mAB,mBA, mX, mF}},	// * SLT,
		{5, { mF, mA, mB,mB, mA,  mF, mF}},	// * JMN,
		{1, { mF, mF, mF,mF, mF,  mF, mF}},	// * JMP,
		{1, { mF, mF, mF,mF, mF,  mF, mF}},	// * NOP,
		{6, { mF, mA, mB,mAB,mBA, mX, mF}},	// * MUL,
		{6, { mF, mA, mB,mAB,mBA, mX, mF}},	// * MODM,
		{6, { mF, mA, mB,mAB,mBA, mX, mF}},	// * DIV,
		{7, { mF, mA, mB,mAB,mBA, mX, mI}},	// LDP,
		{7, { mF, mA, mB,mAB,mBA, mX, mI}}	// STP,				/* 18 */
	};
	BLOSSOM_ASSERT(opcode < OPCODE_LAST);
	BLOSSOM_ASSERT(modifier < MODIFIER_LAST);
	return EFFECTIVE_MODIFIER_LOOKUP[opcode].lookup[modifier];
}

bool OpInfo::Branches(const OPCODE opcode) {
	static const bool OPCODE_IS_BRANCH_OR_SKIP[OPCODE_LAST] = {
		false, // DAT,				/* must be 0 */
		true,  // SPL,
		false, // MOV,
		true,  // DJN,
		false, // ADD,
		true,  // JMZ,
		false, // SUB,
		true,  // SEQ,
		true,  // SNE,
		true,  // SLT,
		true,  // JMN,
		true,  // JMP,
		false, // NOP,
		false, // MUL,
		false, // MODM,
		false, // DIV,
		false, // LDP,
		false  // STP,				/* 18 */
	};
	BLOSSOM_ASSERT(opcode < OPCODE_LAST);
	return OPCODE_IS_BRANCH_OR_SKIP[opcode];
}

bool OpInfo::Skips(const OPCODE opcode) {
	static const bool OPCODE_IS_SKIP[OPCODE_LAST] = {
		false, // DAT,				/* must be 0 */
		false, // SPL,
		false, // MOV,
		false, // DJN,
		false, // ADD,
		false, // JMZ,
		false, // SUB,
		true,  // SEQ,
		true,  // SNE,
		true,  // SLT,
		false, // JMN,
		false, // JMP,
		false, // NOP,
		false, // MUL,
		false, // MODM,
		false, // DIV,
		false, // LDP,
		false  // STP,				/* 18 */
	};
	BLOSSOM_ASSERT(opcode < OPCODE_LAST);
	return OPCODE_IS_SKIP[opcode];
}

bool OpInfo::Continues(const OPCODE opcode) {
	static const bool OPCODE_CONTINUES[OPCODE_LAST] = {
		false, // DAT,				/* must be 0 */
		true,  // SPL,
		true,  // MOV,
		true,  // DJN,
		true,  // ADD,
		true,  // JMZ,
		true,  // SUB,
		true,  // SEQ,
		true,  // SNE,
		true,  // SLT,
		true,  // JMN,
		false, // JMP,
		true,  // NOP,
		true,  // MUL,
		true,  // MODM,
		true,  // DIV,
		true,  // LDP,
		true  // STP,				/* 18 */
	};
	BLOSSOM_ASSERT(opcode < OPCODE_LAST);
	return OPCODE_CONTINUES[opcode];
}



