#ifndef __CRand_HPP__
#define __CRand_HPP__

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
 
/* the expectation is that, because we are using our own random number generator,
 * we can start to express warriors as prng vectors relative to a known warrior
 * when creating warriors on distant nodes in a distributed environment, rather
 * than needing to send the whole warrior each time.  Neat! */

class CRand {
	public:
		/* random generators return a (0 >= value < max) */
		static float frand(const float max);
		static int irand(const int max);
	private:
};

#endif // __CRand_HPP__
