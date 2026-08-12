#ifndef __INSN_MARKOV_HPP_
#define __INSN_MARKOV_HPP_

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

class CWorld;

#include <iostream>
#include <string>
//#include <list>

class CInsnGeneratorMarkov {
	public:
		// construction/destruction
		CInsnGeneratorMarkov(CWorld& world);
		~CInsnGeneratorMarkov();
		void load(const std::string& filename);
		void load(std::istream& in);
		void dump(std::ostream& out) const;
		// suggesting an opcode
		u16_t suggest();
		u16_t suggest_after(u16_t before);
		u16_t suggest_between(u16_t before,u16_t after);
	private:
		static const unsigned FILE_VERSION = 0x10F4;
		CWorld& _world;
		class CMarkov {
		public:
			// construction/destruction
			CMarkov(const CMarkov* parent);
			~CMarkov();
			void load(std::istream& in);
			void load_start(std::istream& in);
			void dump(std::ostream& out,unsigned indent) const;
			void dump(std::ostream& out) const;
			u16_t in() const;
			// suggesting an opcode
			const CMarkov* suggest() const; 
			const CMarkov* markov(u16_t in) const;
		private:
			const CMarkov* _parent;
			u16_t _in;
			u32_t _size, _count;
			CMarkov** _chain;
		};
		CMarkov _head, _start;
};

#endif //__INSN_MARKOV_HPP_

