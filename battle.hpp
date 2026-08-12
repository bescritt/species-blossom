#ifndef __BATTLE_HPP__
#define __BATTLE_HPP__

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

class CWorld;
class CWarrior;

#include <string>
#include <vector>

class CBattle {
public:
	enum TYPE {
		ROUND_ROBIN,
		BENCHMARK
	};
	enum FLAGS {
		ALLOW_PSPACE =	0x00000001,
		SELF_FIGHTS =	0x00000002
	};
	struct TResults {
		TResults();
		unsigned win;
		unsigned loss;
		unsigned tie;
	};
	virtual ~CBattle();
	virtual TYPE type() const = 0;
	virtual unsigned flags() const = 0;
	virtual unsigned rounds() const = 0;
	virtual unsigned warrior_count() const = 0;
	virtual const CWarrior* warrior(unsigned idx) const = 0;
	virtual TResults results(unsigned idx) const = 0;
protected:
	CBattle();
};

class IBattleOwner {
public:
	/** IBattleOwner assumes ownership of battle */
	virtual void battle_done(CBattle* battle,void* opaque_info) = 0;
};

class CBattleScheduler {
public:
	static CBattleScheduler* Create(CWorld& world,const std::string& type,const std::string& params);
	virtual ~CBattleScheduler();
	virtual void schedule(const CWarrior& warrior,const CWarrior& enemy,unsigned rounds,IBattleOwner* owner,void* owner_opaque_info,unsigned flags = 0) = 0;
	virtual void schedule(const std::vector<const CWarrior*>& warriors,unsigned rounds,IBattleOwner* owner,void* owner_opaque_info,CBattle::TYPE type,unsigned flags) = 0;
	virtual unsigned pending() = 0; ///< any battles still ongoing, results not yet back?
	virtual bool loop() = 0; ///< give the scheduler a chance to update any scores, and returns whether it may need to be run again at a future date
protected:
	CBattleScheduler();
};

#endif //__BATTLE_HPP__

