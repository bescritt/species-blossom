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

#include "world.hpp"
#include "battle.hpp"
#include "population.hpp"
#include "blossom_utils.hpp"
#include "reproduction.hpp"
#include "benchmark.hpp"

//************* CWorld *************\\

CWorld::CWorld():
	_maxlength(100), _coresize(8000), _hill("-94nop"), _cycles(80000), 
	_maxprocesses(8000), _pspacesize(8000/12), _minsep(100), _rounds(100),
	_battle_scheduler(0), _reproduction(0), _plotter(0) {
	_battle_scheduler = CBattleScheduler::Create(*this,"local-exhaust","");
	_reproduction = CReproduction::Create(*this,"random-crossover","");
}

CWorld::~CWorld() {
	for(unsigned i=0; i<_benchmark.size(); i++)
		delete _benchmark[i];
	for(unsigned i=0; i<_population.size(); i++)
		delete _population[i];
	delete _battle_scheduler;
	delete _reproduction;
}

const std::string& CWorld::hill() const { return _hill; }
unsigned CWorld::maxlength() const { return _maxlength; }
unsigned CWorld::coresize() const { return _coresize; }
unsigned CWorld::pspacesize() const { return _pspacesize; }
unsigned CWorld::maxprocesses() const { return _maxprocesses; }
unsigned CWorld::cycles() const { return _cycles; }
unsigned CWorld::minsep() const { return _minsep; }
unsigned CWorld::rounds() const { return _rounds; }

void CWorld::set_rounds(unsigned rounds) {
	_rounds = rounds;
}

void CWorld::set_cycles(unsigned cycles) {
	_cycles = cycles;
}

void CWorld::red_assert(std::ostream& out) const {
	out << ";assert (" << coresize() << "==CORESIZE)" << std::endl;
}

CBattleScheduler& CWorld::battle_scheduler() { return *_battle_scheduler; }

/** assumes ownership of benchmark */
unsigned CWorld::add_benchmark(CBenchmark* benchmark) {
	_benchmark.push_back(benchmark);
	return benchmark_count()-1;
}

unsigned CWorld::benchmark_count() const {
	return _benchmark.size();
}

const CBenchmark& CWorld::benchmark(unsigned idx) const {
	BLOSSOM_ASSERT(idx < benchmark_count());
	return *_benchmark[idx];
}

unsigned CWorld::population_count() const {
	return _population.size();
}

CPopulation& CWorld::population(unsigned idx) {
	BLOSSOM_ASSERT(idx < population_count());
	return *_population[idx];
}

unsigned CWorld::add_population(CPopulation* population) {
	_population.push_back(population);
	return population_count()-1;
}

CReproduction& CWorld::reproduction() { return *_reproduction; }

bool CWorld::loop() {
	const unsigned count = population_count();
	bool all_pop_done = true;
	for(unsigned i=0; i<count; i++) {
		all_pop_done &= _population[i]->loop();
		battle_scheduler().loop();
	}
	return !all_pop_done;
}

void CWorld::set_plotter(plotter_t* plotter) {
	_plotter = plotter;
}

plotter_t* CWorld::plotter() const {
	return _plotter;
}

