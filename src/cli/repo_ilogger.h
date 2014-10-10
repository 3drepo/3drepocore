/*
-------------------------------------------------------------------------------
3D Repo (http://3drepo.org)
-------------------------------------------------------------------------------
Copyright (c) 2013 3D Repo                                                  
All rights reserved.

Author: Jozef Dobos
Date: 10/07/2013
Version: 1.0.0
-------------------------------------------------------------------------------
*/

#ifndef REPO_ILOGGER_H
#define REPO_ILOGGER_H

//-----------------------------------------------------------------------------
#include <string>
#include <sstream>
#include <iostream>
//-----------------------------------------------------------------------------
#include <boost/uuid/uuid_generators.hpp>
//-----------------------------------------------------------------------------

namespace repo {

//! Repo logging types.
enum RepoLogType { 
	REPO_INFO,
	REPO_CODE,
	REPO_WARNING,
	REPO_ERROR,
	REPO_EXPERIMENT
};

//! Streams the given string into a specific class.
template <class T>
T fromString(const std::string & s)
{
	T result;
	std::stringstream sstr;
	sstr << s;
	sstr >> result;
	return result;
}

//! Converts a given variable to string. 
template <class T>
std::string toString(const T & d)
{
	std::stringstream sstr;
	sstr << d;
	return sstr.str();
}

//! Converts a text to a uuid by applying "{" + "}" to it.
template <class T>
boost::uuids::uuid textToUUID(const T & text)
{
	std::stringstream sstr;		
	sstr << "{" << text << "}";
	boost::uuids::string_generator gen;	
	return gen(sstr.str());	
}

//! Converts a given string to a uuid.
static boost::uuids::uuid stringToUUID(const std::string & s)
{
	boost::uuids::string_generator gen;
	return gen(s);	
}

/*!
 * Logging interface that defines method to log messages. Requires 
 * implementation in the GUI class or the application that makes use of the
 * 3D Repo Core library.
 */
class RepoILogger
{
	public :

		virtual ~RepoILogger() {};
				
		//! Logs the given message according to its logging type.
		virtual void log(const RepoLogType, const std::string &) = 0;	

		virtual void log(const RepoLogType t, const char * msg)
		{ log(t, std::string(msg)); };

		//! Converts the given message into a string and logs it.
		template <class T>
		void log(const RepoLogType t, const T & msg) 
		{ log(t, toString(msg)); };

};

} // end namespace repo

#endif // end REPO_ILOGGER_H
