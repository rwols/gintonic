#pragma once

#include <exception>

#include <gintonic/nsbegin.hpp>

/**
 * @brief      Throw this to quit the application.
 */
class QuitApplication : public std::exception
{
  public:
    QuitApplication() = default;
    ~QuitApplication() noexcept override = default;
    const char* what() const noexcept override { return "QuitApplication"; }
};

#include <gintonic/nsend.hpp>
