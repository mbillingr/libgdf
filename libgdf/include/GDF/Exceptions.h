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

#ifndef __EXCEPTIONS_H_INCLUDED__
#define __EXCEPTIONS_H_INCLUDED__

#include <exception>
#include <stdexcept>
#include <list>
#include <string>
#include <sstream>

namespace gdf {
    namespace exception
    {

        /// A general exception
        class general : public std::exception
        {
        public:
            general( const std::string str ) { msg = str; }
            virtual ~general( ) throw() { }
            const char *what( ) const throw() { return msg.c_str(); }
        private:
            std::string msg;
        };

        /// An index exceeds it's valid range.
        class index_out_of_range : public std::range_error
        {
        public:
            index_out_of_range( std::string str ) : range_error("Index out of range: "+str) { }
        };

        /// Preconditions for operation are not met.
        class invalid_operation : public std::runtime_error
        {
        public:
            invalid_operation( std::string str ) : runtime_error("Invalid operation: "+str) { }
        };

        /// A container of some sort (list, vector, array, ... ) is empty
        class empty_container : public std::runtime_error
        {
        public:
            empty_container( std::string str ) : runtime_error("Empty container: "+str) { }
        };

        /// A member function from ChannelDataBase was called. This should not happen and is usually caused by providing an argument with the wrong datatype to a data access function.
        class bad_type_assigned_to_channel : public std::domain_error
        {
        public:
            bad_type_assigned_to_channel( std::string str = "" ) : domain_error("Bad datatype access to channel: "+str) { }
        };

        /// Type ID used is not defined in Types.h
        class invalid_type_id : public std::domain_error
        {
        public:
            invalid_type_id( std::string str ) : domain_error("Invalid type ID: "+str) { }
        };

        /// Channels of different types were used where it's not allowed
        class mixed_types_not_allowed : public std::domain_error
        {
        public:
            mixed_types_not_allowed( std::string str ) : domain_error("Invalid type mix: "+str) { }
        };

        /// Channel does not exist
        class signal_exists_not : public std::domain_error
        {
        public:
            signal_exists_not( std::string str ) : domain_error("Signal does not exist: "+str) { }
        };

        /// Channel does exist
        class signal_exists : public std::domain_error
        {
        public:
            signal_exists( std::string str ) : domain_error("Signal does exist: "+str) { }
        };

        /// Channel does not exist
        class nonexistent_channel_access : public std::out_of_range
        {
        public:
            nonexistent_channel_access( std::string str ) : out_of_range("Channel does not exist: "+str) { }
        };

        /// Number of channels does not match.
        class mismatch_channel_number : public std::domain_error
        {
        public:
            mismatch_channel_number( std::string str ) : domain_error("Number of channels mismatch: "+str) { }
        };

        /// Record Buffer is corrupt. This likely indicates an internal programming error.
        class corrupt_recordbuffer : public std::runtime_error
        {
        public:
            corrupt_recordbuffer( std::string str ) : runtime_error("Record buffer corrupted: "+str) { }
        };

        /// Feature is not implemented yet.
        class feature_not_implemented : public std::logic_error
        {
        public:
            feature_not_implemented( std::string str ) : logic_error("Feature not implemented: "+str) { }
        };

        /// File is not open
        class file_not_open : public std::invalid_argument
        {
        public:
            file_not_open( std::string str ) : invalid_argument("File is not open: "+str) { }
        };

        /// File exists
        class file_exists : public std::invalid_argument
        {
        public:
            file_exists( std::string str ) : invalid_argument("File exists: "+str) { }
        };

        /// File dos not exist
        class file_exists_not : public std::invalid_argument
        {
        public:
            file_exists_not( std::string str ) : invalid_argument("File does not exists: "+str) { }
        };

        /// File exists
        class serialization_error : public std::logic_error
        {
        public:
            serialization_error( std::string str ) : logic_error("Serialization error: "+str) { }
        };

        /// Illegal attempt to change event mode
        class illegal_eventmode_change : public std::domain_error
        {
        public:
            illegal_eventmode_change( std::string str ) : domain_error("Illegal attempt to change event mode: "+str) { }
        };

        /// Invalid event mode
        class invalid_eventmode : public std::domain_error
        {
        public:
            invalid_eventmode( std::string str ) : domain_error("Invalid event mode: "+str) { }
        };

        /// Operation on wrong event mode
        class wrong_eventmode : public std::domain_error
        {
        public:
            wrong_eventmode( std::string str ) : domain_error("Wrong event mode: "+str) { }
        };

        /// Header Issues
        class header_issues : public std::exception
        {
        public:
            header_issues( std::list< std::string > w, std::list< std::string > e )
                : warnings(w), errors(e)
            { }

            header_issues( std::list< std::string > w )
                : warnings(w), errors()
            { }

            std::list< std::string > warnings, errors;

            virtual ~header_issues( ) throw() { }

            const char *what( ) const throw()
            {
                std::stringstream ss;
                std::list< std::string >::const_iterator it;
                if( warnings.size( ) > 0 )
                    ss << "Warnings: " << std::endl;
                for( it=warnings.begin(); it!=warnings.end(); it++ )
                    ss << " -> " << *it << std::endl;

                if( errors.size( ) > 0 )
                {
                    ss << "Errors: " << std::endl;
                for( it=errors.begin(); it!=errors.end(); it++ )
                    ss << " -> " << *it << std::endl;
                }
                return ss.str( ).c_str( );
            }
        };
    }
}

#endif
