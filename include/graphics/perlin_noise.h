#ifndef LANDSCAPE_PERLIN_NOISE_H
#define LANDSCAPE_PERLIN_NOISE_H

double perlin(double x, double y, double z);

class PerlinNoiseGenerator
{
public:
	PerlinNoiseGenerator(int repeat = 0);
	
	/// Outputs a pseudorandom double in the range of [0, 1].
	///
	/// \param x X coordinate
	/// \param y Y coordinate
	/// \param z Z coordinate
	/// \return Pseudorandom value in the range of [0, 1]
	double noise(double x, double y, double z);
private:
	int repeat;
	
	double fade(double t);
	int inc(int num);
	double grad(int hash, double x, double y, double z);
	double lerp(double a, double b, double x);
};

#endif //LANDSCAPE_PERLIN_NOISE_H
