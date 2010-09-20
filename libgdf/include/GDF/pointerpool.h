#ifndef POINTERPOOL_H
#define POINTERPOOL_H

#include <list>
#include <iostream>

template<typename T>
class PointerPool
{
public:
    /// Constructor
    PointerPool(  T type_template, size_t num_el = 1000 )
    {
        if( num_el == 0 )
            throw std::invalid_argument( "PointerPool must not be empty." );
        m_size_increment = num_el;

        m_avail = num_el;

        m_instances.resize( num_el, type_template );

        typename std::list<T>::iterator it = m_instances.begin( );
        for( ; it != m_instances.end(); it++ )
            m_free_pointers.push_back( &(*it) );

        std::cout << "PointerPool::PointerPool( )" << std::endl;
    }

    /// Destructor
    virtual ~PointerPool( )
    {
        std::cout << "PointerPool::~PointerPool( )" << std::endl;
    }

    /// Add new elements to pool
    void expand( size_t num_el )
    {
        std::cout << "PointerPool::expand( )" << std::endl;
        for( size_t i=0; i<num_el; i++ )
        {
            m_instances.push_back( m_instances.front( ) );  // insert copies of the first element
            m_free_pointers.push_back( &m_instances.back() );
        }
        m_avail += num_el;
    }

    /// Get a free element from the pool
    T* pop( )
    {
        if( m_avail == 0 )
            expand( m_size_increment );

        T* ptr = m_free_pointers.front( );
        m_free_pointers.pop_front( );
        m_avail--;
        return ptr;
    }

    /// Get a free element from the pool and initialize with src
    /*T* pop( const T &src )
    {
        if( m_avail == 0 )
            expand( m_size_increment );

        T* ptr = m_free_pointers.front( );
        m_free_pointers.pop_front( );
        m_avail--;

        *ptr = src;

        return ptr;
    }*/

    void push( T* ptr )
    {
        m_free_pointers.push_back( ptr );
        m_avail++;
    }

private:
    size_t m_size_increment;
    size_t m_avail;

    std::list<T> m_instances;
    std::list<T*> m_free_pointers;
};

#endif // POINTERPOOL_H
