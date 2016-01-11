/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#ifndef __CONTEXT_PLACE_STATUS_GRAPH_H__
#define __CONTEXT_PLACE_STATUS_GRAPH_H__

#include <memory>
#include <vector>
#include <set>

namespace ctx {

	typedef int node_t;
	typedef std::set<node_t> nhood_t; // neighbouring nodes
	typedef std::vector<std::shared_ptr<nhood_t>> graph_t;
	typedef std::set<node_t> component_t;
	typedef std::vector<std::shared_ptr<component_t>> components_t;

	/*
	 * make connected components of a given graph
	 * caution: the graph will be changed (its nodes will be cleared)
	 */
	std::shared_ptr<components_t> connected_components(graph_t &graph);

}	/* namespace ctx */

#endif /* __CONTEXT_PLACE_STATUS_GRAPH_H__ */
