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

#ifndef _CONTEXT_PLACE_RECOGNITION_GRAPH_H_
#define _CONTEXT_PLACE_RECOGNITION_GRAPH_H_

#include <memory>
#include <vector>
#include <set>

namespace ctx {

namespace graph {

	typedef int Node;
	typedef std::set<Node> NeighbourNodes;
	typedef std::vector<std::shared_ptr<NeighbourNodes>> Graph;
	typedef std::set<Node> Component;
	typedef std::vector<std::shared_ptr<Component>> Components;

	/*
	 * make connected components of a given graph
	 * caution: the graph will be changed (its nodes will be cleared)
	 */
	std::shared_ptr<Components> connectedComponents(Graph &graph);

}	/* namespace graph */

}	/* namespace ctx */

#endif /* End of _CONTEXT_PLACE_RECOGNITION_GRAPH_H_ */
