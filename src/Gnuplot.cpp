//
// Created by Renatus Madrigal on 5/26/2025.
//

#include "phosphorus/Gnuplot.h"

#if defined(_WIN32) || defined(_WIN64)
#include "Windows.h"

#include <iomanip>

namespace phosphorus {

class Gnuplot::GnuplotImpl {
  friend class Gnuplot;

public:
  static constexpr const char *kGnuplotPrompt = "gnuplot> ";
  static constexpr const char *kGnuplotEndFlag = "End";

  /**
   * @brief Constructs a Gnuplot instance and starts the gnuplot process.
   * @details We do not start the gnuplot process in the constructor
   */
  GnuplotImpl() = default;

  GnuplotImpl(const GnuplotImpl &) = delete;
  GnuplotImpl &operator=(const GnuplotImpl &) = delete;

  GnuplotImpl(GnuplotImpl &&other) noexcept
      : hChildStdInWrite(other.hChildStdInWrite),
        hChildStdOutRead(other.hChildStdOutRead), piChild(other.piChild),
        command_(std::move(other.command_)) {
    other.hChildStdInWrite = nullptr;
    other.hChildStdOutRead = nullptr;
  }

  ~GnuplotImpl() {
    stop(); // Ensure the gnuplot process is stopped
  }

  GnuplotImpl &operator=(GnuplotImpl &&other) noexcept {
    if (this != &other) {
      hChildStdInWrite = other.hChildStdInWrite;
      hChildStdOutRead = other.hChildStdOutRead;
      piChild = other.piChild;
      command_ = std::move(other.command_);

      other.hChildStdInWrite = nullptr;
      other.hChildStdOutRead = nullptr;
    }
    return *this;
  }

  auto setCommand(const std::string &command) -> GnuplotImpl & {
    command_ = command;
    return *this;
  }

  auto start() -> GnuplotImpl & {
    // Change code page to UTF-8 for gnuplot
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Temporary handlers
    HANDLE hChildStdInRead = nullptr, hChildStdOutWrite = nullptr;

    SECURITY_ATTRIBUTES sa = {sizeof(sa), nullptr, TRUE};

    // Set up the pipes for standard input and output
    CreatePipe(&hChildStdInRead, &hChildStdInWrite, &sa, 0);
    CreatePipe(&hChildStdOutRead, &hChildStdOutWrite, &sa, 0);

    // SetHandleInformation(hChildStdInRead, HANDLE_FLAG_INHERIT, 0);

    // Set up the STARTUPINFO structure
    STARTUPINFOA si = {sizeof(si)};
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hChildStdInRead;
    si.hStdOutput = hChildStdOutWrite;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    auto create_result = CreateProcessA(      // Create a new process
        nullptr,                              // Application name
        const_cast<char *>(command_.c_str()), // Command line
        nullptr,                              // Process handle not inheritable
        nullptr,                              // Thread handle not inheritable
        TRUE,                                 // Set handle inheritance to TRUE
        CREATE_NO_WINDOW,                     // No creation flag
        nullptr,                              // Use parent's environment
        nullptr,                              // Use parent's current directory
        &si,     // Pointer to STARTUPINFO structure
        &piChild // Pointer to PROCESS_INFORMATION structure
    );
    if (!create_result) {
      throw GnuplotException("Failed to start gnuplot process: " +
                             std::to_string(GetLastError()));
    }

    // Close the unnecessary handles
    CloseHandle(hChildStdInRead);
    CloseHandle(hChildStdOutWrite);

    return *this;
  }

  void stop() {
    if (hChildStdInWrite) {
      CloseHandle(hChildStdInWrite);
      hChildStdInWrite = nullptr;
    }
    if (hChildStdOutRead) {
      CloseHandle(hChildStdOutRead);
      hChildStdOutRead = nullptr;
    }
    if (piChild.hProcess) {
      TerminateProcess(piChild.hProcess, 0);
      DWORD exit_code = 0;
      if (GetExitCodeProcess(piChild.hProcess, &exit_code) &&
          exit_code == STILL_ACTIVE) {
        TerminateProcess(piChild.hProcess, 1);
      }
      CloseHandle(piChild.hProcess);
      piChild.hProcess = nullptr;
    }
    if (piChild.hThread) {
      CloseHandle(piChild.hThread);
      piChild.hThread = nullptr;
    }
  }

  bool waitForPrompt(size_t timeout = 5000) {
    static constexpr size_t kInterval = 50; // milliseconds
    static constexpr size_t kBufferSize = 1024;

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    static BYTE buffer[kBufferSize];

    DWORD total_wait = 0;
    DWORD byte_available = 0;

    while (total_wait < timeout) {
      if (!PeekNamedPipe(hChildStdOutRead, // Child standard output read handle
                         nullptr,          // Pointer to buffer (not used)
                         0,                // Number of bytes to read
                         nullptr,          // Pointer to number of bytes read
                         &byte_available,  // Number of bytes available
                         nullptr           // Number of bytes read
                         )) {
        throw GnuplotException("PeekNamedPipe failed" +
                               std::to_string(GetLastError()));
      }

      if (byte_available > 0) {
        DWORD bytes_read = 0;
        if (!ReadFile(hChildStdOutRead, // Child standard output read handle
                      buffer,           // Buffer to read into
                      kBufferSize - 1,  // Size of the buffer
                      &bytes_read,      // Pointer to number of bytes read
                      nullptr)          // Pointer to overlapped structure
        ) {
          throw GnuplotException("ReadFile failed: " +
                                 std::to_string(GetLastError()));
        }
        for (size_t i = 0; i < bytes_read; i++) {
          std::cout << std::hex << std::setw(2) << std::setfill('0')
                    << static_cast<int>(buffer[i]) << " ";
        }
        std::cout << std::endl;
        buffer[bytes_read] = 0; // Null-terminate the buffer
        buffer_.append(reinterpret_cast<char *>(buffer), bytes_read);

        // Check if the prompt is in the output
        if (strstr(reinterpret_cast<const char *>(buffer), kGnuplotEndFlag)) {
          return true; // Prompt found
        }
      }

      // Sleep for a short interval before checking again
      Sleep(kInterval);
      total_wait += kInterval;
    }
    std::cout << "Length: " << buffer_.size() << std::endl;
    for (auto c : buffer_) {
      std::cout << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(c) << " ";
    }
    std::cout << std::endl;
    return false; // Timeout reached, prompt not found
  }

  void execute(const std::string &command) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    if (!hChildStdInWrite) {
      throw GnuplotException("Gnuplot process not started");
    }

    buffer_.clear(); // Clear the buffer before sending a new command

    // Write the command to the gnuplot process
    DWORD bytes_written = 0;
    if (!WriteFile(hChildStdInWrite, // Child standard input write handle
                   command.c_str(),  // Command to write
                   command.size(),   // Size of the command
                   &bytes_written,   // Pointer to number of bytes written
                   nullptr)          // Pointer to overlapped structure
        || bytes_written < command.size()) {
      throw GnuplotException("WriteFile failed: " +
                             std::to_string(GetLastError()));
    }

    // Flush the input buffer
    FlushFileBuffers(hChildStdInWrite);

    static constexpr auto kSleepTime = 1000;
    // Wait for a short time to allow gnuplot to process the command
    Sleep(kSleepTime);
    // FIXME: Wait for the gnuplot prompt
    // if (!waitForPrompt()) {
    //   throw GnuplotException("Timeout waiting for gnuplot prompt");
    // }
  }

private:
  HANDLE hChildStdInWrite = nullptr;
  HANDLE hChildStdOutRead = nullptr;
  PROCESS_INFORMATION piChild{};
  std::string command_ = Gnuplot::kGnuplotExecutable;

  // The buffer to store the output from gnuplot.
  // Note that it may be binary data.
  std::string buffer_{};
};

} // namespace phosphorus

#else

#endif

namespace phosphorus {

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

Gnuplot &Gnuplot::setCommand(const std::string &command) {
  impl_->setCommand(command);
  return *this;
}

Gnuplot &Gnuplot::execute(const std::string &command) {
  impl_->execute(commandPreprocessor(command));
  return *this;
}

std::string Gnuplot::commandPreprocessor(const std::string &command) {
  std::string processed_command = command;
  while (!processed_command.empty() && processed_command.back() == '\n') {
    processed_command.pop_back(); // Remove trailing '\n'
  }
  processed_command += ";print \'";
  processed_command += GnuplotImpl::kGnuplotEndFlag;
  processed_command += "\';\n";
  std::cout << processed_command
            << std::endl; // Print the command to stdout for debugging
  return processed_command;
}

std::string &Gnuplot::getBuffer() const { return impl_->buffer_; }

} // namespace phosphorus
