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
// Copyright 2012 Martin Billinger

#include "config-tests.h"

#include <GDF/Reader.h>

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

#include <boost/numeric/conversion/cast.hpp>

using namespace std;

const string reffile = string(GDF_SOURCE_ROOT)+"/sampledata/alltypes.gdf";

// 512 Bytes of text which is coded in the file
const string text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed elit massa, mattis vitae pretium a, congue et nisi. Duis quis sollicitudin turpis. Proin eget erat ac nulla adipiscing gravida. Proin eget pharetra felis. In posuere risus sed leo ultricies fermentum. Etiam eu justo elit. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Quisque id erat diam, eget fermentum est. Etiam in nisl vel lectus ornare porta. Aenean quam est, rutrum ac sollicitudin eu, volutpat non enim. Morbi lacus lorem cras amet.";

int main( )
{
    try
    {
        cout << "Creating Reader instance." << endl;
        gdf::Reader r;

        cout << "Opening '" << reffile << "' for reading." << endl;
        r.open( reffile );

        size_t M = r.getMainHeader_readonly().get_num_signals();
        if( M != 10 )
            throw(std::invalid_argument("ERROR -- Wrong number of channels (expected 10)."));

        for( size_t i=0; i<M; i++ )
        {
            for( size_t j=0; j<text.length(); j++ )
            {
                double d = r.getSample( i, j );
                char ch = boost::numeric_cast<char>( d );
                cout << ch;
                if( ch != text[j] )
                    throw(std::invalid_argument("ERROR -- Wrong content."));
            }
            cout << endl;
            cout << endl;
        }

        r.close( );

        return 0;   // test succeeded
    }
    catch( std::exception &e )
    {
        std::cout << "Caught Exception: " << e.what( ) << endl;
    }
    catch( ... )
    {
        std::cout << "Caught Unknown Exception." << endl;
    }

    return 1;   // test failed
}
