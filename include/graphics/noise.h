#ifndef LANDSCAPE_PERLIN_NOISE_H
#define LANDSCAPE_PERLIN_NOISE_H

#include <graphics/image.h>
#include <graphics/texture.h>

namespace Noise
{
	/// A 3D Perlin noise generator.
	class Perlin
	{
	public:
		/// Creates an instance of a Perlin noise generator.
		/// \param repeat Repeat value.
		Perlin(int repeat = 0);
		
		/// Outputs a pseudorandom double in the range of [0, 1].
		///
		/// \param x X coordinate
		/// \param y Y coordinate
		/// \param z Z coordinate
		/// \return Pseudorandom value in the range of [0, 1]
		double noise(double x, double y, double z) const;
	private:
		int repeat;
		
		double fade(double t) const;
		int inc(int num) const;
		double grad(int hash, double x, double y, double z) const;
		double lerp(double a, double b, double x) const;
	};
	
	/// 2D noise encapsulated in an Image
	class Image : public ::Image
	{
	public:
		/// Creates an Image instance with 2D grayscale noise.
		/// \param perlin Perlin noise generator instance.
		/// \param width Width of the resulting image.
		/// \param height Height of the resulting image.
		/// \param channels Channel count - will fill all channels with
		/// a gray color, unless its the fourth channel, which will be
		/// filled with 255s.
		/// \param scale Inverted scale of the noise - e.g. 2.0f will
		/// make it appear more zoomed out than 1.0f.
		Image(
			Perlin perlin, int width, int height, ColorLayout layout,
			float scale = 5.0f);
		
		/// Destroys the Image instance.
		~Image();
	};
}

#endif //LANDSCAPE_PERLIN_NOISE_H