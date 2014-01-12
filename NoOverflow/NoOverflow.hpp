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
/* written in 11/03/2013 */
/**
@example NoOverflow
Example:
@code
//This class is pretty straight forward. Just declare NoOverflow<DATATYPE> identifier; and use it as it is a DATATYPE.
#include <cstdint>
#include "path/to/lemon.hpp"
//Or:
//#include "path/to/NoOverflow/NoOverflow.hpp"
NoOverflow<uint32_t> result = 1;
for(int i=0;i<99999;i++)
	result *= 2; //this line should throw an exception because the result will be >= pow(2, 32)
@endcode
*/

#include <functional>
#include <exception>
#include <stdexcept>
#include <limits>
#include <type_traits>


template<class T>
class NoOverflow{
	T value;
	struct InvalidType{}; //uses for std::conditional<> to determine function overload for signed and unsigned data type XXX:dirty trick?
	/*constexpr bool mostSigBit(T param){ //get the value of the most significant bit
		#ifdef __LITTLE_ENDIAN__
			return param&(0x1<<(sizeof(T)*CHAR_BIT));
		#ifdef __BIG_ENDIAN__
			return param&0x10;
	}*/
	public:
	constexpr NoOverflow(const T param):value(param){} //conversion operator.
	constexpr NoOverflow():value(0){}
	NoOverflow(const NoOverflow<T>& param):value(param.value){}
	NoOverflow(NoOverflow<T>&& param):value(std::move(param.value)){}
	NoOverflow<T>& operator=(NoOverflow<T> param){
		std::swap(this->value, param.value);
		return *this;
	}
	/*functions for signed types*/
	//template< class U = typename std::enable_if<std::is_signed<T>::value> >
	NoOverflow<T> operator+(const typename std::conditional<std::is_signed<T>::value, T, InvalidType>::type& param) const{
		if((this->value>0&&param>0) //Both this->value and param are positive
			&& this->value>std::numeric_limits<T>::max()-param ) // Derived from this->value+param > std::numeric_limits<T>::max()
				throw std::overflow_error("Overflow error detected on addition operation");
		else if( std::is_signed<T>::value &&
			(this->value<0&&param<0) && //Both this->value and param are negative
			this->value<std::numeric_limits<T>::min()-param ) // Derived from this->value+param < std::numeric_limits<T>::min()
				throw std::underflow_error("Underflow error detected on addition operation");
		return NoOverflow<T>( this->value+param );
	}
	NoOverflow<T> operator-(const typename std::conditional<std::is_signed<T>::value, T, InvalidType>::type& param) const{
		if((this->value<0&&param>0) &&  //this->value is negative and param is positive.
			this->value<std::numeric_limits<T>::min()+param ) // Derived from this->value-param < std::numeric_limits<T>::min()
				throw std::overflow_error("Underflow error detected on addition operation");
		else if( std::is_signed<T>::value &&
			(this->value>0&&param<0) && //this->value is positive and param is negative
			this->value>std::numeric_limits<T>::max()+param ) // Derived from this->value-param > std::numeric_limits<T>::max()
				throw std::underflow_error("Overflow error detected on addition operation");
		return NoOverflow<T>( this->value-param );
	}
	NoOverflow<T> operator*(const typename std::conditional<std::is_signed<T>::value, T, InvalidType>::type& param) const{
		if(param!=0){
			if( (this->value>0&&param>0) && //Both this->value and param are positive
				this->value>std::numeric_limits<T>::max()/param ) // Derived from this->value*param > std::numeric_limits<T>::max()
					throw std::overflow_error("Overflow error detected on multiplication operation");
			else if( std::is_signed<T>::value &&
				(this->value<0&&param<0) && //Both this->value and param are negative
				this->value<std::numeric_limits<T>::max()/param ) // Derived from this->value*param > std::numeric_limits<T>::max() NOTE: This is different from the above one. It uses operator< instead of operator>
					throw std::overflow_error("Overflow error detected on multiplication operation");
			else if( this->value>std::numeric_limits<T>::min()/param ) // Derived from this->value*param < std::numeric_limits<T>::min()
				throw std::underflow_error("Underflow error detected on multiplication operation");
		}
		return NoOverflow<T>( this->value*param );
	}

	/*functions for unsigned types*/
	NoOverflow<T> operator+(const typename std::conditional<std::is_unsigned<T>::value, T, InvalidType>::type& param) const{
		if(this->value>std::numeric_limits<T>::max()-param ) // Derived from this->value+param > std::numeric_limits<T>::max()
			throw std::overflow_error("Overflow error detected on addition operation");
		return NoOverflow<T>( this->value+param );
	}
	NoOverflow<T> operator-(const typename std::conditional<std::is_unsigned<T>::value, T, InvalidType>::type& param) const{
		if( this->value<std::numeric_limits<T>::min()+param ) // Derived from this->value-param < std::numeric_limits<T>::min()
			throw std::overflow_error("Underflow error detected on addition operation");
		return NoOverflow<T>( this->value-param );
	}
	NoOverflow<T> operator*(const typename std::conditional<std::is_unsigned<T>::value, T, InvalidType>::type& param) const{
		if(param!=0){
			if( this->value>std::numeric_limits<T>::max()/param ) // Derived from this->value*param > std::numeric_limits<T>::max()
				throw std::overflow_error("Overflow error detected on multiplication operation");
		}
		return NoOverflow<T>( this->value*param );
	}

	NoOverflow<T> operator/(const T& param) const{
		if(param==0) throw std::range_error("Division by zero error detected on division operation");
		return NoOverflow<T>( this->value/param );
	}
	NoOverflow<T> operator%(const T& param) const{
		if(param==0) throw std::range_error("Division by zero error detected on modulo operation");
		return NoOverflow<T>( this->value%param );
	}
	NoOverflow<T> operator&(const T& param) const{ return NoOverflow<T>( this->value&param ); }
	NoOverflow<T> operator|(const T& param) const{ return NoOverflow<T>( this->value|param ); }
	NoOverflow<T> operator^(const T& param) const{ return NoOverflow<T>( this->value^param ); }
	NoOverflow<T> operator<<(const T& param) const{ return NoOverflow<T>( this->value<<param ); }
	NoOverflow<T> operator>>(const T& param) const{ return NoOverflow<T>( this->value>>param ); }

	NoOverflow<T>& operator++(){ *this+=static_cast<T>(1); return *this; } //prefix //Do not assume a two's complement. The value might not wrap after overflow.
	NoOverflow<T>& operator--(){ *this-=static_cast<T>(1); return *this; }
	NoOverflow<T> operator++(int){ T dummy = this->value; *this+=static_cast<T>(1); return NoOverflow<T>(dummy); } //postfix
	NoOverflow<T> operator--(int){ T dummy = this->value; *this-=static_cast<T>(1); return NoOverflow<T>(dummy); }

	NoOverflow<T> operator+() const{ return NoOverflow<T>( *this ); }
	NoOverflow<T> operator-() const{ return NoOverflow<T>( -this->value ); }
	NoOverflow<T> operator~() const{ return NoOverflow<T>( ~this->value ); }

	NoOverflow<T>& operator+=(const T& param){ *this = *this+param; return *this; }
	NoOverflow<T>& operator-=(const T& param){ *this = *this-param; return *this; }
	NoOverflow<T>& operator*=(const T& param){ *this = *this*param; return *this; }
	NoOverflow<T>& operator/=(const T& param){ *this = *this/param; return *this; }
	NoOverflow<T>& operator%=(const T& param){ *this = *this%param; return *this; }
	NoOverflow<T>& operator&=(const T& param){ *this = *this&param; return *this; }
	NoOverflow<T>& operator|=(const T& param){ *this = *this|param; return *this; }
	NoOverflow<T>& operator^=(const T& param){ *this = *this^param; return *this; }
	NoOverflow<T>& operator<<=(const T& param){ *this = *this<<+param; return *this; }
	NoOverflow<T>& operator>>=(const T& param){ *this = *this>>param; return *this; }

	bool operator!(){ return !this->value; }
	bool operator&&(const T& param){ return this->value&&param; }
	bool operator||(const T& param){ return this->value||param; }

	bool operator==(const T& param){ return this->value==param; }
	bool operator!=(const T& param){ return this->value!=param; }
	bool operator<(const T& param){ return this->value<param; }
	bool operator>(const T& param){ return this->value>param; }
	bool operator<=(const T& param){ return this->value<=param; }
	bool operator>=(const T& param){ return this->value>=param; }

	operator T(){ return this->value; }
};
