#include <iostream>
#include <cmath>
#include <cstdint>
#include <graphics/noise.h>

// Noise::Perlin

/// Hash lookup table as defined by Ken Perlin. This is a randomly arranged array
/// of all numbers from 0-255 inclusive.
int perms[] = {
	151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
	140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
	120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57,
	177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168,  68, 175,
	74, 165, 71, 134, 139, 48, 27, 166, 77, 146,158, 231, 83, 111, 229, 122,
	60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54,
	65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169,
	200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3,
	64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85,
	212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170,
	213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43,
	172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185,
	112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191,
	179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31,
	181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150,
	254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195,
	78, 66, 215, 61, 156, 180
};

Noise::Perlin::Perlin(int repeat): repeat(repeat) {}

double
Noise::Perlin::noise(double x, double y, double z) const
{
	if (repeat)
	{
		// Does input really have to be a double?
		x = (int)x % repeat;
		y = (int)y % repeat;
		z = (int)z % repeat;
	}
	
	// Calculate permutation table index for each coordinate
	int xi = (int)x & 255;
	int yi = (int)y & 255;
	int zi = (int)z & 255;
	
	// Calculate remainders for each coordinate
	double xf = x - (int)x;
	double yf = y - (int)y;
	double zf = z - (int)z;
	
	// Ease coordinate values
	double u = fade(xf);
	double v = fade(yf);
	double w = fade(zf);
	
	// Perlin noise hash function using the permutation table
	int aaa, aba, aab, abb, baa, bba, bab, bbb;
	aaa = perms[perms[perms[    xi ]+    yi ]+    zi ];
	aba = perms[perms[perms[    xi ]+inc(yi)]+    zi ];
	aab = perms[perms[perms[    xi ]+    yi ]+inc(zi)];
	abb = perms[perms[perms[    xi ]+inc(yi)]+inc(zi)];
	baa = perms[perms[perms[inc(xi)]+    yi ]+    zi ];
	bba = perms[perms[perms[inc(xi)]+inc(yi)]+    zi ];
	bab = perms[perms[perms[inc(xi)]+    yi ]+inc(zi)];
	bbb = perms[perms[perms[inc(xi)]+inc(yi)]+inc(zi)];
	
	// The gradient function calculates the dot product between a
	// pseudorandom gradient vector and the vector from the input coordinate
	// to the 8 surrounding points in its unit cube.
	// This is all then lerped together as a sort of weighted average based
	// on the faded (u, v, w) values we made earlier.
	double x1, x2, y1, y2;
	x1 = lerp(
		grad(aaa, xf, yf, zf),
		grad(baa, xf - 1, yf, zf),
		u);
	x2 = lerp(
		grad(aba, xf, yf - 1, zf),
		grad(bba, xf - 1, yf - 1, zf),
		u);
	y1 = lerp(x1, x2, v);
	
	x1 = lerp(
		grad(aab, xf, yf, zf - 1),
		grad(bab, xf - 1, yf, zf - 1),
		u);
	x2 = lerp(
		grad(abb, xf, yf - 1, zf - 1),
		grad(bbb, xf - 1, yf - 1, zf - 1),
		u);
	y2 = lerp(x1, x2, v);
	
	// For convenience we bind the result to [0, 1]
	return (lerp(y1, y2, w) + 1) / 2;
}

double
Noise::Perlin::octave_noise(
	double x, double y, double z, int octaves, double persistence
) const {
	double total = 0;
	double frequency = 1;
	double amplitude = 1;
	double max_val = 0;  // Used for normalizing result to 0.0 - 1.0
	
	for(int i = 0; i < octaves; i++) {
		total += noise(x * frequency, y * frequency, z * frequency)
			* amplitude;
		
		max_val += amplitude;
		
		amplitude *= persistence;
		frequency *= 2;
	}
	
	return total / max_val;
}

/// Fade function easing coordinate values so they will ease towards
/// integral values, ending up smoothing the final output.
/// 6t^5 - 15t^4 + 10t^3
double
Noise::Perlin::fade(double t) const
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

int
Noise::Perlin::inc(int num) const
{
	num++;
	if (repeat > 0) num %= repeat;
	return num;
}

double
Noise::Perlin::grad(int hash, double x, double y, double z) const
{
	switch(hash & 0xF)
	{
		case 0x0:
			return  x + y;
		case 0x1:
			return -x + y;
		case 0x2:
			return  x - y;
		case 0x3:
			return -x - y;
		case 0x4:
			return  x + z;
		case 0x5:
			return -x + z;
		case 0x6:
			return  x - z;
		case 0x7:
			return -x - z;
		case 0x8:
			return  y + z;
		case 0x9:
			return -y + z;
		case 0xA:
			return  y - z;
		case 0xB:
			return -y - z;
		case 0xC:
			return  y + x;
		case 0xD:
			return -y + z;
		case 0xE:
			return  y - x;
		case 0xF:
			return -y - z;
		default:
			return 0; // Never happens
	}
}

double
Noise::Perlin::lerp(double a, double b, double x) const
{
	return a + x * (b - a);
}

// Noise::Image

Noise::Image::Image(
	Noise::Generator &generator, int width, int height, ColorLayout layout,
	float frequency
) : ::Image(nullptr, width, height, color_layout_byte_size(layout))
{
	int channels = color_layout_byte_size(layout);
	
	// Manually allocate the buffer.
	data = new unsigned char[width * height * channels];
	
	// Iterate through each pixel.
	for (int ix = 0; ix < width; ix++)
		for (int iy = 0; iy < height; iy++)
		{
			// Convert the indices to [0, scale] range.
			double x_noise = (frequency / width) * ix;
			double y_noise = (frequency / height) * iy;
			double noise = generator.noise(x_noise, y_noise, 1.0f);
			
			// Convert to a [0, 255] int.
			int pixel_idx =
				(ix * channels) + (iy * width * channels);
			int gray = (uint8_t)(255.0f * noise);
			
			// Insert in required channels. Unless its alpha, then
			// just set it to 255.
			switch (layout)
			{
				case layout_rgba:
					data[pixel_idx + 3] = (uint8_t)255;
				case layout_rgb:
					data[pixel_idx + 2] = (uint8_t)gray;
				case layout_rg:
					data[pixel_idx + 1] = (uint8_t)gray;
				case layout_r:
					data[pixel_idx] = (uint8_t)gray;
					break;
				case layout_depth16:
				default:
					std::cout
					<< "Unsupported ColorLayout type "
						<< std::hex << layout << "."
						<< std::endl;
			}
		}
}

Noise::Image::Image(
	Noise::OctavedGenerator &generator, int width, int height,
	ColorLayout layout, float frequency, int octaves, double persistence
) : ::Image(nullptr, width, height, color_layout_byte_size(layout))
{
	int channels = color_layout_byte_size(layout);
	
	// Manually allocate the buffer.
	data = new unsigned char[width * height * channels];
	
	// Iterate through each pixel.
	for (int ix = 0; ix < width; ix++)
		for (int iy = 0; iy < height; iy++)
		{
			// Convert the indices to [0, scale] range.
			double xn = (frequency / width) * ix;
			double yn = (frequency / height) * iy;
			double noise = generator.octave_noise(
				xn, yn, 1.0f, octaves, persistence);
			int gray = (uint8_t)(255.0f * noise);
			
			// Convert to a [0, 255] int.
			int pixel_idx =
				(ix * channels) + (iy * width * channels);
			
			// Insert in required channels. Unless its alpha, then
			// just set it to 255.
			switch (layout)
			{
				case layout_rgba:
					data[pixel_idx + 3] = (uint8_t)255;
				case layout_rgb:
					data[pixel_idx + 2] = (uint8_t)gray;
				case layout_rg:
					data[pixel_idx + 1] = (uint8_t)gray;
				case layout_r:
					data[pixel_idx] = (uint8_t)gray;
					break;
				case layout_depth16:
				default:
					std::cout
					<< "Unsupported ColorLayout type "
						<< std::hex << layout << "."
						<< std::endl;
			}
		}
}

Noise::Image::~Image()
{
	::Image::~Image();
	// Since we manually allocate the buffer, destroy it.
	delete[] data;
	
}
