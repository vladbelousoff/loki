/*
 * This file is part of the Loki Project.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "engine/utils/types.h"
#include "libassert/assert.hpp"
#include "sockpp/tcp_connector.h"
#include "spdlog/spdlog.h"

#include <string>
#include <vector>
#include <boost/pfr.hpp>

namespace loki {

  class ByteBuffer
  {
    constexpr static std::size_t DEFAULT_SIZE = 0x1000;

  public:
    explicit ByteBuffer();

  public:
    void reset();

    template<typename T>
    void append(T value)
    {
      auto bytes = reinterpret_cast<const std::uint8_t*>(&value);
      buffer.insert(buffer.end(), bytes, bytes + sizeof(T));
    }

    void append(const std::vector<loki::u8>& value);
    void append(std::string_view value);

    template<typename Type, std::size_t Size>
    void append(const std::array<Type, Size>& value)
    {
      buffer.insert(buffer.end(), value.begin(), value.end());
    }

    template<typename T>
    T read()
    {
      T value;
      read(&value, sizeof(T));
      return value;
    }

    template<typename Type, std::size_t Size>
    void read(std::array<Type, Size>& arr)
    {
      read(arr.data(), Size * sizeof(Type));
    }

    void read(void* data, std::size_t n)
    {
      DEBUG_ASSERT(r_pos + n <= buffer.size());
      std::memcpy(data, &buffer[r_pos], n);
      r_pos += n;
    }

    void skip(std::size_t n)
    {
      r_pos += n;
    }

    bool can_read() const
    {
      return r_pos != buffer.size();
    }

    std::size_t get_r_pos() const
    {
      return r_pos;
    }

    void set_r_pos(std::size_t pos)
    {
      r_pos = pos;
    }

    void send(sockpp::tcp_socket& conn) const;
    ssize_t recv(sockpp::tcp_socket& conn);

    template<typename T>
    void save_buffer(T& value);

    template<typename T>
    void load_buffer(T& value);

  protected:
    std::vector<loki::u8> buffer;
    std::size_t r_pos = 0;
  };

  template<typename>
  struct always_false : std::false_type
  {};

  template<typename T>
  concept IsStdArray = requires { typename std::array<typename T::value_type, T::size_type::value>; };

  template<typename T>
  concept IsStdVector = requires { typename std::vector<typename T::value_type>; };

  template<typename T>
  concept IsUserClass = std::is_class_v<T> && !IsStdArray<T> && !IsStdVector<T>;

  template<typename T, typename Enable = void>
  struct LoadFieldHelper
  {
    static void load(ByteBuffer&, T&)
    {
      static_assert(always_false<T>::value, "No implementation for this type");
    }
  };

  template<typename T, typename Enable = void>
  struct SaveFieldHelper
  {
    static void save(ByteBuffer&, T&)
    {
      static_assert(always_false<T>::value, "No implementation for this type");
    }
  };

  template<typename T>
  struct LoadFieldHelper<T, typename std::enable_if<IsUserClass<T>>::type>
  {
    static void load(ByteBuffer& buffer, T& value)
    {
      boost::pfr::for_each_field(value, [&buffer](auto& field) {
        LoadFieldHelper<typename std::remove_cvref<decltype(field)>::type>::load(buffer, field);
      });
    }
  };

  template<typename T>
  struct SaveFieldHelper<T, typename std::enable_if<IsUserClass<T>>::type>
  {
    static void save(ByteBuffer& buffer, T& value)
    {
      boost::pfr::for_each_field(value, [&buffer](auto& field) {
        SaveFieldHelper<typename std::remove_cvref<decltype(field)>::type>::save(buffer, field);
      });
    }
  };

  template<typename T>
  struct LoadFieldHelper<T, typename std::enable_if<std::is_fundamental<T>::value>::type>
  {
    static void load(ByteBuffer& buffer, T& value)
    {
      value = buffer.read<T>();
    }
  };

  template<typename T>
  struct SaveFieldHelper<T, typename std::enable_if<std::is_fundamental<T>::value>::type>
  {
    static void save(ByteBuffer& buffer, T& value)
    {
      buffer.append(value);
    }
  };

  template<>
  struct LoadFieldHelper<std::vector<u8>>
  {
    static void load(ByteBuffer& buffer, std::vector<u8>& value)
    {
      std::size_t size = buffer.read<u8>();
      value.resize(size);
      buffer.read(value.data(), value.size());
    }
  };

  template<>
  struct SaveFieldHelper<std::vector<u8>>
  {
    static void save(ByteBuffer& buffer, std::vector<u8>& value)
    {
      buffer.append(value);
    }
  };

  template<>
  struct LoadFieldHelper<std::string>
  {
    static void load(ByteBuffer& buffer, std::string& value)
    {
      std::ostringstream oss;
      char ch;
      do {
        ch = buffer.read<char>();
        oss << ch;
      } while (ch != 0);

      value = oss.str();
    }
  };

  template<>
  struct SaveFieldHelper<std::string>
  {
    static void save(ByteBuffer& buffer, std::string& value)
    {
      buffer.append(std::string_view(value));
    }
  };

  template<std::size_t N>
  struct LoadFieldHelper<std::array<u8, N>>
  {
    static void load(ByteBuffer& buffer, std::array<u8, N>& value)
    {
      buffer.read(value);
    }
  };

  template<std::size_t N>
  struct SaveFieldHelper<std::array<u8, N>>
  {
    static void save(ByteBuffer& buffer, std::array<u8, N>& value)
    {
      buffer.append(value);
    }
  };

  template<typename T>
  void ByteBuffer::save_buffer(T& value)
  {
    SaveFieldHelper<T>::save(*this, value);
  }

  template<typename T>
  void ByteBuffer::load_buffer(T& value)
  {
    LoadFieldHelper<T>::load(*this, value);
  }

} // namespace loki

