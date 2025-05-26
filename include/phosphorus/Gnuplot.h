//
// Created by Renatus Madrigal on 5/25/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_GNUPLOT_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_GNUPLOT_H

#include <iostream>
#include <string>
#include <utility>

namespace phosphorus {

/**
 * @brief A simple gnuplot interface.
 */
class Gnuplot {
  class GnuplotImpl;

public:
  static constexpr const char *kGnuplotExecutable = "gnuplot -p";

  Gnuplot();
  ~Gnuplot();

  // Uncopiable and movable.
  Gnuplot(const Gnuplot &) = delete;
  Gnuplot &operator=(const Gnuplot &) = delete;
  Gnuplot(Gnuplot &&other) noexcept;
  Gnuplot &operator=(Gnuplot &&other) noexcept;

  Gnuplot &setCommand(const std::string &);

  Gnuplot &execute(const std::string &command);

  [[nodiscard]] std::string &getBuffer() const;

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

class GnuplotException final : public std::exception {
public:
  explicit GnuplotException(std::string message) : what_(std::move(message)) {}

  [[nodiscard]] const char *what() const noexcept override {
    return what_.c_str();
  }

private:
  std::string what_;
};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_GNUPLOT_H
