/*
NoOverflow, a datatype that throws exceptions when there is a data overflow or data underflow.
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
#include <exception>
#include <limits>
#include <limits.h>
#include "NoOverflow.hpp"
#include "../shorthandMacros.hpp"

#include <assert.h>
template <class T>
void test(){
	//postfix and prefix tests
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::max(); alpha++; assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::max(); ++alpha; assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::min(); alpha--; assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::min(); --alpha; assert(false); }catch(std::exception& e){}

	//assignment test
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::max(); alpha+=1; assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::min(); alpha-=1; assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::max()/2; alpha*=3; assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::max()/2; alpha*=static_cast<T>(-3); assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = -(std::numeric_limits<T>::max()/2); alpha*=static_cast<T>(-3); assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = 50; alpha/=0; assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = 50; alpha%=0; assert(false); }catch(std::exception& e){}

	//arithmetic test
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::max(); alpha=alpha+static_cast<T>(1); assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::min(); alpha=alpha-static_cast<T>(1); assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::max()/2; alpha=alpha*static_cast<T>(3); assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = std::numeric_limits<T>::max()/2; alpha=alpha*static_cast<T>(-3); assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = -(std::numeric_limits<T>::max()/2); alpha=alpha*static_cast<T>(-3); assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = 50; alpha=alpha/static_cast<T>(0); assert(false); }catch(std::exception& e){}
	try{ NoOverflow<T> alpha = 50; alpha=alpha%static_cast<T>(0); assert(false); }catch(std::exception& e){}
	try{
		NoOverflow<uint32_t> result = 1;
		for(int i=0;i<99999;i++)
			result *= 2; //this code should throw an exception because the result will be >= pow(2, 32)
		assert(false);
	}catch(std::exception& e){}

}

/*template<class T, class = typename std::enable_if<std::is_unsigned<T>::value>::type>
void func(){ std::cout<<"is_unsigned = true"<<std::endl; }
template<class T, class = typename std::enable_if<std::is_signed<T>::value>::type>
void func(){ std::cout<<"is_signed = true"<<std::endl; }*/

TESTCASES(NoOverflow)
	test<char>();			test<unsigned char>();
	test<short>();			test<unsigned short>();
	test<int>();			test<unsigned int>();
	test<long int>();		test<unsigned long int>();
	test<long long int>();	test<unsigned long long int>();
TESTCASES_END(NoOverflow)

