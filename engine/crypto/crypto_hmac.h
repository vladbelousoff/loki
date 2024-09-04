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

#include "crypto_hash.h"

namespace loki {

  template<GenericHashImpl::HashCreator HashCreator, std::size_t DigestLength>
  class GenericHMAC
  {
  public:
    static constexpr std::size_t DIGEST_LENGTH = DigestLength;
    using Digest = std::array<u8, DIGEST_LENGTH>;

    template<typename Container>
    static Digest get_digest_of(const Container& seed, const u8* data, std::size_t len)
    {
      GenericHMAC hash(seed);
      hash.update_data(data, len);
      hash.finalize();
      return hash.get_digest();
    }

    template<typename Container, typename... Ts>
    static auto get_digest_of(const Container& seed, Ts&&... pack) -> std::enable_if_t<!(std::is_integral_v<std::decay_t<Ts>> || ...), Digest>
    {
      GenericHMAC hash(seed);
      (hash.update_data(std::forward<Ts>(pack)), ...);
      hash.finalize();
      return hash.get_digest();
    }

    GenericHMAC(const u8* seed, std::size_t len)
      : context(GenericHashImpl::create_context())
      , key(EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, nullptr, seed, (int)len))
    {
      i32 result = EVP_DigestSignInit(context, nullptr, HashCreator(), nullptr, key);
      ASSERT(result == 1);
    }

    template<typename Container>
    explicit GenericHMAC(const Container& container)
      : GenericHMAC(std::data(container), std::size(container))
    {
    }

    GenericHMAC(const GenericHMAC& right)
      : context(GenericHashImpl::create_context())
    {
      *this = right;
    }

    GenericHMAC(GenericHMAC&& right) noexcept
    {
      *this = std::move(right);
    }

    ~GenericHMAC()
    {
      if (context) {
        GenericHashImpl::destroy_context(context);
        context = nullptr;
      }

      if (key) {
        EVP_PKEY_free(key);
        key = nullptr;
      }
    }

    GenericHMAC& operator=(const GenericHMAC& other)
    {
      if (this == &other) {
        return *this;
      }

      i32 result = EVP_MD_CTX_copy_ex(context, other.context);
      ASSERT(result == 1);
      key = other.key;      // EVP_PKEY uses reference counting internally, just copy the pointer
      EVP_PKEY_up_ref(key); // Bump reference count for PKEY, as every instance of this class holds two references to
                            // PKEY and destructor decrements it twice
      digest = other.digest;

      return *this;
    }

    GenericHMAC& operator=(GenericHMAC&& other) noexcept
    {
      if (this == &other) {
        return *this;
      }

      context = std::exchange(other.context, GenericHashImpl::create_context());
      key = std::exchange(other.key, EVP_PKEY_new());
      digest = std::exchange(other.digest, Digest{});

      return *this;
    }

    void update_data(const u8* data, std::size_t length)
    {
      i32 result = EVP_DigestSignUpdate(context, data, length);
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
      std::size_t length = DIGEST_LENGTH;
      i32 result = EVP_DigestSignFinal(context, digest.data(), &length);
      DEBUG_ASSERT(result == 1);
      DEBUG_ASSERT(length == DIGEST_LENGTH);
    }

    const Digest& get_digest() const
    {
      return digest;
    }

  private:
    EVP_MD_CTX* context{};
    EVP_PKEY* key{};
    Digest digest{};
  };

  namespace crypto {

    using HMAC_SHA1 = GenericHMAC<EVP_sha1, constants::SHA1_DIGEST_LENGTH_BYTES>;
    using HMAC_SHA256 = GenericHMAC<EVP_sha256, constants::SHA256_DIGEST_LENGTH_BYTES>;

  } // namespace crypto

} // namespace loki
