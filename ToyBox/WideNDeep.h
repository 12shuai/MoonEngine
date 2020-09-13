#pragma once
#include <vector>
#include <imgui.h>

#include "NNBase.h"
#include "MathUtils.h"
#include "Plotter.h"

namespace MOON {
	namespace NN {

		class WideDeep : public NNBase {
		public:

			NVariable* weight;
			NVariable* bias;
			Neuron* prob;

			// construct a wide & deep network
			WideDeep(NGraph* graph, int input_size, int output_size = 2, const LossFunction lossFunc = CE_SM,
				const OptimizerType optFunc = ADAM) : NNBase(input_size, output_size, graph) {
				feature = new NVariable(Vector2(input_size, 1), graph, false, false);
				weight = new NVariable(Vector2(output_size, input_size), graph, true, true);
				bias = new NVariable(Vector2(output_size, 1), graph, true, true);

				predict = new NAdd({
					new NMatMul({
						weight,
						feature
					}, graph),
					bias
					}, graph);

				// 对模型输出的预测值施加SoftMax得到多分类概率
				prob = new SoftMax(predict, graph);
				loss = graph->CreateLossFunction(lossFunc, { predict, label });
				opt = graph->CreateOptimizer(optFunc, loss);
			}

			static void wide_and_deep() {
				int epoches = 60, batches = 50;

				// TODO: generate fake data here

				auto graph = NNM::globalNNGraph;
				WideDeep WD(graph, 5);

				for (int e = 0; e < epoches; e++) {
					for (int b = 0; b < batches; b++) {
						//WD.feature->SetVal(...);
						//WD.label->SetVal(...);
						WD.prob->Forward();
						WD.loss->Forward();
						WD.opt->Step();
					}
				}

				// TODO: print result here

			}

		};

	}
}