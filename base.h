#ifndef __BASE_H__
#define __BASE_H__

#include <bits/stdc++.h>
template<typename T>
inline int byte_cast(T x){return x>255?255:(x<0?0:x); }
template<typename T>
inline int byte_cast2(T x){return x>255?255:(x<0?abs(x):x); }

typedef unsigned char  BYTE;
typedef unsigned short WORD; 
typedef unsigned long  DWORD;
typedef unsigned long  LONG;

std::vector<std::string> str_split(const std::string& s, const char& delim) {
	std::vector<std::string> tokens;
	size_t lastPos = s.find_first_not_of(delim, 0);
	size_t pos = s.find(delim, lastPos);
	while (lastPos != std::string::npos) {
		tokens.emplace_back(s.substr(lastPos, pos - lastPos));
		lastPos = s.find_first_not_of(delim, pos);
		pos = s.find(delim, lastPos);
	}
	return tokens;
}
#endif
