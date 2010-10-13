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

#ifndef MXSTRUCTACCESS_H_INCLUDED
#define MXSTRUCTACCESS_H_INCLUDED

#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/cstdint.hpp>

#include <string>
#include <exception>
#include <mex.h>
#include <memory.h>
#include <map>
#include <vector>
#include <iostream>

namespace mx
{
    using namespace boost;

    enum StringConversion
    {
        TONORMAL,
        TOUPPER,
        TOLOWER
    };

    std::string toUpper( const std::string str )
    {
            std::string out( str );
            for( size_t i=0; i<str.length( ); i++ )
            {
                    out[i] = toupper( str[i] );
            }
            return out;
    }

    std::string toLower( const std::string str )
    {
            std::string out( str );
            for( size_t i=0; i<str.length( ); i++ )
            {
                    out[i] = tolower( str[i] );
            }
            return out;
    }

    struct Exception : public std::exception
    {
       std::string s;
       Exception(std::string ss) : s(ss) {}
       virtual ~Exception( ) throw() { }
       const char* what() const throw() { return s.c_str(); }
    };

    struct Exception_nofield : public std::exception
    {
       std::string s;
       Exception_nofield(std::string ss) : s(ss) {}
       virtual ~Exception_nofield( ) throw() { }
       const char* what() const throw() { return s.c_str(); }
    };

    void setField( mxArray *struc, mxArray *value, std::string access, size_t index = 1 );
    mxArray *getField( const mxArray *struc, std::string access, size_t index = 1 );

    // convenience functions

    // directly set a string
    void setFieldString( mxArray *struc, std::string str, std::string access, size_t index = 1 )
    {
        setField( struc, mxCreateString(str.c_str()), access, index );
    }

    // directly set a non-0-terminated string of given length
    void setFieldString( mxArray *struc, const char *str, size_t length, std::string access, size_t index = 1 )
    {
        char *tmp = new char[length+1];
        memcpy( tmp, str, length );
        tmp[length] = 0;
        setField( struc, mxCreateString(tmp), access, index );
        delete[] tmp;
    }

    void setFieldNumeric( mxArray *struc, float value, std::string access, size_t index = 1 )
    {
        mxArray *tmp = mxCreateNumericMatrix( 1, 1, mxSINGLE_CLASS, mxREAL );
        *reinterpret_cast<float*>( mxGetData(tmp) ) = value;
        setField( struc, tmp, access, index );
    }

    void setFieldNumeric( mxArray *struc, double value, std::string access, size_t index = 1 )
    {
        mxArray *tmp = mxCreateNumericMatrix( 1, 1, mxDOUBLE_CLASS, mxREAL );
        *reinterpret_cast<double*>( mxGetData(tmp) ) = value;
        setField( struc, tmp, access, index );
    }

    void setFieldNumeric( mxArray *struc, uint8_t value, std::string access, size_t index = 1 )
    {
        mxArray *tmp = mxCreateNumericMatrix( 1, 1, mxUINT8_CLASS, mxREAL );
        *reinterpret_cast<uint8_t*>( mxGetData(tmp) ) = value;
        setField( struc, tmp, access, index );
    }

    void setFieldNumeric( mxArray *struc, uint16_t value, std::string access, size_t index = 1 )
    {
        mxArray *tmp = mxCreateNumericMatrix( 1, 1, mxUINT16_CLASS, mxREAL );
        *reinterpret_cast<uint16_t*>( mxGetData(tmp) ) = value;
        setField( struc, tmp, access, index );
    }

    void setFieldNumeric( mxArray *struc, uint32_t value, std::string access, size_t index = 1 )
    {
        mxArray *tmp = mxCreateNumericMatrix( 1, 1, mxUINT32_CLASS, mxREAL );
        *reinterpret_cast<uint32_t*>( mxGetData(tmp) ) = value;
        setField( struc, tmp, access, index );
    }

    void setFieldNumeric( mxArray *struc, int64_t value, std::string access, size_t index = 1 )
    {
        mxArray *tmp = mxCreateNumericMatrix( 1, 1, mxINT64_CLASS, mxREAL );
        *reinterpret_cast<int64_t*>( mxGetData(tmp) ) = value;
        setField( struc, tmp, access, index );
    }

    void setFieldNumeric( mxArray *struc, uint64_t value, std::string access, size_t index = 1 )
    {
        mxArray *tmp = mxCreateNumericMatrix( 1, 1, mxUINT64_CLASS, mxREAL );
        *reinterpret_cast<uint64_t*>( mxGetData(tmp) ) = value;
        setField( struc, tmp, access, index );
    }

    std::string getString( const mxArray *field, StringConversion conv = TONORMAL )
    {
        if( mxGetClassID(field) != mxCHAR_CLASS )
            throw mx::Exception( "MATLAB Class is no character/string type" );

        char *ch;
        ch = new char[2048];
        if( mxGetString( field, ch, 2047 ) )
            throw mx::Exception( "Error converting MATLAB string '"+std::string(ch)+"' (maybe too long?)." );
        std::string str( ch );
        delete[] ch;
        switch( conv )
        {
        default: break;
        case TOUPPER: str = toUpper( str ); break;
        case TOLOWER: str = toLower( str ); break;
        }
        return str;
    }

    std::string getFieldString( const mxArray *struc, std::string access, size_t index = 1, StringConversion conv = TONORMAL )
    {
        mxArray *field = getField( struc, access, index );
        if( !field )
            throw mx::Exception( "Field '"+access+"' does not exist." );

        if( mxGetClassID(field) != mxCHAR_CLASS )
            throw mx::Exception( "MATLAB Class is no character type" );

        char *ch;
        ch = new char[2048];
        if( mxGetString( field, ch, 2047 ) )
            throw mx::Exception( "Error converting MATLAB string '"+std::string(ch)+"' (maybe too long?)." );
        std::string str( ch );
        delete[] ch;
        switch( conv )
        {
        default: break;
        case TOUPPER: str = toUpper( str ); break;
        case TOLOWER: str = toLower( str ); break;
        }
        return str;
    }


    void getFieldString( char *dst, size_t maxN, const mxArray *struc, std::string access, size_t index = 1 )
    {
        mxArray *field = getField( struc, access, index );
        if( !field )
            throw mx::Exception( "Field '"+access+"' does not exist." );

        if( mxGetClassID(field) != mxCHAR_CLASS )
            throw mx::Exception( "MATLAB Class is no character type" );

        char *ch = new char[maxN+1];
        if( mxGetString( field, ch, maxN+1 ) )
            throw mx::Exception( "Error converting MATLAB string '"+std::string(ch)+"' (maybe too long?)." );
        memcpy( dst, ch, maxN );
        delete[] ch;
    }

    template<class T>
    T getFieldNumeric( const mxArray *struc, std::string access, size_t index = 1 )
    {
        using boost::numeric_cast;
        using boost::numeric::bad_numeric_cast;
        using boost::numeric::negative_overflow;
        using boost::numeric::positive_overflow;


        mxArray *field = getField( struc, access, index );
        if( !field )
            throw mx::Exception( "Field '"+access+"' does not exist." );

        try
        {
            switch( mxGetClassID( field ) )
            {
            default : throw mx::Exception( "Unknown MATLAB class." ); break;
            case mxFUNCTION_CLASS:
            case mxCELL_CLASS :
            case mxSTRUCT_CLASS:
            case mxLOGICAL_CLASS:
            case mxCHAR_CLASS:
                throw mx::Exception( "MATLAB class is not numeric." ); break;
            case mxDOUBLE_CLASS:
                return numeric_cast<T>( *reinterpret_cast<double*>(mxGetData(field)) ); break;
            case mxSINGLE_CLASS:
                return numeric_cast<T>( *reinterpret_cast<float*>(mxGetData(field)) ); break;
            case mxINT8_CLASS:
                return numeric_cast<T>( *reinterpret_cast<int8_t*>(mxGetData(field)) ); break;
            case mxUINT8_CLASS:
                return numeric_cast<T>( *reinterpret_cast<uint8_t*>(mxGetData(field)) ); break;
            case mxINT16_CLASS:
                return numeric_cast<T>( *reinterpret_cast<int16_t*>(mxGetData(field)) ); break;
            case mxUINT16_CLASS:
                return numeric_cast<T>( *reinterpret_cast<uint16_t*>(mxGetData(field)) ); break;
            case mxINT32_CLASS:
                return numeric_cast<T>( *reinterpret_cast<int32_t*>(mxGetData(field)) ); break;
            case mxUINT32_CLASS:
                return numeric_cast<T>( *reinterpret_cast<uint32_t*>(mxGetData(field)) ); break;
            case mxINT64_CLASS:
                return numeric_cast<T>( *reinterpret_cast<int64_t*>(mxGetData(field)) ); break;
            case mxUINT64_CLASS:
                return numeric_cast<T>( *reinterpret_cast<uint64_t*>(mxGetData(field)) ); break;
            }
        }
        catch(bad_numeric_cast& e) {
            throw mx::Exception( std::string(e.what()) + " while converting '"+access+"'." );
        }

    }

    template<class T>
    void getFieldNumericArray( T *dst, size_t num_el, const mxArray *struc, std::string access, size_t index = 1 )
    {
        using boost::numeric_cast;
        using boost::numeric::bad_numeric_cast;
        using boost::numeric::negative_overflow;
        using boost::numeric::positive_overflow;


        mxArray *field = getField( struc, access, index );

        if( !field )
            throw mx::Exception( "Field '"+access+"' does not exist." );

        if( mxGetM(field) * mxGetN(field) != num_el )
            throw mx::Exception( "'"+access+"' wrong number of elements." );

        try
        {
            for( size_t n=0; n<num_el; n++ )
            {
                switch( mxGetClassID( field ) )
                {
                default : throw mx::Exception( "Unknown MATLAB class: "+std::string(mxGetClassName(field)) ); break;
                case mxFUNCTION_CLASS:
                case mxCELL_CLASS :
                case mxSTRUCT_CLASS:
                case mxLOGICAL_CLASS:
                case mxCHAR_CLASS:
                    throw mx::Exception( "MATLAB class is not numeric." ); break;
                case mxDOUBLE_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<double*>(mxGetData(field))[n] ); break;
                case mxSINGLE_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<float*>(mxGetData(field))[n] ); break;
                case mxINT8_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<int8_t*>(mxGetData(field))[n] ); break;
                case mxUINT8_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<uint8_t*>(mxGetData(field))[n] ); break;
                case mxINT16_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<int16_t*>(mxGetData(field))[n] ); break;
                case mxUINT16_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<uint16_t*>(mxGetData(field))[n] ); break;
                case mxINT32_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<int32_t*>(mxGetData(field))[n] ); break;
                case mxUINT32_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<uint32_t*>(mxGetData(field))[n] ); break;
                case mxINT64_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<int64_t*>(mxGetData(field))[n] ); break;
                case mxUINT64_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<uint64_t*>(mxGetData(field))[n] ); break;
                }
            }
        }
        catch(bad_numeric_cast& e) {
            throw mx::Exception( std::string(e.what()) + " while converting '"+access+"'." );
        }
    }

    template<class T>
    T getNumeric( const mxArray *field )
    {
        using boost::numeric_cast;
        using boost::numeric::bad_numeric_cast;

        try
        {
            switch( mxGetClassID( field ) )
            {
            default : throw mx::Exception( "Unknown MATLAB class: "+std::string(mxGetClassName(field)) ); break;
            case mxFUNCTION_CLASS:
            case mxCELL_CLASS :
            case mxSTRUCT_CLASS:
            case mxLOGICAL_CLASS:
            case mxCHAR_CLASS:
                throw mx::Exception( "MATLAB class is not numeric." ); break;
            case mxDOUBLE_CLASS:
                return numeric_cast<T>( *reinterpret_cast<double*>(mxGetData(field)) ); break;
            case mxSINGLE_CLASS:
                return numeric_cast<T>( *reinterpret_cast<float*>(mxGetData(field)) ); break;
            case mxINT8_CLASS:
                return numeric_cast<T>( *reinterpret_cast<int8_t*>(mxGetData(field)) ); break;
            case mxUINT8_CLASS:
                return numeric_cast<T>( *reinterpret_cast<uint8_t*>(mxGetData(field)) ); break;
            case mxINT16_CLASS:
                return numeric_cast<T>( *reinterpret_cast<int16_t*>(mxGetData(field)) ); break;
            case mxUINT16_CLASS:
                return numeric_cast<T>( *reinterpret_cast<uint16_t*>(mxGetData(field)) ); break;
            case mxINT32_CLASS:
                return numeric_cast<T>( *reinterpret_cast<int32_t*>(mxGetData(field)) ); break;
            case mxUINT32_CLASS:
                return numeric_cast<T>( *reinterpret_cast<uint32_t*>(mxGetData(field)) ); break;
            case mxINT64_CLASS:
                return numeric_cast<T>( *reinterpret_cast<int64_t*>(mxGetData(field)) ); break;
            case mxUINT64_CLASS:
                return numeric_cast<T>( *reinterpret_cast<uint64_t*>(mxGetData(field)) ); break;
            }
        }
        catch(bad_numeric_cast& e) {
            throw mx::Exception( e.what() );
        }
    }

    template<class T>
    void getNumericArray( T *dst, size_t num_el, const mxArray *field )
    {
        using boost::numeric_cast;
        using boost::numeric::bad_numeric_cast;

        if( mxGetM(field) * mxGetN(field) != num_el )
            throw mx::Exception( "wrong number of elements." );

        try
        {
            for( size_t n=0; n<num_el; n++ )
            {
                switch( mxGetClassID( field ) )
                {
                default : throw mx::Exception( "Unknown MATLAB class: "+std::string(mxGetClassName(field)) ); break;
                case mxFUNCTION_CLASS:
                case mxCELL_CLASS :
                case mxSTRUCT_CLASS:
                case mxLOGICAL_CLASS:
                case mxCHAR_CLASS:
                    throw mx::Exception( "MATLAB class is not numeric." ); break;
                case mxDOUBLE_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<double*>(mxGetData(field))[n] ); break;
                case mxSINGLE_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<float*>(mxGetData(field))[n] ); break;
                case mxINT8_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<int8_t*>(mxGetData(field))[n] ); break;
                case mxUINT8_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<uint8_t*>(mxGetData(field))[n] ); break;
                case mxINT16_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<int16_t*>(mxGetData(field))[n] ); break;
                case mxUINT16_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<uint16_t*>(mxGetData(field))[n] ); break;
                case mxINT32_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<int32_t*>(mxGetData(field))[n] ); break;
                case mxUINT32_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<uint32_t*>(mxGetData(field))[n] ); break;
                case mxINT64_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<int64_t*>(mxGetData(field))[n] ); break;
                case mxUINT64_CLASS:
                    dst[n] = numeric_cast<T>( reinterpret_cast<uint64_t*>(mxGetData(field))[n] ); break;
                }
            }
        }
        catch(bad_numeric_cast& e) {
            throw mx::Exception( e.what() );
        }
    }

    template<class T>
    std::vector<T> getNumericArray( const mxArray *field )
    {
        size_t N = mxGetNumberOfElements( field );
        std::vector<T> vec;
        vec.resize( N );

        getNumericArray<T>( &vec[0], N, field );
        return vec;
    }

    std::map<std::string,const mxArray*> getFieldRecursive( const mxArray *struc, std::string prefix = "" )
    {
        std::map<std::string,const mxArray*> elements;

        if( !mxIsStruct( struc ) )
            return elements;

        size_t num_el =  mxGetNumberOfElements( struc );
        size_t num_fl =  mxGetNumberOfFields( struc );

        for( size_t el=0; el<num_el; el++ )
        {
            for( size_t fl=0; fl<num_fl; fl++ )
            {
                std::string fullname = prefix;
                if( num_el > 1 )
                    fullname = fullname + "(" + lexical_cast<std::string>(el) + ")";
                if( prefix != "" || num_el > 1 )
                    fullname = fullname +  ".";
                fullname = fullname + mxGetFieldNameByNumber( struc, fl );

                mxArray *field = mxGetFieldByNumber( struc, el, fl );

                if( field )
                {
                    if( mxIsStruct(field) )
                    {
                        std::map<std::string,const mxArray*> tmp = getFieldRecursive( field, fullname );
                        elements.insert( tmp.begin(), tmp.end() );
                    }
                    else
                        elements[fullname] = field;
                }
            }
        }

        return elements;
    }

}

void mx::setField( mxArray *struc, mxArray *value, std::string access, size_t index )
{
    using boost::lexical_cast;

    if( !mxIsStruct( struc ) )
        throw mx::Exception( "setField: mxArray is not a struct." );

    if( index > mxGetNumberOfElements( struc ) || index < 1 )
        throw mx::Exception( "setField: index out of bounds." );

    std::string fieldname;
    size_t field_index;

    bool have_subfields;

    // first test if there are any subfields to process
    size_t punkt = access.find( '.' );
    if( punkt == std::string::npos )
    {
        fieldname = access;
        have_subfields = false;
    }
    else
    {
        fieldname = access.substr( 0, punkt );
        have_subfields = true;
    }

    // now test if we want to access an array element
    size_t ar1 = fieldname.find( '(' );
    size_t ar2 = fieldname.find( ')' );

    if( ar1 != std::string::npos && ar2 != std::string::npos )
    {
        field_index = lexical_cast<unsigned long>( fieldname.substr( ar1+1, ar2-ar1-1 ) );
        if( field_index < 1 )
            throw mx::Exception( "setField: "+fieldname+" index out of bounds." );
        fieldname = fieldname.substr( 0, ar1 );
    }
    else if( ar1 == std::string::npos && ar2 == std::string::npos )
        field_index = 1;
    else
        throw mx::Exception( "setField: bracket error." );

    mxArray *field = mxGetField( struc, index-1, fieldname.c_str() );

    if( have_subfields )
    {
        // if field does not exist, add and initialize as new substruct
        if( !field )
        {
            mxAddField( struc, fieldname.c_str() );
            mxSetField( struc, index-1, fieldname.c_str(), mxCreateStructMatrix( field_index, 1, 0, NULL ) );
            field = mxGetField( struc, index-1, fieldname.c_str() );
            //std::cout << " creating subfield " << fieldname << " with " << field_index << " elements." << std::endl;
        }

        // do some error checking
        if( !mxIsStruct( field ) )
            throw mx::Exception( "setField: "+fieldname+" is no structure." );

        if( field_index > mxGetNumberOfElements( field ) )
            throw mx::Exception( "setField: "+fieldname+"("+lexical_cast<std::string>(field_index)+") index out of bounds." );

        // recurse into subfield
        setField( field, value, access.substr( punkt+1, access.length( ) ), field_index );
    }
    else
    {
        // add field if it does not exist
        if( !field )
        {
            mxAddField( struc, fieldname.c_str() );
            //std::cout << " creating field " << fieldname << "." << std::endl;
        }

        // set value of field
        mxSetField( struc, index-1, fieldname.c_str(), value );
    }
}

mxArray *mx::getField( const mxArray *struc, std::string access, size_t index )
{
    using boost::lexical_cast;

    if( !mxIsStruct( struc ) )
        throw mx::Exception( "getField: mxArray is not a struct." );

    if( index > mxGetNumberOfElements( struc ) || index < 1 )
        throw mx::Exception( "getField: index out of bounds." );

    std::string fieldname;
    size_t field_index;

    bool have_subfields;

    // first test if there are any subfields to process
    size_t punkt = access.find( '.' );
    if( punkt == std::string::npos )
    {
        fieldname = access;
        have_subfields = false;
    }
    else
    {
        fieldname = access.substr( 0, punkt );
        have_subfields = true;
    }

    // now test if we want to access an array element
    size_t ar1 = fieldname.find( '(' );
    size_t ar2 = fieldname.find( ')' );

    if( ar1 != std::string::npos && ar2 != std::string::npos )
    {
        field_index = lexical_cast<unsigned long>( fieldname.substr( ar1+1, ar2-ar1-1 ) );
        if( field_index < 1 )
            throw mx::Exception( "getField: "+fieldname+" index out of bounds." );
        fieldname = fieldname.substr( 0, ar1 );
    }
    else if( ar1 == std::string::npos && ar2 == std::string::npos )
        field_index = 1;
    else
        throw mx::Exception( "getField: bracket error." );

    mxArray *field = mxGetField( struc, index-1, fieldname.c_str() );

    if( !field )
        throw mx::Exception_nofield( "getField: field " + fieldname + " does not exist." );

    if( have_subfields )
    {
        // do some error checking
        if( !mxIsStruct( field ) )
            throw mx::Exception( "getField: "+fieldname+" is no structure." );

        if( field_index > mxGetNumberOfElements( field ) )
            throw mx::Exception( "getField: "+fieldname+"("+lexical_cast<std::string>(field_index)+") index out of bounds." );

        //std::cout << "recursing into " << fieldname << "(" << lexical_cast<std::string>(field_index) << ")" << std::endl;
        // recurse into subfield
        return getField( field, access.substr( punkt+1, access.length( ) ), field_index );
    }
    else
    {
        //std::cout << "returning " << fieldname << std::endl;
        return mxGetField( struc, index-1, fieldname.c_str() );
    }
}

#endif
