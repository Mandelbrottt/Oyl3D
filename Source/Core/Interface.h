#pragma once

class Interface
{
protected:
	Interface() = default;

public:
	virtual ~Interface() = default;

	virtual void Foo() = 0;
};