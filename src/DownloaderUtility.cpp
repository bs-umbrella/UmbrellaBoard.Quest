#include "DownloaderUtility.hpp"

#include "libcurl/shared/curl.h"
#include "libcurl/shared/easy.h"
#include "fmt/core.h"

#include <sstream>
#include <ranges>

namespace Umbrella {
    std::future<DownloaderUtility::Response<std::vector<uint8_t>>> DownloaderUtility::GetData(std::string_view url, QueryMap queries, HeaderMap headers, std::launch launchType) const {
        return std::async(launchType, &DownloaderUtility::GetData_internal, this, std::string(url), std::forward<QueryMap>(queries), std::forward<HeaderMap>(headers));
    }

    std::future<DownloaderUtility::Response<rapidjson::Document>> DownloaderUtility::GetJson(std::string_view url, QueryMap queries, HeaderMap headers, std::launch launchType) const {
        return std::async(launchType, &DownloaderUtility::GetJson_internal, this, std::string(url), std::forward<QueryMap>(queries), std::forward<HeaderMap>(headers));
    }

    std::future<DownloaderUtility::Response<std::string>> DownloaderUtility::GetString(std::string_view url, QueryMap queries, HeaderMap headers, std::launch launchType) const {
        return std::async(launchType, &DownloaderUtility::GetString_internal, this, std::string(url), std::forward<QueryMap>(queries), std::forward<HeaderMap>(headers));
    }

    std::string query_encode(std::string_view s) {
		std::stringstream ret;

		#define IS_BETWEEN(ch, low, high) (ch >= low && ch <= high)
		#define IS_ALPHA(ch) (IS_BETWEEN(ch, 'A', 'Z') || IS_BETWEEN(ch, 'a', 'z'))
		#define IS_DIGIT(ch) IS_BETWEEN(ch, '0', '9')
		#define IS_HEXDIG(ch) (IS_DIGIT(ch) || IS_BETWEEN(ch, 'A', 'F') || IS_BETWEEN(ch, 'a', 'f'))

		for(size_t i = 0; i < s.size();) {
			char ch = s[i++];

			if (IS_ALPHA(ch) || IS_DIGIT(ch))
			{
				ret << ch;
			}
			else if ((ch == '%') && IS_HEXDIG(s[i+0]) && IS_HEXDIG(s[i+1]))
			{
				ret << s.substr(i-1, 3);
				i += 2;
			}
			else
			{
				switch (ch)
				{
					case '-':
					case '.':
					case '_':
					case '~':
					case '!':
					case '$':
					case '&':
					case '\'':
					case '(':
					case ')':
					case '*':
					case '+':
					case ',':
					case ';':
					case '=':
					case ':':
					case '@':
					case '/':
					case '?':
					case '[':
					case ']':
						ret << ch;
						break;

					default:
					{
						static const char hex[] = "0123456789ABCDEF";
						ret << "%" << hex[(ch >> 4) & 0xF] << hex[ch & 0xF];
						break;
					}
				}
			}
		}

		return ret.str();
	}

    static auto write_vec_cb(uint8_t* content, std::size_t size, std::size_t nmemb, std::vector<uint8_t>* vec) -> std::size_t {
        std::span<uint8_t> addedData(content, (size * nmemb));
        vec->insert(std::next(vec->begin(), vec->size()), addedData.begin(), addedData.end());
        return addedData.size();
    };

    DownloaderUtility::Response<std::vector<uint8_t>> DownloaderUtility::GetData_internal(std::string url, QueryMap queries, HeaderMap headers) const {
        Response<std::vector<uint8_t>> response;

        auto curl = curl_easy_init();
        struct curl_slist *curl_headers = NULL;
        for (const auto& [key, value] : headers) {
            curl_headers = curl_slist_append(curl_headers, fmt::format("{}: {}", key, value).c_str());
        }

        std::stringstream fullUrl;
        fullUrl << url;
        if (!queries.empty()) {
            bool first = true;
            for (auto const& [key, value] : queries) {
                fullUrl << (first ? '?' : '&') << key << '=' << value;
                first = false;
            }
        }

        std::string encodedUrl = query_encode(fullUrl.str());

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
        curl_easy_setopt(curl, CURLOPT_URL, encodedUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, _timeOut);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

        std::vector<uint8_t> content;

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_vec_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);

        curl_easy_setopt(curl, CURLOPT_USERAGENT, _userAgent.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

        response.curlStatus = curl_easy_perform(curl);
        if (response.curlStatus == CURLE_OK) response.content = std::move(content);

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.httpCode);
        curl_easy_cleanup(curl);
        return response;
    }

    DownloaderUtility::Response<rapidjson::Document> DownloaderUtility::GetJson_internal(std::string url, QueryMap queries, HeaderMap headers) const {
        // we can just leverage the getdata method to download stuff, and then parse it differently from there
        auto data = GetData_internal(url, queries, headers);
        if (data && data.content.has_value()) { // download was succesful and data was gotten
            auto& content = data.content.value();

            rapidjson::Document doc;
            doc.Parse((char*)content.data(), content.size());

            if (!doc.HasParseError()) {
                return {
                    .httpCode = std::move(data.httpCode),
                    .curlStatus = std::move(data.curlStatus),
                    .content = std::move(doc)
                };
            }
        }

        // this wasn't a valid json object, or the download failed
        return {
            .httpCode = data.httpCode,
            .curlStatus = data.curlStatus,
            .content = std::nullopt
        };
    }

    DownloaderUtility::Response<std::string> DownloaderUtility::GetString_internal(std::string url, QueryMap queries, HeaderMap headers) const {
        // we can just leverage the getdata method to download stuff, and then just create a string from there
        auto data = GetData_internal(url, queries, headers);
        if (data && data.content.has_value()) { // download was succesful and data was gotten
            auto& content = data.content.value();
            return {
                .httpCode = std::move(data.httpCode),
                .curlStatus = std::move(data.curlStatus),
                .content = std::string((char*)content.data(), content.size())
            };
        }

        // download failed
        return {
            .httpCode = data.httpCode,
            .curlStatus = data.curlStatus,
            .content = std::nullopt
        };
    }
}
