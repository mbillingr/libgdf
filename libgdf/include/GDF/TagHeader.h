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
// Copyright 2010 Martin Billinger

#ifndef __TAGHEADER_H_INCLUDED__
#define __TAGHEADER_H_INCLUDED__

namespace gdf
{
    /// Contains all information required to construct the optional GDF tag header
    class TagHeader
    {
    public:
        /// Returns number the length of the header (number of 256 byte blocks)
        size_t getLength( ) { return 0; }
    private:
    };
}

#endif
