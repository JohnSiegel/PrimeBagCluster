#pragma once

#include <vector>

typedef unsigned long long ulong;
typedef unsigned int uint;

/**
	This helper function is used to mark off composite numbers from a bit field given a
	prime number and the range of numbers to iterate through.
*/
static void countMultiplesInBitField(std::vector<bool>& bitField, uint prime, uint min, uint max)
{
	/*
		We only need to loop from prime ^ 2 to the max bound.
		All other multiples will be found from other primes.
	*/
	ulong square = ulong(prime) * prime;

	for (ulong multiple = square; multiple <= max; multiple += prime)
	{
		if (multiple >= min)
		{
			/*
				Mark the multiples as composite.
			*/
			bitField[multiple - min] = false;
		}
	}
};

/**
	This class is responsible for generating prime numbers efficiently at runtime.
	The algorithm used is the Segmented Sieve of Eratosthenes. The space complexity
	of a SieveOfEratosthenes object is O(N) where N = the number of calculated prime
	numbers.
*/
class SieveOfEratosthenes
{
public:

	/**
		This constructor takes in an optional pointer to an ordered vector of prime 
		numbers. If you input a pointer to a vector containing composite numbers, the 
		entire sieve will become invalid. If you do not input a prime number vector, 
		then the sieve will start with no initial prime numbers and will start counting 
		from 2.
	*/
	SieveOfEratosthenes(const std::vector<uint>* primeNumbers);

	/**
		This method will return the prime number at a specified index starting at 0.
		If the prime number at that index is not yet calculated, the sieve will begin 
		calculating it. For large numbers this method can take a long time to calculate. 
		If the number at the given index is already calculated, this runs in constant time.
	*/
	uint getPrimeNumber(size_t index);

	/**
		Returns the vector containing all currently calculated prime numbers.
	*/
	const std::vector<uint>& getCalculatedPrimes() const;

	/**
		Returns the number of prime numbers that have been calculated.
	*/
	size_t getNumCalculatedPrimes() const;

private:

	/**
		This function runs Segmented Sieve of Eratosthenes until there are at least
		numPrimes calculated prime numbers.
		
		With N = the highest currently calculated prime number:
		-------------------------------------------------------
		Time complexity of calculation:
		O(N * log(log(N)))

		With K = the upper limit of the segment range:
		-------------------------------------------------------
		Space complexity of calculation:
		O(sqrt(K))
	*/
	void sieve(size_t numPrimes);

	/**
		This is the highest number that has been definitively tested for primality.
	*/
	uint highestTestedNum;

	/**
		This is the number is used as the upper limit of the segment range.
	*/
	uint sieveLimit;

	/**
		This vector contains all of the calculated prime numbers in order.
	*/
	std::vector<uint> primes;
};