#pragma once

#include "quantiser/QuantisedFrame.h"

class FrameWriter {
public:
    virtual ~FrameWriter() {};

    virtual void start(int atoms, int frames, int ISTART = 0, int NSAVC = 1, double DELTA = 0.0) = 0;
    virtual void next_frame(const QuantisedFrame & qframe) = 0;
    virtual void end() = 0;
};
