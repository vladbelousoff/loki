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

#include <array>
#include <openssl/bn.h>
#include <stdexcept>
#include <vector>

namespace loki {

  class BigNum
  {
  public:
    BigNum();

    BigNum(const BigNum& other);
    BigNum& operator=(const BigNum& other);

    ~BigNum();

  public:
    void set_dword(std::int32_t word);
    void set_dword(std::uint32_t word);
    void set_qword(std::uint64_t word);

    void set_binary(const std::uint8_t* bytes, std::size_t len);

    template<typename Container>
    auto set_binary(const Container& c) -> std::enable_if_t<!std::is_pointer_v<std::decay_t<Container>>>
    {
      set_binary(std::data(c), std::size(c));
    }

    static BigNum from_binary(const std::uint8_t* bytes, std::size_t len)
    {
      BigNum ret;
      ret.set_binary(bytes, len);
      return ret;
    }

    template<typename Container>
    static BigNum from_binary(const Container& c)
    {
      BigNum ret;
      ret.set_binary(c);
      return ret;
    }

    static BigNum from_random(std::int32_t num_bits)
    {
      BigNum ret;
      ret.set_rand(num_bits);
      return ret;
    }

    bool set_hex_str(const char* str);
    void set_rand(std::int32_t num_bits);

    BigNum exp(const BigNum& bn1) const;
    BigNum mod_exp(const BigNum& bn1, const BigNum& bn2) const;

    BigNum& operator+=(const BigNum& other);
    BigNum operator+(const BigNum& other) const;

    BigNum& operator-=(const BigNum& other);
    BigNum operator-(const BigNum& other) const;

    BigNum& operator*=(const BigNum& other);
    BigNum operator*(const BigNum& other) const;

    BigNum& operator/=(const BigNum& other);
    BigNum operator/(const BigNum& other) const;

    BigNum& operator%=(const BigNum& other);
    BigNum operator%(const BigNum& other) const;

    BigNum& operator<<=(int n);
    BigNum operator<<(int n) const;

    int cmp(const BigNum& other) const;

    bool operator==(const BigNum& other) const
    {
      return cmp(other) == 0;
    }

    int get_num_bytes() const
    {
      return BN_num_bytes(bn);
    }

    void get_bytes(std::uint8_t* buf, std::size_t buf_size) const;

    std::vector<std::uint8_t> to_byte_vector() const;

    template<std::size_t Size>
    std::array<std::uint8_t, Size> to_byte_array() const
    {
      std::array<std::uint8_t, Size> buf;
      get_bytes(buf.data(), Size);
      return buf;
    }

    std::string as_hex_str() const;

    BIGNUM* get() const
    {
      return bn;
    }

  private:
    BIGNUM* bn{};
  };

} // namespace loki

