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

#include "benchmark.hpp"
#include "warrior.hpp"
#include "world.hpp"
#include "blossom_utils.hpp"

//************* CBenchmark *************\\

CBenchmark::CBenchmark(CWorld& world,const std::string& name): _world(world), _name(name) {
}

CBenchmark::~CBenchmark() {
	for(unsigned i=0; i<size(); i++) {
		delete _warrior[i];
	}
}

const std::string& CBenchmark::name() const { return _name; }

unsigned CBenchmark::size() const { return _warrior.size(); }

void CBenchmark::add(const char* rc_name) {
	_warrior.push_back(new CWarrior(_world,rc_name,true));
}

const CWarrior& CBenchmark::warrior(unsigned idx) const {
	BLOSSOM_ASSERT(size() > idx);
	return *_warrior[idx];
}

const CWarrior& CBenchmark::operator[](unsigned idx) const {
	return warrior(idx);
}

