/* "Species" - a CoreWars evolver.  Copyright (C) 2003 'Varfar' 
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

#include "inst_gen.hpp"

// concrete implementation headers
#include "inst_gen_weighted_random.hpp"
#include "inst_gen_markov.hpp"
#include "inst_gen_markov_2.hpp"

#include "opcode_branch_lookup.hpp"

#include "species.hpp"
#include "rand.hpp"

//#include <string.h>

/************ CInstGenerator implementation ****************************/

const char *CInstGenerator::IMPL_DESC[CInstGenerator::IMPL_LAST] = { "weighted_random","markov","markov_2" };

CInstGenerator::CInstGenerator() {
	for(unsigned i=0; i<OPCODE_LAST; i++) {
		_opcode_allowed[(OPCODE)i] = true; // assume it is
	}
}

const char *CInstGenerator::impl_desc(const CInstGenerator::IMPL type) {
	if((type >= 0) && (type < IMPL_LAST))
		return IMPL_DESC[type];
	return 0; // bad
}

CInstGenerator::IMPL CInstGenerator::desc_to_impl(const char *desc) {
	for(int i=0; i<IMPL_LAST; i++) {
		if(strcasecmp(desc,IMPL_DESC[i])==0)
			return (IMPL)i;
	}
	PANIC(MISC,"bad inst_gen type",NULL);
}

CInstGenerator *CInstGenerator::read_ini(INIFile &ini) {
	KeyValuePair *kvp;
	CInstGenerator *ret;
	// identify which implementation, based upon keywords
	kvp = ini.get("inst_gen");
	if(0 == kvp) PANIC(MISC,"inst_gen expected",NULL);
	switch(desc_to_impl(kvp->getValueAsChar())) {
		case MARKOV_2:
			ret = new CInstGeneratorMarkov2();
			break;
		case WEIGHTED_RANDOM:
			ret = new CInstGeneratorWeightedRandom();
			break;
		case MARKOV:
			ret = new CInstGeneratorMarkov();
			break;
		default:
			PANIC(MISC,kvp->getValueAsChar(),"inst_gen not yet supported");
	}
	ret->read_ini_impl(ini);
	return ret;
}

CInstGenerator *CInstGenerator::read_override_ini(CInstGenerator *gen,INIFile &ini) {
	KeyValuePair *kvp;
	CInstGenerator *ret;
	// identify which implementation, based upon keywords
	kvp = ini.get("inst_gen");
	if(0 == kvp) return gen; // nothing to do?
	switch(desc_to_impl(kvp->getValueAsChar())) {
		case MARKOV_2:
			if(same_type(gen,ret)) {
				ret =  new CInstGeneratorMarkov2((CInstGeneratorMarkov2&)*gen);
				ret->read_override_ini_impl(ini);
			} else {
				ret = new CInstGeneratorMarkov2();
				ret->read_ini_impl(ini);
			}
			break;		
		case MARKOV:
			if(same_type(gen,ret)) {
				ret =  new CInstGeneratorMarkov((CInstGeneratorMarkov&)*gen);
				ret->read_override_ini_impl(ini);
			} else {
				ret = new CInstGeneratorMarkov();
				ret->read_ini_impl(ini);
			}
			break;
		case WEIGHTED_RANDOM:
			if(same_type(gen,ret)) {
				ret =  new CInstGeneratorWeightedRandom((CInstGeneratorWeightedRandom&)*gen);
				ret->read_override_ini_impl(ini);
			} else {
				ret = new CInstGeneratorWeightedRandom();
				ret->read_ini_impl(ini);
			}
			break;
		default:
			PANIC(MISC,kvp->getValueAsChar(),"inst_gen not yet supported");
	}
	return ret;
}

void CInstGenerator::generate_completely_random(insn_t &instruction) const { // utility
	OPCODE op;
	do {
		op = (OPCODE)CRand::irand(OPCODE_LAST);
	} while(!opcode_allowed(op));
	instruction.in = OP(op,CRand::irand(MODIFIER_LAST),CRand::irand(ADDRMODE_LAST),CRand::irand(ADDRMODE_LAST));
	instruction.set_modifier(EFFECTIVE_MODIFIER(op,instruction.modifier()));
	_chromosome->type()->operands()->rnd(instruction,COperand::A);
	_chromosome->type()->operands()->rnd(instruction,COperand::B);
}

void CInstGenerator::read_opcode_allowed(INIFile &ini) {
	for(unsigned i=0; i<OPCODE_LAST; i++) {
		KeyValuePair *kvp = ini.get(MNEMONIC_OPCODE((OPCODE)i));
		if(0 != kvp) {
			_opcode_allowed[(OPCODE)i] = kvp->getValueAsBool(_opcode_allowed[(OPCODE)i]);
		}
	}
}

void CInstGenerator::write_opcode_allowed(std::ostream &os,const CInstGenerator *parent) const {
	for(unsigned i=0; i<OPCODE_LAST; i++) {
		bool allowed = opcode_allowed((OPCODE)i);
		if(0 != parent) allowed = (allowed == parent->opcode_allowed((OPCODE)i));
		if(!allowed) {
			os << MNEMONIC_OPCODE((OPCODE)i) << '=' << opcode_allowed((OPCODE)i) << std::endl;
		}
	}
}


