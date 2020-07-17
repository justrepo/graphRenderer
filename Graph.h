//
// Created by nikita on 7/16/20.
//
#pragma once

#include "Link.h"
#include "TriangleBoolSquareMatrix.h"
#include <list>

using std::map;
using std::ifstream;
using std::list;

class Graph : public sf::Drawable {
  vector<shared_ptr<Node>> nodes;
  vector<Link> links;
  TriangleBoolSquareMatrix adjacencyMatrix;

  bool showOnlySubgraph = false;
  vector<size_t> subgraph;

  void visit(const shared_ptr<Node> &node, map<shared_ptr<Node>, bool> &visited) const;

  void addNRandomNodes(size_t numberOfVertices, double maxCoord);

  void addLinksTillConnection();

  [[nodiscard]] bool canAddLinkBetween(size_t firstNodeIndex, size_t secondNodeIndex) const;

  void addRandomLink();

  void addLink(size_t firstNodeIndex, size_t secondNodeIndex);

  void fromTree(const shared_ptr<Node> &root, size_t rootIndex);

  [[nodiscard]] bool isNodeInSubgraph(shared_ptr<Node> node) const;

  [[nodiscard]] bool isLinkInSubgraph(const Link &link) const;

public:
  ~Graph() override;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

  [[nodiscard]] bool isConnected() const;

  static Graph generatePlanar(size_t numberOfVertices, double maxCoord);

  static Graph generateCombinationTree(size_t numberOfNodes, size_t numberOfLayers, double maxCoord);

  void save(const string &name) const;

  void load(const string &name);

  void load(istream &matrixIn, istream &nodesIn);

  void showSubgraph(const vector<size_t> &newSubgraph);

  void showFullGraph();
};