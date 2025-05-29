//
// Created by Renatus Madrigal on 5/25/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_GNUPLOT_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_GNUPLOT_H

#include "phosphorus/Coordinate.h"
#include <iostream>
#include <span>
#include <string>
#include <vector>

namespace phosphorus {

/**
 * @brief A simple gnuplot interface.
 */
class Gnuplot {
  class GnuplotImpl;
  friend class AnimateGenerator;

public:
  struct PlotConfig {
    enum class PlotType {
      None,
      Dots,
      Lines,
      Points,
      LinesPoints,
      Boxes,
    };

    enum class SmoothType {
      None,
      Unique,
      Acsplines,
      Beziers,
      Csplines,
      Sbeziers,
    };

    static constexpr std::string plotTypeToString(PlotType type) {
      switch (type) {
      case PlotType::Dots:
        return "dots";
      case PlotType::Lines:
        return "lines";
      case PlotType::Points:
        return "points";
      case PlotType::LinesPoints:
        return "linespoints";
      case PlotType::Boxes:
        return "boxes";
      default:
        return "";
      }
    }

    static constexpr std::string smoothTypeToString(SmoothType type) {
      switch (type) {
      case SmoothType::Unique:
        return "unique";
      case SmoothType::Acsplines:
        return "acsplines";
      case SmoothType::Beziers:
        return "beziers";
      case SmoothType::Csplines:
        return "csplines";
      case SmoothType::Sbeziers:
        return "sbeziers";
      default:
        return "";
      }
    }

    std::span<double> x{};                // x data
    std::span<double> y{};                // y data
    int index = -1;                       // index of the data in the file
    std::pair<int, int> every{0, 0};      // every nth point to plot
    PlotType with = PlotType::None;       // line type
    std::string title{};                  // title of line
    SmoothType smooth = SmoothType::None; // smoothing type
    std::string style{};                  // style of line, e.g., "lines lw 2"
  };

  struct FigureConfig {
    std::pair<double, double> xrange{0.0, 0.0};  // x-axis range
    std::pair<double, double> yrange{0.0, 0.0};  // y-axis range
    std::pair<double, double> xoffset{0.0, 0.0}; // offset for the x-axis
    std::pair<double, double> yoffset{0.0, 0.0}; // offset for the y-axis
    std::string xlabel{};                        // x-axis label
    std::string ylabel{};                        // y-axis label
    bool grid = true;                            // show grid
  };

  Gnuplot();
  ~Gnuplot();

  // Uncopiable and movable.
  Gnuplot(const Gnuplot &) = delete;
  Gnuplot &operator=(const Gnuplot &) = delete;
  Gnuplot(Gnuplot &&other) noexcept;
  Gnuplot &operator=(Gnuplot &&other) noexcept;

  Gnuplot &execute(const std::string &command);

  Gnuplot &setFigureConfig(const FigureConfig &config) {
    figure_config_ = config;
    return *this;
  }

  Gnuplot &plot(const PlotConfig &config) {
    plot_configs_.emplace_back(config);
    return *this;
  }

  Gnuplot &clear() {
    plot_configs_.clear();
    return *this;
  }

  Gnuplot &savefig(const std::string &filename);
  Gnuplot &show();

  int wait() const;

private:
  struct TempFileGuard;

  static std::string commandPreprocessor(const std::string &);

  // TODO: Redesign this using range-like api
  static void generateDataBlock(const std::string &filename,
                                std::span<double> x, std::span<double> y);
  [[nodiscard]] std::string generatePlotCommand(const std::string &) const;
  [[nodiscard]] std::string generateFigureCommand() const;

  FigureConfig figure_config_;
  std::vector<PlotConfig> plot_configs_;

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
