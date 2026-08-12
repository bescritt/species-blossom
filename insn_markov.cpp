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

#include "insn_markov.hpp"
#include "world.hpp"
#include "rand.hpp"
#include "blossom_utils.hpp"

#include <fstream>

// construction/destruction
CInsnGeneratorMarkov::CInsnGeneratorMarkov(CWorld& world): _world(world), _start(NULL), _head(NULL) {
}

CInsnGeneratorMarkov::~CInsnGeneratorMarkov() {}

void CInsnGeneratorMarkov::load(std::istream& in) {
	BLOSSOM_ASSERT_ALWAYS(in.good());
	BLOSSOM_ASSERT_ALWAYS(FILE_VERSION == binary_read<u32_t>(in));
	BLOSSOM_ASSERT_ALWAYS(_world.coresize() == binary_read<u32_t>(in));
	_start.load_start(in);
	_head.load(in);
	BLOSSOM_ASSERT_ALWAYS(in.good());
}

void CInsnGeneratorMarkov::load(const std::string& filename) {
	std::ifstream in(filename.c_str(),std::ifstream::binary|std::ifstream::in);
	load(in);
	in.close();
}

void CInsnGeneratorMarkov::dump(std::ostream& out) const {
	_start.dump(out,0);
	_head.dump(out,0);
}

// suggesting an opcode

u16_t CInsnGeneratorMarkov::suggest() {
	return _head.suggest()->in();
}

u16_t CInsnGeneratorMarkov::suggest_after(u16_t before) {
	//TODO: be cleverer
	return suggest();
}

u16_t CInsnGeneratorMarkov::suggest_between(u16_t before,u16_t after) {
	//TODO: be cleverer
	return suggest();
}

// construction/destruction

CInsnGeneratorMarkov::CMarkov::CMarkov(const CMarkov* parent): _parent(parent), _in(0), _size(0), _count(0), _chain(0) {
}

CInsnGeneratorMarkov::CMarkov::~CMarkov() {
	for(unsigned i=0; i<_size; i++)
		delete _chain[i];
	delete[] _chain;
}

void CInsnGeneratorMarkov::CMarkov::load(std::istream& in) {
	_in = binary_read<u16_t>(in);
	_count = binary_read<u32_t>(in);
	_size = binary_read<u16_t>(in);
	_chain = new CMarkov*[_size];
	for(unsigned i=0; i<_size; i++)
		_chain[i] = 0;
	unsigned count = 0;
	for(unsigned i=0; i<_size; i++) {
		_chain[i] = new CMarkov(this);
		_chain[i]->load(in);
		count += _chain[i]->_count;
	}
	if(_parent) {
		if(count > _count)
			dump(std::cerr);
		BLOSSOM_ASSERT(count <= _count);
	} else {
		BLOSSOM_ASSERT(!_count);
		_count = count;
	}
}

void CInsnGeneratorMarkov::CMarkov::load_start(std::istream& in) {
	_size = binary_read<u16_t>(in);
	_chain = new CMarkov*[_size];
	for(unsigned i=0; i<_size; i++)
		_chain[i] = 0;
	for(unsigned i=0; i<_size; i++) {
		_chain[i] = new CMarkov(this);
		_chain[i]->_in = binary_read<u16_t>(in);
		_chain[i]->_count = binary_read<u32_t>(in);
		_count += _chain[i]->_count;
	}
}

void CInsnGeneratorMarkov::CMarkov::dump(std::ostream& out,unsigned indent) const {
	for(unsigned i=0; i<indent; i++)
		out << ' ';
	dump(out);
	for(unsigned i=0; i<_size; i++)
		_chain[i]->dump(out,indent+1);
}

void CInsnGeneratorMarkov::CMarkov::dump(std::ostream& out) const {
	insn_t in;
	in.in = _in;
	out << "in=" << Mnemonic::Opcode(in.opcode()) << '.' << Mnemonic::Modifier(in.modifier())
		<< '-' << Mnemonic::AddrMode(in.addrmode_a()) << Mnemonic::AddrMode(in.addrmode_b())
		<< ", size=" << _size << ", count=" << _count << std::endl;
}

// suggesting an opcode

const CInsnGeneratorMarkov::CMarkov* CInsnGeneratorMarkov::CMarkov::suggest() const {
	BLOSSOM_ASSERT(_count);
	const unsigned TARGET = CRand::irand(_count);
	unsigned y = 0;
	for(unsigned i=0; i<_size; i++) {
		y += _chain[i]->_count;
		if(y > TARGET)
			return _chain[i];
	}
	BLOSSOM_ASSERT(0);
	return this;
}

const CInsnGeneratorMarkov::CMarkov* CInsnGeneratorMarkov::CMarkov::markov(u16_t in) const {
	for(unsigned i=0; i<_size; i++) {
		const CMarkov* markov = _chain[i];
		if(markov->in() == in)
			return markov;
	}
	return NULL;
}

u16_t CInsnGeneratorMarkov::CMarkov::in() const {
	return _in;
}

