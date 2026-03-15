
#pragma once

#include "TabularData.h"
#include "IOutputTarget.h"

class IOutputWriter
{
public:
    virtual ~IOutputWriter() = default;
    virtual void Write(const TabularData& data, const IOutputTarget& target) = 0;
};
