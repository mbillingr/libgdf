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
// Copyright 2010 Christoph Eibel

#ifndef __EVENTCONVERTER_H_INCLUDED__
#define __EVENTCONVERTER_H_INCLUDED__

#include "EventHeader.h"
#include "Exceptions.h"

#include <vector>

namespace gdf
{
    //-------------------------------------------------------------------------
    /// @brief converts a vector of mode 1 events into mode 3 events
    ///        using the flag 0x8000 which marks the type of mode 1 stop events
    /// @throws general if events could not be converted
    std::vector<Mode3Event> convertMode1EventsIntoMode3Events (std::vector<Mode1Event>
                                                               const& mode_1_events)
        throw (exception::general);
}

#endif
