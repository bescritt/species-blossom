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

#include "reproduction.hpp"
#include "warrior.hpp"
#include "world.hpp"
#include "blossom_utils.hpp"
#include "rand.hpp"

#include <iostream>

//************* utility code to cull non-executable warriors *************\\

static bool might_execute(const CWarrior& warrior) {
	for(unsigned i=warrior.start(); i<warrior.len(); i++) {
		const OPCODE op = warrior[i].opcode();
		if(OpInfo::Branches(op))
			return true;
		if(!OpInfo::Continues(op))
			return false;
	}
	return false;
}

//************* CReproductionRandomCrossoverHeritage *************\\

class CReproductionRandomCrossoverHeritage: public CHeritage {
public:
	CReproductionRandomCrossoverHeritage(const CEvolvedWarrior& father,const CEvolvedWarrior& mother);
	virtual const CEvolvedWarrior* father() const;
	virtual const CEvolvedWarrior* mother() const;
	virtual void to_red(std::ostream& out) const;
	void set_info(const std::string& info);
protected:
	const CEvolvedWarrior& _father;
	const CEvolvedWarrior& _mother;
	std::string _info;
};

CReproductionRandomCrossoverHeritage::CReproductionRandomCrossoverHeritage(const CEvolvedWarrior& father,const CEvolvedWarrior& mother):
	_father(father), _mother(mother) {
	_father.IncChildCount();
	_mother.IncChildCount();
}
	
const CEvolvedWarrior* CReproductionRandomCrossoverHeritage::father() const { return &_father; }
const CEvolvedWarrior* CReproductionRandomCrossoverHeritage::mother() const { return &_mother; }

void CReproductionRandomCrossoverHeritage::to_red(std::ostream& out) const {
	out	<< ";father " << _father.uid() << std::endl
		<< ";mother " << _mother.uid() << std::endl;
	if(!_info.empty())
		out << ";heritage " << _info << std::endl;
}

void CReproductionRandomCrossoverHeritage::set_info(const std::string& info) { _info = info; }

//************* CReproductionRandomCrossover *************\\

class CReproductionRandomCrossover: public CReproduction {
public:
	CReproductionRandomCrossover(CWorld& world,const std::string& params);
	virtual CEvolvedWarrior* Create(const CEvolvedWarrior& father,const CEvolvedWarrior& mother);
private:
	enum MUTATION { // where should mutation occur?
		MUT_NONE,
		MUT_OPCODE,
		MUT_MODIFIER,
		MUT_ADDRMODE_A,
		MUT_ADDRMODE_B,
		MUT_OPERAND_A,
		MUT_OPERAND_B,
		MUTATION_LAST
	};
	enum COMPONENT {
		RND_NONE,
		RND_CROSSOVER,
		RND_INSERT,
		RND_REPLACE,
		RND_DELETE,
		COMPONENT_LAST
	};
	// for development purposes, static const for now..
	static const unsigned _mutation[MUTATION_LAST], // roulette-wheel of where mutation will occur
		_mutation_sum, // sum mutation
		_component[COMPONENT_LAST],
		_component_sum;
	MUTATION mutation() const;
	COMPONENT component() const;
};

const unsigned CReproductionRandomCrossover::_mutation[MUTATION_LAST] = {
	600/*MUT_NONE*/,50/*MUT_OPCODE*/,50/*MUT_MODIFIER*/,50/*MUT_ADDRMODE_A*/,50/*MUT_ADDRMODE_B*/,
	100/*MUT_OPERAND_A*/,100/*MUT_OPERAND_B*/
};
const unsigned CReproductionRandomCrossover::_mutation_sum = 1000;

const unsigned CReproductionRandomCrossover::_component[COMPONENT_LAST] = {
	700/*RND_NONE*/,100/*RND_CROSSOVER*/,50/*RND_INSERT*/,100/*RND_REPLACE*/,50/*RND_DELETE*/
};
const unsigned CReproductionRandomCrossover::_component_sum = 1000;

CReproductionRandomCrossover::CReproductionRandomCrossover(CWorld& world,const std::string& /*params*/):
	CReproduction(world) {}

CEvolvedWarrior* CReproductionRandomCrossover::Create(const CEvolvedWarrior& father,const CEvolvedWarrior& mother) {
	CReproductionRandomCrossoverHeritage* heritage = new CReproductionRandomCrossoverHeritage(father,mother);
	CEvolvedWarrior* warrior = new CEvolvedWarrior(_world,heritage);
	unsigned tries = 0;
	bool success = false;
	do {
		tries++;
		// temporarily copy father verbetim
		warrior_t& direct = warrior->direct();
		direct.len = CRand::irand(_world.maxlength()-1) + 1;
		const unsigned len = warrior->len();
		direct.start = CRand::irand(len);
		std::string info;
		info.reserve(direct.len*2);
		info = 'f';
		const CEvolvedWarrior* parent = &father;
		for(unsigned i=0; i<len; ) {
			switch(component()) {
				case RND_CROSSOVER:
					if(parent == &father)
						parent = &mother;
					else
						parent = &father;
					info += 'x';
					break;
				default:
					if(parent->len() > i) {
						direct.code[i] = parent->code(i);
						info += '-';
					} else {
						direct.code[i].set_clear();
						info += '_';
					}
					i++;
			};
		}
		success = might_execute(*warrior);
		if(success) {
			heritage->set_info(info);
			warrior->freeze();
			std::cout << "(took " << tries << " tries)" << std::endl;
		} else {
			warrior->reset();
		}
	} while(!success);
	return warrior;
}

CReproductionRandomCrossover::MUTATION CReproductionRandomCrossover::mutation() const {
	const unsigned stop = CRand::irand(_mutation_sum);
	unsigned ball = 0;
	for(unsigned i=0; i<MUTATION_LAST; i++) {
		ball += _mutation[i];
		if(stop < ball)
			return (MUTATION)i;
	}
	BLOSSOM_INVARIANT_ALWAYS();
	return MUTATION_LAST;
}

CReproductionRandomCrossover::COMPONENT CReproductionRandomCrossover::component() const {
	const unsigned stop = CRand::irand(_component_sum);
	unsigned ball = 0;
	for(unsigned i=0; i<COMPONENT_LAST; i++) {
		ball += _component[i];
		if(stop < ball)
			return (COMPONENT)i;
	}
	BLOSSOM_INVARIANT_ALWAYS();
	return COMPONENT_LAST;
}

//************* CReproduction *************\\

CReproduction* CReproduction::Create(CWorld& world,const std::string& type,const std::string& params) {
	if(type == "random-crossover") {
		return new CReproductionRandomCrossover(world,params);
	} else {
		BLOSSOM_INVARIANT_ALWAYS();
		return NULL;
	}
}

CReproduction::~CReproduction() {}

CReproduction::CReproduction(CWorld& world): _world(world) {}

//************* CHeritage *************\\

CHeritage::CHeritage() {}

CHeritage::~CHeritage() {}
