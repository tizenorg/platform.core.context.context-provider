/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <memory>
#include <queue>
#include "Graph.h"

std::shared_ptr<ctx::graph::Components> ctx::graph::connectedComponents(Graph &graph)
{
	std::shared_ptr<Components> ccs = std::make_shared<Components>();
	std::set<int> fringe;

	for (Node i = 0; i < static_cast<Node>(graph.size()); i++) {
		if (!graph[i])
			continue;

		// neighbourhood of node i exists (was not removed)
		std::shared_ptr<Component> c = std::make_shared<Component>();
		ccs->push_back(c);
		fringe.insert(i);
		while (!fringe.empty()) {
			Node currNode = *fringe.begin();
			fringe.erase(fringe.begin());
			c->insert(currNode);

			std::shared_ptr<NeighbourNodes> currNhood = graph[currNode];
			for (Node nhoodNode : *currNhood) {
				if (graph[nhoodNode] && fringe.find(nhoodNode) == fringe.end())
					fringe.insert(nhoodNode);
			}
			graph[currNode].reset();  // removing current node
		}
	}
	return ccs;
}
