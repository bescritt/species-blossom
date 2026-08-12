#ifndef WARRIOR_HPP
#define WARRIOR_HPP

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
#include "battle.hpp"
#include <string>
#include <ostream>
#include <vector>

class CWorld;
class CReproduction;
class CBenchmark;
class CHeritage;

class CWarrior: protected warrior_t {
public:
	typedef unsigned uid_t;
	CWarrior(CWorld& world,const std::string& rc_name,bool read_only);
	virtual ~CWarrior();
	virtual std::string rc_name() const;
	unsigned start() const;
	unsigned len() const;
	const insn_t& code(unsigned idx) const;
	const insn_t& operator[](unsigned idx) const;
	void set_code(unsigned idx,const insn_t& in);
	warrior_t& direct();
	const warrior_t& direct() const;
	void to_red(std::ostream& out) const;
	void to_rc(std::ostream& out) const;
	uid_t uid() const;
protected:
	CWarrior(CWorld& world);
	CWorld& _world;
	std::string _rc_name;
	uid_t _uid;
	bool _read_only;
	virtual void to_red_header(std::ostream& out) const;
};

class CEvolvedWarrior: public CWarrior, private IBattleOwner {
public:
	CEvolvedWarrior(CWorld& world,const CHeritage* heritage);
	CEvolvedWarrior(CWorld& world,const std::string& rc_name);
	virtual ~CEvolvedWarrior();
	//virtual std::string rc_name() const;
	void fight();
	bool fighting() const;
	bool fought() const;
	unsigned normalised_score(const CBenchmark& benchmark) const;
	unsigned overall_score() const;
	const CHeritage* heritage() const;
	void freeze();
	void reset();
	void IncChildCount() const;
	void DecChildCount() const;
	unsigned ChildCount() const;
	bool operator <(const CEvolvedWarrior& other) const;
private:
	const CHeritage* _heritage;
	virtual void to_red_header(std::ostream& out) const;
	struct TScore {
		unsigned _pending;
		const CBenchmark* _benchmark;
		unsigned _score, _normalised_score;
	};
	typedef std::vector<TScore> TScores;
	TScores _score;
	void fight(const CBenchmark& benchmark,unsigned rounds);
	virtual void battle_done(CBattle* battle,void* opaque_info);
	TScores::const_iterator find(const CBenchmark& benchmark) const;
	TScores::iterator find(const CBenchmark& benchmark);
	mutable unsigned _child_count;
};

#endif //WARRIOR_HPP

