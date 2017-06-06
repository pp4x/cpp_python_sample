
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
#include <vector>

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

struct CBar
{

	void print(Foo * foo)
	{
		std::cout << "back to C++: ";
		foo->print_foo();
	}
};

struct Point
{

	static object define_class()
	{
		class_<Point> pointClass("Point", init<int, int>());
		pointClass.def_readwrite("x", &Point::x)
			.def_readwrite("y", &Point::y);
		return pointClass;
	}

	int x = 0;
	int y = 0;

	Point(int x, int y) : x(x), y(y)
	{
	}
};

template<class X>
object wrap_vector(const std::string & name)
{
	using vec_t = std::vector<X>;
	class_<vec_t> clazz(name.c_str(), no_init);

	auto begin = (typename vec_t::iterator(vec_t::*)()) & vec_t::begin;
	auto end = (typename vec_t::iterator(vec_t::*)()) & vec_t::end;

	clazz.def("__iter__", range(begin, end));

	return clazz;
}

/*
 * 
 */
int main(int, char** argv)
{
	Foo foo("natively constructed!");
	std::vector<int> ints;
	std::vector<Point> points;
	for (int x = 0; x < 10; ++x)
	{
		ints.emplace_back(x);
		points.emplace_back(x, x);
	}
	try
	{

		auto base_python_path = getenv("PYTHONPATH");
		std::string python_path = std::string(".:") + std::string(base_python_path ? base_python_path : "");
		setenv("PYTHONPATH", python_path.c_str(), true);

		// initialise python interpreter.
		Py_SetProgramName(argv[0]);
		Py_Initialize();

		// define wrapper classes.
		class_<Foo> fooClass("Foo", init<std::string>());
		fooClass.def("print_foo", &Foo::print_foo);

		class_<CBar> cbarClass("CBar", init<>());
		cbarClass.def("printf", &CBar::print);

		// initialise objects.
		object main = import("__main__");
		object main_ns = main.attr("__dict__");
		main_ns["Foo"] = fooClass;
		main_ns["bar"] = import("bar");
		main_ns["foow"] = foo;
		main_ns["CBar"] = cbarClass;
		main_ns["IntVec"] = wrap_vector<int>("IntVec");
		main_ns["ints"] = &ints;
		main_ns["Point"] = Point::define_class();
		main_ns["PointVec"] = wrap_vector<Point>("PointVec");
		main_ns["points"] = &points;

		// instantiate some objects and call them from python side.
		exec(
			"print 'instatiation of objects.'\n"
			"bar1 = bar.Bar(Foo('Hello C++ Object!'))\n"
			"bar2 = bar.Bar(foow)\n"
			"print 'call some functions passing differently constructed objects.'\n"
			"bar2.print_bar()\n"
			"bar3 = CBar()\n"
			"bar3.printf(bar1.foo())\n"
			"bar3.printf(foow)\n"
			"print 'vector<int> iteration.'\n"
			"for i in ints:\n"
			" print i\n"
			"print 'vector<Point> iteration.'\n"
			"for p in points:\n"
			" print p.x, p.y\n"
			, main_ns);

		// call method from native side.
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

