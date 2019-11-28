#pragma once
#include <vec2.hpp>
#include <vec3.hpp>
#include <detail/func_geometric.inl>


//Ported from TGAG
class SimplexNoise {
public:
	int hash[256] = {
		151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
		140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148,
		247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32,
		57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175,
		74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122,
		60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54,
		65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169,
		200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64,
		52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212,
		207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213,
		119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
		129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104,
		218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241,
		81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157,
		184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93,
		222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
	};

	int const  hashMask = 255;

	float* gradients1D = new float[] {
		1.0f, -1.0f
	};

	int const gradientsMask1D = 1;

	glm::vec2 gradients2D[8] = {
		glm::vec2(1.0f, 0.0f),
		glm::vec2(-1.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f,-1.0f),
		normalize(glm::vec2(1.0f, 1.0f)),
		normalize(glm::vec2(-1.0f, 1.0f)),
		normalize(glm::vec2(1.0f,-1.0f)),
		normalize(glm::vec2(-1.0f,-1.0f))
	};

	int const gradientsMask2D = 7;

	glm::vec3 gradients3D[32] = {
		normalize(glm::vec3(1.0f, 1.0f, 0.0f)),
		normalize(glm::vec3(-1.0f, 1.0f, 0.0f)),
		normalize(glm::vec3(1.0f,-1.0f, 0.0f)),
		normalize(glm::vec3(-1.0f,-1.0f, 0.0f)),
		normalize(glm::vec3(1.0f, 0.0f, 1.0f)),
		normalize(glm::vec3(-1.0f, 0.0f, 1.0f)),
		normalize(glm::vec3(1.0f, 0.0f,-1.0f)),
		normalize(glm::vec3(-1.0f, 0.0f,-1.0f)),
		normalize(glm::vec3(0.0f, 1.0f, 1.0f)),
		normalize(glm::vec3(0.0f,-1.0f, 1.0f)),
		normalize(glm::vec3(0.0f, 1.0f,-1.0f)),
		normalize(glm::vec3(0.0f,-1.0f,-1.0f)),

		normalize(glm::vec3(1.0f, 1.0f, 0.0f)),
		normalize(glm::vec3(-1.0f, 1.0f, 0.0f)),
		normalize(glm::vec3(1.0f,-1.0f, 0.0f)),
		normalize(glm::vec3(-1.0f,-1.0f, 0.0f)),
		normalize(glm::vec3(1.0f, 0.0f, 1.0f)),
		normalize(glm::vec3(-1.0f, 0.0f, 1.0f)),
		normalize(glm::vec3(1.0f, 0.0f,-1.0f)),
		normalize(glm::vec3(-1.0f, 0.0f,-1.0f)),
		normalize(glm::vec3(0.0f, 1.0f, 1.0f)),
		normalize(glm::vec3(0.0f,-1.0f, 1.0f)),
		normalize(glm::vec3(0.0f, 1.0f,-1.0f)),
		normalize(glm::vec3(0.0f,-1.0f,-1.0f)),

		normalize(glm::vec3(1.0f, 1.0f, 1.0f)),
		normalize(glm::vec3(-1.0f, 1.0f, 1.0f)),
		normalize(glm::vec3(1.0f,-1.0f, 1.0f)),
		normalize(glm::vec3(-1.0f,-1.0f, 1.0f)),
		normalize(glm::vec3(1.0f, 1.0f,-1.0f)),
		normalize(glm::vec3(-1.0f, 1.0f,-1.0f)),
		normalize(glm::vec3(1.0f,-1.0f,-1.0f)),
		normalize(glm::vec3(-1.0f,-1.0f,-1.0f))
	};

	int const gradientsMask3D = 31;

	float squaresToTriangles = (3.0f - sqrt(3.0f)) / 6.0f; //Converts points from square grid to triangel grid
	float trianglesToSquares = (sqrt(3.0f) - 1.0f) / 2.0f; //Converts points from triangle grid to square grid
	float tetrahedronToCube = (1.0f / 3.0f);
	float cubeToTetrahedron = (1.0f / 6.0f);

	float simplexScale2D = 296.0f * sqrt(2) / 15.0f;
	float simplexScale3D = 812.0f * sqrt(3) / 35.0f;

	float Dot(glm::vec2 g, float x, float y) {
		return g.x * x + g.y * y;
	}
	float Dot(glm::vec3 g, float x, float y, float z) {
		return g.x * x + g.y * y + g.z * z;
	}

	/// <summary>
	/// Scales the -1 to 1 range numbers to 0 to 1.
	/// </summary>
	/// <param name="input">float</param>
	/// <returns>float</returns>
	float scale01(float input) {
		return (input + 1.0f) / 2.0f;
	}

	///////////////////////////////////////////////////////////////////////////////
	/// __      __   _              _   _       _                               /// 
	/// \ \    / /  | |            | \ | |     (_)                              /// 
	///  \ \  / /_ _| |_   _  ___  |  \| | ___  _ ___  ___                      /// 
	///   \ \/ / _` | | | | |/ _ \ | . ` |/ _ \| / __|/ _ \                     ///
	///    \  / (_| | | |_| |  __/ | |\  | (_) | \__ \  __/                     /// 
	///     \/ \__,_|_|\__,_|\___| |_| \_|\___/|_|___/\___|                     /// 
	///////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Computes the simplex value 2D noise for the point, with the given frequency.
	/// Higher frequency makes the output more noisy.
	/// </summary>
	/// <param name="point"></param>
	/// <param name="frequency"></param>
	/// <returns>float value noise -1 to 1 range</returns>
	float SimplexValue1D(glm::vec3 point, float frequency) {
		point *= frequency;

		int ix = floor(point.x);

		float sample = SimplexValue1DPart(point, ix);
		sample += SimplexValue1DPart(point, ix + 1);

		return sample * (2.0f / hashMask) - 1.0f;
	}

	/// <summary>
	/// Computes the simplex value 2D noise for the point, with the given frequency.
	/// Higher frequency makes the output more noisy.
	/// </summary>
	/// <param name="point"></param>
	/// <param name="frequency"></param>
	/// <returns>float value noise -1 to 1 range</returns>
	float SimplexValue2D(glm::vec3 point, float frequency) {
		point *= frequency;

		float skew = (point.x + point.y) * trianglesToSquares; //Transform the triangle grid to a cube grid
		float sx = point.x + skew;
		float sy = point.y + skew;

		int ix = floor(sx);
		int iy = floor(sy);

		float sample = SimplexValue2DPart(point, ix, iy);
		sample += SimplexValue2DPart(point, ix + 1, iy + 1);

		if (sx - ix >= sy - iy) { // Work out which triangle the point is inside
			sample += SimplexValue2DPart(point, ix + 1, iy);
		} else {
			sample += SimplexValue2DPart(point, ix, iy + 1);
		}

		return sample * (8.0f *2.0f / hashMask) - 1.0f;
	}

	/// <summary>
	/// Computes the simplex value 3D noise for the point, with the given frequency.
	/// Higher frequency makes the output more noisy.
	/// </summary>
	/// <param name="point"></param>
	/// <param name="frequency"></param>
	/// <returns>float value noise -1 to 1 range</returns>
	float SimplexValue3D(glm::vec3 point, float frequency) {
		point *= frequency;

		float skew = (point.x + point.y + point.z) * tetrahedronToCube;
		float sx = point.x + skew;
		float sy = point.y + skew;
		float sz = point.z + skew;

		int ix = floor(sx);
		int iy = floor(sy);
		int iz = floor(sz);

		float sample = SimplexValue3DPart(point, ix, iy, iz);
		sample += SimplexValue3DPart(point, ix + 1, iy + 1, iz + 1);

		float x = sx - ix;
		float y = sy - iy;
		float z = sz - iz;

		if (x >= y) { //Figure out which tetrahedron we are in
			if (x >= z) {
				sample += SimplexValue3DPart(point, ix + 1, iy, iz);
				if (y >= z) {
					sample += SimplexValue3DPart(point, ix + 1, iy + 1, iz);
				} else {
					sample += SimplexValue3DPart(point, ix + 1, iy, iz + 1);
				}
			} else {
				sample += SimplexValue3DPart(point, ix, iy, iz + 1);
				sample += SimplexValue3DPart(point, ix + 1, iy, iz + 1);
			}
		} else {
			if (y >= z) {
				sample += SimplexValue3DPart(point, ix, iy + 1, iz);
				if (x >= z) {
					sample += SimplexValue3DPart(point, ix + 1, iy + 1, iz);
				} else {
					sample += SimplexValue3DPart(point, ix, iy + 1, iz + 1);
				}
			} else {
				sample += SimplexValue3DPart(point, ix, iy, iz + 1);
				sample += SimplexValue3DPart(point, ix, iy + 1, iz + 1);
			}
		}

		return sample * (8.0f *2.0f / hashMask) - 1.0f;
	}


	/// <summary>
	/// Computes the simplex noise value for a part (One end of a unit line segment)
	/// </summary>
	/// <param name="point"></param>
	/// <param name="ix"></param>
	/// <returns>Simplex noise value for part</returns>
	float SimplexValue1DPart(glm::vec3 point, int ix) {
		float x = point.x - ix;
		float f = 1.0f - x * x;
		float f2 = f * f;
		float f3 = f * f2;
		float h = hash[ix & hashMask];
		return f3 * h;
	}

	/// <summary>
	/// Computes the simplex noise value for a part (One corner of a triangle)
	/// </summary>
	/// <param name="point"></param>
	/// <param name="ix"></param>
	/// <param name="iy"></param>
	/// <returns>Simplex value for triangle point</returns>
	float SimplexValue2DPart(glm::vec3 point, int ix, int iy) {
		float unskew = (ix + iy) * squaresToTriangles;
		float x = point.x - ix + unskew;
		float y = point.y - iy + unskew;
		float f = 0.5f - x * x - y * y;
		if (f >0.0f) {
			float f2 = f * f;
			float f3 = f * f2;
			float h = hash[hash[ix & hashMask] + iy & hashMask];
			return f3 * h;
		}
		return 0;
	}

	/// <summary>
	/// Computes the simplex noise value for a corner in a tetrahedron
	/// </summary>
	/// <param name="point"></param>
	/// <param name="ix"></param>
	/// <param name="iy"></param>
	/// <param name="iz"></param>
	/// <returns>The simplex noise value for the corner in the tetrahedron</returns>
	float SimplexValue3DPart(glm::vec3 point, int ix, int iy, int iz) {
		float unskew = (ix + iy + iz) * cubeToTetrahedron;
		float x = point.x - ix + unskew;
		float y = point.y - iy + unskew;
		float z = point.z - iz + unskew;
		float f = 0.5f - x * x - y * y - z * z;
		if (f >0.0f) {
			float f2 = f * f;
			float f3 = f * f2;
			float h = hash[hash[hash[ix & hashMask] + iy & hashMask] + iz & hashMask];
			return f3 * h;
		}
		return 0;
	}

	///////////////////////////////////////////////////////////////////////////////////
	///   _____               _ _            _     _   _       _          			///
	///  / ____|             | (_)          | |   | \ | |     (_)         			///
	/// | |  __ _ __ __ _  __| |_  ___ _ __ | |_  |  \| | ___  _ ___  ___ 			///
	/// | | |_ | '__/ _` |/ _` | |/ _ \ '_ \| __| | . ` |/ _ \| / __|/ _ \			///
	/// | |__| | | | (_| | (_| | |  __/ | | | |_  | |\  | (_) | \__ \  __/			///
	///  \_____|_|  \__,_|\__,_|_|\___|_| |_|\__| |_| \_|\___/|_|___/\___|			///
	/////////////////////////////////////////////////////////////////////////////////// 

	/// <summary>
	/// Computes the simplex  2D noise for the point, with the given frequency.
	/// Higher frequency makes the output more noisy.
	/// </summary>
	/// <param name="point"></param>
	/// <param name="frequency"></param>
	/// <returns>float  noise -1 to 1 range</returns>
	float Simplex1D(glm::vec3 point, float frequency) {
		point *= frequency;

		int ix = floor(point.x);

		float sample = Simplex1DPart(point, ix);
		sample += Simplex1DPart(point, ix + 1);

		return sample * (4.0f / 7.0f);
	}

	/// <summary>
	/// Computes the simplex  2D noise for the point, with the given frequency.
	/// Higher frequency makes the output more noisy.
	/// </summary>
	/// <param name="point"></param>
	/// <param name="frequency"></param>
	/// <returns>float  noise -1 to 1 range</returns>
	float Simplex2D(glm::vec3 point, float frequency) {
		point *= frequency;

		float skew = (point.x + point.y) * trianglesToSquares; //Transform the triangle grid to a cube grid
		float sx = point.x + skew;
		float sy = point.y + skew;

		int ix = floor(sx);
		int iy = floor(sy);

		float sample = Simplex2DPart(point, ix, iy);
		sample += Simplex2DPart(point, ix + 1, iy + 1);

		if (sx - ix >= sy - iy) { // Work out which triangle the point is inside
			sample += Simplex2DPart(point, ix + 1, iy);
		} else {
			sample += Simplex2DPart(point, ix, iy + 1);
		}

		return sample * simplexScale2D;
	}

	/// <summary>
	/// Computes the simplex  3D noise for the point, with the given frequency.
	/// Higher frequency makes the output more noisy.
	/// </summary>
	/// <param name="point"></param>
	/// <param name="frequency"></param>
	/// <returns>float  noise -1 to 1 range</returns>
	float Simplex3D(glm::vec3 point, float frequency) {
		point *= frequency;

		float skew = (point.x + point.y + point.z) * tetrahedronToCube;
		float sx = point.x + skew;
		float sy = point.y + skew;
		float sz = point.z + skew;

		int ix = floor(sx);
		int iy = floor(sy);
		int iz = floor(sz);

		float sample = Simplex3DPart(point, ix, iy, iz);
		sample += Simplex3DPart(point, ix + 1, iy + 1, iz + 1);

		float x = sx - ix;
		float y = sy - iy;
		float z = sz - iz;

		if (x >= y) { //Figure out which tetrahedron we are in
			if (x >= z) {
				sample += Simplex3DPart(point, ix + 1, iy, iz);
				if (y >= z) {
					sample += Simplex3DPart(point, ix + 1, iy + 1, iz);
				} else {
					sample += Simplex3DPart(point, ix + 1, iy, iz + 1);
				}
			} else {
				sample += Simplex3DPart(point, ix, iy, iz + 1);
				sample += Simplex3DPart(point, ix + 1, iy, iz + 1);
			}
		} else {
			if (y >= z) {
				sample += Simplex3DPart(point, ix, iy + 1, iz);
				if (x >= z) {
					sample += Simplex3DPart(point, ix + 1, iy + 1, iz);
				} else {
					sample += Simplex3DPart(point, ix, iy + 1, iz + 1);
				}
			} else {
				sample += Simplex3DPart(point, ix, iy, iz + 1);
				sample += Simplex3DPart(point, ix, iy + 1, iz + 1);
			}
		}

		return sample * simplexScale3D;
	}


	/// <summary>
	/// Computes the simplex noise  for a part (One end of a unit line segment)
	/// </summary>
	/// <param name="point"></param>
	/// <param name="ix"></param>
	/// <returns>Simplex noise  for part</returns>
	float Simplex1DPart(glm::vec3 point, int ix) {
		float x = point.x - ix;
		float f = 1.0f - x * x;
		float f2 = f * f;
		float f3 = f * f2;
		float g = gradients1D[hash[ix & hashMask] & gradientsMask1D];
		float v = g * x;
		return f3 * v;
	}

	/// <summary>
	/// Computes the simplex noise  for a part (One corner of a triangle)
	/// </summary>
	/// <param name="point"></param>
	/// <param name="ix"></param>
	/// <param name="iy"></param>
	/// <returns>Simplex  for triangle point</returns>
	float Simplex2DPart(glm::vec3 point, int ix, int iy) {
		float unskew = (ix + iy) * squaresToTriangles;
		float x = point.x - ix + unskew;
		float y = point.y - iy + unskew;
		float f = 0.5f - x * x - y * y;
		if (f >0.0f) {
			float f2 = f * f;
			float f3 = f * f2;
			glm::vec2 g = gradients2D[hash[hash[ix & hashMask] + iy & hashMask] & gradientsMask2D];
			float v = Dot(g, x, y);
			return f3 * v;
		}
		return 0;
	}

	/// <summary>
	/// Computes the simplex noise  for a corner in a tetrahedron
	/// </summary>
	/// <param name="point"></param>
	/// <param name="ix"></param>
	/// <param name="iy"></param>
	/// <param name="iz"></param>
	/// <returns>The simplex noise  for the corner in the tetrahedron</returns>
	float Simplex3DPart(glm::vec3 point, int ix, int iy, int iz) {
		float unskew = (ix + iy + iz) * cubeToTetrahedron;
		float x = point.x - ix + unskew;
		float y = point.y - iy + unskew;
		float z = point.z - iz + unskew;
		float f = 0.5f - x * x - y * y - z * z;
		if (f >0.0f) {
			float f2 = f * f;
			float f3 = f * f2;
			glm::vec3 g = gradients3D[hash[hash[hash[ix & hashMask] + iy & hashMask] + iz & hashMask] & gradientsMask3D];
			float v = Dot(g, x, y, z);
			return f3 * v;
		}
		return 0;
	}
};

