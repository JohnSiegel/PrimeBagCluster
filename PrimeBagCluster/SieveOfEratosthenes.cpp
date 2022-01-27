#include "SieveOfEratosthenes.h"

typedef unsigned int uint;
typedef unsigned long long ulong;

SieveOfEratosthenes::SieveOfEratosthenes(const std::vector<uint>* primeNumbers = nullptr)
{
	/*
		We know 0 and 1 are composite numbers. These can be considered tested.
	*/
	highestTestedNum = 1;
	sieveLimit = 1;

	/*
		If a pointer to a prime number vector has been provided, iterate through it
	*/
	if (primeNumbers)
	{
		for (uint i : *primeNumbers)
		{
			/*
				Copy each prime number.
			*/
			primes.push_back(i);
		}

		/*
			We have tested all numbers up the final number in the primes vector.
		*/
		highestTestedNum = primes.back();
		sieveLimit = highestTestedNum;
	}
}

const std::vector<uint>& SieveOfEratosthenes::getCalculatedPrimes() const
{
	return primes;
}

size_t SieveOfEratosthenes::getNumCalculatedPrimes() const
{
	return primes.size();
}

uint SieveOfEratosthenes::getPrimeNumber(size_t index)
{
	sieve(index + 1);

	return primes[index];
}

void SieveOfEratosthenes::sieve(size_t numPrimes)
{
	/*
		Run until all the necessary primes are calculated.
	*/
	while (primes.size() < numPrimes)
	{
		/*
			Lower bound of this segment.
		*/
		uint min = highestTestedNum + 1;

		/*
			Update the limit on this sieve of eratosthenes if needed.
		*/
		while (sieveLimit <= min)
		{
			sieveLimit *= 2;
		}

		/*
			Dimensions of the bit field
		*/
		uint root = uint(sqrt(sieveLimit)), max = std::min(sieveLimit, min + root);

		/*
			This bit field is used to represent the primality of each number
			in this segment. Initially, all numbers will be considered prime.
			The composite numbers will be sieved out.
		*/
		std::vector<bool> bitField(max - min + 1);
		std::fill(bitField.begin(), bitField.end(), true);

		/*
			Sieve out the multiples of each calculated prime number.
		*/
		for (uint prime : primes)
		{
			countMultiplesInBitField(bitField, prime, min, max);
		}

		for (uint index = 0; index < bitField.size(); index++)
		{
			if (bitField[index])
			{
				/*
					Sieve out the mulitples of this prime number and then push
					it onto the back of the prime number vector.
				*/
				uint prime = index + min;
				countMultiplesInBitField(bitField, prime, min, max);
				primes.push_back(prime);
			}
		}

		/*
			We have now tested up to the upper bound of this segment.
		*/
		highestTestedNum = max;
	}
}