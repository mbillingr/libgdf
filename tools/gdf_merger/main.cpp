//
// This file is part of libGDF.
//
// libGDF is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// libGDF is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with libGDF.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2010 Christian Breitwieser


//---------------------------------------------------------------------------------------

#include "gdfmerger.h"

#include <string>
#include <vector>
#include <iostream>
#include <iterator>

#include <boost/program_options.hpp>

namespace po  = boost::program_options;

using std::vector;
using std::string;
using std::cerr;
using std::cout;
using std::endl;
using std::ostream;
using std::ostream_iterator;


//---------------------------------------------------------------------------------------

template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
  std::copy(v.begin(), v.end(), ostream_iterator<T>(cout, " "));
    return os;
}

//---------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input-files,i",  po::value< vector<string> >()->composing() , "input files")
        ("output-file,o", po::value< vector<string> >() , "output file")
    ;

    po::positional_options_description p;
    p.add("input-files", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if(vm.count("help") || vm.size() == 0)
    {
      cout << "Usage: gdf_merger [options]\n";
      cout << desc;
      return 0;
    }

    if(!vm.count("output-file"))
    {
      cerr << "Error -- No output file given!" << endl;
      return(1);
    }
    if(vm.count("output-file") > 1)
    {
      cerr << "Error -- More than 1 output file given!" << endl;
      return(1);
    }

    if(!vm.count("input-files"))
    {
      cerr << "Error -- No input file(s) given!" << endl;
      return(1);
    }

    cout << std::left;
    cout << "Merging gdf files ... " << endl;
    cout.width(23);
    cout << "  -- Input files are: " << vm["input-files"].as< vector<string> >() << endl;
    cout.width(23);
    cout << "  -- Output file is: "  << vm["output-file"].as< vector<string> >() << endl;

    gdfMerger merger(vm["input-files"].as< vector<string> >(),
                     vm["output-file"].as< vector<string> >()[0]);

    merger.merge();

    cout << " ... done." << endl;

  }
  catch(std::exception& e)
  {
    cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...)
  {
    cerr << "Exception of unknown type!\n";
  }

  return(0);
}

//---------------------------------------------------------------------------------------
