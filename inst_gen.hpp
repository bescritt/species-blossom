#ifndef __INST_GEN_HPP__
#define __INST_GEN_HPP__

/* "Species" - a CoreWars evolver.  Copyright (C) 2003 'Varfar' 
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
#include "ini.hpp"
#include <ostream>

class CChromosome; // forward

/* to implement a generator:
 * 1) derive publically from CInstGenerator
 * 1) give it an identifier in the EImpl enumeration
 * 2) make the constructor set the _type member
 * 3) make CInstGenerator factories understand this new implementation and how to identify it
 * 4) implement the read_ini/write_ini functionality
 */
 
class CInstGenerator { // an abstract class, use the factory to get an implementation
	public:
		enum MODE {
			FORWARD,
			INSERT,
			MODE_LAST
		};
		virtual ~CInstGenerator() {}
		// factory
		static CInstGenerator *read_ini(INIFile &ini); // create a new implementation
		static CInstGenerator *read_override_ini(CInstGenerator *gen,INIFile &ini); /* loads an implementation;
			if the ini specifies a new implementation: if the type is the same as the parent, it will load a diff;
			if the type is different, all values must be specified */
		// this sets the 'cursor' that suggestions will be based upon
		CChromosome *chromosome() const { return _chromosome; }
		void set_chromosome(CChromosome *chromosome) { _chromosome = chromosome; }
		void set_chromosome_index(unsigned index) { _index = index; }
		// actual query points
		virtual void suggest_instruction(insn_t &instruction,const MODE mode) = 0;
		// and saving
		virtual void write_ini(std::ostream &os) = 0;
		virtual void write_override_ini(std::ostream &os,const CInstGenerator *parent) = 0;
		// misc
		static bool same_type(const CInstGenerator *gen1,const CInstGenerator *gen2) { return (gen1->type() == gen2->type()); } 
		bool opcode_allowed(const OPCODE opcode) const { return _opcode_allowed[opcode]; }
	protected:
		CInstGenerator();
		enum IMPL { // all implementations must be listed here
			WEIGHTED_RANDOM,
			MARKOV,
			MARKOV_2,
			IMPL_LAST
		} _type;
		static const char *impl_desc(const IMPL type);
		static IMPL desc_to_impl(const char *desc);
		const IMPL type() const { return _type; }
		virtual void read_ini_impl(INIFile &ini) = 0;
		virtual CInstGenerator *read_override_ini_impl(INIFile &ini) = 0;
		unsigned index() const { return _index; }
		void generate_completely_random(insn_t &instruction) const; // utility
		// this stuff here is useful for any subclasses wishing to limit opcodes; recommended but not essential
		void read_opcode_allowed(INIFile &ini);
		void write_opcode_allowed(std::ostream &os,const CInstGenerator *parent = 0) const;
	private:
		bool _opcode_allowed[OPCODE_LAST];
		static const char *IMPL_DESC[IMPL_LAST];
		CChromosome *_chromosome;
		unsigned _index;
};

#endif // ifndef __INST_GEN_HPP__

