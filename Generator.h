#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "adevs.h"
#include "Customer.h"
#include <list>
#include <fstream>
/**
* This class produces Customers according to the provided schedule.
*/
class Generator: public adevs::Atomic<IO_Type>
{
public:
	/// Constructor.
	Generator() {}
	Generator(const char* data_file);
	/// Internal transition function.
	void delta_int();
	/// External transition function.
	void delta_ext(double e, const adevs::Bag<IO_Type>& xb);
	/// Confluent transition function.
	void delta_conf(const adevs::Bag<IO_Type>& xb);
	/// Output function.
	void output_func(adevs::Bag<IO_Type>& yb);
	/// Time advance function.
	double ta();
	/// Output value garbage collection.
	void gc_output(adevs::Bag<IO_Type>& g);
	/// Destructor.
	~Generator() {}
	/// Model output port.
	static const int arrive;
private:
	/// List of arriving customers.
	std::list<Customer*> arrivals;
};

// Assign a locally unique number to the arrival port
const int Generator::arrive = 0;
Generator::Generator(const char* sched_file):
Atomic<IO_Type>()
{
	// Open the file containing the schedule
	fstream input_strm(sched_file);
	// Store the arrivals in a list
	double next_arrival_time = 0.0;
	double last_arrival_time = 0.0;
	while (true)
	{
		Customer* customer = new Customer;
		input_strm >> next_arrival_time >> customer->twait;
		// Check for end of file
		if (input_strm.eof())
		{
			delete customer;
			break;
		}
		// The entry time holds the inter arrival times, not the
		// absolute entry time.
		customer->tenter = next_arrival_time-last_arrival_time;
		// Put the customer at the back of the line
		arrivals.push_back(customer);
		last_arrival_time = next_arrival_time;
	}
}

void Generator::delta_ext(double e, const Bag<IO_Type>& xb)
{
	/// The generator is input free, and so it ignores external events.
}

void Generator::delta_conf(const Bag<IO_Type>& xb)
{
	/// The generator is input free, and so it ignores input.
	delta_int();
}

void Generator::output_func(Bag<IO_Type>& yb)
{
	// First customer in the list is produced as output
	IO_Type output(arrive,arrivals.front());
	yb.insert(output);
}

void Generator::delta_int()
{
	// Remove the first customer. Because it was used as the
	// output object, it will be deleted during the gc_output()
	// method call at the end of the simulation cycle.
	arrivals.pop_front();
}

double Generator::ta()
{
	// If there are not more customers, next event time is infinity
	if (arrivals.empty()) return DBL_MAX;
	// Otherwise, wait until the next arrival
	return arrivals.front()->tenter;
}

void Generator::gc_output(Bag<IO_Type>& g)
{
	// Delete the customer that was produced as output
	Bag<IO_Type>::iterator i;
	for (i = g.begin(); i != g.end(); i++)
	{
		delete (*i).value;
	}
}

#endif
