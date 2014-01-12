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
/*
5/1/2014:
	Design.
	Test
6/1/2014:
	Implemented a working version
	Weak type system adopted.
7/1/2014:
	test cases
	bugfixes
9/1/2014:
	documentation: added the word "proxy" in comment of RComponent.
11/1/2014:
	detached RComponentProxy from RComponent
	detached serializer from RMap.hpp and RMap.cpp
12/1/2014
	adapted into lemon tookit
*/
#include <iostream>
#include <sstream>
#include <functional>
#include <cassert>
#include <algorithm>
#include "RMap.hpp"
#include "RMapSerializer/json.hpp"

/***************************
 *RComponent Implementation* 
 **************************/
template <class StrType>
size_t RComponent<StrType>::size() const{
	const RMap<StrType>* rMap = dynamic_cast<const RMap<StrType>*>(this);
	if(rMap!=nullptr)
		return rMap->map.size();
	else
		return std::numeric_limits<size_t>::max();
}

/********************************
 *RComponentProxy Implementation* 
 *******************************/
template <class StrType>
RComponentProxy<StrType>& RComponentProxy<StrType>::operator[](const StrType& key){
	RMap<StrType>* rMap = dynamic_cast<RMap<StrType>*>( (this->parent.map[this->key]).get() );
	if(rMap!=nullptr){
		static std::shared_ptr< RComponentProxy<StrType> > compoCreator;
		compoCreator = std::shared_ptr< RComponentProxy<StrType> >( new RComponentProxy<StrType>(*rMap, key) );
		return *compoCreator;
	}
	throw std::invalid_argument("RComponentProxy::operator[]: The underlying refernece in not a RMap");
}

template <class StrType>
RComponentProxy<StrType>::RComponentProxy(RMap<StrType>& parent, const StrType& key):parent(parent),key(key){}

template <class StrType>
RComponentProxy<StrType>::RComponentProxy(){}

template <class StrType> template <class T, class>
RComponentProxy<StrType>& RComponentProxy<StrType>::operator=(const T& param){
	this->parent.map[this->key] = std::shared_ptr<RComponent<StrType>>( new RLeaf<StrType>(param) );
	return *this;
}

template <class StrType>
RComponentProxy<StrType>& RComponentProxy<StrType>::operator=(RMap<StrType>* param){
	this->parent.map[this->key] = std::shared_ptr<RComponent<StrType>>( param );
	return *this;
}

template<class StrType> template<class T, class>
RComponentProxy<StrType>::operator const T() const{
	RComponent<StrType>* rMap = (this->parent.map[this->key]).get();
	if(dynamic_cast<const RLeaf<StrType>*>(rMap)!=nullptr)
		return dynamic_cast<const RLeaf<StrType>&>(*rMap);
	return dynamic_cast<const RMap<StrType>&>(*rMap);
}

template<class StrType>
RComponentProxy<StrType>::operator const StrType() const{
	RComponent<StrType>* rMap = (this->parent.map[this->key]).get();
	if(dynamic_cast<const RLeaf<StrType>*>(rMap)!=nullptr)
		return ( dynamic_cast<const RLeaf<StrType>*>(rMap)->get() );
	std::basic_stringstream<typename StrType::value_type> ss;
	ss<< dynamic_cast<const RMap<StrType>&>(*rMap);
	return ss.str();
}

template<class StrType>
RComponentProxy<StrType>::operator RMap<StrType>&(){
	RComponent<StrType>* rMap = (this->parent.map[this->key]).get();
	return dynamic_cast<RMap<StrType>&>(*rMap);
}

template<class StrType>
RComponentProxy<StrType>::operator RLeaf<StrType>&(){
	RComponent<StrType>* rMap = (this->parent.map[this->key]).get();
	return dynamic_cast<RLeaf<StrType>&>(*rMap);
}

template<class StrType>
const RComponent<StrType>& RComponentProxy<StrType>::get() const{
	return *(this->parent.map[this->key]);
}

/*********************
 *RMap Implementation* 
 ********************/
template <class StrType>
RMap<StrType>::RMap(StrType str) :RComponent<StrType>(){
	std::basic_stringstream<typename StrType::value_type> ss;
	ss.str(str);
	ss>> *this;
}

template <class StrType>
RComponentProxy<StrType> RMap<StrType>::operator[](const StrType& key){
	RMap<StrType>& lValueThis = *this;
	return RComponentProxy<StrType>(lValueThis, key);
}

template <class StrType> template <class T>
RMap<StrType>::operator const T() const{
	std::basic_stringstream<typename StrType::value_type> ss;
	ss<< *this;
	T ret;
	ss>> ret;
	return ret;
}

template <class StrType>
bool RMap<StrType>::operator==(const RMap<StrType>& param){
	//TODO: SLOW! Rewrite this in a smarter way.
	std::basic_stringstream<typename StrType::value_type> ss1;
	ss1<< *this;
	std::basic_stringstream<typename StrType::value_type> ss2;
	ss2<< param;
	return ss1.str() == ss2.str();
}
template <class StrType>
bool RMap<StrType>::operator!=(const RMap<StrType>& param){
	return !(*this == param);
}

/**********************
 *RLeaf Implementation* 
 *********************/
template <class StrType> template <class T>
RLeaf<StrType>::RLeaf(T value){
	std::basic_stringstream<typename StrType::value_type> ss;
	ss<< value;
	this->value = ss.str();
}

template <class StrType> template <class T>
RLeaf<StrType>::operator const T() const{
	std::basic_stringstream<typename StrType::value_type> ss(value);
	T ret;
	ss>> ret;
	return ret;
}

template <class StrType>
const StrType& RLeaf<StrType>::get() const{ return value; }

TESTCASES(RComponent)
	std::stringstream ss;
	RMap<> a;
	//basic variable storage tests
	a["hi"] = 50;
	a["bye"] = 90.8;
	a["f**\"k you"] = "jaetlkuretio65";
	a["screw this"] = new RMap<>();
	a["screw this"]["hohoho"] = 4560;
	assert(a["hi"]==50);
	assert(a["bye"]==90.8);
	assert(a["f**\"k you"]=="jaetlkuretio65");
	assert(a["screw this"]["hohoho"]==4560);

	//serialization tests
	std::string EXPECTED_STR = R"({ "bye" : "90.8", "f**\"k you" : "jaetlkuretio65", "hi" : "50", "screw this" : { "hohoho" : "4560" } })";
	ss<< a;
	assert(ss.str()==EXPECTED_STR);

	//deserialization tests
	RMap<> b;
	ss.str(EXPECTED_STR);
	ss>> b;
	assert(b==EXPECTED_STR);
	assert(b==a);
	assert(b["hi"]==50);
	assert(b["bye"]==90.8);
	assert(b["f**\"k you"]=="jaetlkuretio65");
	assert(b["screw this"]["hohoho"]==4560);

	//value insertion tests
	b["screw this"]["kukuku"] = 200.37;
	assert(b!=EXPECTED_STR);
	assert(b!=a);
	assert(b["screw this"]["kukuku"]==200.37);

	//type conversion tests
	assert(b["f**\"k you"]==std::string("jaetlkuretio65"));
	b["hi"] += "50";
	assert(b["hi"]=="5050");
	assert(b["hi"]==5050);
	assert(b["hi"]==5050.0);
	b["hi"] += 50;
	assert(b["hi"]=="5100");
	assert(b["hi"]==5100);
	assert(b["hi"]==5100.0);

	//escape string test
	RMap<> escapeStringTest;
	escapeStringTest["\\\\\\"] = 10;
	escapeStringTest["a\\\\\\"] = 50;
	escapeStringTest["\\\\\\a"] = 70;
	escapeStringTest["a\\\\\\a"] = 9001;
	escapeStringTest["\"\"\""] = 10;
	escapeStringTest["a\"\"\""] = 50;
	escapeStringTest["\"\"\"a"] = 70;
	escapeStringTest["a\"\"\"a"] = 9001;
	ss.str("");
	ss<<escapeStringTest;
	RMap<> escapeStringTestClone;
	ss>>escapeStringTestClone;
	assert(escapeStringTest==escapeStringTestClone);

	//numeric key tests
	RMap<> numericKeyTest;
	for(size_t i=0; i<50; i++)
		numericKeyTest[std::to_string(i)] = i;
	ss.str("");
	ss<<numericKeyTest;
	assert(ss.str()==R"({ "0" : "0", "1" : "1", "10" : "10", "11" : "11", "12" : "12", "13" : "13", "14" : "14", "15" : "15", "16" : "16", "17" : "17", "18" : "18", "19" : "19", "2" : "2", "20" : "20", "21" : "21", "22" : "22", "23" : "23", "24" : "24", "25" : "25", "26" : "26", "27" : "27", "28" : "28", "29" : "29", "3" : "3", "30" : "30", "31" : "31", "32" : "32", "33" : "33", "34" : "34", "35" : "35", "36" : "36", "37" : "37", "38" : "38", "39" : "39", "4" : "4", "40" : "40", "41" : "41", "42" : "42", "43" : "43", "44" : "44", "45" : "45", "46" : "46", "47" : "47", "48" : "48", "49" : "49", "5" : "5", "6" : "6", "7" : "7", "8" : "8", "9" : "9" })");

	//recursion test
	RMap<> recursionTest;
	RMap<>* recursionPtr = &recursionTest;
	for(size_t i=0;i<128;i++){
		RMap<>* oldPtr = recursionPtr;
		recursionPtr = new RMap<>;
		(*oldPtr)["recursion"] = recursionPtr;
	}
	ss.str("");
	ss<<recursionTest;
	assert(ss.str()==R"({ "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : { "recursion" : {  } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } } })");

TESTCASES_END(RComponent)
