#ifndef RMAP_SERIALIZE_JSON
#define RMAP_SERIALIZE_JSON

#include <iostream>
#include <cassert>
#include "../RMap.hpp"

/**************************************************************
 *RMap (De-)serialization functions declaration+implementation*
 * Note: the definition is put in .hpp file on purpose.       *
 * See: https://stackoverflow.com/questions/648900/c-templates-undefined-reference/648905#648905  *
 *************************************************************/
 
namespace{ //quote and unquote functions
	template < class StrType=std::string>
	StrType quote(StrType str){
		//NOTE: The order of replacement is IMPORTANT
		// replace \ with \\ .
		int offset = 0;
		while(true){
			offset = str.find("\\", offset);
			if(offset==StrType::npos)
				break;
			str = str.replace(offset, 1, "\\\\");
			offset+=2;
		}
		// replace " with \" .
		offset = 0;
		while(true){
			offset = str.find("\"", offset);
			if(offset==StrType::npos)
				break;
			str = str.replace(offset, 1, "\\\"");
			offset+=2;
		}
		return std::string(1, '"')+str+'"';
	}

	template < class StrType=std::string>
	StrType unquote(StrType str){
		if(str.front()!='"'||str.back()!='"')
			throw std::invalid_argument("unquote: Expected \", the string is not properly fomatted.");
		str = str.substr(1, str.size()-2); //remove the leading and tailing "
		//NOTE: The order of replacement is IMPORTANT
		// replace \" with " .
		int offset = 0;
		while(true){
			offset = str.find("\\\"", offset);
			if(offset==StrType::npos)
				break;
			str = str.replace(offset, 2, "\"");
			offset++;
		}
		// replace \\ with \ .
		offset = 0;
		while(true){
			offset = str.find("\\\\", offset);
			if(offset==StrType::npos)
				break;
			str = str.replace(offset, 2, "\\");
			offset++;
		}
		return str;
	}
} //namespace

template < class StrType=std::string, class CharT=typename StrType::value_type>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const RComponent<StrType>& param){
	try{
		os<< dynamic_cast<const RMap<StrType>&>(param);
	}catch(std::bad_cast&){
		try{
			os<< dynamic_cast<const RLeaf<StrType>&>(param);
		}catch(std::bad_cast&){
			os <<"<ERROR: UNKNOWN TYPE>";
		}
	}
	return os;
}

template < class StrType=std::string, class CharT=typename StrType::value_type>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const RComponentProxy<StrType>& param){
	os<< param.get();
	return os;
}

template < class StrType=std::string, class CharT=typename StrType::value_type>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const RMap<StrType>& param){
	os<< "{ ";
	int count = 0;
	for(const auto& i: param){
		if(dynamic_cast<const RMap<StrType>*>(&param)!=nullptr)
			os<< quote(i.first)<< " : "<< *(i.second);
		else
			os<< quote(i.first)<< " : \""<< *(i.second)<< '"';
		count++;
		if(count<param.size())
			os<< ", ";
	}
	os<< " }";
	return os;
}

template < class StrType=std::string, class CharT=typename StrType::value_type>
std::basic_ostream<CharT>& operator<<(std::basic_ostream<CharT>& os, const RLeaf<StrType>& param){
	std::basic_stringstream<typename StrType::value_type> ss;
	ss<< param.get();
	os<< quote(ss.str());
	return os;
}

template < class StrType=std::string, class CharT=typename StrType::value_type>
std::basic_istream<CharT>& operator>>(std::basic_istream<CharT>& is, RComponent<StrType>& param){
	//TODO: seems useless. People only store thing in RMap.
	//remove this function? (Remove in .hpp also)
	assert(false); //Unimplemented becasue this function seems useless.
}

template < class StrType=std::string, class CharT=typename StrType::value_type>
std::basic_ostream<CharT>& operator>>(std::basic_istream<CharT>& is, RComponentProxy<StrType>& param){
	//TODO: seems useless. People only store thing in RMap.
	//remove this function? (Remove in .hpp also)
	assert(false); //Unimplemented becasue this function seems useless.
}

namespace{ //parse functions
	/** Parse string. For operator>>(std::basic_istream<CharT>& is, RMap<StrType>& param)
	 * @param iterator staring with "
	 * @return unescaped string until another " is reach.*/
	template < class StrType=std::string >
	StrType parseStr(typename StrType::iterator& it){
		if(*it!='"')
			throw std::invalid_argument("parseStr: Expected \", the string is not properly fomatted.");
		it++; //get rid of the leading "
		StrType ret;
		while(*it!='"'){
			if(*it=='\0')
				throw std::invalid_argument("parseStr: Unexpected \\0: the string is not properly fomatted.");
			if(*it=='\\')
				it++;
			ret += *it;
			it++;
		}
		it++; //get rid of the tailing "
		return ret;
	}

	/** Parse RMap. For operator>>(std::basic_istream<CharT>& is, RMap<StrType>& param)
	 * @param iterator staring with {
	 * @return the RMap read.*/
	template < class StrType=std::string >
	RMap<StrType>& parseMap(RMap<StrType>& rMap, typename StrType::iterator& it){
		if(*it!='{')
			throw std::invalid_argument("parseMap: Expected '{', the string is not properly fomatted.");
		it++; //get rid of '{'

		while(true){
			if(*it=='\0')
				throw std::invalid_argument("parseMap: Unexpected \\0: the string is not properly fomatted.");

			//parse key
			while(*it==' ') it++;
			StrType key = parseStr<StrType>(it);

			//look for :
			while(*it==' ') it++;
			if(*it!=':')
				throw std::invalid_argument("parseMap: Expected ':', the string is not properly fomatted.");
			it++; //get rid of ':'

			//parse value
			while(*it==' ') it++;
			if(*it=='"'){
				rMap[key] = parseStr<StrType>(it);
			}else if(*it=='{'){
				rMap[key] = new RMap<StrType>;
				parseMap<StrType>(rMap[key], it);
			}else{
				throw std::invalid_argument("parseMap: Expected '\"' or ':', the string is not properly fomatted.");
			}

			//look for , or }
			while(*it==' ') it++;
			if(*it==','){
				it++; //get rid of ','
				continue;
			}else if(*it=='}'){
				it++; //get rid of '}', in case of recursion.
				break;
			}else{
				throw std::invalid_argument("parseMap: Expected ',' or '}' , the string is not properly fomatted.");
			}
		}
		return rMap;
	}
} //namespace

template < class StrType=std::string, class CharT=typename StrType::value_type>
std::basic_istream<CharT>& operator>>(std::basic_istream<CharT>& is, RMap<StrType>& param){
	size_t bufferSize = is.rdbuf()->in_avail();
	char* buffer = new CharT[bufferSize+1];
	is.read(buffer, bufferSize);
	buffer[bufferSize] = '\0';

	StrType str(buffer);
	typename StrType::iterator it = str.begin();
	parseMap<StrType>(param, it);

	delete[] buffer;

	return is;
}

template < class StrType=std::string, class CharT=typename StrType::value_type>
std::basic_istream<CharT>& operator>>(std::basic_istream<CharT>& is, RLeaf<StrType>& param){
	//TODO: seems useless. remove this function? (Remove in .hpp also)
	StrType str;
	is>>str;
	str = unquote(str);
	param = RLeaf<StrType>(str);
}



#endif
