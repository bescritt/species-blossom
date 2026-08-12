#ifndef BLOSSOM_UTILS_HPP
#define BLOSSOM_UTILS_HPP

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

#include <iostream>

class BlossomException {
public:
	enum Type {
		ASSERT_FAILED,
		INVARIANT,
		TYPE_LAST,
	};
	BlossomException(Type type,const char* file,unsigned line);
	void to_stream(std::ostream& out) const;
private:
	Type _type;
	const char* _file;
	unsigned _line;
};

std::ostream& operator<<(std::ostream& out,const BlossomException& be);

template<typename T> T binary_read(std::istream& in) {
	T ret;
	in.read((char*)&ret,sizeof(T));
	return ret;
}

template<typename T> void binary_write(std::ostream& out,const T& t) {
	out.write((char*)&t,sizeof(T));
}

#define BLOSSOM_INVARIANT_ALWAYS() throw new BlossomException(BlossomException::INVARIANT,__FILE__,__LINE__);
#define BLOSSOM_ASSERT_ALWAYS(condition) if(!(condition)) throw new BlossomException(BlossomException::ASSERT_FAILED,__FILE__,__LINE__);


#if BLOSSOM_DEBUG > 0

#define BLOSSOM_ASSERT(condition) BLOSSOM_ASSERT_ALWAYS(condition)
#define BLOSSOM_INVARIANT() BLOSSOM_INVARIANT_ALWAYS()

#else

#define BLOSSOM_ASSERT(condition)
#define BLOSSOM_INVARIANT()

#endif

#endif //BLOSSOM_UTILS_HPP

