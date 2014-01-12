/*
NoCheat, a cheat-proof datatype.
Copyright (c) 2013, Wong Cho Ching
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
*/
/* written in 10/03/2013 and 11/03/2013
Modified to adapt into lemon tookit in 12/01/2014
*/

#include <iostream>
#include "NoCheat.hpp"
#include "../shorthandMacros.hpp"

#include <assert.h>
template <class T>
void test(){
	NoCheat<T> alpha = 30;
	NoCheat<T> beta = 500;
	//postfix and prefix tests
	assert(alpha++==30);
	assert(alpha==31);
	assert(++alpha==32);

	assert(--alpha==31);
	assert(alpha--==31);
	assert(alpha==30);

	//comparison tests
	alpha += 470;
	assert(alpha==beta);
	assert(alpha<=beta);
	assert(alpha>=beta);
	assert(alpha!=++beta);
	assert(beta>alpha);
	assert(beta>=alpha);
	assert(alpha<beta);
	assert(alpha<=beta);

	//assignment test
	beta = 500;
	alpha = beta;
	assert(alpha==500);
	alpha += beta;
	assert(alpha==1000);
	alpha -= 50;
	assert(alpha==950);
	alpha *= 2;
	assert(alpha==1900);
	alpha /= 2;
	assert(alpha==950);
	alpha %= 3;
	assert(alpha==2);
	alpha = 0x82;
	alpha &= 0xF0;
	assert(alpha==0x80);
	alpha = 0x82;
	alpha |= 0x01;
	assert(alpha==0x83);
	alpha = 0x82;
	alpha ^= 0x0F;
	assert(alpha==0x8D);
	alpha = 0x42;
	alpha <<= 1;
	assert(alpha==0x84);
	alpha = 0xF6;
	alpha >>= 2;
	assert(alpha==0x3D);

	//arithmetic test
	alpha = 30;
	beta = 500;
	assert(alpha==30);
	assert(alpha+beta==530);
	assert(alpha-beta==-470);
	assert(alpha*beta==15000);
	assert(beta/alpha==16);
	assert(beta%alpha==20);
	alpha = 0x82;
	assert((alpha&0xF0)==0x80);
	assert((alpha|0x01)==0x83);
	assert((alpha^0x0F)==0x8D);
	alpha = 0x42;
	assert((alpha<<1)==0x84);
	alpha = 0xF6;
	assert((alpha>>2)==0x3D);
}

TESTCASES(NoCheat)
	test<int>();
TESTCASES_END(NoCheat)
