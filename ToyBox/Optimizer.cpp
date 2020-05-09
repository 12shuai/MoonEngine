#include "Optimizer.h"
#include "NNManager.h"
#include "NGraph.h"

namespace MOON {
	namespace NN {

		Optimizer::Optimizer(NGraph* graph, Neuron* target, float learning_rate = 0.01f, int batch_size = 12) {
			this->id = graph->GenNeuronID();
			this->graph = graph;
			this->target = target;
			this->batchSize = batch_size;
			this->lr = learning_rate;

			// Ϊÿ������ѵ���Ľڵ��ۼ�һ��Mini Batch��ȫ���������ݶ�
			//this->acc_gradient.clear();
			this->acc_cnt = 0;
			graph->AddOpt(this);
		}

		void Optimizer::ClearGrad() {
			acc_gradient.clear();
		}

		// ���򴫲������ݶ�
		void Optimizer::Backward() {
			graph->ClearJacobian();
			for (auto iter = graph->neurons.begin(); iter != graph->neurons.end(); iter++) {
				if (typeid(**iter) == typeid(NVariable)) {
					NVariable* node = dynamic_cast<NVariable*>(*iter);
					if (node->trainable) {
						node->Backward(target);

						// ���ս�����������Խڵ�ֵ���������������ſ˱���һ��������������ת�����ݶȣ���������
						// ���ｫ�ݶ� reshape ����ڵ�ֵ��ͬ����״������Խڵ�ֵ���и���
						Matrix gradient = node->jacobi->Reshape(node->Shape());
						//std::cout << "gradient, id: " << node->id << std::endl;
						//std::cout << gradient << std::endl;

						if (acc_gradient.find(node->id) == acc_gradient.end())
							acc_gradient[node->id] = gradient;
						else acc_gradient[node->id] += gradient;
					}
				}
			}
		}

		// ǰ�򴫲��������ڵ�
		void Optimizer::Forward() {
			target->Forward();
		}

		Matrix Optimizer::GetGradient(const Neuron* node) {
			auto n = acc_gradient.find(node->id);
			assert(n != acc_gradient.end());
			//std::cout << "node: " << node->id << " : \n" << *node->value << std::endl;
			//std::cout << "gradient of node: " << node->id << " : \n" << (*n).second / batchSize << std::endl;
			return (*n).second / batchSize;
		}

		void Optimizer::Step() {
			this->Forward();
			this->Backward();
			this->acc_cnt++;

			if (acc_cnt >= batchSize) {
				this->Update();
				this->ClearGrad();
				this->acc_cnt = 0;
			}
		}

	}
}