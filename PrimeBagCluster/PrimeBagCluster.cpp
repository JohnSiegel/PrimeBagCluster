#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE

#include "PrimeBagCluster.h"

#include <iostream>
#include <string>

int main()
{
	PrimeTable<std::string> tab;
	PrimeBag<std::string> bag(&tab);
	PrimeBag<std::string> bag1(&tab);
	
	

	return 0;
}