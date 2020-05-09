#pragma once
#include <cmath>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <unordered_map>

#include "NVariable.h"

namespace MOON {
	namespace NN {
		extern class NGraph;
		class Optimizer {
		public:
			unsigned int id;
			NGraph* graph;
			Neuron* target;

			int batchSize;
			float lr; // learning rate

			std::unordered_map<int, Matrix> acc_gradient;
			int acc_cnt;

			Optimizer(NGraph* graph, Neuron* target, float learning_rate, int batch_size);
			virtual ~Optimizer() = default;

			// ǰ�򴫲��������ڵ��ֵ
			void Forward();
			// ���򴫲��������ڵ�Ը����ڵ���ݶ�
			void Backward();
			// ����ݶ�
			void ClearGrad();
			// ����һ��Mini Batch��������ƽ���ݶ�
			Matrix GetGradient(const Neuron* node);
			// ����Mini Batch��ƽ���ݶ��Ը��ֲ�ͬ�ķ������±���ֵ
			virtual void Update() = 0;
			// ���㲢�ۼ��������ݶȣ�һ��Mini Batch������ִ�б�������
			void Step();

		};
	}
}
