//
// Created by nikita on 7/16/20.
//
#pragma once

#include "Node.h"

struct Link : public sf::Drawable {
  double a, b, c, l;
  double x1, x2, y1, y2;
  shared_ptr<Node> first, second;

  Link(shared_ptr<Node> first, shared_ptr<Node> second);

  ~Link() override;

  void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

  [[nodiscard]] bool doIntersect(const Link &otherLine) const;

  [[nodiscard]] bool doIntersect(const Node &node) const;
};