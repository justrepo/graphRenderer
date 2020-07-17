#include "TextFactory.h"
#include "Graph.h"
#include <TGUI/TGUI.hpp>
#include <filesystem>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

shared_ptr<tgui::Label> createCentredLabel(const sf::String &text) {
  auto newLabel = tgui::Label::create(text);

  newLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
  newLabel->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);

  return newLabel;
}

void saveImageOfGraph(const Graph &graph, const string &name) {
  sf::RenderTexture texture;
  texture.create(600, 600);

  texture.clear(sf::Color::White);
  texture.draw(graph);
  texture.display();

  texture.getTexture().copyToImage().saveToFile("img/" + name + ".png");
}

int main(int argc, char **argv) {
  if (argc == 1) {
    {
      auto &textFactory = TextFactory::getTextFactory();
      textFactory.loadFontFromFile("../fonts/Arial.TTF");
      textFactory.setCharacterColor(sf::Color::Black);
    }
    {
      auto &nodeSettings = Node::NodeSettings::getNodeSettings();
      nodeSettings.color = sf::Color(94, 129, 181);
    }

    Graph graph;

    sf::RenderWindow window(sf::VideoMode(1200, 600), "My window");
    tgui::Gui gui(window);

    shared_ptr<tgui::Canvas> graphCanvas;
    {
      auto centralLayout = tgui::HorizontalLayout::create();
      {
        auto controlsLayout = tgui::VerticalLayout::create();

        shared_ptr<tgui::EditBox> nBox;
        shared_ptr<tgui::EditBox> kBox;
        {
          auto settingsLayout = tgui::HorizontalLayout::create();
          {
            settingsLayout->add(createCentredLabel("radius: "), .3);

            auto radiusBox = tgui::EditBox::create();
            radiusBox->connect(radiusBox->onTextChange.getName(), [&](const string &newString) {
              try {
                Node::NodeSettings::getNodeSettings().radius = stod(newString);
              } catch (const exception &e) {}
            });
            radiusBox->setText("30");
            settingsLayout->add(radiusBox);

            settingsLayout->add(createCentredLabel("character size: "), .4);

            auto characterSizeBox = tgui::EditBox::create();
            characterSizeBox->connect(characterSizeBox->onTextChange.getName(), [&](const string &newString) {
              size_t characterSize;
              try {
                characterSize = stod(newString);
              } catch (const exception &e) {}
              TextFactory::getTextFactory().setCharacterSize(characterSize);
            });
            characterSizeBox->setText("19");
            settingsLayout->add(characterSizeBox);
          }
          controlsLayout->add(settingsLayout);

          auto nkLayout = tgui::HorizontalLayout::create();
          {
            nkLayout->add(createCentredLabel("n: "), .1);

            nBox = tgui::EditBox::create();
            nkLayout->add(nBox);

            nkLayout->add(createCentredLabel("k: "), .1);

            kBox = tgui::EditBox::create();
            nkLayout->add(kBox);
          }
          controlsLayout->add(nkLayout);

          auto generatorsLayout = tgui::HorizontalLayout::create();
          {
            auto planarGenerator = tgui::Button::create("Generate planar graph");
            planarGenerator->connect(planarGenerator->onClick.getName(), [&]() {
              size_t numberOfNodes;
              try {
                numberOfNodes = stoull(nBox->getText().toAnsiString());
              } catch (const exception &e) {
                return;
              }
              graph = Graph::generatePlanar(numberOfNodes, 600);
            });
            generatorsLayout->add(planarGenerator);

            auto treeGenerator = tgui::Button::create("Generate combination tree");
            treeGenerator->connect(treeGenerator->onClick.getName(), [&]() {
              size_t numberOfNodes;
              try {
                numberOfNodes = stoull(nBox->getText().toAnsiString());
              } catch (const exception &e) {
                return;
              }
              size_t numberOfLayers;
              try {
                numberOfLayers = stoull(kBox->getText().toAnsiString());
              } catch (const exception &e) {
                return;
              }
              graph = Graph::generateCombinationTree(numberOfNodes, numberOfLayers, 600);
            });
            generatorsLayout->add(treeGenerator);
          }
          controlsLayout->add(generatorsLayout);

          auto fileNameLayout = tgui::HorizontalLayout::create();
          shared_ptr<tgui::EditBox> fileNameBox;
          {
            fileNameLayout->add(createCentredLabel("File name: "), .1);

            fileNameBox = tgui::EditBox::create();
            fileNameLayout->add(fileNameBox);
          }
          controlsLayout->add(fileNameLayout);

          auto saveLoadLayout = tgui::HorizontalLayout::create();
          {
            auto saveButton = tgui::Button::create("Save graph");
            saveButton->connect(saveButton->onClick.getName(), [&]() {
              graph.save(fileNameBox->getText());
            });
            saveLoadLayout->add(saveButton);

            auto loadButton = tgui::Button::create("Load graph");
            loadButton->connect(loadButton->onClick.getName(), [&]() {
              graph.load(fileNameBox->getText());
            });
            saveLoadLayout->add(loadButton);

            auto saveImageButton = tgui::Button::create("Save image of graph");
            saveImageButton->connect(saveImageButton->onClick.getName(), [&]() {
              saveImageOfGraph(graph, fileNameBox->getText());
            });
            saveLoadLayout->add(saveImageButton);
          }
          controlsLayout->add(saveLoadLayout);

          auto subgraphLayout = tgui::HorizontalLayout::create();
          shared_ptr<tgui::EditBox> subgraphBox;
          {
            subgraphLayout->add(createCentredLabel("vertices: "));

            subgraphBox = tgui::EditBox::create();
            subgraphLayout->add(subgraphBox);
          }
          controlsLayout->add(subgraphLayout);

          auto subgraphButtonsLayout = tgui::HorizontalLayout::create();
          {
            auto showButton = tgui::Button::create("Show selected subgraph");
            showButton->connect(showButton->onClick.getName(), [&]() {
              if (showButton->getText()[5] == 's') {
                string subgraphString = subgraphBox->getText();
                stringstream subgraphStream(subgraphString);
                vector<size_t> subgraph;
                while (subgraphStream) {
                  size_t node;
                  subgraphStream >> node;
                  subgraphStream.get();
                  subgraph.push_back(node);
                }
                graph.showSubgraph(subgraph);
                showButton->setText("Show full graph");
              } else {
                graph.showFullGraph();
                showButton->setText("Show selected subgraph");
              }
            });
            subgraphButtonsLayout->add(showButton);
          }
          controlsLayout->add(subgraphButtonsLayout);

          controlsLayout->addSpace(5);
        }
        centralLayout->add(controlsLayout);

        graphCanvas = tgui::Canvas::create();
        centralLayout->add(graphCanvas);
      }
      gui.add(centralLayout);
    }

    while (window.isOpen()) {
      sf::Event event{};
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed ||
            (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
          window.close();
        }
        gui.handleEvent(event);
      }

      graphCanvas->clear(sf::Color::White);
      graphCanvas->draw(graph);
      graphCanvas->display();

      window.clear(sf::Color::White);
      gui.draw();
      window.display();
    }
  } else {
    {
      auto &textFactory = TextFactory::getTextFactory();
      textFactory.loadFontFromFile("../fonts/Arial.TTF");
      textFactory.setCharacterSize(19);
      textFactory.setCharacterColor(sf::Color::Black);
    }
    {
      auto &nodeSettings = Node::NodeSettings::getNodeSettings();
      nodeSettings.color = sf::Color(94, 129, 181);
      nodeSettings.radius = 10;
    }
    fs::path dir = argv[1];
    for (const auto &entry:fs::directory_iterator(dir)) {
      auto fileName = entry.path().filename().string();
      if (fileName.find("test_") == 0 && fileName.find("_result") == string::npos) {
        ifstream matrixIn(entry.path());
        matrixIn.ignore(numeric_limits<streamsize>::max(), '\n');
        matrixIn.ignore(numeric_limits<streamsize>::max(), '\n');
        ifstream nodesIn("nodes/" + fileName);
        Graph graph;
        graph.load(matrixIn, nodesIn);

        ifstream subgraphIn(entry.path().string() + "_result");
        vector<size_t> subgraph;
        while (subgraphIn.get() != '}') {
          size_t node;
          subgraphIn >> node;
          subgraph.push_back(node);
        }
        graph.showSubgraph(subgraph);
        saveImageOfGraph(graph, fileName + "_sel");
      }
    }
  }
  return 0;
}
