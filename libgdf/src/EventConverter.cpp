//
// This file is part of libGDF.
//
// libGDF is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// libGDF is distributed in the hope that it will be useful,
// but WITHout ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with libGDF.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2010 Christoph Eibel

#include "GDF/EventConverter.h"
#include "GDF/Types.h"

#include <map>
#include <vector>
#include <algorithm>

using namespace std;

namespace gdf
{
    typedef map<uint16, vector<Mode1Event> > TypeEventMap;

    uint16 const STOP_FLAG = 0x8000;
    uint16 const NOT_STOP_FLAG = 0x7FFF;

    //-------------------------------------------------------------------------
    vector<Mode3Event> convertMode1EventsIntoMode3Events (vector<Mode1Event> const& mode_1_events)
            throw (exception::general)
    {
        vector<Mode3Event> mode_3_events;

        TypeEventMap type_event_map;

        for (size_t index = 0; index < mode_1_events.size (); index++)
        {
            Mode1Event const& event_1 = mode_1_events[index];
            type_event_map[event_1.type & NOT_STOP_FLAG].push_back (event_1);
        }

        for (TypeEventMap::iterator iter = type_event_map.begin ();
             iter != type_event_map.end (); ++iter)
        {
            sort (iter->second.begin (), iter->second.end ());
            for (unsigned index = 0; index < iter->second.size (); index++)
            {
                if (iter->second[index].type & STOP_FLAG)
                    throw exception::general ("events could not be converted from mode 1 to mode 3");
                //    throw exception::general ("events of same type are overlapping, not supported for mode 1 events");

                Mode3Event event_3;
                event_3.channel = 0;                             // ALL CHANNELS
                event_3.type = iter->first;
                event_3.position = iter->second[index].position;

                if (iter->second[index+1].type == (iter->first | STOP_FLAG))
                {
                    event_3.duration = iter->second[index + 1].position - event_3.position;
                    index++;
                }
                else
                    event_3.duration = 0;

                mode_3_events.push_back (event_3);
            }
        }

        return mode_3_events;
    }
}
