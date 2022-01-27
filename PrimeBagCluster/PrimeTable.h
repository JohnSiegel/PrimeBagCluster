#pragma once


#include "PrimeTable.h"
#include "SieveOfEratosthenes.h"
#include <unordered_map>
#include <queue>
#include <future>
#include <boost/multiprecision/cpp_int.hpp>

/**
	PrimeTable objects are used to assign unique prime numbers to values. The
	underlying data structure is a hash map with type parameter V being the key 
	type and the corresponding values are unsigned prime ints.

	The space complexity of a PrimeTable is O(N) where N = the number of unique
	values added to the table.
*/
template <typename V>
class PrimeTable
{
	typedef boost::multiprecision::cpp_int bignum;
	typedef unsigned long long ulong;
	typedef unsigned int uint;

public:
	/**
		This constructor initializes the Sieve of Eratosthenes with an optional
		pointer to a vector of prime numbers.
	*/
	PrimeTable(const std::vector<uint>* primeNumbers = nullptr)
		: sieveOfEratosthenes(primeNumbers)
	{
	}

	/**
		This method adds a value to the table and assigns it a unique prime number.
		Returns the prime number associated with the given value.
	*/
	uint add(const V& value)
	{
		uint prime{ 0 };

		const auto& iter = primeMap.find(value);

		/*
			If the number is not already contained in the table
		*/
		if (iter == primeMap.end())
		{
			/*
				If there are prime numbers in the primeHoles queue, those should be
				prioritized to improve efficiency.
			*/
			if (primeHoles.size())
			{
				prime = primeHoles.top();
				primeHoles.pop();
			}
			else
			{
				/*
					If there is a precalculated prime number use that
				*/
				if (nextPrime.valid())
				{
					prime = nextPrime.get();
				}
				/*
					If not, calculate the next number. This usually only has to be once on the first insertion.
				*/
				else
				{
					prime = sieveOfEratosthenes.getPrimeNumber(primeMap.size());
				}

				/*
					Precalculate the next prime number.
				*/
				nextPrime = std::async(std::launch::async, &SieveOfEratosthenes::getPrimeNumber, &sieveOfEratosthenes, primeMap.size() + 1);
			}
			
			/*
				Insert the value and prime into the map
			*/
			primeMap[value] = prime;
			reversePrimeMap[prime] = value;
		}
		else
		{
			/*
				If the value is already contained, return the prime associated.
			*/
			prime = iter->second;
		}

		return prime;
	}

	/**
		Returns the prime number associated with a value. Returns 0 if it could not be found.
	*/
	uint getPrime(const V& value) const
	{
		const auto& iter = primeMap.find(value);

		if (iter != primeMap.end())
		{
			return iter->second;
		}
		else
		{
			return 0;
		}
	}

	/**
		This method removes a value from the prime table. Returns 0 if the
		value is not contained in the table, and returns the associated prime number
		if the value was successfully removed. This could potentially run in O(N) 
		time where N = the number of prime numbers that have been erased from the map
		without being reassigned.
	*/
	uint remove(const V& value)
	{
		const auto& iter = primeMap.find(value);

		if (iter != primeMap.end())
		{
			uint prime = iter->second;

			primeHoles.push(prime);

			primeMap.erase(value);
			reversePrimeMap.erase(prime);

			return prime;
		}

		return 0;
	}

	/**
		This function clears the entire table.
	*/
	void clear()
	{
		/*
			Join the worker thread.
		*/
		nextPrime.get();
		
		/*
			Clear the hash maps.
		*/
		primeMap.clear();
		reversePrimeMap.clear();

		/*
			Re initialize the prime holes queue.
		*/
		primeHoles;
	}

	/**
		Returns the map of values to their primes.
	*/
	const std::unordered_map<V, uint>& getPrimeMap() const
	{
		return primeMap;
	}

	/**
		Returns whether or not a prime number has been assigned to a 
		value in this table.
	*/
	bool containsPrime(uint prime) const
	{
		const auto& iter = reversePrimeMap.find(prime);

		return iter != reversePrimeMap.end();
	}

	/**
		Returns the value associated with a given prime number. This
		will throw an error if the given prime is not assigned to a value.
	*/
	const V& getValue(uint prime) const
	{
		return reversePrimeMap.at(prime);
	}

	/**
		Returns a list of all calculated prime numbers.
	*/
	const std::vector<uint>& getPrimeNumbers() const
	{
		return sieveOfEratosthenes.getCalculatedPrimes();
	}
private:
	/**
		This is task calculates the next prime number to be added ahead of time.
	*/
	std::future<uint> nextPrime;

	/**
		The sieve of eratosthenes is used to calculate primes at runtime.
	*/
	SieveOfEratosthenes sieveOfEratosthenes;

	/**
		The prime holes vector is used to store the primes that were once
		occupied but have since been deleted.
	*/
	std::priority_queue<uint> primeHoles;

	/**
		This map is used to assign a unique prime number to each value.
	*/
	std::unordered_map<V, uint> primeMap;

	/*
		This map is used to lookup a value by its prime.
	*/
	std::unordered_map<uint, V> reversePrimeMap;
};