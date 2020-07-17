//
// Created by nikita on 7/16/20.
//

#include "Node.h"
#include "TextFactory.h"

using std::to_string;

Node::Node(size_t id, double x, double y) : id(id), name(to_string(id)), x(x), y(y) {}

Node::~Node() = default;

void Node::draw(sf::RenderTarget &target, sf::RenderStates states) const {
  auto &nodeSettings = NodeSettings::getNodeSettings();
  sf::CircleShape circleShape((float(nodeSettings.radius)));
  circleShape.setFillColor(nodeSettings.color);
  circleShape.setPosition(float(x - nodeSettings.radius), float(y - nodeSettings.radius));

  auto text = TextFactory::getTextFactory().getText(name);
  {
    const auto &textLocalBounds = text.getLocalBounds();
    text.setPosition(sf::Vector2f(x - textLocalBounds.width / 2. - textLocalBounds.left,
                                  y - textLocalBounds.height / 2. - textLocalBounds.top));
  }

  target.draw(circleShape, states);
  target.draw(text, states);
}

Node::NodeSettings &Node::NodeSettings::getNodeSettings() {
  static NodeSettings nodeSettings;
  return nodeSettings;
}

Node::NodeSettings::NodeSettings() = default;
