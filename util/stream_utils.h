#pragma once
#include <string>
#include <fstream>
#include <iostream>

#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>

namespace util
{
  // Returns stream which outputs data to both console and file
  std::ostream & GetFileAndConsoleStream(std::string const & file_name)
  {
    namespace bio = boost::iostreams;
    using namespace std;

    static std::fstream fileStream(file_name, std::ios::out);
    static bio::tee_filter<ostream> fileFilter(fileStream);
    static bio::filtering_ostream filter;

    if (filter.empty())
    {
      filter.push(fileFilter);
      filter.push(cout);
    }

    return filter;
  }
}
