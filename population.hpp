#ifndef __POPULATION_HPP__
#define __POPULATION_HPP__

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

#include <vector>
#include <string>

#include "warrior.hpp"

class CPopulation {
public:
	CPopulation(CWorld& world,const std::string& name);
	virtual ~CPopulation();
	const std::string& name() const;
	unsigned append(CEvolvedWarrior* warrior);
	unsigned size() const;
	const CEvolvedWarrior& warrior(unsigned idx) const;
	const CEvolvedWarrior* warrior_by_uid(CWarrior::uid_t uid) const;
	void breed(unsigned num);
	bool loop();
	CWorld& world();
	const CWorld& world() const;
private:
	CWorld& _world;
	std::string _name;
	typedef	std::vector<CEvolvedWarrior*> TWarriors;
	TWarriors _warriors;
	TWarriors _old_warriors;
};

#endif //__POPULATION_HPP__

