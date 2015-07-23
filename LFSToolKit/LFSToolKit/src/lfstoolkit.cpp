/*
 *
 * ©K. D. Hedger. Thu 23 Jul 14:34:11 BST 2015 kdhedger68713@gmail.com

 * This file (lfstoolkit.cpp) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define VISIBLE __attribute__((visibility("default")))
#define PROTECTED __attribute__((visibility("protected")))
#define HIDDEN __attribute__((visibility("hidden")))

int	testint=100;

void AAAouttest(char* name)
{
	printf("Hello World NOT to %s!\n",name);
}


void LFSTK_outHW(char* name)
{
	printf("Hello World to %s\n",name);
}

class LFSTK_testclass
{
	public:
		LFSTK_testclass();
		~LFSTK_testclass();
		int y;
};

LFSTK_testclass::~LFSTK_testclass()
{
}

LFSTK_testclass::LFSTK_testclass()
{
	y=12345;
}
#endif
