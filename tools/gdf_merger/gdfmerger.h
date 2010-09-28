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

#ifndef GDFMERGER_H
#define GDFMERGER_H

//---------------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <iostream>

#include "GDF/Reader.h"
#include "GDF/Writer.h"

//---------------------------------------------------------------------------------------

class gdfMerger
{
  public:
    gdfMerger(const std::vector<std::string>& inputs,
              const std::string&);
    ~gdfMerger();
    void merge();

  private:

    const std::vector<std::string>& input_files_;
    const std::string& output_file_;

    gdf::Reader reader_;
    gdf::Writer writer_;

    gdf::uint32 event_offset_;

};

//---------------------------------------------------------------------------------------

#endif // GDFMERGER_H
