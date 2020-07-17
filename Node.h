//
// Created by nikita on 7/16/20.
//
#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

using std::string;
using std::vector;
using std::shared_ptr;

struct Node : public sf::Drawable {
  struct NodeSettings {
    double radius = 10;
    sf::Color color;

    static NodeSettings &getNodeSettings();

  private:
    NodeSettings();
  };

  size_t id;
  string name;
  double x = 0, y = 0;
  vector<shared_ptr<Node>> adjacentNodes;

  Node(size_t id, double x, double y);

  ~Node() override;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};