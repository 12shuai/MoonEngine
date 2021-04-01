#pragma once
#include <string>
//#include <cstdarg>

#include "Texture.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Color.h"

namespace MOON {
	#define ProceduralTexture ProceduralMapGenerator::ProceduralMapType

	/* CPU & GPU based procedural map generator */
	class ProceduralMapGenerator {
	public:
		enum ProceduralMapType {
			// noise
			WhiteNoise, FirstNoise = WhiteNoise,
			ValueNoise,
			PerlinNoise,
			OctavePerlin,
			SimplexNoise,
			WorleyNoise,
			VoronoiNoise,
			TysonPolygon,
			CloudNoise,

			// pattern
			Checker,
			Disco,
			WoodPattern,
			BubblePattern, FirstPattern = BubblePattern,
			DigitalStorm,
			TruchetPattern,
			ColorfulTruchet
		};

	#pragma region GPU_based
		static Shader* FireShader;
		static Shader* NoiseShader;
		static Shader* PatternShader;

		static void LoadShaders();

		/*static bool GPUNoiseMaker(FrameBuffer* texHolder, const ProceduralMapType& type,
			const unsigned int channelSelection, ...) {
			va_list arg_ptr;
			va_start(arg_ptr, type);
			GPUNoiseMaker(
				type, texHolder, 
				va_arg(arg_ptr, Vector2), 
				va_arg(arg_ptr, float),
				va_arg(arg_ptr, float), 
				va_arg(arg_ptr, float),
				channelSelection
			);
			va_end(arg_ptr);
			return true;
		}*/

		static bool GPUFireTexMaker(FrameBuffer* texHolder, const Vector2& offset, 
			const float& scale, const float& power, const float& time, const Vector3& tint);

		static bool GPUNoiseMaker(const ProceduralMapType& type, 
			FrameBuffer* texHolder, const Vector2& offset = Vector2::ZERO(),
			const float& scale = 1.0f, const float& dim = 0.0f, const float& time = 0.0f,
			const unsigned int channelSelection = 0);

		static bool GPUPatternMaker(const ProceduralMapType& type,
			FrameBuffer* texHolder, const Vector2& offset = Vector2::ZERO(),
			const float& scale = 1.0f, const float& dim = 0.0f, const float& time = 0.0f,
			const unsigned int channelSelection = 0);

		static bool GPUChecker(FrameBuffer* texHolder, const Vector3& offset = Vector2::ZERO(),
			const float& scale = 1.0f);

		static bool GPUWhiteNoise(FrameBuffer* texHolder, const Vector3& offset = Vector2::ZERO(),
			const float& scale = 1.0f);

		static bool GPUPerlinNoise(FrameBuffer* texHolder, const Vector2& offset = Vector2::ZERO(),
			const float& scale = 1.0f, const float& dim = 0.0f, const float& time = 0.0f);
		static bool GPUPerlinNoise(FrameBuffer* texHolder, const Vector4& offset = Vector4::ZERO(),
			const float& scale = 1.0f) {
			GPUPerlinNoise(texHolder, Vector2(offset.x, offset.y), scale, offset.z, offset.w);
		}

		static bool GPUPerlinNoise_fbm(FrameBuffer* texHolder, const Vector2& offset = Vector2::ZERO(),
			const float& scale = 1.0f, const float& dim = 0.0f, const float& time = 0.0f);

		static bool GPUSimplexNoise(FrameBuffer* texHolder, const Vector2& offset = Vector2::ZERO(),
			const float& scale = 1.0f, const float& dim = 0.0f, const float& time = 0.0f);
		static bool GPUSimplexNoise(FrameBuffer* texHolder, const Vector4& offset = Vector4::ZERO(),
			const float& scale = 1.0f) {
			GPUSimplexNoise(texHolder, Vector2(offset.x, offset.y), scale, offset.z, offset.w);
		}

		static bool GPUWorleyNoise(FrameBuffer* texHolder, const Vector2& offset = Vector2::ZERO(),
			const float& scale = 1.0f, const float& dim = 0.0f, const float& time = 0.0f);

		static bool GPUVoronoiNoise(FrameBuffer* texHolder, const Vector2& offset = Vector2::ZERO(),
			const float& scale = 1.0f, const float& dim = 0.0f, const float& time = 0.0f);
	#pragma endregion

	#pragma region CPU_based
		static bool CPUNoiseMaker(GLubyte *targetImage, const Vector2 &mapSize, const ProceduralMapType& type) {
			if (type == Checker) {
				return CPUCheckImage(targetImage, mapSize);
			} else if (type == WhiteNoise) {

			} else if (type == ValueNoise) {

			}
			return false;
		}

		static bool CPUWhiteNoise(GLubyte *checkImage, const Vector2 &mapSize);
		static bool CPUValueNoise(GLubyte *checkImage, const Vector2 &mapSize);
		static bool CPUPerlinNoise(GLubyte *checkImage, const Vector2 &mapSize);
		static bool CPUWorleyNoise(GLubyte *checkImage, const Vector2 &mapSize);
		static bool CPUVoronoiNoise(GLubyte *checkImage, const Vector2 &mapSize);
		static bool CPUCheckImage(GLubyte *checkImage, const Vector2 &mapSize);
		static bool CPUBlankImage(GLubyte *targetImage);
	#pragma endregion
	};
}

/*
*NOTE:
��1��va_list
������һ��ָ��arg_ptr, ����ָʾ��ѡ�Ĳ���.

��2��va_start(arg_ptr, argN)
ʹ�����б�ָ��arg_ptrָ���������б��еĵ�һ����ѡ������
argN��λ�ڵ�һ����ѡ����֮ǰ�Ĺ̶�����, ����˵���һ���̶�����������Ҫ��һ���̶�������.
��:
��һva������������void va_test(char a, char b, char c, ...), 
�����Ĺ̶�����������a,b,c, ���һ���̶�����argNΪc, ��˾���va_start(arg_ptr, c).

��3��va_arg(arg_ptr, type)
���ز����б���ָ��arg_ptr��ָ�Ĳ���, ��������Ϊtype. 
��ʹָ��arg_ptrָ������б�����һ������.
*���ص��ǿ�ѡ����, �������̶�����.

��4��va_end(arg_ptr)
��ղ����б�, ���ò���ָ��arg_ptr��Ч.

* ȡֵ��������ʱ���������ȡ�����ֵ
*/
