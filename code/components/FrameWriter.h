#pragma once

#include "quantiser/QuantisedFrame.h"

class FrameWriter {
public:
    virtual ~FrameWriter() = 0;

    virtual void next_frame(const QuantisedFrame & qframe) = 0;
};
