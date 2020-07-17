//
// Created by nikita on 7/16/20.
//

#include "Graph.h"
#include <random>
#include <fstream>

using std::mt19937;
using std::uniform_int_distribution;
using std::random_device;
using std::to_string;
using std::make_shared;
using std::ofstream;
using std::ifstream;
using std::endl;

Graph::~Graph() = default;

void Graph::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  for (const auto &link:links) {
    if (!showOnlySubgraph || isLinkInSubgraph(link)) {
      link.draw(target, states);
    }
  }
  for (const auto &node:nodes) {
    if (!showOnlySubgraph || isNodeInSubgraph(node)) {
      node->draw(target, states);
    }
  }
}

Graph Graph::generatePlanar(size_t numberOfVertices, double maxCoord) {
  Graph graph;
  graph.addNRandomNodes(numberOfVertices, maxCoord);
  graph.addLinksTillConnection();
  return graph;
}

bool Graph::isConnected() const {
  if (!nodes.empty()) {
    map<shared_ptr<Node>, bool> visited;
    visit(nodes.front(), visited);
    for (const auto &node:nodes) {
      if (!visited[node]) {
        return false;
      }
    }
  }
  return true;
}

void Graph::visit(const shared_ptr<Node> &node, map<shared_ptr<Node>, bool> &visited) const {
  visited[node] = true;
  for (const auto &adjacentNode:node->adjacentNodes) {
    if (!visited[adjacentNode]) {
      visit(adjacentNode, visited);
    }
  }
}

mt19937 &getEngine() {
  static mt19937 twisterEngine((random_device()()));
  return twisterEngine;
}

void Graph::addNRandomNodes(size_t numberOfVertices, double maxCoord) {
  auto nodeRadius = Node::NodeSettings::getNodeSettings().radius;
  size_t maximumCanFit = maxCoord / nodeRadius / 4.;

  double offset = 2 * maxCoord * nodeRadius / (maxCoord + 4 * nodeRadius) + nodeRadius;
  double spaceLength = offset + nodeRadius;

  vector<vector<bool>> occupied(maximumCanFit);
  for (auto &row:occupied) {
    row.resize(maximumCanFit);
  }

  uniform_int_distribution distribution(0, int(maximumCanFit - 1));
  auto &engine = getEngine();

  for (int i = 0; i < numberOfVertices; ++i) {
    int x = distribution(engine), y = distribution(engine);
    while (occupied[x][y]) {
      x = distribution(engine);
      y = distribution(engine);
    }
    auto newNode = make_shared<Node>(i, offset + x * spaceLength, offset + y * spaceLength);
    nodes.push_back(newNode);
    occupied[x][y] = true;
  }

  adjacencyMatrix.setDimension(nodes.size());
}

void Graph::addLinksTillConnection() {
  while (!isConnected()) {
    addRandomLink();
  }
}

bool Graph::canAddLinkBetween(size_t firstNodeIndex, size_t secondNodeIndex) const {
  if (firstNodeIndex == secondNodeIndex) {
    return false;
  }

  if (firstNodeIndex > secondNodeIndex) {
    if (adjacencyMatrix.at(firstNodeIndex, secondNodeIndex)) {
      return false;
    }
  } else {
    if (adjacencyMatrix.at(secondNodeIndex, firstNodeIndex)) {
      return false;
    }
  }

  const auto &firstNode = nodes[firstNodeIndex], &secondNode = nodes[secondNodeIndex];
  Link link(firstNode, secondNode);
  for (const auto &otherLink:links) {
    if (link.doIntersect(otherLink)) {
      return false;
    }
  }
  for (const auto &node:nodes) {
    if (node != firstNode && node != secondNode) {
      if (link.doIntersect(*node)) {
        return false;
      }
    }
  }
  return true;
}

void Graph::addRandomLink() {
  uniform_int_distribution distribution(0, int(nodes.size() - 1));
  auto &engine = getEngine();

  int firstNodeIndex = distribution(engine), secondNodeIndex = distribution(engine);
  while (!canAddLinkBetween(firstNodeIndex, secondNodeIndex)) {
    firstNodeIndex = distribution(engine);
    secondNodeIndex = distribution(engine);
  }

  const auto &firstNode = nodes[firstNodeIndex], &secondNode = nodes[secondNodeIndex];
  firstNode->adjacentNodes.push_back(secondNode);
  secondNode->adjacentNodes.push_back(firstNode);

  addLink(firstNodeIndex, secondNodeIndex);
}

void Graph::addLink(size_t firstNodeIndex, size_t secondNodeIndex) {
  const auto &firstNode = nodes[firstNodeIndex], &secondNode = nodes[secondNodeIndex];
  links.emplace_back(firstNode, secondNode);
  if (firstNodeIndex > secondNodeIndex) {
    adjacencyMatrix.at(firstNodeIndex, secondNodeIndex) = true;
  } else {
    adjacencyMatrix.at(secondNodeIndex, firstNodeIndex) = true;
  }
}

string toString(const list<size_t> &mList) {
  if (mList.empty()) {
    return "{}";
  } else {
    string ans = "{" + to_string(mList.front());
    for (auto element = ++mList.begin(); element != mList.end(); ++element) {
      ans += ", " + to_string(*element);
    }
    ans += "}";
    return ans;
  }
}

shared_ptr<Node> getNodeOfCombinationTree(size_t numberOfNodes, size_t numberOfLayers, const list<size_t> &nodeList) {
  auto newNode = make_shared<Node>(0, 0, 0);
  newNode->name = toString(nodeList);
  if (nodeList.size() != numberOfLayers) {
    auto newNodeList = nodeList;
    if (nodeList.empty()) {
      newNodeList.push_back(1);
    } else {
      newNodeList.push_back(nodeList.back() + 1);
    }
    newNode->adjacentNodes.push_back(getNodeOfCombinationTree(numberOfNodes, numberOfLayers, newNodeList));
    while (newNodeList.back() < numberOfNodes - numberOfLayers + nodeList.size() + 1) {
      ++newNodeList.back();
      newNode->adjacentNodes.push_back(getNodeOfCombinationTree(numberOfNodes, numberOfLayers, newNodeList));
    }
  }

  return newNode;
}

size_t getNumberOfLeaves(const shared_ptr<Node> &root) {
  size_t result = 1;
  for (const auto &adjacentNode:root->adjacentNodes) {
    result += getNumberOfLeaves(adjacentNode);
  }
  return result;
}

size_t getTreeWidth(const shared_ptr<Node> &root) {
  if (root->adjacentNodes.empty()) {
    return 1;
  } else {
    size_t result = 0;
    for (const auto &adjacentNode:root->adjacentNodes) {
      result += getTreeWidth(adjacentNode);
    }
    return result;
  }
}

double fixX(const shared_ptr<Node> &root, double offset, double spaceLength, double radius) {
  if (root->adjacentNodes.empty()) {
    root->x = offset + radius;
    return radius * 2;
  } else {
    double treeOffset = 0;
    for (const auto &adjacentNode : root->adjacentNodes) {
      treeOffset += fixX(adjacentNode, offset + treeOffset, spaceLength, radius) + spaceLength;
    }
    treeOffset -= spaceLength;
    root->x = offset + treeOffset / 2.;
    return treeOffset;
  }
}

void fixY(const shared_ptr<Node> &root, double spaceLength, double radius, size_t layer) {
  root->y = spaceLength + radius + layer * (spaceLength + radius * 2);
  for (const auto &adjacentNode:root->adjacentNodes) {
    fixY(adjacentNode, spaceLength, radius, layer + 1);
  }
}

Graph Graph::generateCombinationTree(size_t numberOfNodes, size_t numberOfLayers, double maxCoord) {
  auto treeRoot = getNodeOfCombinationTree(numberOfNodes, numberOfLayers, list<size_t>());
  auto treeWidth = getTreeWidth(treeRoot);

  double radius = Node::NodeSettings::getNodeSettings().radius;
  double spaceLength = (maxCoord - 2. * radius * treeWidth) / (treeWidth + 1.);
  fixX(treeRoot, spaceLength, spaceLength, radius);

  spaceLength = (maxCoord - 2. * radius * (numberOfLayers + 1.)) / (numberOfLayers + 2.);
  fixY(treeRoot, spaceLength, radius, 0);

  Graph graph;
  graph.adjacencyMatrix.setDimension(getNumberOfLeaves(treeRoot));
  graph.nodes.push_back(treeRoot);
  graph.fromTree(treeRoot, 0);

  return graph;
}

void Graph::fromTree(const shared_ptr<Node> &root, size_t rootIndex) {
  for (const auto &adjacentNode:root->adjacentNodes) {
    size_t elementIndex = nodes.size();
    nodes.push_back(adjacentNode);
    adjacentNode->id = elementIndex;
    addLink(rootIndex, elementIndex);
    fromTree(adjacentNode, elementIndex);
  }
}

void Graph::save(const string &name) const {
  ofstream matrixOut("matrix/" + name);
  adjacencyMatrix.writeToStreamFull(matrixOut);
  ofstream nodesOut("nodes/" + name);
  nodesOut << nodes.size() << endl;
  for (const auto &node:nodes) {
    nodesOut << node->x << " " << node->y << " " << node->name << endl;
  }
}

void Graph::load(const string &name) {
  ifstream matrixIn("matrix/" + name);
  ifstream nodesIn("nodes/" + name);
  load(matrixIn, nodesIn);
}

void Graph::showSubgraph(const vector<size_t> &newSubgraph) {
  showOnlySubgraph = true;
  subgraph = newSubgraph;
}

void Graph::showFullGraph() {
  showOnlySubgraph = false;
}

bool Graph::isNodeInSubgraph(shared_ptr<Node> node) const {
  return find(subgraph.begin(), subgraph.end(), node->id) != subgraph.end();
}

bool Graph::isLinkInSubgraph(const Link &link) const {
  return isNodeInSubgraph(link.first) && isNodeInSubgraph(link.second);
}

void Graph::load(istream &matrixIn, istream &nodesIn) {
  adjacencyMatrix.readFromStreamFull(matrixIn);

  size_t n;
  nodesIn >> n;
  for (size_t i = 0; i < n; ++i) {
    double x, y;
    nodesIn >> x >> y;
    auto node = make_shared<Node>(i, x, y);
    if (nodesIn.get() != '\n') {
      getline(nodesIn, node->name);
    }
    nodes.push_back(node);
    for (size_t j = 0; j < i; ++j) {
      if (adjacencyMatrix.at(i, j)) {
        node->adjacentNodes.push_back(nodes[j]);
        nodes[j]->adjacentNodes.push_back(node);
        links.emplace_back(node, nodes[j]);
      }
    }
  }
}
