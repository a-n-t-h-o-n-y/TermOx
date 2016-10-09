/// \file bad_optional_access.hpp
#ifndef BAD_OPTIONAL_ACCESS_HPP
#define BAD_OPTIONAL_ACCESS_HPP

#include <exception>
#include <memory>
#include <string>

namespace mcurses
{

class bad_optional_access: public std::exception {
public:
	bad_optional_access(){}
	bad_optional_access(const std::string& w):what_{w}{}
	bad_optional_access(std::string&& w):what_{std::forward<std::string>(w)}{}
	
	const char* what() const throw() override
	{
		return what_.c_str();
	}
private:
	std::string what_;
};

}	// namespace mcurses

#endif	// BAD_OPTIONAL_ACCESS_HPP