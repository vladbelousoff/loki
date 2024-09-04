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

#include <functional>
#include <openssl/evp.h>

#include "crypro_constants.h"
#include "engine/utils/types.h"
#include "libassert/assert.hpp"

namespace loki {

  struct GenericHashImpl
  {
    typedef EVP_MD const* (*HashCreator)();

    static auto create_context() noexcept -> EVP_MD_CTX*
    {
      return EVP_MD_CTX_new();
    }

    static void destroy_context(EVP_MD_CTX* context)
    {
      EVP_MD_CTX_free(context);
    }
  };

  template<GenericHashImpl::HashCreator HashCreator, std::size_t DigestLength>
  class GenericHash
  {
  public:
    static constexpr std::size_t DIGEST_LENGTH = DigestLength;
    using Digest = std::array<u8, DIGEST_LENGTH>;

    static auto get_digest_of(const u8* data, std::size_t len) -> Digest
    {
      GenericHash hash;
      hash.update_data(data, len);
      hash.finalize();
      return hash.get_digest();
    }

    template<typename... Ts>
    static auto get_digest_of(Ts&&... pack) -> std::enable_if_t<!(std::is_integral_v<std::decay_t<Ts>> || ...), Digest>
    {
      GenericHash hash;
      (hash.update_data(std::forward<Ts>(pack)), ...);
      hash.finalize();
      return hash.get_digest();
    }

    GenericHash()
      : context(GenericHashImpl::create_context())
    {
      i32 result = EVP_DigestInit_ex(context, HashCreator(), nullptr);
      DEBUG_ASSERT(result == 1);
    }

    GenericHash(const GenericHash& other)
      : context(GenericHashImpl::create_context())
    {
      *this = other;
    }

    GenericHash(GenericHash&& other) noexcept
    {
      *this = std::move(other);
    }

    ~GenericHash()
    {
      if (context) {
        GenericHashImpl::destroy_context(context);
        context = nullptr;
      }
    }

    GenericHash& operator=(const GenericHash& other)
    {
      if (this == &other) {
        return *this;
      }

      i32 result = EVP_MD_CTX_copy_ex(context, other.context);
      DEBUG_ASSERT(result == 1);
      digest = other.digest;

      return *this;
    }

    GenericHash& operator=(GenericHash&& other) noexcept
    {
      if (this == &other) {
        return *this;
      }

      context = std::exchange(other.context, GenericHashImpl::create_context());
      digest = std::exchange(other.digest, Digest{});

      return *this;
    }

    void update_data(const u8* data, std::size_t length)
    {
      i32 result = EVP_DigestUpdate(context, data, length);
      DEBUG_ASSERT(result == 1);
    }

    void update_data(std::string_view str)
    {
      update_data(reinterpret_cast<const u8*>(str.data()), str.size());
    }

    void update_data(const std::string& str)
    {
      update_data(std::string_view(str));
    }

    void update_data(const char* str)
    {
      update_data(std::string_view(str));
    }

    template<typename Container>
    void update_data(const Container& c)
    {
      update_data(std::data(c), std::size(c));
    }

    void finalize()
    {
      u32 length;
      i32 result = EVP_DigestFinal_ex(context, digest.data(), &length);
      DEBUG_ASSERT(result == 1);
      DEBUG_ASSERT(length == DIGEST_LENGTH);
    }

    const Digest& get_digest() const
    {
      return digest;
    }

  private:
    EVP_MD_CTX* context{};
    Digest digest{};
  };

  namespace crypto {

    using MD5 = GenericHash<EVP_md5, constants::MD5_DIGEST_LENGTH_BYTES>;
    using SHA1 = GenericHash<EVP_sha1, constants::SHA1_DIGEST_LENGTH_BYTES>;
    using SHA256 = GenericHash<EVP_sha256, constants::SHA256_DIGEST_LENGTH_BYTES>;

  } // namespace crypto

} // namespace loki
