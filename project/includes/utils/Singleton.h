#pragma once

#include <memory>

template <typename T>
class Singleton {
protected:
  Singleton() = default;
  Singleton(const Singleton&) = delete;
  Singleton& operator=(const Singleton&) = delete;

public:
  static T* Get() {
    static T instance;
    return &instance;
  }
};