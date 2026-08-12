#ifndef REPRODUCTION_HPP
#define REPRODUCTION_HPP

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

#include <string>
#include <ostream>

class CEvolvedWarrior;
class CWorld;

class CHeritage {
public:
	virtual ~CHeritage();
	virtual const CEvolvedWarrior* father() const = 0;
	virtual const CEvolvedWarrior* mother() const = 0;
	virtual void to_red(std::ostream& out) const = 0;
protected:
	CHeritage();
};

class CReproduction {
public:
	static CReproduction* Create(CWorld& world,const std::string& type,const std::string& params);
	virtual ~CReproduction();
	virtual CEvolvedWarrior* Create(const CEvolvedWarrior& father,const CEvolvedWarrior& mother) = 0;
protected:
	CWorld& _world;
	CReproduction(CWorld& world);
};

#endif //REPRODUCTION_HPP

