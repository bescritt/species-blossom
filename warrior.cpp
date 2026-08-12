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

#include "warrior.hpp"
#include "blossom_utils.hpp"
#include "world.hpp"
#include "reproduction.hpp"
#include "benchmark.hpp"

#include <sstream>
#include <iostream>

//************* CWarrior *************\\

static CWarrior::uid_t next_uid() {
	static CWarrior::uid_t seq = 0;
	return seq++;
}

CWarrior::CWarrior(CWorld& world,const std::string& rc_name,bool read_only): 		_world(world), _rc_name(rc_name), _read_only(read_only) {
	_uid = next_uid();
	warrior_t::len = 0;
	warrior_t::start = 0;
	asm_fname(_rc_name.c_str(),this,_world.coresize());
	BLOSSOM_ASSERT(_world.maxlength() >= len());
}

CWarrior::CWarrior(CWorld& world):
	_world(world), _read_only(false) {
	_uid = next_uid();
	warrior_t::len = 0;
	warrior_t::start = 0;
}

CWarrior::~CWarrior() {}

std::string CWarrior::rc_name() const { return _rc_name; }

unsigned CWarrior::start() const { return warrior_t::start; }
unsigned CWarrior::len() const { return warrior_t::len; }

CWarrior::uid_t CWarrior::uid() const { return _uid; }

void CWarrior::to_red(std::ostream& out) const {
	out 	<< ";redcode" << _world.hill() << std::endl
		<< ";filename " << rc_name() << std::endl;
	to_red_header(out);
	to_rc(out);
}

void CWarrior::to_red_header(std::ostream& out) const {}

void CWarrior::to_rc(std::ostream& out) const {
	char line[40];
	out << "\torg start" << std::endl;
	for(unsigned i=0; i<len(); i++) {
		if(start()==i)
			out << "start";
		dis1(line,code(i),_world.coresize());
		out << '\t' << line << std::endl;
	}
	out << "\tend" << std::endl;
}

const insn_t& CWarrior::code(unsigned idx) const {
	BLOSSOM_ASSERT(len() > idx);
	return warrior_t::code[idx];
}

const insn_t& CWarrior::operator[](unsigned idx) const {
	return code(idx);
}

void CWarrior::set_code(unsigned idx,const insn_t& in) {
	BLOSSOM_ASSERT(len() > idx);
	BLOSSOM_ASSERT(!_read_only);
	//todo: validate in
	warrior_t::code[idx] = in;
}

warrior_t& CWarrior::direct() {
	BLOSSOM_ASSERT(!_read_only);
	return *this;
}

const warrior_t& CWarrior::direct() const {
	return *this;
}

//************* CEvolvedWarrior *************\\

CEvolvedWarrior::CEvolvedWarrior(CWorld& world,const CHeritage* heritage):
	CWarrior(world), _heritage(heritage), _child_count(0) {
	std::stringstream ret(std::stringstream::in|std::stringstream::out);
	ret << "evolved-" << uid() << ".rc";
	_rc_name = ret.str();
}

CEvolvedWarrior::CEvolvedWarrior(CWorld& world,const std::string& rc_name):
	CWarrior(world,rc_name,false), _heritage(0), _child_count(0) {}

CEvolvedWarrior::~CEvolvedWarrior() {
	delete _heritage;
}

void CEvolvedWarrior::to_red_header(std::ostream& out) const {
	if(_heritage)
		_heritage->to_red(out);
	_world.red_assert(out);
}

void CEvolvedWarrior::fight() {
	const unsigned count = _world.benchmark_count();
	for(unsigned i=0; i<count; i++) {
		const CBenchmark& benchmark = _world.benchmark(i);
		if(_score.end() == find(benchmark)) { // not yet graded?
			fight(benchmark,_world.rounds());
		}
	}
}

/** are any scheduled fights still underway? */
bool CEvolvedWarrior::fighting() const {
	for(TScores::const_iterator i=_score.begin(); i!=_score.end(); ++i) {
		if(i->_pending)
			return true;
	}
	return false;
}

/** has this warrior been scheduled to fight, and are all those scheduled fights completed? */
bool CEvolvedWarrior::fought() const {
	if(_score.size()) {
		for(TScores::const_iterator i=_score.begin(); i!=_score.end(); ++i) {
			if(i->_pending)
				return false;
		}
		return true;
	} else { // not yet fighting!
		return false;
	}
}

void CEvolvedWarrior::fight(const CBenchmark& benchmark,unsigned rounds) {
	BLOSSOM_ASSERT(_score.end() == find(benchmark));
	TScore ret;
	ret._benchmark = &benchmark;
	ret._score = 0;
	ret._normalised_score = 0;
	ret._pending = benchmark.size();
	_score.push_back(ret);
	for(unsigned i=0; i<ret._pending; i++) {
		_world.battle_scheduler().schedule(*this,benchmark[i],rounds,this,const_cast<CBenchmark*>(ret._benchmark));
	}
}

unsigned CEvolvedWarrior::normalised_score(const CBenchmark& benchmark) const {
	TScores::const_iterator i=find(benchmark);
	if(i != _score.end() && !i->_pending)
		return i->_normalised_score;
	return ~0;
}

unsigned CEvolvedWarrior::overall_score() const {
	unsigned count = 0;
	unsigned overall = 0;
	for(TScores::const_iterator i=_score.begin(); i!=_score.end(); ++i)
		if(i->_pending)
			return ~0;
		else {
			overall += i->_normalised_score;
			count++;
		}
	if(!count)
		return ~0;
	return overall / count;
}

void CEvolvedWarrior::battle_done(CBattle* battle,void* opaque_info) {
	BLOSSOM_ASSERT(battle && opaque_info);
	BLOSSOM_ASSERT(battle->warrior_count() == 2);
	BLOSSOM_ASSERT(battle->warrior(0) == this);
	BLOSSOM_ASSERT(battle->rounds() == _world.rounds());
	static const unsigned WIN = 3, LOSS = 0, TIE = 1;
	const CBenchmark* benchmark = const_cast<const CBenchmark*>(static_cast<CBenchmark*>(opaque_info));
	std::cout << rc_name() << " vs. " << battle->warrior(1)->rc_name();
	TScores::iterator i=find(*benchmark);
	if(_score.end() != i) {
		BLOSSOM_ASSERT(i->_pending);
		const CBattle::TResults results = battle->results(0);
		std::cout << " (" << results.win << ',' << results.loss << ',' << results.tie << ')' << std::endl;
		i->_score += (results.win*WIN);
		i->_score += (results.tie*TIE);
		i->_score += (results.loss*LOSS);
		if(!--i->_pending) {
			const unsigned rounds = _world.rounds() * benchmark->size();
			const unsigned maxpossible = rounds * 3;
			i->_normalised_score = ((i->_score*1000)/maxpossible);
			std::cout << rc_name() << " scores " << i->_normalised_score << " against " << benchmark->name() << std::endl;
		}
	} else {
		BLOSSOM_INVARIANT_ALWAYS();
	}
	delete battle;
}

CEvolvedWarrior::TScores::const_iterator CEvolvedWarrior::find(const CBenchmark& benchmark) const {
	for(TScores::const_iterator i=_score.begin(); i!=_score.end(); ++i)
		if(i->_benchmark == &benchmark)
			return i;
	return _score.end();
}

CEvolvedWarrior::TScores::iterator CEvolvedWarrior::find(const CBenchmark& benchmark) {
	for(TScores::iterator i=_score.begin(); i!=_score.end(); ++i)
		if(i->_benchmark == &benchmark)
			return i;
	return _score.end();
}

const CHeritage* CEvolvedWarrior::heritage() const { return _heritage; }

void CEvolvedWarrior::freeze() { 
	BLOSSOM_ASSERT(!_read_only);
	_read_only = true;
}

void CEvolvedWarrior::reset() {
	BLOSSOM_ASSERT(!_read_only);
	warrior_t::start = 0;
	warrior_t::len = 0;
}

bool CEvolvedWarrior::operator <(const CEvolvedWarrior& other) const {
	return other.overall_score() < overall_score();
}

void CEvolvedWarrior::IncChildCount() const {
	_child_count++;
}

void CEvolvedWarrior::DecChildCount() const {
	BLOSSOM_ASSERT(_child_count);
	_child_count--;
}

unsigned CEvolvedWarrior::ChildCount() const {
	return _child_count;
}

