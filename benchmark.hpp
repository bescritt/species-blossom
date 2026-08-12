#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

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
#include <vector>

class CWorld;
class CWarrior;

class CBenchmark {
public:
	CBenchmark(CWorld& world,const std::string& name);
	~CBenchmark();
	const std::string& name() const;
	unsigned size() const;
	void add(const char* rc_name);
	const CWarrior& warrior(unsigned idx) const;
	const CWarrior& operator[](unsigned idx) const;
private:
	CWorld& _world;
	std::string _name;
	std::vector<CWarrior*> _warrior;
};

#endif //BENCHMARK_HPP
