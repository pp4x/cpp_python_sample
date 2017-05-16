
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
	}
};

/*
 * 
 */
int main(int, char** argv)
{
	try
	{
		auto base_python_path = getenv("PYTHONPATH");
		std::string python_path = std::string(".:") + std::string(base_python_path ? base_python_path : "");
		setenv("PYTHONPATH", python_path.c_str(), true);

		Py_SetProgramName(argv[0]);
		Py_Initialize();

		class_<Foo> wFoo("Foo", init<std::string>());
		wFoo.def("print_foo", &Foo::print_foo);

		object foo_test = import("footime");
		object main = import("__main__");
		object main_ns = main.attr("__dict__");
		main_ns["Foo"] = wFoo;
		main_ns["footime"] = foo_test;
		exec("foo = footime.FooTime(Foo('Hello C++ Object!'))\n", main_ns);
		object foo = main_ns["foo"];
		object print_foo = foo.attr("print_foo");
		print_foo();
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

