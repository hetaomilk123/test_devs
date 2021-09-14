#ifndef __CLERK_H__
#define __CLERK_H__

#include "adevs.h"
#include "Customer.h"
#include <list>
/**
* The Clerk class is derived from the adevs Atomic class.
* The Clerk’s input/output type is specified using the template
* parameter of the base class.
*/

using namespace adevs;

// adevs::Atomic<adevs::PortValue<Customer*> >
class Clerk: public adevs::Atomic<IO_Type>
{
public:
	/// Constructor.
	Clerk() {}
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
	~Clerk() {}
	/// Model input port.
	static const int arrive = 0;
	/// Model output port.
	static const int depart = 1;
private:
	/// The clerk’s clock
	double t;
	/// List of waiting customers.
	std::list<Customer*> line;
	/// Time spent so far on the customer at the front of the line
	double t_spent;
};

// 触发条件：The arrival of a customer
void Clerk::delta_ext(double e, const Bag<IO_Type>& xb)
{
	// Print a notice of the external transition
	cout << "Clerk: Computed the external transition function at t = " << t+e << endl;
	// Update the clock
	t += e;
	// Update the time spent on the customer at the front of the line
	if (!line.empty())
	{
		t_spent += e;
	}
	// Add the new customers to the back of the line.
	Bag<IO_Type>::const_iterator i = xb.begin();
	for (; i != xb.end(); i++)
	{
		// Copy the incoming Customer and place it at the back of the line.
		line.push_back(new Customer(*((*i).value)));
		// Record the time at which the customer entered the line.
		line.back()->tenter = t;
	}
	// Summarize the model state
	cout << "Clerk: There are " << line.size() << " customers waiting." << endl;
	cout << "Clerk: The next customer will leave at t = " << t+ta() << "." << endl;
}

double Clerk::ta()
{
	// If the list is empty, then next event is at inf
	if (line.empty()) return DBL_MAX;
	// Otherwise, return the time remaining to process the current customer
	return line.front()->twait-t_spent;
}

// 触发条件：when the time advance expires
void Clerk::output_func(Bag<IO_Type>& yb)
{
	// Get the departing customer
	Customer* leaving = line.front();
	// Set the departure time
	leaving->tleave = t + ta();
	// Eject the customer
	IO_Type y(depart,leaving);
	yb.insert(y);
	// Print a notice of the departure
	cout << "Clerk: Computed the output function at t = " << t+ta() << endl;
	cout << "Clerk: A customer just departed!" << endl;
}

// 触发条件：which is called immediately after the output func method
void Clerk::delta_int()
{
	// Print a notice of the internal transition
	cout << "Clerk: Computed the internal transition function at t = " << t+ta() << endl;
	// Update the clock
	t += ta();
	// Reset the spent time
	t_spent = 0.0;
	// Remove the departing customer from the front of the line.
	line.pop_front();
	// Summarize the model state
	cout << "Clerk: There are " << line.size() << " customers waiting." << endl;
	cout << "Clerk: The next customer will leave at t = " << t+ta() << "." << endl;
}

void Clerk::gc_output(Bag<IO_Type>& g)
{
	// Delete the customer that was produced as output
	Bag<IO_Type>::iterator i;
	for (i = g.begin(); i != g.end(); i++)
	{
		delete (*i).value;
	}
}

void Clerk::delta_conf(const Bag<IO_Type>& xb)
{
	delta_int();
	delta_ext(0.0,xb);
}

#endif
