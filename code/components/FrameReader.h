#pragma once

#include "quantiser/QuantisedFrame.h"

class FrameReader {
public:
    virtual ~FrameReader() {};

    virtual void start() = 0;
    virtual bool next_frame(QuantisedFrame & qframe) = 0;
    virtual void end() = 0;
};
