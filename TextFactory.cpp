//
// Created by nikita on 7/16/20.
//

#include "TextFactory.h"
#include <fstream>

using std::ifstream;
using std::runtime_error;
using std::make_unique;

TextFactory::TextFactory() = default;

TextFactory &TextFactory::getTextFactory() {
  static TextFactory textFactory;
  return textFactory;
}

void TextFactory::loadFontFromFile(const string &path) {
  fontData = make_unique<char[]>(1000000);

  ifstream in(path);
  if (!in) {
    throw runtime_error("Unable to open file " + path);
  }

  in.read(fontData.get(), 1000000);
  size_t size = in.tellg();

  if (!font.loadFromMemory(fontData.get(), size)) {
    throw runtime_error("Unable to load font from file " + path);
  }
}

void TextFactory::setCharacterSize(size_t newCharacterSize) {
  characterSize = newCharacterSize;
}

void TextFactory::setCharacterColor(sf::Color newCharacterColor) {
  characterColor = newCharacterColor;
}

sf::Text TextFactory::getText(const string &text) const {
  sf::Text result(text, font, characterSize);
  result.setFillColor(characterColor);
  return result;
}
