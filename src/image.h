#include <stdlib.h>
#include <vector>

//image class just dumped to hide...
class RGBValue
{
	public:
	RGBValue(float rI=0, float gI=0, float bI=0)
	: r(rI)
	, g(gI)
	, b(bI)
	{
		if (r>1)
			r=1.0;
		if (g>1)
			g=1.0;
		if (b>1)
			b=1.0;

		if (r<0)
			r=0.0;
		if (g<0)
			g=0.0;
		if (b<0)
			b=0.0;
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

class Image
{
	public:
	Image(int width, int height)
	: _width(width)
	, _height(height)
	{
		_image.resize(3*_width*_height);
	}

	Image(const char * filename) {
		readImage(filename);
	}

	void setPixel(int i, int j, const RGBValue & rgb)
	{
		_image[3*(_width*j+i)]=rgb[0];
		_image[3*(_width*j+i)+1]=rgb[1];
		_image[3*(_width*j+i)+2]=rgb[2];
		
	}
	std::vector<float> _image;
	int _width;
	int _height;

	bool writeImage(const char * filename);	
	private:
	bool readImage(const char * filename);
};

class Texture {
	public:
	Texture(Image img) : _image_data(img)
	{}
	const Image _image_data;
};
