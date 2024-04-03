#pragma once

#include <future>
#include <string_view>
#include <unordered_map>
#if __has_include("beatsaber-hook/shared/config/rapidjson-utils.hpp")
#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"
#else
#include "rapidjson/Document.h"
#endif

namespace Umbrella {
    struct DownloaderUtility {
            /// @brief downloader utility response object
            template<typename T>
            struct Response {
                /// @brief http status code
                int httpCode;
                /// @brief CURLcode enum
                int curlStatus;
                /// @brief content in requested format
                std::optional<T> content;

                /// @brief whether the request was succesful. doesn't mean content has a value pending whether it deserialized
                inline constexpr operator bool() const noexcept {
                    return httpCode >= 200 && httpCode < 300 && curlStatus == 0;
                }

                inline constexpr T const& operator *() const { return content.operator *(); }
                inline constexpr T& operator *() { return content.operator *(); }

                inline constexpr T const* operator ->() const { return content.operator ->(); }
                inline constexpr T* operator ->() { return content.operator ->(); }
            };

            using QueryMap = std::unordered_map<std::string, std::string>;
            using HeaderMap = std::unordered_map<std::string, std::string>;

            /// @brief gets the data at the url with queries attached if any given
            /// @param url url to get from
            /// @param queries key value pair queries, i.e. url?key=value&key2=value2
            /// @param headers get request headers key value pair headers
            /// @param launchType how to launch the async
            std::future<Response<std::vector<uint8_t>>> GetData(std::string_view url, QueryMap queries = {}, HeaderMap headers = {}, std::launch launchType = std::launch::async) const;

            /// @brief gets the json at the url with the queries attached if any given
            /// @param url url to get from
            /// @param queries key value pair queries, i.e. url?key=value&key2=value2
            /// @param headers get request headers key value pair headers
            /// @param launchType how to launch the async
            std::future<Response<rapidjson::Document>> GetJson(std::string_view url, QueryMap queries = {}, HeaderMap headers = {}, std::launch launchType = std::launch::async) const;

            /// @brief gets the content at the url with queries attached if any given
            /// @param url url to get from
            /// @param queries key value pair queries, i.e. url?key=value&key2=value2
            /// @param headers get request headers key value pair headers
            /// @param launchType how to launch the async
            std::future<Response<std::string>> GetString(std::string_view url, QueryMap queries = {}, HeaderMap headers = {}, std::launch launchType = std::launch::async) const;

            constexpr uint32_t get_TimeOut() const noexcept { return _timeOut; }
            constexpr void set_TimeOut(uint32_t timeOut) { _timeOut = timeOut; }
            __declspec(property(get=get_TimeOut, put=set_TimeOut)) uint32_t TimeOut;

            std::string_view get_UserAgent() const { return _userAgent; }
            void set_UserAgent(std::string_view userAgent) { _userAgent = std::string(userAgent); }
            __declspec(property(get=get_UserAgent, put=set_UserAgent)) std::string_view UserAgent;
        private:
            uint32_t _timeOut;
            std::string _userAgent;

            /// @brief internal GetData implementation
            Response<std::vector<uint8_t>> GetData_internal(std::string url, QueryMap queries, HeaderMap headers) const;

            /// @brief internal GetJson implementation
            Response<rapidjson::Document> GetJson_internal(std::string url, QueryMap queries, HeaderMap headers) const;

            /// @brief internal GetString implementation
            Response<std::string> GetString_internal(std::string url, QueryMap queries, HeaderMap headers) const;

    };
}
