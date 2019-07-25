#ifndef LANDSCAPE_RANGE_H
#define LANDSCAPE_RANGE_H

class RangeIterator
{
public:
	RangeIterator(int value_)
		: value(value_){}
	
	bool operator!=(RangeIterator const& other) const
	{
		return value != other.value;
	}
	
	int const& operator*() const
	{
		return value;
	}
	
	RangeIterator& operator++()
	{
		++value;
		return *this;
	}

private:
	int value;
};

class Range
{
public:
	Range(int from_, int to_)
		: from(from_), to(to_){}
	
	Range(int to_)
		: from(0), to(to_){}
	
	RangeIterator begin() const
	{
		return RangeIterator(from);
	}
	
	RangeIterator end() const
	{
		return RangeIterator(to + 1);
	}

private:
	int const from;
	int const to;
};

#endif //LANDSCAPE_RANGE_H
