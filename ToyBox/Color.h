#pragma once
#include <vector>
#include <iostream>

#include "Vector4.h"
#include "MathUtils.h"

namespace MOON {
	class Color {
	public:
		// based on : https://www.sioe.cn/yingyong/yanse-rgb-16/
		static Vector4 RED() { return Vector4(1.0f, 0.0f, 0.0f, 1.0f); }
		static Vector4 BLUE() { return Vector4(0.0f, 0.0f, 1.0f, 1.0f); }
		static Vector4 GREEN() { return Vector4(0.0f, 1.0f, 0.0f, 1.0f); }
		static Vector4 WHITE() { return Vector4(1.0f, 1.0f, 1.0f, 1.0f); }
		static Vector4 BLACK() { return Vector4(0.0f, 0.0f, 0.0f, 1.0f); }
		static Vector4 YELLOW() { return Vector4(1.0f, 1.0f, 0.0f, 1.0f); }
		static Vector4 ORANGE() { return Vector4(1.0f, 0.647f, 0.0f, 1.0f); }
		static Vector4 Gray() { return Vector4(0.439f, 0.502f, 0.565f, 1.0f); }

		static Vector4 Random(bool randomAlpha = false) { return Vector4(
			MoonMath::Random01(), 
			MoonMath::Random01(), 
			MoonMath::Random01(), 
			randomAlpha ? MoonMath::Random01() : 1.0f
		);}

		inline static float GrayScaleVal(const Vector4 &color) { return (color.x + color.y + color.z) / 3.0f; }
		inline static Vector4 GrayScaleColor(const Vector4 &color) {
			float grayCol = GrayScaleVal(color);
			return Vector4(grayCol, grayCol, grayCol, color.w);
		}

		// convert object ID to unique color
		inline static Vector4 IDEncoder(const unsigned int &ID) {
			if (ID >= MOON_IDBOUNDARY) { 
				std::cout << "ERROR: ID is too large! ID: " << ID << std::endl;
				return Vector4::ZERO(); 
			}

			/// black (0, 0, 0, 0) is backgroundColor
			/// so object ID is start from 1
			std::vector<int> data = MoonMath::TENtoNBase(ID, 256);

			// reverse order
			return Vector4(data[0], data.size() > 1 ? data[1] : 0, 
						   data.size() > 2 ? data[2] : 0, 
						   255 - (data.size() > 3 ? data[3] : 0)) / 255.0f;
		}

		/// *NOTE: alpha is start from 1 to 0, while r,g,b is start from 0 to 1
		// convert object color to object ID
		inline static unsigned int IDDecoder(Vector4 Color) {
			if (Color == Vector4::ZERO()) {
				//std::cout << "ERROR: ID can not be 0!" << std::endl;
				return 0;
			}

			Color *= 255;
			return (255 - Color[3]) * 256 * 256 * 256 + Color[2] * 256 * 256 + Color[1] * 256 + Color[0];
		}

		// convert object color to object ID
		inline static unsigned int IDDecoder(float r, float g, float b, float a) {
			if (!r && !g && !b && !a) {
				//std::cout << "ERROR: ID can not be 0!" << std::endl;
				return 0;
			}

			r *= 255; g *= 255; b *= 255; a *= 255;
			return (255 - a) * 256 * 256 * 256 + b * 256 * 256 + g * 256 + r;
		}
	};
}