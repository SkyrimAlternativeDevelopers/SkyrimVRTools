#pragma once

class IHookInterfaceFactory
{
public:
	virtual void* Build(void* realInterface) = 0;
	virtual std::string GetWrappedVersion() = 0;
};