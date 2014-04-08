// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with libGDF.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2010 Martin Billinger

#ifndef __COMMANDS_H_INCLUDED__
#define __COMMANDS_H_INCLUDED__

#include <map>
#include <stdexcept>
#include <string>
#include "mex.h"

class Command
{
public:
    virtual void execute( mxArray *plhs[], const mxArray *prhs[] ) = 0;

    void operator()( size_t nlhs, mxArray *plhs[], size_t nrhs, const mxArray *prhs[] )
    {
        if( m_nlhs != nlhs )
            throw std::invalid_argument( "Invalid number of output arguments." );

        if( m_nrhs != nrhs )
            throw std::invalid_argument( "Invalid number of input arguments." );

        execute( plhs, prhs );
    }

private:
    friend class CommandManager;

    void setNP( size_t nlhs, size_t nrhs) { m_nlhs = nlhs; m_nrhs = nrhs; }

    size_t m_nlhs, m_nrhs;
};

class CommandManager
{
public:
    CommandManager( )
    {
        default_cmd = NULL;
    }

    virtual ~CommandManager( )
    {
        std::map< std::string, Command* >::iterator it = commands.begin( );
        for( ; it != commands.end(); it++ )
            delete it->second;
    }

    void setDefaultCommand( const std::string cmdstr )
    {
        std::map< std::string, Command* >::iterator it = commands.find( toUpper( cmdstr ) );
        if( it == commands.end() )
            throw std::invalid_argument( "Invalid command: "+toUpper( cmdstr ) );
        default_cmd = it->second;
    }

    void registerCommand( const std::string cmdstr, Command * cmd, size_t nlhs, size_t nrhs )
    {
        cmd->setNP( nlhs, nrhs );
        commands[toUpper( cmdstr )] = cmd;
    }

    void execute( const std::string cmdstr, int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
    {
        std::map< std::string, Command* >::iterator it = commands.find( toUpper( cmdstr ) );
        if( it == commands.end() )
            throw std::invalid_argument( "Invalid command: "+toUpper( cmdstr ) );
        (*it->second)( nlhs, plhs, nrhs, prhs );
    }

    void execute( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[] )
    {
        if( !default_cmd )
            throw std::invalid_argument( "No default command specified." );
        (*default_cmd)( nlhs, plhs, nrhs, prhs );
    }

    static std::string toUpper( const std::string str )
    {
            std::string out( str );
            for( size_t i=0; i<str.length( ); i++ )
            {
                    out[i] = toupper( str[i] );
            }
            return out;
    }

private:
    std::map< std::string, Command* > commands;
    Command *default_cmd;
};

#endif // COMMANDS_H
