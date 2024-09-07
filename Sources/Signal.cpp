#include "Signal.hpp"

Core::Signal::Signal() : value(false)
{
}

Core::Signal::Signal(bool defaultValue) : value(defaultValue)
{
}

Core::Signal::~Signal()
{
}

bool Core::Signal::Store(bool valueIn)
{
	value.store(valueIn);
	return valueIn;
}

bool Core::Signal::Load() const
{
	return value.load();
}
