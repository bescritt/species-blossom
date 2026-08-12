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

#include "blossom_utils.hpp"

//************* BlossomException *************\\

BlossomException::BlossomException(Type type,const char* file,unsigned line):
	_type(type), _file(file), _line(line) {
};

void BlossomException::to_stream(std::ostream& out) const {
	static const char* Mnemonic[TYPE_LAST] = { "ASSERT_FAILED","INVARIANT" }; 
	out << "BlossomException: ";
	if(TYPE_LAST > _type)
		out << Mnemonic[_type];
	else
		out << "Unknown! #" << static_cast<unsigned>(_type);
	out << " in " << _file << ':' << _line << std::endl;
}


std::ostream& operator<<(std::ostream& out,const BlossomException& be) {
	be.to_stream(out);
	return out;
}

