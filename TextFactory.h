//
// Created by nikita on 7/16/20.
//
#pragma once

#include <SFML/Graphics.hpp>

using std::string;
using std::unique_ptr;

class TextFactory {
  unique_ptr<char[]> fontData;
  sf::Font font;
  size_t characterSize = 10;
  sf::Color characterColor;

  TextFactory();

public:
  static TextFactory &getTextFactory();

  void loadFontFromFile(const string &path);

  void setCharacterSize(size_t newCharacterSize);

  void setCharacterColor(sf::Color newCharacterColor);

  sf::Text getText(const string &text) const;
};