//
// Created by Renatus Madrigal on 5/25/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_GNUPLOT_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_GNUPLOT_H

#include <iostream>
#include <string>

namespace phosphorus {

/**
 * @brief A simple gnuplot interface.
 */
class Gnuplot {
  class GnuplotImpl;

public:
  struct PlotArgument {};

  Gnuplot();
  ~Gnuplot();

  // Uncopiable and movable.
  Gnuplot(const Gnuplot &) = delete;
  Gnuplot &operator=(const Gnuplot &) = delete;
  Gnuplot(Gnuplot &&other) noexcept;
  Gnuplot &operator=(Gnuplot &&other) noexcept;

  Gnuplot &setCommand(const std::string &);

  Gnuplot &execute(const std::string &command);

  // iostream support for Gnuplot.
  std::istream &istream();
  std::ostream &ostream();

private:
  static std::string commandPreprocessor(const std::string &);

  // We use a unique pointer to manage the Gnuplot implementation.
  // This allows us to hide the implementation details and
  // implement for Windows and Linux separately.
  std::unique_ptr<GnuplotImpl> impl_;
};

class GnuplotException final : public std::runtime_error {
public:
  explicit GnuplotException(const std::string &what) : runtime_error(what) {}

  [[nodiscard]] const char *what() const noexcept override {
    return runtime_error::what();
  }
};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_GNUPLOT_H
