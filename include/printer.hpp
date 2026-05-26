#pragma once
#include <iostream>

#include "config.hpp"

constexpr int MIN_LABEL_WIDTH{16};
constexpr int DEFAULT_LABEL_WIDTH{18};

class Printer {
 private:
  int labelWidth{DEFAULT_LABEL_WIDTH};

 public:
  Printer() = default;
  explicit Printer(int width) { setLabelWidth(width); }

  void printer(const DatabaseConfig& config, std::ostream& out = std::cout);

  void setLabelWidth(int width);
  [[nodiscard]] auto getLabelWidth() const -> int { return labelWidth; }
};
