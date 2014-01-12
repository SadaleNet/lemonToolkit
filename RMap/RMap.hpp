/*
RMap, a recursive data structure.
Copyright (c) 2014, Wong Cho Ching
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

/**
@example RMap
Examples:
@code
#include "path/to/lemon.hpp"
//Or:
//#include "path/to/RMap/RMap.hpp"
//#include "path/to/RMap/RMapSerializer/json.hpp"
//^ Note: you may want to update the line #include "../shorthandMacros.hpp" in RMap.hpp
RMap<> users;
users["user1"] = new RMap<>;
users["user1"]["password"] = "whatever";
users["user1"]["score"] = 9001;
users["user1"]["score"] += 1;
users["user1"]["kills"] = 0;
users["user1"]["weapons"] = new RMap<>;
users["user1"]["weapons"][std::tostring(0)] = "cannon";
users["user1"]["weapons"][std::tostring(1)] = "laser";

users["user2"] = new RMap<>;
users["user1"]["password"] = "whatever";
users["user1"]["score"] = 123456;
std::cout <<users <<std::endl;
@endcode

@note
@code
-RMap is weak-typed. All keys and values are stored with std::basic_string
	This may cause strange behavior like:
	rMap["key"] = 50;
	rMap["key"] += 50; //100
	rMap["key"] = 50;
	rMap["key"] += "50"; //10050 because operator+= from std::basic_string is used.
-A serializer header file is #include'd in lemon.hpp and RMap.cpp . To use a new serializer, remember to update the #include
-This library is not written for performance in mind. It uses dynamic_cast frequently. It do a lot of unnecessary std::string casting.
	Possibly solution: add an enum RComponentType
		add an virtual function: RComponentType getType();
		check the type returned with enum, then downcast with static_cast
	I may implement them when I am bored :P
@endcode
*/

#ifndef RMAP_H
#define RMAP_H

#include <string>
#include <map>
#include <memory>
#include <limits>
#include <type_traits>
#include <stdexcept>
#include <cstdint>
#include "../shorthandMacros.hpp"

template <class StrType> class RMap;
template <class StrType> class RLeaf;

/**
 * RComponent: an abstract class of composite pattern for RMap and RLeaf.
*/
template < class StrType=std::string >
class RComponent{
	public:
	/**
	 * @return number of children. If it's a leaf, returns std::numeric_limits<size_t>::max();
	 * */
	virtual size_t size() const;
};


/**
 * RComponentProxy:
 * virtual proxy for creating/modifying RMap and RLeaf.
*/
template < class StrType=std::string >
class RComponentProxy: public RComponent<StrType>{
	private:
	static RMap<StrType> NO_PARENT;
	RMap<StrType>& parent = NO_PARENT;
	StrType key;

	public:
	RComponentProxy(RMap<StrType>& parent, const StrType& key);
	RComponentProxy();

	/**
	 * Access chilren by index
	 * 
	 * This function first downcast itself to RMap, if success, access the children in RMap.
	 * If the children does not exist, then a RComponentCreator is returned, which will create either a RMap or a RLeaf
	 * 		according to the parameter given in assignment operator.
	 * 
	 * @param  index of a child
	 * @return the children with the given index.
	 * 		If it does not exist, it returns a RComponentCreator.
	 * @throw If the object is RLeaf, this function throws a std::bad_cast.
	 */
	RComponentProxy& operator[](const StrType& key);
	
	/**
	 * Creates a RLeaf and assign a value to it. Then append it to RMap
	 * 
	 * @param The value to be assigned
	 * @return The value assigned
	 * */
	template <class T, class=typename std::enable_if<!std::is_same<RMap<StrType>, T>::value, T>::type>
	RComponentProxy<StrType>& operator=(const T& param);

	/**
	 * @see operator=(const T&)
	 * */
	//TODO: unimplemented
	/*template <class T, class=typename std::enable_if<!std::is_same<RMap<StrType>, T>::value, T>::type>
	T& operator=(T&& b);*/

	/**
	 * Creates a RLeaf and assign a value to it.
	 * 
	 * @param The RMap to be assigned
	 * @return The RMap assigned
	 * */
	RComponentProxy<StrType>& operator=(RMap<StrType>* param);

	/**
	 * Access data of a leaf.
	 * 
	 * This function first downcast itself to RLeaf<T>, if success, it returns the content of it.
	 * 
	 * @return the content of RLeaf
	 * @throw If the object is not a RLeaf, this function throws a std::bad_cast.
	 * */
	template <class T, class=typename std::enable_if<!std::is_convertible<T, StrType>::value, T>::type>
	operator const T() const;

	/**
	 * Access data of a leaf, in std::basic_string.
	 * 
	 * This function first downcast itself to RLeaf<T>, if success, it returns the content of it.
	 * 
	 * @return the content of RLeaf
	 * @throw If the object is not a RLeaf, this function throws an std::bad_cast.
	 * */
	operator const StrType() const;

	/**
	 * Downcast to RLeaf
	 * 
	 * @return the downcasted RLeaf
	 * @throw If the object is not a RLeaf, this function throws a std::bad_cast.
	 * */
	operator RLeaf<StrType>&();

	/**
	 * Downcast to RMap
	 * 
	 * @return the downcasted RMap
	 * @throw If the object is not a RMap, this function throws a std::bad_cast.
	 * */
	operator RMap<StrType>&();

	/**
	 * Get the underlying reference.
	 * 
	 * @returns this->parent.map[this->key]
	 * */
	const RComponent<StrType>& get() const;

	//Define operators
	template<class T, class RetType=typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type>
		RetType operator+(const T& param) const{ return this->operator const RetType()+param; }
	template<class T, class RetType=typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type>
		RetType operator-(const T& param) const{ return this->operator const RetType()-param; }
	template<class T, class RetType=typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type>
		RetType operator*(const T& param) const{ return this->operator const RetType()*param; }
	template<class T, class RetType=typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type>
		RetType operator/(const T& param) const{ return this->operator const RetType()/param; }
	template<class T, class RetType=typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type>
		RetType operator%(const T& param) const{ return this->operator const RetType()%param; }
	template<class T, class RetType=typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type>
		RetType operator&(const T& param) const{ return this->operator const RetType()&param; }
	template<class T, class RetType=typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type>
		RetType operator|(const T& param) const{ return this->operator const RetType()|param; }
	template<class T, class RetType=typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type>
		RetType operator^(const T& param) const{ return this->operator const RetType()^param; }
	template<class T, class RetType=typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type>
		RetType operator<<(const T& param) const{ return this->operator const RetType()<<param; }
	template<class T, class RetType=typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type>
		RetType operator>>(const T& param) const{ return static_cast<const RetType>(*this)>>param; }

	RComponentProxy<StrType>& operator++(){ return (*this+=1); } //prefix
	RComponentProxy<StrType>& operator--(){ return (*this-=1); }
	RComponentProxy<StrType> operator++(int){ RComponentProxy<StrType> old = *this; *this+=1; return old; } //postfix
	RComponentProxy<StrType> operator--(int){ RComponentProxy<StrType> old = *this; *this-=1; return old; }

	double operator+() const{ return *this; }
	double operator-() const{ return -static_cast<double>(*this); }
	intmax_t operator~() const{ return ~static_cast<intmax_t>(*this); }

	template<class T> RComponentProxy<StrType>& operator+=(const T& param){ *this = this->operator +(param); return *this; }
	template<class T> RComponentProxy<StrType>& operator-=(const T& param){ *this = *this-param; return *this; }
	template<class T> RComponentProxy<StrType>& operator*=(const T& param){ *this = *this*param; return *this; }
	template<class T> RComponentProxy<StrType>& operator/=(const T& param){ *this = *this/param; return *this; }
	template<class T> RComponentProxy<StrType>& operator%=(const T& param){ *this = *this%param; return *this; }
	template<class T> RComponentProxy<StrType>& operator&=(const T& param){ *this = *this&param; return *this; }
	template<class T> RComponentProxy<StrType>& operator|=(const T& param){ *this = *this|param; return *this; }
	template<class T> RComponentProxy<StrType>& operator^=(const T& param){ *this = *this^param; return *this; }
	template<class T> RComponentProxy<StrType>& operator<<=(const T& param){ *this = *this<<param; return *this; }
	template<class T> RComponentProxy<StrType>& operator>>=(const T& param){ *this = *this>>param; return *this; }

	bool operator!(){ return !static_cast<double>(*this); }
	template<class T> bool operator&&(const T& param){ return this->operator const T()&&param; }
	template<class T> bool operator||(const T& param){ return this->operator const T()||param; }

	template<class T> bool operator==(const T& param){ return this->operator const typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type()==param; }
	template<class T> bool operator!=(const T& param){ return this->operator const typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type()!=param; }
	template<class T> bool operator<(const T& param){ return this->operator const typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type()<param; }
	template<class T> bool operator>(const T& param){ return this->operator const typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type()>param; }
	template<class T> bool operator<=(const T& param){ return this->operator const typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type()<=param; }
	template<class T> bool operator>=(const T& param){ return this->operator const typename std::conditional<std::is_convertible<T, StrType>::value, StrType, T>::type()>=param; }
};

template < class StrType=std::string >
class RMap: public RComponent<StrType>{
	private:
	friend class RComponent<StrType>;
	typedef std::map< StrType, std::shared_ptr< RComponent<StrType> > > MapType; 
	MapType map;
	friend RComponentProxy<StrType>;

	public:
	RMap() = default;
	RMap(StrType str);

	/**
	 * Creates a RComponentProxy object and return to the client.
	 * The returned RComponentProxy can be used to insert and modify elements into this RMap.
	*/
	RComponentProxy<StrType> operator[](const StrType& key);

	///converts RMap to arbitrary variable.
	template <class T> operator const T() const;

	///Compare RMaps. Returns true if the contents are the same, false else.
	bool operator==(const RMap<StrType>& param);
	///Compare RMaps. Returns true if the contents are different, false else.
	bool operator!=(const RMap<StrType>& param);

	FORWARD(map, typename MapType::iterator, begin, )
	FORWARD(map, typename MapType::const_iterator, begin, const)
	FORWARD(map, typename MapType::const_iterator, cbegin, const)
	FORWARD(map, typename MapType::iterator, end, )
	FORWARD(map, typename MapType::const_iterator, end, const)
	FORWARD(map, typename MapType::const_iterator, cend, const)
	FORWARD(map, bool, empty, const)
	FORWARD(map, void, clear, )

};

template <class StrType=std::string>
class RLeaf: public RComponent<StrType>{
	private:
	StrType value;

	public:
	template <class T> RLeaf(T value);
	template <class T> operator const T() const;
	const StrType& get() const;
};

template < class StrType >
RMap<StrType> RComponentProxy<StrType>::NO_PARENT; //instantiate the static member variable

#endif
