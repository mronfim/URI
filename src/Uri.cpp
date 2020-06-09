/**
 * @file Uri.cpp
 * 
 * This module contains the implementation of the Uri::Uri class.
 * 
 */

#include <stdexcept>
#include <string>
#include <vector>
#include <Uri/Uri.h>

namespace Uri
{
    /**
     * This contains the private properties of a Uri instance.
     */
    struct Uri::Impl {
        /**
         * This is the "scheme" element of the URI.
         */
        std::string scheme;

        /**
         * This is the "host" element of the URI.
         */
        std::string host;

        /**
         * This flag indicates whether or not the
         * URI includes a port number.
         */
        bool hasPort = false;

        /**
         * This is the port number element of the URI.
         */
        uint16_t port = 0;

        /**
         * This is the "path" element of the URI,
         * as a sequence of segments.
         */
        std::vector<std::string> path;

        /**
         * This is the "query" element of the URI.
         */
        std::string query;

        /**
         * This is the "fragment" element of the URI.
         */
        std::string fragment;
    };

    Uri::~Uri() = default;

    Uri::Uri()
        : impl_(new Impl)
    {
    }

    bool Uri::ParseFromString(const std::string& uriString)
    {
        // First parse the scheme.
        size_t nextIdx;
        impl_->scheme = parseScheme(uriString, nextIdx);
        auto rest = uriString.substr(nextIdx);

        // Next, parse the userinfo, host, and port number.
        std::string authority = parseAuthority(rest, nextIdx);
        if (!parseAuthorityComponents(authority)) {
            return false;
        }
        rest = rest.substr(nextIdx);

        // Next, parse the fragment if there is one.
        impl_->fragment.clear();
        const auto fragment = rest.find('#');
        if (fragment != std::string::npos) {
            impl_->fragment = rest.substr(fragment + 1);
            rest = rest.substr(0, fragment);
        }

        // Then, parse the query string.
        impl_->query.clear();
        const auto query = rest.find('?');
        if (query != std::string::npos) {
            impl_->query = rest.substr(query + 1);
            rest = rest.substr(0, query);
        }

        // Then, parse the path.
        impl_->path.clear();
        if (rest == "/") {
            // Special case of a path that is empty but needs a single
            // empty string element to indicate that it is absolute.
            impl_->path.push_back("");
            rest.clear();
        }
        else if (!rest.empty()) {
            for (;;) {
                auto pathDelimiter = rest.find('/');
                if (pathDelimiter == std::string::npos) {
                    impl_->path.push_back(rest);
                    break;
                }
                else {
                    impl_->path.emplace_back(
                        rest.begin(),
                        rest.begin() + pathDelimiter
                    );
                    rest = rest.substr(pathDelimiter + 1);
                }
            };
        }

        return true;
    }

    std::string Uri::GetScheme() const
    {
        return impl_->scheme;
    }

    std::string Uri::GetHost() const
    {
        return impl_->host;
    }

    std::vector<std::string> Uri::GetPath() const
    {
        return impl_->path;
    }

    bool Uri::HasPort() const
    {
        return impl_->hasPort;
    }

    uint16_t Uri::GetPort() const
    {
        return impl_->port;
    }

    bool Uri::IsRelativeReference() const
    {
        return impl_->scheme.empty();
    }

    bool Uri::ContainsRelativePath() const
    {
        if (impl_->path.empty()) {
            return true;
        }
        else {
            return !impl_->path[0].empty();
        }
    }

    std::string Uri::GetQuery() const
    {
        return impl_->query;
    }

    std::string Uri::GetFragment() const
    {
        return impl_->fragment;
    }

    std::string Uri::parseScheme(const std::string& uri, size_t& nextIdx)
    {
        const auto schemeEnd = uri.find(":");
        if (schemeEnd == std::string::npos) {
            nextIdx = 0;
            return "";
        }

        // Check if there is a dot-segment before the presumed scheme delimiter.
        const auto dotSegment = uri.find("./");
        if (dotSegment != std::string::npos && dotSegment < schemeEnd) {
            nextIdx = 0;
            return "";
        }

        nextIdx = schemeEnd + 1;
        return uri.substr(0, schemeEnd);
    }

    std::string Uri::parseAuthority(const std::string& uri, size_t& nextIdx)
    {
        const auto doubleForwardSlash = uri.find("//");
        if (doubleForwardSlash == std::string::npos) {
            // if there is no "//", then there is no authority
            nextIdx = 0;
            return "";
        }

        // Check if there is a forward slash before the double forward slash.
        const auto dotSegment = uri.find("/");
        if (dotSegment != std::string::npos && dotSegment < doubleForwardSlash) {
            // The authority needs to come before the path
            nextIdx = 0;
            return "";
        }

        auto authorityEnd = uri.find_first_of("/?#", doubleForwardSlash + 2);
        if (authorityEnd == std::string::npos) {
            authorityEnd = uri.length();
        }

        nextIdx = authorityEnd;
        return uri.substr(doubleForwardSlash + 2, authorityEnd - 2);
    }

    bool Uri::parseAuthorityComponents(const std::string& authority)
    {
        impl_->host.clear();
        impl_->port = 0;
        impl_->hasPort = false;

        size_t nextIdx = 0;
        const auto userinfoDelim = authority.find("@");
        if (userinfoDelim != std::string::npos) {
            // TODO: set userinfo
            nextIdx = userinfoDelim + 1;
        }

        const auto portDelim = authority.find(":", nextIdx);
        if (portDelim != std::string::npos) {
            uint32_t newPort = 0;
            for (auto c : authority.substr(portDelim + 1)) {
                if (c < '0' || c > '9') {
                    return false;
                }
                newPort *= 10;
                newPort += (uint16_t)(c - '0');
                if ((newPort & ~((1 << 16) - 1)) != 0) {
                    return false;
                }
            }

            impl_->port = (uint16_t)newPort;
            impl_->hasPort = true;
            impl_->host = authority.substr(nextIdx, portDelim - nextIdx);
        }
        else {
            impl_->host = authority.substr(nextIdx);
        }

        return true;
    }
    
}