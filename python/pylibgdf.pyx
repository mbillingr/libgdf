# distutils: language = c++
# distutils: libraries = tid tobicore tobiid boost_thread SDL

from libcpp.vector cimport vector
from libcpp.string cimport string
from libcpp cimport bool
import cython

class WriterFlags:
    writer_ev_file = 0
    writer_ev_memory = 1
    writer_overwrite = 2
    
class Datatypes:
    s2n = {'invalid':0, 'int8':1, 'uint8':2, 'int16':3, 'uint16':4, 'int32':5, 'uint32':6, 'int64':7, 'uint64':8, 'float32':16, 'float64':17}
    n2s = dict((v,k) for k,v in Datatypes.s2n.iteritems())
    
string_encoding = 'ascii'

ctypedef fused numeric:
    cython.int
    cython.uint
    
# =================================================================================
    
# GDF::SignalHeader - C++ Interface
cdef extern from "GDF/SignalHeader.h" namespace "gdf":
    cdef cppclass SignalHeader:
        SignalHeader( )        
        void set_label( string )
        string get_label( )
        void set_transducer_type( string )
        string get_transducer_type( )
        void set_physical_dimension( string )
        string get_physical_dimension( )
        void set_physical_dimension_code( unsigned short )
        unsigned short get_physical_dimension_code( )
        void set_physmin( double )
        double get_physmin( )
        void set_physmax( double )
        double get_physmax( )
        void set_digmin( double )
        double get_digmin( )
        void set_digmax( double )
        double get_digmax( )
        void set_lowpass( float )
        float get_lowpass( )
        void set_highpass( float )
        float get_highpass( )        
        void set_notch( float )
        float get_notch( )
        void set_datatype( unsigned int )
        unsigned int get_datatype( )
        void set_sensor_pos( float )
        float get_sensor_pos( )
        void set_sensor_info( float )
        float get_sensor_info( )
        void set_samplerate( float )
        float get_samplerate( )
        
# =================================================================================

# GDF::GDFHeaderAccess - C++ Interface
cdef extern from "GDF/GDFHeaderAccess.h" namespace "gdf":
    ctypedef GDFHeaderAccess* const_GDFHeaderAccess_ptr "const gdf::GDFHeaderAccess*"
    cdef cppclass GDFHeaderAccess:
        size_t getNumSignals()
        SignalHeader& getSignalHeader( size_t )
        SignalHeader& getSignalHeader_readonly( size_t )
        SignalHeader& getMainHeader( size_t )
        SignalHeader& getMainHeader_readonly( size_t )
        vector[unsigned short] getSignalIndices( )

# HeaderWrapper - Python Class
cdef class HeaderWrapper:
    '''class that provides access to GDF header information'''
    cdef GDFHeaderAccess *header_w  # for writing
    cdef const_GDFHeaderAccess_ptr header_r  # for reading
    def get_num_signals(self):
        return self.header_r.getNumSignals()
               
    def set_label(self, idx, label):
        self.header_w.getSignalHeader(idx).set_label( label.encode(string_encoding) )
    def get_label(self, idx):
        return self.header_r.getSignalHeader_readonly(idx).get_label( ).strip().decode(string_encoding)
    def get_labels(self):
        indices = self.header_r.getSignalIndices()
        return [self.get_label(i) for i in indices]
        
    def set_datatype(self, idx, type):
        self.header_w.getSignalHeader(idx).set_datatype( Datatypes.s2n[type] )
    def get_datatype(self, idx):
        return Datatypes.n2s[self.header_r.getSignalHeader_readonly(idx).get_datatype()]
    def get_datatypes(self):
        indices = self.header_r.getSignalIndices()
        return [self.get_datatype(i) for i in indices]
        

# =================================================================================

# GDF::Writer - C++ interface
cdef extern from "GDF/Writer.h" namespace "gdf":
    cdef cppclass Writer:
        Writer( ) except +
        void open( int ) except +
        #void open( string, int ) except +
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
        GDFHeaderAccess &getHeaderAccess_readonly( ) 
        GDFHeaderAccess &getHeaderAccess( ) 
        #const MainHeader &getMainHeader_readonly( ) const
        #MainHeader &getMainHeader( )
        #const SignalHeader &getSignalHeader_readonly( size_t idx ) const 
        #inline size_t getNumSignals( ) const
        #SignalHeader &getSignalHeader( size_t )
        
# GDF::Writer - Python wrapper
cdef class GDFWriter(HeaderWrapper):
    cdef Writer *thisptr
    def __cinit__( self ):        
        self.thisptr = new Writer( )
        self.header_w = &self.thisptr.getHeaderAccess()
        self.header_r = &self.thisptr.getHeaderAccess_readonly()
    def __dealloc__( self ):
        del self.thisptr
    def open( self, flags=WriterFlags.writer_ev_file ):
        self.thisptr.open( flags )
    def isOpen(self):
        return self.thisptr.isOpen( )
    def setFilename(self, filename):
        self.thisptr.setFilename( filename.encode(string_encoding) )
    def setMaxFullRecords(self, number):
        self.thisptr.setMaxFullRecords( number )
    def createSignal(self, index, throwexc = True):
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
    
# =================================================================================
    
    
    #EOF
