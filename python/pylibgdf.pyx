# distutils: language = c++
# distutils: libraries = tid tobicore tobiid boost_thread SDL

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp cimport bool

class WriterFlags:
    writer_ev_file = 0
    writer_ev_memory = 1
    writer_overwrite = 2

# GDF::Writer - C++ interface
cdef extern from "GDF/Writer.h" namespace "gdf":
    cdef cppclass Writer:
        Writer( ) except +
        void open( int ) except +
        void open( string, int ) except +
        void close( ) except +
        bool isOpen( )
        void setFilename( string ) except +
        void setMaxFullRecords( size_t ) except +
        bool createSignal( size_t, bool throwexc ) except +
        #void swapSignals( size_t, size_t )
        #void relocateSignal( size_t, size_t )
        size_t getFirstFreeSignalIndex( ) except +
        #void blitFromSerialBufferPhys( double*, vector[size_t] )
        void addSamplePhys( size_t, double )
        #template<typename T> void addSampleRaw( const size_t channel_idx, const T value )
        #void blitSamplesPhys( const size_t channel_idx, const float64 *values, size_t num )
        void blitSamplesPhys( size_t, vector[double] values )
        #template<typename T> void blitSamplesRaw( const size_t channel_idx, const T *values, size_t num )
        #template<typename T> void blitSamplesRaw( const size_t channel_idx, const std::vector<T> &values )
        #void addRecord( Record *r )
        #Record *acquireRecord( )
        #void writeRecordDirect( Record *r )
        void flush( )
        void setEventMode( unsigned char mode ) except +
        void setEventSamplingRate( float ) except +
        #void addEvent( const Mode1Event &ev );
        void addEvent( unsigned int, unsigned short )
        #void addEvent( const Mode3Event &ev );
        void addEvent( unsigned int, unsigned short, unsigned short, unsigned int )
        #void addEvent( uint32 position, uint16 type, uint16 channel, float32 value );
        #const GDFHeaderAccess &getHeaderAccess_readonly( ) const 
        #GDFHeaderAccess &getHeaderAccess( ) 
        #const MainHeader &getMainHeader_readonly( ) const
        #MainHeader &getMainHeader( )
        #const SignalHeader &getSignalHeader_readonly( size_t idx ) const 
        #inline size_t getNumSignals( ) const
        #SignalHeader &getSignalHeader( size_t idx )
        
# GDF::Writer - Python wrapper
cdef class GDFWriter:
    cdef Writer *thisptr
    def __cinit__( self ):        
        self.thisptr = new Writer( )
    def __dealloc__( self ):
        del self.thisptr
    def open( self, flags=WriterFlags.writer_ev_file ):
        self.thisptr.open( flags )
    def isOpen(self):
        return self.thisptr.isOpen( )
    def setFilename(self, filename):
        self.thisptr.setFilename( filename )
    def setMaxFullRecords(self, number):
        self.thisptr.setMaxFullRecords( number )
    def createSignal(self, index, throwexc):
        self.thisptr.createSignal( index, throwexc )
    def getFirstFreeSignalIndex(self):    
        return self.thisptr.getFirstFreeSignalIndex( )
    def addSample( self, channel_idx, value ):
        self.thisptr.addSamplePhys( channel_idx, value )
    def blitSamples( self, channel_idx, values ):
        self.thisptr.blitSamplesPhys( channel_idx, values )
    def flush(self):
        self.thisptr.flush()
    def setEventMode( self, mode ):
        self.thisptr.setEventMode( mode )
    def setEventSamplingRate( self, fs = -1 ):
        self.thisptr.setEventSamplingRate( fs )
    def addEvent1(self, position, code):
        self.thisptr.addEvent( position, code )
    def addEvent3( self, position, type, channel, duration ):
        self.thisptr.addEvent( position, type, channel, duration )
    
    
    
    
    #EOF
