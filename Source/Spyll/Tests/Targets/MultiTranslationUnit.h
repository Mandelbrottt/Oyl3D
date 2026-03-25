#pragma once

#define __REFLECT__ [[clang::annotate("reflect")]]

class __REFLECT__ Base
{
public:
	int foo;

	void
	Bar();
};

class __REFLECT__ Derived : public Base
{
public:
	int foz;

	void
	Baz();
};


