#ifndef LANDSCAPE_PERLIN_NOISE_H
#define LANDSCAPE_PERLIN_NOISE_H

#include <algorithm>
#include <numeric>
#include <memory>
#include <graphics/image.h>
#include <graphics/texture.h>

namespace Noise
{
	/// An abstract 3D noise generator class.
	class Generator
	{
	public:
		virtual double noise(double x, double y, double z) const = 0;
	};
	
	/// A Perlin noise generator.
	class Perlin : public Generator
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
		
		double octave_noise(
			double x, double y, double z, int octaves,
			double persistence);
	private:
		int repeat;
		
		double fade(double t) const;
		int inc(int num) const;
		double grad(int hash, double x, double y, double z) const;
		double lerp(double a, double b, double x) const;
	};
	
	/// A composite noise generator, generating noise using output composed
	/// from its components.
	template <class GenPtrArray>
	class Composite : public Generator
	{
	public:
		/// Creates a composite noise generator.
		/// \param generators Container of pointers to Generator
		/// instances.
		Composite(GenPtrArray generators): array(generators) {};
		
		/// Outputs a pseudorandom double in the range of [0, 1].
		///
		/// \param x X coordinate
		/// \param y Y coordinate
		/// \param z Z coordinate
		/// \return Pseudorandom value in the range of [0, 1]
		double noise(double x, double y, double z) const
		{
			using GenPtr = std::shared_ptr<Generator>;
			double sum = std::accumulate(
				array.begin(), array.end(), 0.0f,
				[&](GenPtr g1, GenPtr g2) {
					return g1->noise(x, y, z)
						+ g2->noise(x, y, z);
				});
			return sum / array.size();
		};
	private:
		GenPtrArray array;
	};
	
	/// 2D noise encapsulated in an Image
	class Image : public ::Image
	{
	public:
		/// Creates an Image instance with 2D grayscale noise.
		/// \param perlin Perlin noise generator instance.
		/// \param width Width of the resulting image.
		/// \param height Height of the resulting image.
		/// \param layout Layout of the resulting image.
		/// \param frequency Frequency of the noise.
		Image(
			Perlin perlin, int width, int height,
			ColorLayout layout, float frequency = 5.0f);
		
		Image(
			Perlin perlin, int width, int height,
			ColorLayout layout, float frequency, int octaves,
			double persistence);
		
		/// Destroys the Image instance.
		~Image();
	};
}

#endif //LANDSCAPE_PERLIN_NOISE_H
