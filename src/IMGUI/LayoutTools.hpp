#pragma once

struct PositionProvider
{
    ~PositionProvider() = default;
};

struct LVertical : public PositionProvider
{};

struct LHorizontal : public PositionProvider
{};



struct SizeProvider
{
    ~SizeProvider() = default;
};
