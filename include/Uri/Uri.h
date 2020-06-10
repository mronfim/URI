#ifndef URI_H
#define URI_H

/**
 * @file Uri.h
 * 
 * This module declares the Uri::Uri class.
 * 
 */

#include <stdint.h>
#include <memory>
#include <string>
#include <vector>

namespace Uri
{
    /**
     * This class represents a Uniform Resource Identifier (URI),
     * as defined in RFC 3986 (https://tools.ietf.org/html/rfc3986).
     */
    class Uri
    {
        // Lifecycle management
    public:
        ~Uri();
        Uri(const Uri&) = delete;
        Uri(Uri&&) = delete;
        Uri& operator=(const Uri&) = delete;
        Uri& operator=(Uri&&) = delete;

        // Public methods
    public:
        /**
         * This is the default constructor
         */
        Uri();

        /**
         * This method builds the URI from the elements parsed
         * from the given string rendering of a URI.
         *
         * @param[in] uriString
         *      This is the string rendering of the URI to parse.
         *
         * @return
         *      An indication of whether or not the URI was
         *      parsed successfully is returned.
         */
        bool ParseFromString(const std::string& uriString);

        /**
         * This method returns the "scheme" element of the URI.
         *
         * @return
         *      The "scheme" element of the URI is returned.
         *
         * @retval ""
         *      This is returned if there is no "scheme" element in the URI.
         */
        std::string GetScheme() const;

        /**
         * This method returns the "userinfo" element of the URI.
         *
         * @return
         *      The "userinfo" element of the URI is returned.
         *
         * @retval ""
         *      This is returned if there is no "userinfo" element in the URI.
         */
        std::string GetUserInfo() const;

        /**
         * This method returns the "host" element of the URI.
         *
         * @return
         *      The "host" element of the URI is returned.
         *
         * @retval ""
         *      This is returned if there is no "host" element in the URI.
         */
        std::string GetHost() const;

        /**
         * This method returns the "path" element of the URI,
         * as a sequence of segments.
         *
         * @note
         *      If the first segment of the path is an empty string,
         *      then the URI has an absolute path.
         *
         * @return
         *      The "path" element of the URI is returned
         *      as a sequence of segments.
         */
        std::vector<std::string> GetPath() const;

        /**
         * This method returns an indication of whether or not the
         * URI includes a port number.
         *
         * @return
         *      An indication of whether or not the
         *      URI includes a port number is returned.
         */
        bool HasPort() const;

        /**
         * This method returns the port number element of the URI,
         * if it has one.
         *
         * @return
         *      The port number element of the URI is returned.
         *
         * @note
         *      The returned port number is only valid if the
         *      HasPort method returns true.
         */
        uint16_t GetPort() const;

        /**
         * This method returns an indication of whether or not the
         * URI is a relative reference.
         *
         * @return
         *      An indication of whether or not the
         *      URI is a relative reference is returned.
         */
        bool IsRelativeReference() const;

        /**
         * This method returns an indication of whether or not the
         * URI contains a relative path.
         *
         * @return
         *      An indication of whether or not the
         *      URI constains a relative path is returned.
         */
        bool ContainsRelativePath() const;

        /**
         * This method returns the "query" element of the URI.
         *
         * @return
         *      The "query" element of the URI is returned.
         *
         * @retval ""
         *      This is returned if there is no "query" element in the URI.
         */
        std::string GetQuery() const;

        /**
         * This method returns the "fragment" element of the URI.
         *
         * @return
         *      The "fragment" element of the URI is returned.
         *
         * @retval ""
         *      This is returned if there is no "fragment" element in the URI.
         */
        std::string GetFragment() const;

        // private properties
    private:
        /**
         * This is the type of structure that contains the private
         * properties of the instance. It is defined in the implementation
         * and declared here to ensure that it is scoped inside the class.
         */
        struct Impl;

        /**
         * This contains the private properties of the instance.
         */
        std::unique_ptr<struct Impl>impl_;

        // private methods
    private:
        /**
         * This method parses the "scheme" part of the URI, returning the
         * scheme as a string, and sets nextIdx to the beginning of the next
         * part of the URI.
         *
         * @param[in] uri
         *      The string rendering of the URI
         *
         * @param[in] nextIdx
         *      A reference to store the index of the beginning of the next
         *      part of the URI.
         *
         * @return
         *      A string representation of the scheme is returned.
         *
         * @retval ""
         *      This is returned if there is no scheme.
         */
        std::string parseScheme(const std::string& uri, size_t& nextIdx);

        /**
         * This method parses the "authority" part of the URI. If the URI has
         * an authority, it will start with the characters "//" and it will end
         * with a "/", "?", "#", or by the end of the URI.
         *
         * @param[in] uri
         *      The string rendering of the URI
         *
         * @param[in] nextIdx
         *      A reference to store the index of the beginning of the next
         *      part of the URI.
         *
         * @return
         *      A string representation of the authority is returned.
         *
         * @retval ""
         *      This is returned if there is no authority.
         */
        std::string parseAuthority(const std::string& uri, size_t& nextIdx);

        /**
         * This method parses the different components that can be present
         * in the authority of an URI. These componenets include userinfo,
         * host, and port number.
         *        authority   = [ userinfo "@" ] host [ ":" port ]
         * 
         * @param[in]
         *      A string rendering of the authority part of a URI.
         *
         * @return
         *      An indication of whether the authority is valid or not
         *      is returned.
         *
         * @note
         *      These components are not returned, they are directly set
         *      in the implementation instance.
         */
        bool parseAuthorityComponents(const std::string& authority);
    };
}

#endif /* URI_H */