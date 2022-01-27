#pragma once

#include "PrimeTable.h"
#include <boost/multiprecision/cpp_int.hpp>

typedef boost::multiprecision::cpp_int bignum;
typedef unsigned long long ulong;
typedef unsigned int uint;

/**
	This helper function determines whether a given number is divisible 
	by another number. Used for determining whether one bag contains 
	another bag.
*/
static bool containsHash(const bignum& hash, const bignum& otherHash)
{
	return !(hash % otherHash);
};

/**
	This is an iterator 
*/
template<typename V>
class PrimeBagIterator;

template <typename V>
class PrimeBag
{
	typedef PrimeBagIterator<V> iterator;
	friend class iterator;

public:
	PrimeBag(PrimeTable<V>* table) : globalTable(table)
	{
	}

	iterator begin() const
	{
		return iterator(*this, false);
	}

	iterator end() const
	{
		return iterator(*this, true);
	}

	void add(const V& value)
	{
		uint prime = globalTable->add(value);

		hash *= prime;
		length++;
	}

	void add(const PrimeBag<V>& bag)
	{
		if (bag.globalTable == globalTable)
		{
			bignum& otherHash = bag.hash;

			hash *= otherHash;

			length += bag.length;
		}
	}

	bool remove(const PrimeBag<V>& bag)
	{
		if (bag.globalTable == globalTable)
		{
			if (bag.length <= length)
			{
				if (containsHash(hash, bag.hash))
				{
					hash /= bag.hash;
					length -= bag.length;

					return true;
				}
			}
		}

		return false;
	}

	bool remove(const V& value)
	{
		bignum prime = globalTable->getPrime(value);

		if (prime)
		{
			if (containsHash(hash, prime))
			{
				hash /= prime;
				length--;

				return true;
			}
		}

		return false;
	}

	void clear()
	{
		hash = 1;
		length = 0;
	}

	const PrimeBag<V>& operator&&(const PrimeBag<V>& bag)
	{

	}

	PrimeBag<V>& operator+(const V& value)
	{
		add(value);
		return *this;
	}

	PrimeBag<V>& operator+(const PrimeBag<V>& bag)
	{
		add(bag);
		return *this;
	}

	PrimeBag<V>& operator-(const V& value)
	{
		remove(value);
		return *this;
	}

	PrimeBag<V>& operator-(const PrimeBag<V>& bag)
	{
		remove(bag);
		return *this;
	}

	bool contains(const V& value) const
	{
		uint prime = globalTable->getPrime(value);

		return prime && containsHash(hash, bignum(prime));
		;
	}

	uint size() const
	{
		return length;
	}

	uint count(const V& value) const
	{
		uint prime = globalTable->getPrime(value), result = 0;

		if (prime)
		{
			bignum hashCopy = hash;

			while (containsHash(hashCopy, prime))
			{
				hashCopy /= prime;
				result++;
			}
		}

		return result;
	}

	std::vector<V> asVector() const
	{
		std::vector<V> result;
		bignum hashCopy = hash;
		uint counter = length;

		const std::vector<uint>& primes = globalTable->getPrimeNumbers();

		for (uint prime : primes)
		{
			if (counter > 0)
			{
				const V& value = globalTable->getValue(prime);
				bignum bigPrime = prime;

				while (containsHash(hashCopy, bigPrime))
				{
					hashCopy /= bigPrime;
					counter--;
					result.push_back(value);
				}
			}
			else
			{
				break;
			}
		}

		return result;
	}

public:
	PrimeTable<V>* globalTable;
	bignum hash{ 1 };
	uint length{ 0 };
};

template<typename V>
class PrimeBagIterator
{
public:
	/*
		This constructor will initialize the iterator at a given position for a given map.
	*/
	PrimeBagIterator(const PrimeBag<V>& bag, bool isEnd) : primeBagCopy(bag), refPrimeBag(bag)
	{
		primeTable = refPrimeBag.globalTable;

		if (!bag.length || isEnd)
		{
			end = true;
		}

		if (end)
		{
			primeBagCopy.clear();
			primeIndex = uint(primeTable->getPrimeNumbers().size() - 1);
		}
		else
		{
			++*this;
		}
	}

	const V& operator*() const
	{
		if (!end)
		{
			return primeTable->getValue(getPrimeAtTableIndex());
		}
		else
		{
			throw std::out_of_range("The end() iterator is a nullptr.");
		}
	}

	bool operator==(const PrimeBagIterator<V>& other) const
	{
		if (primeTable != other.primeTable)
		{
			return false;
		}

		return end == other.end && 
			primeBagCopy.length == other.primeBagCopy.length && 
			primeBagCopy.hash == other.primeBagCopy.hash;
	}

	bool operator!=(const PrimeBagIterator<V>& other) const
	{
		return !operator==(other);
	}

	PrimeBagIterator<V>& operator++()
	{
		if (!end)
		{
			if (primeBagCopy.length > 0)
			{
				uint prime = getNextPrimeFactor();
				primeBagCopy.hash /= prime;
				primeBagCopy.length--;
			}
			else
			{
				end = true;
			}
		}

		return *this;
	}

	PrimeBagIterator<V>& operator--()
	{
		uint prime = getPrimeAtTableIndex();

		if (primeBagCopy.length < refPrimeBag.length - 1)
		{
			if (end)
			{
				end = false;
			}
			else
			{
				primeBagCopy.hash = primeBagCopy.hash * prime;
				primeBagCopy.length++;
			}

			getPreviousPrimeFactor();
		}

		return *this;
	}

	bool operator<(const PrimeBagIterator<V>& other) const
	{
		return other.primeBagCopy.length < primeBagCopy.length;
	}

	bool operator>(const PrimeBagIterator<V>& other) const
	{
		return other.primeBagCopy.length > primeBagCopy.length;
	}

	bool operator>=(const PrimeBagIterator<V>& other) const
	{
		return operator>(other) || operator==(other);
	}

	bool operator<=(const PrimeBagIterator<V>& other) const
	{
		return operator<(other) || operator==(other);
	}

public:
	uint getPrimeAtTableIndex() const
	{
		const std::vector<uint>& primes = primeTable->getPrimeNumbers();
		return primes[primeIndex];
	}

	uint getNextPrimeFactor()
	{
		uint prime = getPrimeAtTableIndex();

		while (!containsHash(primeBagCopy.hash, prime))
		{
			primeIndex++;
			prime = getPrimeAtTableIndex();
		}

		return prime;
	}

	uint getPreviousPrimeFactor()
	{
		uint prime = getPrimeAtTableIndex();

		while (!(containsHash(refPrimeBag.hash, primeBagCopy.hash * prime)))
		{
			primeIndex--;
			prime = getPrimeAtTableIndex();
		}

		return prime;
	}

	const PrimeBag<V>& refPrimeBag;
	const PrimeTable<V>* primeTable;
	PrimeBag<V> primeBagCopy;
	uint primeIndex{ 0 };
	bool end{ false };
};