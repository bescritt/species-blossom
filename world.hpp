#ifndef WORLD_HPP
#define WORLD_HPP

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
#include <ostream>

class CBenchmark;
class CPopulation;
class CReproduction;
class CBattleScheduler;
class plotter_t;

class CWorld {
public:
	CWorld();
	~CWorld();
	const std::string& hill() const;
	void red_assert(std::ostream& out) const;
	unsigned maxlength() const;
	unsigned coresize() const;
	unsigned pspacesize() const;
	unsigned maxprocesses() const;
	unsigned minsep() const;
	unsigned cycles() const;
	CBattleScheduler& battle_scheduler();
	unsigned add_benchmark(CBenchmark* benchmark);
	unsigned benchmark_count() const;
	const CBenchmark& benchmark(unsigned idx) const;
	unsigned add_population(CPopulation* population);
	unsigned population_count() const;
	CPopulation& population(unsigned idx);
	unsigned rounds() const;
	void set_rounds(unsigned rounds);
	void set_cycles(unsigned cycles);
	CReproduction& reproduction();
	bool loop();
	void set_plotter(plotter_t* plotter);
	plotter_t* plotter() const;
private:
	unsigned _maxlength, _coresize, _pspacesize, _maxprocesses, _cycles, _minsep, _rounds;
	std::string _hill;
	CBattleScheduler* _battle_scheduler;
	std::vector<CBenchmark*> _benchmark;
	std::vector<CPopulation*> _population;
	CReproduction* _reproduction;
	plotter_t* _plotter;
};

#endif //WORLD_HPP

