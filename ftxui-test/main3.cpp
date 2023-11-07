// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <stdlib.h>                // for EXIT_SUCCESS
#include <ftxui/dom/elements.hpp>  // for text, operator|, vbox, border, Element, Fit, hbox
#include <ftxui/screen/screen.hpp>  // for Full, Screen
#include <memory>                   // for allocator

#include <algorithm>  // for min
#include <memory>     // for make_shared
#include <string>     // for string, wstring
#include <utility>    // for move
#include <vector>     // for vector

#include "ftxui/dom/node.hpp"      // for Render
#include "ftxui/screen/color.hpp"  // for ftxui

#include "ftxui/dom/deprecated.hpp"   // for text, vtext
#include "ftxui/dom/elements.hpp"     // for Element, text, vtext
#include "ftxui/dom/node.hpp"         // for Node
#include "ftxui/dom/requirement.hpp"  // for Requirement
#include "ftxui/screen/box.hpp"       // for Box
#include "ftxui/screen/screen.hpp"    // for Pixel, Screen
#include "ftxui/screen/string.hpp"  // for string_width, Utf8ToGlyphs, to_string

namespace ftxui {
    class MyText : public Node {
    public:
    explicit MyText(const std::string& text) : text_(text) {}

    void ComputeRequirement() override {
        requirement_.min_x = string_width(text_);
        requirement_.min_y = 1;
    }

    void Render(Screen& screen) override {
        int x = box_.x_min;
        const int y = box_.y_min;
        if (y > box_.y_max) {
        return;
        }
        for (const auto& cell : Utf8ToGlyphs(text_)) {
        if (x > box_.x_max) {
            return;
        }
        if (cell == "\n") {
            continue;
        }
        screen.PixelAt(x, y).character = cell;
        ++x;
        }
    }

    private:
    const std::string& text_;
    };
}

int main() {
  using namespace ftxui;

  std::string changeable_text = "oi";

    auto my_text = std::make_shared<MyText>(changeable_text);

int i = 0;

  auto document =  //
      hbox({
          vbox({
              text("Line 1"),
              text("Line 2"),
              text("Line 3"),
          }) | border,

          vbox({
              text("Line 4"),
              text("Line 5"),
              text("Line 6"),
          }) | border,

          vbox({
              text("Line 7"),
              text("Line 8"),
              my_text,
          }) | border,
      });
  //auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
//  auto screen = Screen::Create(Dimension::Full(), Dimension::Full());
//}
  while (1) {
  auto screen = Screen::Create(Dimension::Full(), Dimension::Full());

    changeable_text = std::string("oi ") + std::to_string(i);
    Render(screen, document);
    screen.Print();

    i += 5;
  }

  return EXIT_SUCCESS;
}
