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
Modified at 25/10/2013, removed the redundant + in NoCheat<T>& operator<<=(const T& param){ *this = *this<<param; return *this; }
	It was NoCheat<T>& operator<<=(const T& param){ *this = *this<<+param; return *this; }
*/
/**
@example NoCheat
Example:
@code
//This class is pretty straight forward. Just declare NoCheat<DATATYPE> identifier; and use it as it is a DATATYPE.
#include "path/to/lemon.hpp"
//Or:
//#include "path/to/NoCheat/NoCheat.hpp"
class Player{ public: NoCheat<int> mp = 500; };
Player player;
while(true){
	handleInput();
	if(player.isMoving())
		player.mp--;
	else
		player.mp++;
	renderGraphics();
	delay(10);
}
@endcode
*/

#include <bitset>
#include <climits>
#include <stdint.h>
#include <random>
#include <stdexcept>
#include "external/MurmurHash3.h"

template<class T>
class NoCheat{
	T value;
	uint32_t hash;
	uint32_t seed;
	const static size_t T_BIT_SIZE = sizeof(T)*CHAR_BIT;
	const static size_t SEED_BIT_SIZE = sizeof(seed)*CHAR_BIT;
	typedef union{ unsigned long long ull; T t; }UllT;
	T encrypt(const T param){
		//randomize seed
		std::random_device rd;
		this->seed = rd();
		//initialize bitset
		UllT converter;
		converter.t = param;
		std::bitset<T_BIT_SIZE> oldValue(converter.ull), newValue(0), seedBit(this->seed);

		//encrypt and assign the value
		for(size_t i=0;i<T_BIT_SIZE;i++)
			newValue[i] = static_cast<bool>(seedBit[i%SEED_BIT_SIZE])^oldValue[((i*13)+this->seed%T_BIT_SIZE)%T_BIT_SIZE]; //Only 1 and 13 are the factor of 13. If the factor of sizeof(T) does not contain 1 or 13, (i*13)%sizeof(T) can be use to shuffle the bits.

		converter.ull = newValue.to_ullong();
		this->value = converter.t;

		//hash the value
		MurmurHash3_x86_32(&(this->value),sizeof(this->value), seed, &this->hash);
		return value;
	}
	T decryptValue() const{
		//initialize bitset
		uint32_t newHash;

		//check the hash
		MurmurHash3_x86_32(&(this->value),sizeof(this->value), seed, &newHash);
		if(newHash!=this->hash)
			throw std::runtime_error("Cheat detected.");

		//decrypt and retrun the value
		UllT converter;
		converter.t = this->value;
		std::bitset<T_BIT_SIZE> oldValue(converter.ull), newValue(0), seedBit(this->seed);
		for(size_t i=0;i<T_BIT_SIZE;i++)
			newValue[((i*13)+this->seed%T_BIT_SIZE)%T_BIT_SIZE] = static_cast<bool>(seedBit[i%SEED_BIT_SIZE])^oldValue[i];

		converter.ull = newValue.to_ullong();

		return converter.t;
	}
	public:
	constexpr NoCheat(const T param):value(encrypt(param)){} //conversion operator. constexpr for compile time conversion to avoid the value to be viewable from the binary file.
	constexpr NoCheat():value(encrypt(0)){}
	NoCheat(const NoCheat<T>& param):value(param.value),hash(param.hash),seed(param.seed){}
	NoCheat(NoCheat<T>&& param):value(std::move(param.value)),hash(param.hash),seed(param.seed){}
	NoCheat<T>& operator=(NoCheat<T> param){
		std::swap(this->value, param.value);
		std::swap(this->hash, param.hash);
		std::swap(this->seed, param.seed);
		return *this;
	}
	NoCheat<T> operator+(const T& param) const{ return NoCheat<T>( decryptValue()+param ); }
	NoCheat<T> operator-(const T& param) const{ return NoCheat<T>( decryptValue()-param ); }
	NoCheat<T> operator*(const T& param) const{ return NoCheat<T>( decryptValue()*param ); }
	NoCheat<T> operator/(const T& param) const{ return NoCheat<T>( decryptValue()/param ); }
	NoCheat<T> operator%(const T& param) const{ return NoCheat<T>( decryptValue()%param ); }
	NoCheat<T> operator&(const T& param) const{ return NoCheat<T>( decryptValue()&param ); }
	NoCheat<T> operator|(const T& param) const{ return NoCheat<T>( decryptValue()|param ); }
	NoCheat<T> operator^(const T& param) const{ return NoCheat<T>( decryptValue()^param ); }
	NoCheat<T> operator<<(const T& param) const{ return NoCheat<T>( decryptValue()<<param ); }
	NoCheat<T> operator>>(const T& param) const{ return NoCheat<T>( decryptValue()>>param ); }

	NoCheat<T>& operator++(){ T dummy = decryptValue(); this->value = encrypt(++dummy); return *this; } //prefix
	NoCheat<T>& operator--(){ T dummy = decryptValue(); this->value = encrypt(--dummy); return *this; }
	NoCheat<T> operator++(int){ T dummy = decryptValue(); this->value = encrypt(dummy+1); return NoCheat<T>(dummy); } //postfix
	NoCheat<T> operator--(int){ T dummy = decryptValue(); this->value = encrypt(dummy-1); return NoCheat<T>(dummy); }

	NoCheat<T> operator+() const{ return NoCheat<T>( *this ); }
	NoCheat<T> operator-() const{ return NoCheat<T>( encrypt(-decryptValue()) ); }
	NoCheat<T> operator~() const{ return NoCheat<T>( encrypt(~decryptValue()) ); }

	NoCheat<T>& operator+=(const T& param){ *this = *this+param; return *this; }
	NoCheat<T>& operator-=(const T& param){ *this = *this-param; return *this; }
	NoCheat<T>& operator*=(const T& param){ *this = *this*param; return *this; }
	NoCheat<T>& operator/=(const T& param){ *this = *this/param; return *this; }
	NoCheat<T>& operator%=(const T& param){ *this = *this%param; return *this; }
	NoCheat<T>& operator&=(const T& param){ *this = *this&param; return *this; }
	NoCheat<T>& operator|=(const T& param){ *this = *this|param; return *this; }
	NoCheat<T>& operator^=(const T& param){ *this = *this^param; return *this; }
	NoCheat<T>& operator<<=(const T& param){ *this = *this<<param; return *this; }
	NoCheat<T>& operator>>=(const T& param){ *this = *this>>param; return *this; }

	bool operator!(){ return !decryptValue(); }
	bool operator&&(const T& param){ return decryptValue()&&param; }
	bool operator||(const T& param){ return decryptValue()||param; }

	/*bool operator==(const T& param){ return decryptValue()==param; }
	bool operator!=(const T& param){ return decryptValue()!=param; }
	bool operator<(const T& param){ return decryptValue()<param; }
	bool operator>(const T& param){ return decryptValue()>param; }
	bool operator<=(const T& param){ return decryptValue()<=param; }
	bool operator>=(const T& param){ return decryptValue()>=param; }*/
	//^ Note: the operator T() below can convert to a type that make the built-in comparison operators work

	operator T() const{
		return decryptValue();
	}
};
