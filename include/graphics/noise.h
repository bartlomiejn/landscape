#ifndef LANDSCAPE_PERLIN_NOISE_H
#define LANDSCAPE_PERLIN_NOISE_H

#include <memory>
#include <array>
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
	
	/// An abstract 3D octaved noise generator class.
	class OctavedGenerator
	{
	public:
		virtual double octave_noise(
			double x, double y, double z, int octaves,
			double persistence) const = 0;
	};
	
	/// A Perlin 3D noise generator.
	class Perlin : public Generator, public OctavedGenerator
	{
	public:
		/// Creates an instance of a Perlin noise generator.
		/// \param repeat Repeat value.
		Perlin(int repeat = 0);
		
		/// Outputs a pseudorandom double in the range of [0, 1].
		/// \param x X coordinate.
		///\param y Y coordinate.
		/// \param z Z coordinate.
		/// \return Pseudorandom value in the range of [0, 1].
		virtual double noise(double x, double y, double z) const;
		
		/// Outputs a pseudorandom double in the range of [0, 1].
		/// \param x X coordinate
		/// \param y Y coordinate
		/// \param z Z coordinate
		/// \param octaves Octaves of noise with increasing frequency.
		/// \param persistence Influence multiplier of each consecutive
		/// octave on the end result.
		/// \return Pseudorandom value in the range of [0, 1].
		virtual double octave_noise(
			double x, double y, double z, int octaves,
			double persistence) const;
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
		/// \param generator Noise generator instance.
		/// \param width Width of the resulting image.
		/// \param height Height of the resulting image.
		/// \param layout Layout of the resulting image.
		/// \param frequency Frequency of the noise.
		Image(
			Generator &generator, int width, int height,
			ColorLayout layout, float frequency = 5.0f);
		
		/// Creates an Image with 2D grayscale noise.
		/// \param generator Octaved noise generator instance.
		/// \param width Width of the resulting image.
		/// \param height Height of the resulting image.
		/// \param layout Layout of the resulting image.
		/// \param frequency Frequency of the initial noise octave.
		/// \param octaves Octaves count with increasing frequency.
		/// \param persistence Influence multiplier of each consecutive
		/// octave on the end result.
		Image(
			OctavedGenerator &generator, int width, int height,
			ColorLayout layout, float frequency, int octaves,
			double persistence);
		
		/// Destroys the Image instance.
		~Image();
	};
	
	template <int x_sz, int y_sz, int z_sz>
	class Volume
	{
	public:
		/// Creates a voxel volume using thresholded octaved noise.
		/// \param gen Octaved noise generator instance.
		/// \param x_sz X size.
		/// \param y_sz Y size.
		/// \param z_sz Z size.
		/// \param frequency Frequency of the initial noise octave.
		/// \param octaves Octaves count with increasing frequency.
		/// \param persistence Influence multiplier of each consecutive
		/// octave on the end result.
		/// \param threshold Threshold limit.
		Volume(
			OctavedGenerator &gen, float frequency, int octaves,
			double persistence, double threshold = 0.5f
		){
			for (int ix = 0; ix < x_sz; ix++)
			for (int iy = 0; iy < y_sz; iy++)
			for (int iz = 0; iz < z_sz; iz++)
			{
				// Convert the indices to [0, scale] range.
				double xi = (frequency / x_sz) * ix;
				double yi = (frequency / y_sz) * iy;
				double zi = (frequency / z_sz) * iz;
				double noise = gen.octave_noise(
					xi, yi, zi, octaves, persistence);
 
				// Threshold the output
				set(ix, iy, iz, (noise > threshold)
					? (unsigned char)0x1
					: (unsigned char)0x0);
			}
		};
		
		/// Samples a byte at (x, y, z). Throws an exception if out of
		/// bounds.
		/// \return
		int try_sample(int x, int y, int z) const
		{
			return data.at(index_for(x, y, z));
		};
		
		/// Sets a byte at (x, y, z) to value.
		void set(int x, int y, int z, unsigned char value)
		{
			data[index_for(x, y, z)] = value;
		}
	private:
		std::array<unsigned char, x_sz * y_sz * z_sz> data;
		
		/// Returns an array index for the provided coordinate.
		int index_for(int x, int y, int z) const
		{
			return x + (y * x_sz) + (z * y_sz * x_sz);
		}
	};
}

#endif //LANDSCAPE_PERLIN_NOISE_H
