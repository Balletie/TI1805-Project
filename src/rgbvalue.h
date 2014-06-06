#ifndef RGBVALUE_H_
#define RGBVALUE_H_

class RGBValue
{
	public:
	RGBValue(float rI=0, float gI=0, float bI=0)
	: r(rI)
	, g(gI)
	, b(bI)
	{
	};

	float operator[](int i) const
	{
		switch(i)
		{
			case 0:
				return r;
			case 1:
				return g;
			case 2:
				return b;
			default:
				return r;
		}
	}
	float & operator[](int i)
	{
		switch(i)
		{
			case 0:
				return r;
			case 1:
				return g;
			case 2:
				return b;
			default:
				return r;
		}
	}
	float r, b,g;
};

#endif /* RGBVALUE_H_ */
