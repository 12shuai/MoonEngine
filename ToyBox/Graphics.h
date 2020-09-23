#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "MoonEnums.h"
#include "SceneMgr.h"

namespace MOON {

	class Graphics {
	public:
		static PipelineMode pipeline;
		static ShadingMode shading;
		static SystemProcess process;

		// global parameters
		static std::vector<float> ground;
		static int edit_mode_point_size;

		static void DrawIDLUT();
		static void DrawIDLUT_EditMode();
		static void DrawShadowMap();
		static void SetDrawTarget(SceneView view, const bool& depthTest);
		static void DrawSceneView(SceneView view);
		
		static void SetShadingMode(ShadingMode shading);
		static void DrawModels();
		static void DrawShapes();
		static void DrawHelpers();
		static void DrawCameras();
		static void HighlightSelection();

		// TODO
		static void DrawDeferredShading();
		static void DrawPrototype();
		static void DrawMesh();

	private:
		static unsigned int quadVAO, quadVBO;

		// vertex attributes for a quad that fills the 
		// entire screen in Normalized Device Coordinates.
		static float quadVertices[24];

		// config screen quad VAO
		static void ConfigureScreenQuad();
	};

}

/*
*NOTE:
[ģ�����]
���ã�glEnable(GL_STENCIL_TEST);

void glStencilOp(GLenum sfail, GLenum zfail, GLenum zpass);
sfail ���ɰ����ʧ��ʱ��ִ�еĲ���
zfail ���ɰ����ͨ������Ȳ���ʧ��ʱ��ִ�еĲ���
zpass ���ɰ����ͨ������Ȳ���ͨ��ʱ��ִ�еĲ���
GL_KEEP		���ֵ�ǰ���ɰ建����ֵ
GL_ZERO		�ѵ�ǰ���ɰ建����ֵ��Ϊ0
GL_REPLACE	��glStencilFunc������ָ���Ĳο�ֵ�滻�ɰ����ֵ
GL_INCR		���ӵ�ǰ���ɰ建����ֵ��������������ķ�Χ��
GL_DECR		���ٵ�ǰ���ɰ建����ֵ��������������ķ�Χ��
GL_INVERT	����ǰ���ɰ建����ֵ������λ�ķ�ת

void glStencilFunc(GLenum func, GLint ref, GLuint mask);
func ����ģ����Ե�ͨ����ʽ
ref	 Ϊ�ο�ֵ
mask Ϊ����ֵ
GL_NEVER	�Ӳ�ͨ���ɰ����
GL_ALWAYS	����ͨ���ɰ����
GL_LESS		ֻ�вο�ֵ<(�ɰ建������ֵ&mask)ʱ��ͨ��
GL_LEQUAL	ֻ�вο�ֵ<=(�ɰ建������ֵ&mask)ʱ��ͨ��
GL_EQUAL	ֻ�вο�ֵ=(�ɰ建������ֵ&mask)ʱ��ͨ��
GL_GEQUAL	ֻ�вο�ֵ>=(�ɰ建������ֵ&mask)ʱ��ͨ��
GL_GREATER	ֻ�вο�ֵ>(�ɰ建������ֵ&mask)ʱ��ͨ��
GL_NOTEQUAL	ֻ�вο�ֵ!=(�ɰ建������ֵ&mask)ʱ��ͨ��


[���]
���ã�glEnable(GL_BLEND);

glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
GL_SRC_ALPHA �� GL_ONE_MINUS_SRC_ALPHA �ֱ����Դ��Ŀ����ɫ�ڻ��ʱ��ռ���ص�ö�ٳ���

��ȡֵ������
GL_ZERO, 
GL_ONE, 
GL_DST_COLOR, 
GL_ONE_MINUS_DST_COLOR, 
GL_SRC_ALPHA, 
GL_ONE_MINUS_SRC_ALPHA, 
GL_DST_ALPHA, 
GL_ONE_MINUS_DST_ALPHA, 
GL_SRC_ALPHA_SATURATE

�ڼ����ɫʱ�������Ǹ������ϵ�ö�ٵó�Դ��ɫ��Ŀ����ɫ��ϵ����
Ȼ��ֱ���Դ��Ŀ����ɫ��ˣ��˻�����1ʱȡֵ1��������ٰѵó��Ľ����ӡ�

�����������ö�ٵó���RGBA�ĸ��ɷ��ϵ�ϵ���ֱ�Ϊ 
Դ sfR, sfG, sfB, sfA�� 
Ŀ�� dfR, dfG, dfB, dfA�� 
Դ��Ŀ�����ɫ�ɷ�ֵ�ֱ��� 
sR, sG, sB, sA
dR, dG, dB, dA 
��ʾ�����ս��ɫ�� rR, rG, rB, rA �ֱ�Ϊ
rR = sR * sfR + dR * dfR
rG = sG * sfG + dG * dfG
rB = sB * sfB + dB * dfB
rA = sA * sfA + dA * dfA

����
glBlendFunc(GL_ONE, GL_ONE); ��Դ��Ŀ����ɫ��RGBA�ֱ���ӡ�
glBlendFunc(GL_ONE, GL_ZERO); ��ֻȡԴ��ɫ����Ҳ��Ĭ��ֵ��
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); �ǱȽϵ��͵İ�͸��Ч����
���ԴɫalphaΪ0����ȡĿ��ɫ�����ԴɫalphaΪ1����ȡԴɫ��
������Դɫ��alpha��С��ȡһ���֡�
Դɫ��alphaԽ����Դɫȡ��Խ�࣬���ս��Դɫ�ı��ָ�ǿ��
Դɫ��alphaԽС����Ŀ��ɫ��͸������Խ�ࡣ
*/
