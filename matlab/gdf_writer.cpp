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

#include "gdf_mex.h"
#include "matlab_tools/InstanceManager.h"
#include "matlab_tools/Commands.h"
#include "matlab_tools/mxStructAccess.h"
#include <GDF/Writer.h>
#include <mex.h>

using namespace std;

// ===================================================================================================
//      Object Interface to mex
// ===================================================================================================

class CmexObject
{
public:
    CmexObject( );

    ~CmexObject( );

    void execute( size_t nlhs, mxArray *plhs[], size_t nrhs, const mxArray *prhs[] );

    static CmexObject &getInstance( ) { return instance; }

    InstanceManager<gdf::Writer> writers;
    CommandManager commands;

private:
    static CmexObject instance;
};

// ===================================================================================================
//      Commands
// ===================================================================================================

class CMD_init : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_getheader : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_setheader : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };
class CMD_createsignal : public Command { void execute( mxArray *plhs[], const mxArray *prhs[] ); };

// ===================================================================================================
//      mexFunction
// ===================================================================================================

CmexObject CmexObject::instance;

void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
{
    CmexObject::getInstance( ).execute( nlhs, plhs, nrhs, prhs );
}

// ===================================================================================================
//      CmexObject Definition
// ===================================================================================================


CmexObject::CmexObject( )
{
    commands.registerCommand( "Init", new CMD_init( ), 1, 0 );
    commands.registerCommand( "GetHeader", new CMD_getheader( ), 1, 1 );
    commands.registerCommand( "SetHeader", new CMD_setheader( ), 0, 2 );
    commands.registerCommand( "createsignal", new CMD_createsignal( ), 0, 2 );
}

CmexObject::~CmexObject( )
{
}

void CmexObject::execute( size_t nlhs, mxArray *plhs[], size_t nrhs, const mxArray *prhs[] )
{
    if( nrhs < 1 )
        throw std::invalid_argument( "Expecting at least one argument." );

    if( !mxIsChar(prhs[0]) )
        throw std::invalid_argument( "First argument must be a command (string)." );

    char *command;
    command = new char[32];
    if( mxGetString( prhs[0], command, 32 ) != 0 )
        throw std::invalid_argument( "Unknown problem with command string." );

    commands.execute( command, nlhs, plhs, nrhs-1, prhs+1 );
    delete[] command;
}

// ===================================================================================================
//      Command Definitions
// ===================================================================================================

void CMD_init::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = CmexObject::getInstance().writers.newInstance( );
    plhs[0] = mxCreateNumericMatrix( 1, 1, mxUINT64_CLASS, mxREAL );
    *reinterpret_cast<size_t*>(mxGetData( plhs[0] )) = handle;
}

void CMD_getheader::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    plhs[0] = constructHeaderStruct( w );
    Header2Struct( plhs[0], &w->getHeaderAccess_readonly() );
}

void CMD_setheader::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    Struct2Header( prhs[1], &w->getHeaderAccess( ) );
}

void CMD_createsignal::execute( mxArray *plhs[], const mxArray *prhs[] )
{
    size_t handle = mx::getNumeric<size_t>( prhs[0] );
    gdf::Writer *w = CmexObject::getInstance().writers.get( handle );
    size_t channel = mx::getNumeric<size_t>( prhs[1] );
    if( channel < 1 )
        throw std::invalid_argument( "Channel index must be >= 1." );
    w->createSignal( channel - 1, true );
}

