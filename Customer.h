#ifndef __CUSTOMER_H__
#define __CUSTOMER_H__

#include "adevs.h"
/// A Busy-Mart customer.
struct Customer
{
	/// Time needed for the clerk to process the customer
	double twait;
	/// Time that the customer entered and left the queue
	double tenter, tleave;
};
/// Create an abbreviation for the Clerk’s input/output type.
typedef adevs::PortValue<Customer*> IO_Type;

#endif
