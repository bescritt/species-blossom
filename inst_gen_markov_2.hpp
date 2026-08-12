#ifndef __INST_GEN_MARKOV_2_HPP_
#define __INST_GEN_MARKOV_2_HPP_

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

#include "inst_gen.hpp"
#include "exhaust.hpp"
#include "warrior.hpp"

#include <iostream>
#include <list>

/**
 * the datastructures for efficiently collecting and collating markovs is so
 * different compared to those for recalling them, that I have implemented two
 * different classes for the two purposes: CMutableMarkov and CImmutableMarkov
 **/

class CInstGeneratorMarkov2: public CInstGenerator {
	public:
		// construction/destruction
		CInstGeneratorMarkov2(); // creates a special empty one to be read from ini
		CInstGeneratorMarkov2(const unsigned coresize); // creates a mutable markov
		CInstGeneratorMarkov2(const char *lookup_filename); // creates an immutable markov
		void load(const char *lookup_filename);
		// actual query points
		virtual void suggest_instruction(insn_t &instruction,const MODE mode);
		// learning
		void learn(const std::string rc_filename);
		void cull(const unsigned threshold);
		void prepare(); // mostly sorts
		void save(const std::string lookup_filename);
		// and saving
		virtual void write_ini(std::ostream &os);
		virtual void write_override_ini(std::ostream &os,const CInstGenerator *parent);
		// trivia
		void dump(std::ostream &out); // list *all* markovs
	protected:
		virtual void read_ini_impl(INIFile &ini);
		virtual CInstGenerator *read_override_ini_impl(INIFile &ini);
	private:
		static const unsigned FILE_VERSION = 0x10F4; 
		bool _mutable;
		unsigned _coresize, _threshold;
		std::string _filename;
		// shared bases
		class CStartBase {
			public:
				virtual void dump(std::ostream &out) = 0;
			protected:
				struct TEntry {
					// construction
					TEntry() {}
					TEntry(const u16_t in): _in(in), _count(1) {}
					// for sorting
					static bool comp(const TEntry &a,const TEntry &b) { return a._in < b._in; }
					// fields, public
					u16_t _in;
					unsigned _count;
				};
		};
		// mutable declarations
		class CMutableStart: public CStartBase {
			public:
				void learn(const u16_t in);
				void prepare();
				void save(std::ostream &out);
				virtual void dump(std::ostream &out);
			private:
				typedef std::list<TEntry> TStart;
				typedef TStart::iterator TStartIter;
				TStart _data;
		} *_mutable_start;
		friend class CMutableStart;
		class CMutableMarkov {
			public:
				// construction/destruction
				CMutableMarkov(); // default is only appropriate for root
				CMutableMarkov(const u16_t in);
				~CMutableMarkov();
				// getters
				u16_t in() const { return _count; } // the instruction, packing opcode,modifier,addressing modes
				unsigned count() const { return _count; }
				CMutableMarkov *get(const u16_t in); // this can be a setter too
				// setters
				void inc() { _count++; }
				void cull(const unsigned threshold);
				void cull_leaves();
				void save(std::ostream &out);
				// for sorting
				void prepare();
				bool operator<(const CMutableMarkov &other) const { return in() < other.in(); }
				// trivia
				void dump(std::ostream &out);
			private:
				typedef std::list<CMutableMarkov*> TChain;
				typedef TChain::iterator TChainIter;
				u16_t _in;
				unsigned _count;
				TChain _chain;
				bool is_leaf() const { return (0 == _chain.size()); }
				unsigned count_leaves();
				void dump(std::ostream &out,std::string &path,const int len = 0);
				static bool comp(const CMutableMarkov *a,const CMutableMarkov *b) { return *a < *b; }
		} *_mutable_root;
		friend class CMutableMarkov;
		// immutable declarations
		class CImmutableStart: public CStartBase {
			public:
				CImmutableStart(std::istream &in,const CInstGenerator *opcode_allowed);
				virtual void dump(std::ostream &out);
				u16_t suggest() const;
			private:
				unsigned _count;
				u16_t _size;
				TEntry *_data;
		} *_immutable_start;
		friend class CImmutableStart;
		class CImmutableMarkov {
			public:
				typedef std::list<const CImmutableMarkov*> TMarkovList;
				// construction/destruction
				CImmutableMarkov(); // default is only appropriate for root
				CImmutableMarkov(std::istream &in,const CInstGenerator *opcode_allowed);
				~CImmutableMarkov();
				// getters
				u16_t in() const { return _in; } // the instruction, packing opcode,modifier,addressing modes
				unsigned count() const { return _count; }
				CImmutableMarkov *get(const u16_t in) const; // null if not found
				CImmutableMarkov *suggest() const; // null if chain is to end
				// suggestion
				void suggest(TMarkovList &ret,unsigned &sumprob,const u16_t before,const u16_t after,const bool is_after = false) const; // populate the list with all markovs that are between these two operands; maintain count
				// trivia
				void dump(std::ostream &out) const;
			private:
				u16_t _in,
					_size; // well, how many can there be? ;-)
				unsigned _count;
				CImmutableMarkov **_chain;
				unsigned count_leaves() const;
				void dump(std::ostream &out,std::string &path,const int len = 0) const;
		} *_immutable_root;
		friend class CImmutableMarkov;
		CImmutableMarkov *suggest(const u16_t before,const u16_t after) const; // suggest an opcode that is between before and after
		// utilities
		static void dump(std::ostream &out,const u16_t in);
		static void bwrite_u16_t(std::ostream &out,const u16_t i);
		static void bwrite_unsigned(std::ostream &out,const unsigned i);
		static u16_t bread_u16_t(std::istream &in);
		static unsigned bread_unsigned(std::istream &in);
};

#endif // ifndef __INST_GEN_MARKOV_2_HPP__

