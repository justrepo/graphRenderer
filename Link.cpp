//
// Created by nikita on 7/16/20.
//

#include "Link.h"
#include <cmath>

Link::Link(shared_ptr<Node> first, shared_ptr<Node> second) : first(move(first)), second(move(second)) {
  x1 = this->first->x;
  y1 = this->first->y;
  x2 = this->second->x;
  y2 = this->second->y;

  a = y2 - y1;
  b = x1 - x2;
  c = x2 * y1 - x1 * y2;

  l = sqrt(a * a + b * b);
}

bool eq(double a, double b) {
  return std::abs(a - b) < 1e-10;
}

bool isIn(double x, double x1, double x2) {
  if (x1 < x2) {
    return x + 1e-10 >= x1 && x - 1e-10 <= x2;
  } else {
    return x + 1e-10 >= x2 && x - 1e-10 <= x1;
  }
}

void Link::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  double nx = a / l, ny = b / l;
  for (int i = -4; i < 5; ++i) {
    sf::Vertex lineVertexes[] = {sf::Vector2f(x1 + i * nx / 2., y1 + i * ny / 2.),
                                 sf::Vector2f(x2 + i * nx / 2., y2 + i * ny / 2.)};
    lineVertexes[0].color = lineVertexes[1].color = sf::Color(225, 156, 36);
    target.draw(lineVertexes, 2, sf::Lines, states);
  }
}

bool Link::doIntersect(const Link &otherLine) const {
  double k = otherLine.a * b - a * otherLine.b;
  if (!eq(k, 0) && !(eq(x1, otherLine.x1) && eq(y1, otherLine.y1) || eq(x2, otherLine.x1) && eq(y2, otherLine.y1) ||
                     eq(x1, otherLine.x2) && eq(y1, otherLine.y2) || eq(x2, otherLine.x2) && eq(y2, otherLine.y2))) {
    double x = (otherLine.b * c - b * otherLine.c) / k, y = (a * otherLine.c - otherLine.a * c) / k;
    if (isIn(x, x1, x2) && isIn(x, otherLine.x1, otherLine.x2) && isIn(y, y1, y2) &&
        isIn(y, otherLine.y1, otherLine.y2)) {
      return true;
    }
  }
  return false;
}

bool Link::doIntersect(const Node &node) const {
  double x0 = node.x, y0 = node.y;
  if (!eq(l, 0) && !(eq(x1, x0) && eq(y1, y0) || eq(x2, x0) && eq(y2, y0))) {
    double x = (b * b * x0 - a * (c + b * y0)) / l / l, y = (a * a * y0 - b * (c + a * x0)) / l / l;
    if (isIn(x, x1, x2) && isIn(y, y1, y2)) {
      double dx = x - x0, dy = y - y0, r = Node::NodeSettings::getNodeSettings().radius;
      if (dx * dx + dy * dy < r * r + 1e-10) {
        return true;
      }
    }
  }
  return false;
}

Link::~Link() = default;
