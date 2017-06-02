
/* 
 * File:   main.cc
 * Author: paulo
 *
 * Created on 15 May 2017, 18:58
 */

#include <Python.h>

#include <boost/python.hpp>
#include <cstdlib>
#include <string>
#include <iostream>

using namespace std;
using namespace boost::python;

struct Foo
{
	const std::string foo;

	Foo(std::string f) : foo(f)
	{
	}

	void print_foo() const
	{
		std::cout << foo << std::endl;
		std::cout.flush();
	}
};

struct FooWrapper
{
	FooWrapper(const Foo * foo): 
	foo(foo) 
	{
	}
	
	void print_foo() const
	{
		foo->print_foo();
	}
	
	const Foo * foo;
};

/*
 * 
 */
int main(int, char** argv)
{
	Foo foo("natively constructed!");
	try
	{

		auto base_python_path = getenv("PYTHONPATH");
		std::string python_path = std::string(".:") + std::string(base_python_path ? base_python_path : "");
		setenv("PYTHONPATH", python_path.c_str(), true);

		Py_SetProgramName(argv[0]);
		Py_Initialize();

		class_<Foo> fooClass("Foo", init<std::string>());
		fooClass.def("print_foo", &Foo::print_foo);
		
		class_<FooWrapper> fooWrapper("FooWrapper", init<Foo*>());
		fooWrapper.def("print_foo", &FooWrapper::print_foo);
		

		object main = import("__main__");
		object main_ns = main.attr("__dict__");
		main_ns["Foo"] = fooClass;
		main_ns["FooW"] = fooWrapper;
		main_ns["bar"] = import("bar");
		main_ns["foow"] = fooWrapper(foo);
		exec("bar1 = bar.Bar(Foo('Hello C++ Object!'))\n"
			 "bar2 = bar.Bar(foow)\n"
			 "bar2.print_bar()"
			,main_ns);
		object bar1 = main_ns["bar1"];
		object print_bar = bar1.attr("print_bar");
		print_bar();
	}
	catch (const error_already_set & e)
	{
		PyErr_Print();
	}
	catch (const std::exception & e)
	{
		std::cerr << "Err: " << e.what() << std::endl;
	}

	Py_Finalize();
	return 0;
}

