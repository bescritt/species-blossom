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

#include "battle.hpp"
#include "warrior.hpp"
#include "world.hpp"
#include "blossom_utils.hpp"
#include "rand.hpp"

#include <list>

//************* CBattle *************\\

CBattle::CBattle() {}
CBattle::~CBattle() {}

CBattle::TResults::TResults(): win(0), loss(0), tie(0) {}

//************* CBattleBase *************\\

class CBattleBase: public CBattle {
public:
	CBattleBase(const std::vector<const CWarrior*>& warriors,unsigned rounds,TYPE type,unsigned flags,IBattleOwner* owner,void* owner_opaque_info);
	virtual TYPE type() const;
	virtual unsigned flags() const;
	virtual unsigned rounds() const;
	virtual unsigned warrior_count() const;
	virtual const CWarrior* warrior(unsigned idx) const;
	virtual TResults results(unsigned idx) const;
	virtual TResults& results(unsigned idx);
	void done();
protected:
	TYPE _type;
	unsigned _flags;
	unsigned _rounds;
	std::vector<const CWarrior*> _warriors;
	std::vector<TResults> _results;
	IBattleOwner* _owner;
	void* _owner_opaque_info;
};
	
CBattleBase::CBattleBase(const std::vector<const CWarrior*>& warriors,unsigned rounds,TYPE type,unsigned flags,IBattleOwner* owner,void* owner_opaque_info):
	_type(type), _flags(flags), _rounds(rounds), _warriors(warriors.begin(),warriors.end()), _results(warriors.size()), _owner(owner), _owner_opaque_info(owner_opaque_info) {
	BLOSSOM_ASSERT(_warriors.size());
	BLOSSOM_ASSERT(_owner);
}

CBattle::TYPE CBattleBase::type() const { return _type; }

unsigned CBattleBase::flags() const { return _flags; }

unsigned CBattleBase::rounds() const { return _rounds; }

unsigned CBattleBase::warrior_count() const { return _warriors.size(); }

const CWarrior* CBattleBase::warrior(unsigned idx) const {
	BLOSSOM_ASSERT(idx < warrior_count());
	return _warriors[idx];
}

CBattle::TResults CBattleBase::results(unsigned idx) const {
	BLOSSOM_ASSERT(idx < warrior_count());
	return _results[idx];
}

CBattle::TResults& CBattleBase::results(unsigned idx) {
	BLOSSOM_ASSERT(idx < warrior_count());
	return _results[idx];
}

void CBattleBase::done() {
	if(_owner)
		_owner->battle_done(this,_owner_opaque_info); // takes ownership
	else
		delete this;
}

//************* CBattleSchedulerExhaust *************\\

class CBattleSchedulerExhaust: public CBattleScheduler {
public:
	CBattleSchedulerExhaust(CWorld& world,const std::string& params);
	~CBattleSchedulerExhaust();
	virtual void schedule(const CWarrior& warrior,const CWarrior& enemy,unsigned rounds,IBattleOwner* owner,void* owner_opaque_info,unsigned flags);
	virtual void schedule(const std::vector<const CWarrior*>& warriors,unsigned rounds,IBattleOwner* owner,void* owner_opaque_info,CBattle::TYPE type,unsigned flags);
	virtual unsigned pending();
	virtual bool loop();
private:
	CWorld& _world;
	void fight(CBattleBase* battle) const;
	typedef	std::list<CBattleBase*> TFought;
	TFought _fought;
};

CBattleSchedulerExhaust::CBattleSchedulerExhaust(CWorld& world,const std::string& /*params*/):
	_world(world) {
	sim_alloc_bufs2(2, _world.coresize(), _world.maxprocesses(), _world.cycles(), _world.pspacesize());
}

CBattleSchedulerExhaust::~CBattleSchedulerExhaust() {
	sim_free_bufs();
}

void CBattleSchedulerExhaust::schedule(const CWarrior& warrior,const CWarrior& enemy,unsigned rounds,IBattleOwner* owner,void* owner_opaque_info,unsigned flags) {
	std::vector<const CWarrior*> warriors(2);
	warriors[0] = &warrior;
	warriors[1] = &enemy;
	schedule(warriors,rounds,owner,owner_opaque_info,CBattle::ROUND_ROBIN,flags);
}

void CBattleSchedulerExhaust::schedule(const std::vector<const CWarrior*>& warriors,unsigned rounds,IBattleOwner* owner,void* owner_opaque_info,CBattle::TYPE type,unsigned flags) {
	BLOSSOM_ASSERT_ALWAYS(owner);
	CBattleBase* battle = new CBattleBase(warriors,rounds,type,flags,owner,owner_opaque_info);
	fight(battle);
	_fought.push_back(battle);
}

unsigned CBattleSchedulerExhaust::pending() {
	return _fought.size();
}

bool CBattleSchedulerExhaust::loop() {
	for(TFought::iterator battle=_fought.begin(); battle != _fought.end(); battle++) {
		(*battle)->done();
	}
	_fought.clear();
	return false;
}

void CBattleSchedulerExhaust::fight(CBattleBase* battle) const {
	// actually, we are only set up to do duels so far
	BLOSSOM_ASSERT_ALWAYS(CBattle::ROUND_ROBIN == battle->type());
	BLOSSOM_ASSERT_ALWAYS(battle->warrior_count() == 2);
	static const unsigned startpos = _world.minsep(), range = (_world.coresize()-(startpos*2)), rounds = battle->rounds();
	sim_reset_pspaces();
	for(unsigned i = 0; i < rounds; i++) {
		sim_clear_core();
		sim_load_warrior(0,battle->warrior(0)->direct().code,battle->warrior(0)->len());
		const unsigned enemypos = startpos+CRand::irand(range);
		sim_load_warrior(enemypos,battle->warrior(1)->direct().code,battle->warrior(1)->len());
		switch(sim(2,battle->warrior(0)->start(),(battle->warrior(1)->start()+enemypos),_world.cycles(),0)) {
			case 0: // warrior[0] wins
				battle->results(0).win++;
				battle->results(1).loss++;
				break;
			case 1: // warrior[1] wins
				battle->results(0).loss++;
				battle->results(1).win++;
				break;
			case 2: // all tie
				battle->results(0).tie++;
				battle->results(1).tie++;
				break;
			default: // e.g. sim panic
				BLOSSOM_INVARIANT();
		}
	}
}

//************* CBattleScheduler *************\\

CBattleScheduler* CBattleScheduler::Create(CWorld& world,const std::string& type,const std::string& params) {
	if(type == "local-exhaust") {
		return new CBattleSchedulerExhaust(world,params);
	}
	BLOSSOM_INVARIANT();
}

CBattleScheduler::~CBattleScheduler() {}

CBattleScheduler::CBattleScheduler() {}


