//
// Created by Renatus Madrigal on 5/26/2025.
//

// Fuck the GOD DAMN Windows API.
#define _WIN32_WINNT 0x0601
#define WIN32_LEAN_AND_MEAN

#include "phosphorus/Gnuplot.h"
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <format>
#include <fstream>
#include <random>
#include <range/v3/view.hpp>
#include <sstream>
#include <utility>

namespace asio = boost::asio;
namespace bp = boost::process;
namespace fs = boost::filesystem;
using path = boost::filesystem::path;
using error_code = boost::system::error_code;

namespace phosphorus {

class Gnuplot::GnuplotImpl {
  friend class Gnuplot;

public:
  static constexpr const char *kGnuplotExecutable = "gnuplot";

  GnuplotImpl() = default;
  ~GnuplotImpl() { stop(); }

  GnuplotImpl(const GnuplotImpl &) = delete;
  GnuplotImpl &operator=(const GnuplotImpl &) = delete;
  GnuplotImpl(GnuplotImpl &&other) noexcept = delete;
  GnuplotImpl &operator=(GnuplotImpl &&other) noexcept = delete;

  auto setCommand(const std::string &command) -> GnuplotImpl & {
    this->command_ = command;
    return *this;
  }

  auto start() -> GnuplotImpl & {
    // Reset the gnuplot_ pointer.
    auto ptr = new bp::process{ctx_,     // io_context
                               command_, // gnuplot executable path
                               {},       // subprocess arguments
                               bp::process_stdio{
                                   .in = child_stdin_,   // stdin of subprocess
                                   .out = child_stdout_, // stdout of subprocess
                                   .err = child_stderr_  // stderr of subprocess
                               }};

    gnuplot_.reset(ptr);

    return *this;
  }

  void stop() const {
    if (running()) {
      gnuplot_->terminate();
    }
  }

  void execute(const std::string &command) {
    if (!running()) {
      throw GnuplotException("Gnuplot process is not running");
    }

    child_stdin_.async_write_some(
        asio::buffer(command.data(), command.size()),
        [this](const boost::system::error_code &ec, std::size_t) {
          if (ec) {
            throw GnuplotException("Gnuplot error: " + ec.message());
          }
        });
  }

  [[nodiscard]] bool running() const {
    boost::system::error_code ec;
    return gnuplot_ && gnuplot_->running(ec);
  }

  [[nodiscard]] auto wait() const {
    return gnuplot_->wait(); // Wait for the gnuplot process to finish
  }

private:
  // Use a custom deleter for the gnuplot process to ensure it is terminated
  struct BPProcessDeleter {
    void operator()(bp::process *proc) const {
      if (error_code ec; proc && proc->running(ec)) {
        proc->terminate();
        delete proc;
      }
    }
  };
  using ProcessPtr = std::unique_ptr<bp::process, BPProcessDeleter>;

  asio::io_context ctx_{};
  asio::writable_pipe child_stdin_{ctx_};
  asio::readable_pipe child_stdout_{ctx_};
  asio::readable_pipe child_stderr_{ctx_};
  ProcessPtr gnuplot_ = nullptr;
  path command_ = bp::environment::find_executable(kGnuplotExecutable);
};

// The implementation of Gnuplot methods
Gnuplot::Gnuplot() : impl_(std::make_unique<GnuplotImpl>()) { impl_->start(); }

Gnuplot::~Gnuplot() {
  if (impl_) {
    impl_->stop();
  }
}

Gnuplot::Gnuplot(Gnuplot &&other) noexcept : impl_(std::move(other.impl_)) {}
Gnuplot &Gnuplot::operator=(Gnuplot &&other) noexcept {
  if (this != &other) {
    impl_ = std::move(other.impl_);
  }
  return *this;
}

Gnuplot &Gnuplot::execute(const std::string &command) {
  if (!impl_) {
    throw GnuplotException("Gnuplot instance is not initialized");
  }
  if (!impl_->running()) {
    impl_->start();
  }
  impl_->execute(commandPreprocessor(command));
  return *this;
}

std::string Gnuplot::commandPreprocessor(const std::string &command) {
  std::string processed_command = command;
  while (!processed_command.empty() && processed_command.back() == '\n') {
    processed_command.pop_back(); // Remove trailing '\n'
  }
  processed_command += '\n';
  return processed_command;
}

struct Gnuplot::TempFileGuard {
  explicit TempFileGuard(std::string filename)
      : filename_(std::move(filename)) {}
  ~TempFileGuard() {
    try {
      if (fs::exists(filename_))
        fs::remove(filename_);
    } catch (const fs::filesystem_error &e) {
      // Log the error or handle it as needed
      std::cerr << e.what() << '\n';
    }
  }

  std::string filename_;
};

void Gnuplot::generateDataBlock(const std::string &filename,
                                std::span<double> x, std::span<double> y) {
  using std::format;
  std::fstream file(filename, std::ios::app);
  if (!file.is_open()) {
    throw GnuplotException("Failed to open file: " + filename);
  }
  for (auto [x, y] : ranges::views::zip(x, y)) {
    file << format("{:.9f} {:.9f}\n", x, y);
  }
  file << "\n\n"; // End of data block
  file.close();
}

std::string Gnuplot::generateFigureCommand() const {
  using std::format;
  static constexpr std::pair kDefaultPair = {0.0, 0.0};
  std::stringstream command;
  if (figure_config_.xrange != kDefaultPair)
    command << format("set xrange [{}:{}]\n", //
                      figure_config_.xrange.first,
                      figure_config_.xrange.second);
  if (figure_config_.yrange != kDefaultPair)
    command << format("set yrange [{}:{}]\n", //
                      figure_config_.yrange.first,
                      figure_config_.yrange.second);
  if (!figure_config_.xlabel.empty()) {
    command << format("set xlabel '{}'", figure_config_.xlabel);
    if (figure_config_.xoffset != kDefaultPair) {
      command << format("offset x {}:{}", figure_config_.xoffset.first,
                        figure_config_.xoffset.second);
    }
    command << '\n';
  }
  if (!figure_config_.ylabel.empty()) {
    command << format("set ylabel '{}'", figure_config_.ylabel);
    if (figure_config_.yoffset != kDefaultPair) {
      command << format("offset y {}:{}", figure_config_.yoffset.first,
                        figure_config_.yoffset.second);
    }
    command << '\n';
  }
  if (figure_config_.grid) {
    command << "set grid\n";
  }
  return command.str();
}

std::string Gnuplot::generatePlotCommand(const std::string &temp_file) const {
  using std::format;
  static constexpr std::pair kDefaultPair = {0.0, 0.0};
  std::stringstream command;
  if (figure_config_.xrange != kDefaultPair)
    command << format("set xrange [{}:{}]\n", //
                      figure_config_.xrange.first,
                      figure_config_.xrange.second);
  if (figure_config_.yrange != kDefaultPair)
    command << format("set yrange [{}:{}]\n", //
                      figure_config_.yrange.first,
                      figure_config_.yrange.second);
  if (!figure_config_.xlabel.empty()) {
    command << format("set xlabel '{}'", figure_config_.xlabel);
    if (figure_config_.xoffset != kDefaultPair) {
      command << format("offset x {}:{}", figure_config_.xoffset.first,
                        figure_config_.xoffset.second);
    }
    command << '\n';
  }
  if (!figure_config_.ylabel.empty()) {
    command << format("set ylabel '{}'", figure_config_.ylabel);
    if (figure_config_.yoffset != kDefaultPair) {
      command << format("offset y {}:{}", figure_config_.yoffset.first,
                        figure_config_.yoffset.second);
    }
    command << '\n';
  }
  if (figure_config_.grid) {
    command << "set grid\n";
  }

  command << format("plot ");
  for (auto &&[index, plot_config] : ranges::views::enumerate(plot_configs_)) {
    if (index > 0) {
      command << ", ";
    }
    command << format("'{}' ", temp_file);
    command << format("index {} ", index);
    command << format("using 1:2 ");
    if (plot_config.every != std::pair<int, int>{0, 0}) {
      command << format("every ::{}::{} ", plot_config.every.first,
                        plot_config.every.second);
    }
    if (plot_config.with != PlotConfig::PlotType::None) {
      command << format("with {} ",
                        PlotConfig::plotTypeToString(plot_config.with));
      if (!plot_config.style.empty()) {
        command << format("{}", plot_config.style);
      }
    }
    if (!plot_config.title.empty()) {
      command << format("title '{}' ", plot_config.title);
    }
    if (plot_config.smooth != PlotConfig::SmoothType::None) {
      command << format("smooth {} ",
                        PlotConfig::smoothTypeToString(plot_config.smooth));
    }
    // generateDataBlock(temp_file, plot_config.x, plot_config.y);
  }
  command << "\n"; // End of plot command
  return command.str();
}

#define TEMP_FILE_GUARD(guard)                                                 \
  using std::format;                                                           \
  static std::random_device rd;                                                \
  static std::mt19937 gen(rd());                                               \
  static std::uniform_int_distribution<> dis{};                                \
  TempFileGuard guard(format("{}.dat", dis(gen)))

Gnuplot &Gnuplot::savefig(const std::string &filename) {
  TEMP_FILE_GUARD(guard);

  auto output = format("{}.png", filename);

  if (fs::exists(output)) {
    fs::remove(output); // Remove existing file if it exists
  }

  for (auto &&[index, plot_config] : ranges::views::enumerate(plot_configs_)) {
    generateDataBlock(guard.filename_, plot_config.x, plot_config.y);
  }

  std::stringstream command;
  command << format("set terminal pngcairo enhanced size 800,600\n");
  command << format("set output '{}'\n", output);
  command << generatePlotCommand(guard.filename_);
  command << "replot\n"; // Replot to save the current data
  this->execute(command.str());

  // Let gnuplot know to exit after saving the figure
  // So we can clean up the process and temp file.
  this->execute("exit\n");
  auto res = impl_->wait();
  if (res != 0) {
    throw GnuplotException("Gnuplot process exited with code: " +
                           std::to_string(res));
  }
  std::cout << "Figure saved to " << output << std::endl;

  return *this;
}

Gnuplot &Gnuplot::show() {
  TEMP_FILE_GUARD(guard);

  std::stringstream command;
  command << "set terminal wxt enhanced size 800,600\n";
  command << "set output\n"; // Reset output to default (screen)
  command << generatePlotCommand(guard.filename_);
  command << "replot\n"; // Replot to show the current data

  this->execute(command.str());

  std::cout << "Press any key to continue...\n";
  getchar(); // Press any key to continue

  return *this;
}

int Gnuplot::wait() const { return impl_->wait(); }

} // namespace phosphorus
