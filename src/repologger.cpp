/**
 *  Copyright (C) 2014 3D Repo Ltd
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "repologger.h"
#include <ctime>

const std::string repo::core::RepoLogger::LOG_EXTENSION = ".log";

repo::core::RepoLogger::RepoLogger()
{
    streamBuffers.push_back(new RepoStreamBuffer(this, std::cout, false));
    streamBuffers.push_back(new RepoStreamBuffer(this, std::cerr, false));
}

repo::core::RepoLogger::~RepoLogger()
{
    file.close();

    // Cleanup allocated memory
    for (unsigned int i = 0; i < streamBuffers.size(); ++i)
    {
        delete streamBuffers[i];
        streamBuffers[i] = 0;
    }
}

std::string repo::core::RepoLogger::getHtmlFormattedMessage(
        const std::string &message,
        const RepoSeverity &severity)
{
    std::stringstream formatted;

    //--------------------------------------------------------------------------
    // Retrieve current time
    time_t t = time(0);   // get time now
    const struct tm *now = localtime(&t);
    formatted << normalize(now->tm_hour);
    formatted << ":";
    formatted << normalize(now->tm_min);
    formatted << ":";
    formatted << normalize(now->tm_sec);
    formatted << " - ";

    //--------------------------------------------------------------------------
    // Colour and type selection
    std::string color;
    switch(severity)
    {
        case RepoSeverity::REPO_DEBUG_NUM :
            color = "purple";
            break;
        case RepoSeverity::REPO_INFO_NUM :
            color = "green";
            break;
        case RepoSeverity::REPO_NOTICE_NUM :
            color = "blue";
            break;
        case RepoSeverity::REPO_WARNING_NUM :
            color = "orange";
            break;
        case RepoSeverity::REPO_ERROR_NUM :
        case RepoSeverity::REPO_CRITICAL_NUM :
        case RepoSeverity::REPO_ALERT_NUM :
        case RepoSeverity::REPO_PANIC_NUM :
            color = "red";
            break;
        default :
            color = "black";
    }


    //-------------------------------------------------------------------------
    // HTML formatting
    formatted << "<span style='color:";
    formatted << color;
    formatted << "'>";
    formatted << severity;
    formatted << "</span> - ";

    //--------------------------------------------------------------------------
    // Code formatting
    if (RepoSeverity::REPO_DEBUG == severity)
        formatted << "<code>" << message << "</code>";
    else
        formatted << message;

    return formatted.str();
}

std::string repo::core::RepoLogger::getFilename(
        const std::string &extension)
{
    std::stringstream fileName;
    time_t t = time(0);   // get time now
    const struct tm *now = localtime(&t);
    fileName << (now->tm_year + 1900);
    fileName << "-";
    fileName << normalize(now->tm_mon + 1);
    fileName << "-";
    fileName << normalize(now->tm_mday);
    fileName << extension;
    return fileName.str();
}

void repo::core::RepoLogger::log(
        const std::string &msg,
        const RepoSeverity &severity)
{
    //--------------------------------------------------------------------------
    // Check if the log file name has changed and if so, log this information.
    std::string currentFilename = getFilename();
    if (currentFilename != filename)
    {
        filename = currentFilename;
        file.close();
        file.open(filename, std::ios::app);
        log("Log file located at " + getWorkingDirectory() + filename,
            RepoSeverity::REPO_NOTICE);
    }

    /*
    std::string first;
    std::istringstream iss{msg};
    iss >> first;

    if (first == RepoSeverity::DEBUG.str() ||
        first == RepoSeverity::INFO.str() ||
        first == RepoSeverity::NOTICE.str() ||
        first == RepoSeverity::WARNING.str() ||
        first == RepoSeverity::ERROR.str() ||
        first == RepoSeverity::CRITICAL.str() ||
        first == RepoSeverity::ALERT.str() ||
        first == RepoSeverity::PANIC.str())
        severity = RepoSeverity(first);

    std::ostringstream oss;
       oss << iss.rdbuf();
    std::cout << oss.str(); // remove first word
    */

    //--------------------------------------------------------------------------
    // Write log message to the log file and notify the listeners
    std::string formattedMessage = getHtmlFormattedMessage(msg, severity);
    file << formattedMessage << "\n";

    notifyListeners(formattedMessage);
}

std::string repo::core::RepoLogger::getWorkingDirectory()
{
    // See http://msdn.microsoft.com/en-us/library/sf98bd4y%28VS.80%29.aspx
//    char* buffer;
//    std::string path;
//    if((buffer = getcwd( NULL, 0 )))
//    {
//        path = std::string(buffer);
//        free(buffer);
//    }
//    return path;

//    int MAXPATHLEN = 255;
//    char temp[255];
//    return ( getcwd(temp, MAXPATHLEN) ? std::string( temp ) : std::string("") );
    return "path";
}

void repo::core::RepoLogger::messageGenerated(
        const std::ostream *sender,
        const std::string &message)
{
    if (sender == &(std::cout))
       log(message, RepoSeverity::REPO_INFO);
    else if (sender == &(std::cerr))
       log(message, RepoSeverity::REPO_ERROR);
    else
        log(message);
}

std::string repo::core::RepoLogger::normalize(const int &n)
{
    std::stringstream str;
    if (n < 10)
        str << "0";
    str << n;
    return str.str();
}
