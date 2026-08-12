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

#include "population.hpp"
#include "world.hpp"
#include "warrior.hpp"
#include "blossom_utils.hpp"
#include "rand.hpp"
#include "reproduction.hpp"

#include <algorithm>
#include <iostream>

//************* CPopulation *************\\

CPopulation::CPopulation(CWorld& world,const std::string& name): _world(world), _name(name) {
}

CPopulation::~CPopulation() {
	for(TWarriors::iterator i=_warriors.begin(); i!=_warriors.end(); ++i)
		delete *i;
	for(TWarriors::iterator i=_old_warriors.begin(); i!=_old_warriors.end(); ++i)
		delete *i;
}

const std::string& CPopulation::name() const { return _name; }

unsigned CPopulation::append(CEvolvedWarrior* warrior) {
	_warriors.push_back(warrior);
	return size()-1;
}

unsigned CPopulation::size() const {
	return _warriors.size();
}

const CEvolvedWarrior& CPopulation::warrior(unsigned idx) const {
	BLOSSOM_ASSERT(idx < size());
	return *_warriors[idx];
}

const CEvolvedWarrior* CPopulation::warrior_by_uid(CWarrior::uid_t uid) const {
	for(TWarriors::const_iterator i=_warriors.begin(); i!=_warriors.end(); ++i) {
		if((*i)->uid() == uid) {
			return *i;
		}
	}
	return NULL;
}

void CPopulation::breed(unsigned num) {
	std::cout << "Popluation \"" << _name << "\" is breeding " << num << " new warriors" << std::endl;
	// sort warriors by score
	std::sort(_warriors.begin(),_warriors.end(),std::less<CEvolvedWarrior*>());
	// remove the bottom num to make way for the new ones
	for(unsigned i=0; (i<num) && (_warriors.size()); i++) {
		CEvolvedWarrior* too_bad = _warriors.back();
		std::cout << "\tretiring \"" << too_bad->rc_name() << std::endl;
		_warriors.pop_back();
		if(const CEvolvedWarrior* father = too_bad->heritage()->father())
			father->DecChildCount();
		if(const CEvolvedWarrior* mother = too_bad->heritage()->mother())
			mother->DecChildCount();
		_old_warriors.push_back(too_bad);
	}
	// count the total score of the population
	unsigned total_score = 0;
	for(unsigned i=0; (i<num) && (i<size()); i++) {
		total_score += warrior(i).overall_score();
	}
	// first num get the chance
	for(unsigned i=0; (i<num) && (i<size()); i++) {
		const CEvolvedWarrior& lucky = warrior(i);
		std::cout << i << ": " << lucky.rc_name() << " scores " << lucky.overall_score() << std::endl;
		long roll = CRand::irand(total_score);
		bool mated = false;
		do {
			for(unsigned j=0; j<size(); j++) {
				roll -= warrior(j).overall_score();
				if(0 >= roll) {
					if(j != i) {
						append(_world.reproduction().Create(lucky,warrior(j)));
						mated = true;
					}
					break;
				}
			}
		} while(!mated);
	}
}

bool CPopulation::loop() {
	// schedule fights
	bool all_fought = true;
	for(TWarriors::iterator i=_warriors.begin(); i!=_warriors.end(); ++i) {
		if(!(*i)->fought()) {
			all_fought = false;
			(*i)->fight();
		}
	}
	return all_fought;	
}

CWorld& CPopulation::world() {
	return _world;
}

const CWorld& CPopulation::world() const {
	return _world;
}

