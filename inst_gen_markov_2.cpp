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
 
#include "inst_gen_markov_2.hpp"
#include "error.hpp"
#include "rand.hpp"
#include "species.hpp"

#include "opcode_branch_lookup.hpp"

#include <fstream>
using namespace std;

/************* CMutableMarkov implementation ********************************/

CInstGeneratorMarkov2::CMutableMarkov::CMutableMarkov(): _chain() {
	// zero everything
	_in = 0;
	_count = 0;
}

CInstGeneratorMarkov2::CMutableMarkov::CMutableMarkov(const u16_t in): _chain() {
	_in = in;
	// zero everything else
	_count = 0;
}
 
CInstGeneratorMarkov2::CMutableMarkov::~CMutableMarkov() {
	for(TChainIter iter = _chain.begin(); iter != _chain.end(); ++iter) // cascade
		delete *iter;
	_chain.clear();
}

void CInstGeneratorMarkov2::CMutableMarkov::dump(ostream &out) {
	string path = "";
	for(TChainIter iter = _chain.begin(); iter != _chain.end(); ++iter)
		(*iter)->dump(out,path,1); // recurse
}

void CInstGeneratorMarkov2::CMutableMarkov::dump(ostream &out,string &path,const int len) {
	const int plen = path.length();
	// append this node to path
	path += MNEMONIC_OPCODE((OPCODE)((_in >> opPOS) & opMASK));
	path += '.';
	path += MNEMONIC_MODIFIER((MODIFIER)((_in >> moPOS) & moMASK));
	path += MNEMONIC_ADDRMODE((ADDRMODE)((_in >> maPOS) & maMASK));
	path += MNEMONIC_ADDRMODE((ADDRMODE)((_in >> mbPOS) & mbMASK));
	// is this a leaf node too?
	unsigned leaf_count = count_leaves();
	if(0 < leaf_count)
		out << path << '\t' << len << '\t' << leaf_count << '\t' << _count << endl;
	// and list children
	path += ',';
	for(TChainIter iter = _chain.begin(); iter != _chain.end(); ++iter) {
		(*iter)->dump(out,path,len+1); // recurse
	}
	path.resize(plen); // trim whole tail
}

unsigned CInstGeneratorMarkov2::CMutableMarkov::count_leaves() {
	unsigned ret = _count;
	for(TChainIter iter = _chain.begin(); iter != _chain.end(); ++iter)
		ret -= (*iter)->count();
	return ret;
}

CInstGeneratorMarkov2::CMutableMarkov *CInstGeneratorMarkov2::CMutableMarkov::get(const u16_t in) {
	// search existing allocation
	for(TChainIter iter = _chain.begin(); iter != _chain.end(); ++iter)
		if(in == (*iter)->_in)
			return *iter;
	// not found? create it
	_chain.push_back(new CMutableMarkov(in));
	return _chain.back();
}

void CInstGeneratorMarkov2::CMutableMarkov::cull(const unsigned threshold) {
	// check children; note my selective iteration because an unchecked erase() seems to nuke the iterator
	// ..it has been difficult to find code that works on both codewarrior and gcc..
	for(TChainIter iter = _chain.begin(); iter != _chain.end();) {
		if(threshold < (*iter)->count()) { // ok?
			(*iter)->cull(threshold); // cascade
			++iter;
		} else { // uh no
			delete *iter;
			*iter = 0;
			cout << "x" << flush;
			iter = _chain.erase(iter);
		}
	}
}

void CInstGeneratorMarkov2::CMutableMarkov::cull_leaves() {
	// check children; note my selective iteration because an unchecked erase() seems to nuke the iterator
	// ..it has been difficult to find code that works on both codewarrior and gcc..
	for(TChainIter iter = _chain.begin(); iter != _chain.end();) {
		if(!(*iter)->is_leaf()) { // ok?
			++iter;
		} else { // uh no
			delete *iter;
			*iter = 0;
			cout << "z" << flush;
			iter = _chain.erase(iter);
		}
	}
}

void CInstGeneratorMarkov2::CMutableMarkov::prepare() {
	_chain.sort(comp);
	for(TChainIter iter = _chain.begin(); iter != _chain.end(); iter++) {
		(*iter)->prepare(); // recurse
	}
}

void CInstGeneratorMarkov2::CMutableMarkov::save(ostream &out) {
	bwrite_u16_t(out,_in);
	bwrite_unsigned(out,_count);
	bwrite_u16_t(out,_chain.size());
	for(TChainIter iter = _chain.begin(); iter != _chain.end(); iter++) {
		(*iter)->save(out); // recurse
	}
}

/************* CImmutableMarkov implementation ****************************/

CInstGeneratorMarkov2::CImmutableMarkov::CImmutableMarkov(istream &in,const CInstGenerator *opcode_allowed) {
	_in = bread_u16_t(in);
	_count = bread_unsigned(in);
	if(!opcode_allowed->opcode_allowed((OPCODE)((_in >> opPOS) & opMASK))) { // aha? cull
		_count = 0;
	}
	_size = bread_u16_t(in);
	_chain = new CImmutableMarkov*[_size];
	for(u16_t i=0; i<_size; i++) {
		_chain[i] = new CImmutableMarkov(in,opcode_allowed);
	}
}

void CInstGeneratorMarkov2::CImmutableMarkov::dump(ostream &out) const {
	string path = "";
	for(u16_t i=0; i<_size; i++) {
		_chain[i]->dump(out,path,1); // recurse
	}
}

void CInstGeneratorMarkov2::CImmutableMarkov::dump(ostream &out,string &path,const int len) const {
	const int plen = path.length();
	// append this node to path
	path += MNEMONIC_OPCODE((OPCODE)((_in >> opPOS) & opMASK));
	path += '.';
	path += MNEMONIC_MODIFIER((MODIFIER)((_in >> moPOS) & moMASK));
	path += MNEMONIC_ADDRMODE((ADDRMODE)((_in >> maPOS) & maMASK));
	path += MNEMONIC_ADDRMODE((ADDRMODE)((_in >> mbPOS) & mbMASK));
	// is this a leaf node too?
	unsigned leaf_count = count_leaves();
	if(0 < leaf_count)
		out << path << '\t' << len << '\t' << leaf_count << '\t' << _count << endl;
	// and list children
	path += ',';
	for(u16_t i=0; i<_size; i++) {
		_chain[i]->dump(out,path,len+1); // recurse
	}
	path.resize(plen); // trim whole tail
}

unsigned CInstGeneratorMarkov2::CImmutableMarkov::count_leaves() const {
	unsigned ret = _count;
	for(u16_t i=0; i<_size; i++) {
		ret -= _chain[i]->count();
	}
	return ret;
}

CInstGeneratorMarkov2::CImmutableMarkov *CInstGeneratorMarkov2::CImmutableMarkov::suggest() const {
	const unsigned TARGET = CRand::irand(_count);
	unsigned y = 0;
	for(u16_t i=0; i<_size; i++) {
		y += _chain[i]->count();
		if(y > TARGET)
			return _chain[i];
	}
	return 0;
}

CInstGeneratorMarkov2::CImmutableMarkov *CInstGeneratorMarkov2::CImmutableMarkov::get(const u16_t in) const { // null if not found
	//TODO: we know this is sorted, so use a binary chop!
	for(u16_t i=0; i<_size; i++) {
		if(in == _chain[i]->in()) {
			return _chain[i];
		}
	}
	return 0;
}

void CInstGeneratorMarkov2::CImmutableMarkov::suggest(CInstGeneratorMarkov2::CImmutableMarkov::TMarkovList &ret,unsigned &sumprob,const u16_t before,const u16_t after,const bool is_after) const {
	const bool is_before = (before == in()); // to pass to children
	for(u16_t i=0; i<_size; i++) { // what are my children?
		if(after == _chain[i]->in()) { // I am a candidate?
			ret.push_back(this);
			sumprob += count();
		}
		// and cascade
		_chain[i]->suggest(ret,sumprob,before,after,is_after);
	}
}


/*************** CMutableStart implementation *****************************/

void CInstGeneratorMarkov2::CMutableStart::learn(const u16_t in) {
	for(TStartIter i=_data.begin(); i!=_data.end(); ++i) {
		if(in == i->_in) { // found?
			i->_count++;
			return;
		}
	}
	// not found?
	_data.push_back(TEntry(in));
}

void CInstGeneratorMarkov2::CMutableStart::prepare() {
	_data.sort(TEntry::comp);
}

void CInstGeneratorMarkov2::CMutableStart::save(ostream &out) {
	bwrite_u16_t(out,_data.size());
	for(TStartIter i=_data.begin(); i!=_data.end(); ++i) {
		bwrite_u16_t(out,i->_in);
		bwrite_unsigned(out,i->_count);
	}
}

void CInstGeneratorMarkov2::CMutableStart::dump(ostream &out) {
	for(TStartIter i=_data.begin(); i!=_data.end(); ++i) {
		CInstGeneratorMarkov2::dump(out,i->_in);
		out << '\t' << i->_count << endl;
	}
}

/*************** CImmutableStart implementation ***************************/

CInstGeneratorMarkov2::CImmutableStart::CImmutableStart(istream &in,const CInstGenerator *opcode_allowed) {
	_size = bread_u16_t(in);
	_data = new TEntry[_size];
	_count = 0;
	for(u16_t i=0; i<_size; i++) {
		_data[i]._in = bread_u16_t(in);
		_data[i]._count = bread_unsigned(in);
		if(!opcode_allowed->opcode_allowed((OPCODE)((_data[i]._in >> opPOS) & opMASK))) { // aha? cull
			_data[i]._count = 0;
		}
		_count += _data[i]._count;
	}
}

void CInstGeneratorMarkov2::CImmutableStart::dump(ostream &out) {
	for(u16_t i=0; i<_size; i++) {
		CInstGeneratorMarkov2::dump(out,_data[i]._in);
		out << '\t' << _data[i]._count << endl;
	}
}

u16_t CInstGeneratorMarkov2::CImmutableStart::suggest() const {
	const unsigned TARGET = CRand::irand(_count);
	unsigned y = 0;
	for(u16_t i=0; i<_size; i++) {
		y += _data[i]._count;
		if(TARGET < y) {
			return _data[i]._in;
		}
	}
	PANIC(MISC,"Roulette off end of CInstGeneratorMarkov2::CImmutableStart::suggest()",NULL);
}

/*************** CInstGeneratorMarkov2 implementation *********************/

CInstGeneratorMarkov2::CInstGeneratorMarkov2() { // creates a special empty one to be read from ini
	_type = MARKOV_2;
}

CInstGeneratorMarkov2::CInstGeneratorMarkov2(const unsigned coresize):
	CInstGenerator() { // mutable
	_type = MARKOV_2;
	_mutable = true;
	_coresize = coresize;
	_mutable_start = new CMutableStart();
	_immutable_start = 0;
	_mutable_root = new CMutableMarkov();
	_immutable_root = 0;
}

CInstGeneratorMarkov2::CInstGeneratorMarkov2(const char *lookup_filename):
	CInstGenerator()  { // immutable
	_type = MARKOV_2;
	load(lookup_filename);
}

void CInstGeneratorMarkov2::load(const char *lookup_filename) {
	unsigned file_version;
	_mutable = false;
	_filename = lookup_filename;
	ifstream in(lookup_filename,ifstream::binary|ifstream::in);
	if(!in.good())
		PANIC(MISC,"error openning file",lookup_filename);
	file_version = bread_unsigned(in);
	if(FILE_VERSION != file_version)
		PANIC(MISC,"bad file version in lookup",lookup_filename);
	_coresize = bread_unsigned(in);
	_mutable_start = 0;
	_immutable_start = new CImmutableStart(in,this);
	_mutable_root = 0;
	_immutable_root = new CImmutableMarkov(in,this);
	if(!in.good())
		PANIC(MISC,"error reading from file",lookup_filename);
	in.close();
}

void CInstGeneratorMarkov2::learn(const string rc_filename) {
	if(!_mutable)
		PANIC(MISC,"cannot teach an immutable markov",NULL);
	warrior_t warrior;
	// load it
	cout << "loading " << rc_filename << ".." << endl;
	asm_fname(rc_filename.c_str(),&warrior,_coresize); // will panic fatally if can't
	// and analyse
	_mutable_start->learn(warrior.code[warrior.start].in);
	for(int i=0; i<warrior.len; i++) {
		CMutableMarkov *markov = _mutable_root->get(warrior.code[i].in);
		markov->inc();
		for(int j=i+1; j<warrior.len; j++) {
			markov = markov->get(warrior.code[j].in);
			markov->inc();
		}
	}
}

void CInstGeneratorMarkov2::prepare() { // mostly sorts
	if(!_mutable)
		PANIC(MISC,"cannot prepare an immutable markov",NULL);
	_mutable_start->prepare();
	_mutable_root->prepare();
}

void CInstGeneratorMarkov2::save(const string lookup_filename) {
	if(!_mutable)
		PANIC(MISC,"cannot save an immutable markov",NULL);
	ofstream out(lookup_filename.c_str(),ofstream::binary|ofstream::trunc);
	bwrite_unsigned(out,FILE_VERSION);
	bwrite_unsigned(out,_coresize);
	_mutable_start->save(out);
	_mutable_root->save(out);
	if(!out.good())
		PANIC(MISC,"error writing to ",lookup_filename.c_str());
	out.close();
}

void CInstGeneratorMarkov2::cull(const unsigned threshold) {
	_threshold = threshold;
	_mutable_root->cull(threshold);
	_mutable_root->cull_leaves();
	cout << endl;
}

void CInstGeneratorMarkov2::dump(ostream &out) {
	if(_mutable) {
		_mutable_start->dump(out);
		_mutable_root->dump(out);
	} else {
		_immutable_start->dump(out);
		_immutable_root->dump(out);
	}
}

// and saving
void CInstGeneratorMarkov2::write_ini(ostream &os) {
	os << "inst_gen=" << impl_desc(type()) << endl;
	os << "markov_file=" << _filename << endl;
	write_opcode_allowed(os);
}

void CInstGeneratorMarkov2::write_override_ini(ostream &os,const CInstGenerator *parent) {
	if(parent == this) // nothing to do?
		return;
	os << "inst_gen=" << impl_desc(type()) << endl;
	if(same_type(this,parent)) {
		if(((CInstGeneratorMarkov2*)parent)->_filename != _filename) { 
			os << "markov_file=" << _filename << endl;
		}
	}
	write_opcode_allowed(os,parent);
}

void CInstGeneratorMarkov2::read_ini_impl(INIFile &ini) {
	KeyValuePair *kvp;
	kvp = ini.get("markov_file"); if(0 == kvp) PANIC(MISC,"markov_file key expected",NULL); // null?!
	kvp->getValueAsString(_filename);
	load(_filename.c_str());
	read_opcode_allowed(ini);
}

CInstGenerator *CInstGeneratorMarkov2::read_override_ini_impl(INIFile &ini) {
	string filename;
	CInstGeneratorMarkov2 *ret = this;
	KeyValuePair *kvp;
	kvp = ini.get("markov_file");
	if(0 != kvp) { // specified?
		kvp->getValueAsString(filename);
		if(_filename != filename) {
			ret = new CInstGeneratorMarkov2();
			ret->read_ini_impl(ini);
		}
	} 
	return ret;
}

CInstGeneratorMarkov2::CImmutableMarkov *CInstGeneratorMarkov2::suggest(const u16_t before,const u16_t after) const { // suggest an opcode that is between before and after
	CImmutableMarkov::TMarkovList list;
	unsigned count = 0;
	// gather potential markovs
	_immutable_root->suggest(list,count,before,after);
	// tell user about it?
	if(verbose) {
		if(count > 0) {
			cout << "Between "; dump(cout,before); cout << " and "; dump(cout,after); 
			cout << " are " << list.size() << " markovs (totalling " << count << "):" << endl;
			for(CImmutableMarkov::TMarkovList::iterator i = list.begin(); i != list.end(); ++i) {
				cout << '\t'; dump(cout,(*i)->in()); cout << " = " << (*i)->count() << endl;
			}
		} else {
			cout << "There are no markovs between "; dump(cout,before); cout << " and "; dump(cout,after); cout << endl;
		}
	}
	// nothing to do?  This is common case
	if(0 == count)	return 0;
	// choose one
	unsigned SUM = 0; //TODO: couldn't _immutable_root->suggest() gather this stat??
	for(CImmutableMarkov::TMarkovList::iterator i = list.begin(); i != list.end(); ++i) {
		SUM += (*i)->count();
	}
	const unsigned TARGET = CRand::irand(SUM);
	unsigned y = 0;
	for(CImmutableMarkov::TMarkovList::iterator i = list.begin(); i != list.end(); ++i) {
		y += (*i)->count();
		if(TARGET < y) {
			list.clear(); // hmm, this is implicit anyway..
			return const_cast<CImmutableMarkov*>(*i);
		}
	}
	PANIC(MISC,"roulette off end of Insert Suggest",NULL);
}

void CInstGeneratorMarkov2::suggest_instruction(insn_t &instruction,const MODE mode) {
	// special case?
	if(index() == chromosome()->start()) {
		instruction.in = _immutable_start->suggest();
	} else {
		CImmutableMarkov *markov = 0;
		if(INSERT == mode) {
			markov = suggest(chromosome()->code(index()-1).in,chromosome()->code(index()+1).in);
		} else {
			// find the chain
			for(unsigned start=0; (start<index()) && (0 == markov); start++) {
				markov = _immutable_root->get(chromosome()->code(start).in); // try to follow it
				for(unsigned i=start+1; (i<index()) && (0 != markov); i++) { // follow it
					markov = markov->get(chromosome()->code(i).in);
				}
			}
			// found something?
			if(0 != markov) {
				markov = markov->suggest();
				if(0 == markov) { // hmm, chain ended?
					markov = _immutable_root->suggest();
				}
			}
		}
		// assign new opcode..
		if(0 != markov) {
			instruction.in = markov->in();
		} else {
			generate_completely_random(instruction);
		}
	}
	// and assign new operands
	chromosome()->type()->species()->operands()->rnd(instruction,COperand::A);
	chromosome()->type()->species()->operands()->rnd(instruction,COperand::B);
	if(safety_checks)
		if(!instruction.valid(_coresize))
			PANIC(MISC,"invalid instruction generated by Markov2",NULL);
}

void CInstGeneratorMarkov2::dump(ostream &out,const u16_t in) { // static
	out << MNEMONIC_OPCODE((OPCODE)((in >> opPOS) & opMASK)) << '.' <<
		MNEMONIC_MODIFIER((MODIFIER)((in >> moPOS) & moMASK)) <<
		MNEMONIC_ADDRMODE((ADDRMODE)((in >> maPOS) & maMASK)) <<
		MNEMONIC_ADDRMODE((ADDRMODE)((in >> mbPOS) & mbMASK));
}

void CInstGeneratorMarkov2::bwrite_u16_t(std::ostream &out,const u16_t i) {
	out.write(reinterpret_cast<const char*>(&i),sizeof(i));
}

void CInstGeneratorMarkov2::bwrite_unsigned(std::ostream &out,const unsigned i) {
	out.write(reinterpret_cast<const char*>(&i),sizeof(i));
}

u16_t CInstGeneratorMarkov2::bread_u16_t(std::istream &in) {
	u16_t i;
	in.read(reinterpret_cast<char*>(&i),sizeof(i));
	return i;
}

unsigned CInstGeneratorMarkov2::bread_unsigned(std::istream &in) {
	unsigned i;
	in.read(reinterpret_cast<char*>(&i),sizeof(i));
	return i;
}
