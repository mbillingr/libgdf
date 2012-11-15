
#ifndef __RECORDFULLHANDLER_H_INCLUDED__
#define __RECORDFULLHANDLER_H_INCLUDED__

namespace gdf
{
 class Record;
 
 class RecordFullHandler
    {
    public:
        /// Constructor
        RecordFullHandler( ) { }
        /// Destructor
        virtual ~RecordFullHandler( ) { }
        virtual void triggerRecordFull( Record *rec ) = 0;
    };
}

#endif
