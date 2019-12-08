/*
 Copyright 2010-2019 Kristian Duske

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
 persons to whom the Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef TRENCHBROOM_STRING_UTILS_H
#define TRENCHBROOM_STRING_UTILS_H

#include "string_format.h"

#include <algorithm> // for std::search
#include <cassert>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

namespace kdl {
    /**
     * Splits the given strings along the given delimiters and returns a list of the nonempty parts.
     *
     * @param str the string to split
     * @param delims the delimiters to split with
     * @return the parts
     */
    inline std::vector<std::string> str_split(const std::string_view& str, const std::string_view& delims) {
        if (str.empty()) {
            return {};
        }

        const auto first = str.find_first_not_of(delims);
        if (first == std::string::npos) {
            return {};
        }

        const auto last = str.find_last_not_of(delims);
        assert(last != std::string::npos);
        assert(first <= last);

        std::vector<std::string> result;

        auto lastPos = first;
        auto pos = lastPos;
        while ((pos = str.find_first_of(delims, pos)) < last) {
            auto part = str_trim(str.substr(lastPos, pos - lastPos));
            if (!part.empty()) {
                result.push_back(std::move(part));
            }
            lastPos = ++pos;
        }

        if (lastPos <= last) {
            auto part = str_trim(str.substr(lastPos, last - lastPos + 1));
            if (!part.empty()) {
                result.push_back(std::move(part));
            }
        }

        return result;
    }

    /**
     * Joins the objects in the given range [it, end) using the given delimiters. The objects are converted to string
     * using the stream insertion operator and a string stream.
     *
     * Given an object o, let "o" be the string representation of o obtained using the stream insertion operator <<.
     *
     * If the given range is [], an empty string is returned.
     * If the given range is [o1], the result "o1".
     * If the given range is [o1, o2], then the result is "o1" + delim_for_two + "o2".
     * If the given range is [o1, o2, ..., on] with n > 2, the result is "o1" + delim + "o2" + delim + ... + delim + "on-1" + last_delim + "on".
     *
     * @tparam I the range iterator type
     * @param it the beginning of the range
     * @param end the end of the range
     * @param delim the delimiter to insert for ranges of length > 2
     * @param last_delim the delimter to insert for ranges of length 2
     * @param delim_for_two the delimiter to insert before the last object in ranges of length > 2
     * @return the joined string
     */
    template <typename I>
    std::string str_join(I it, I end, const std::string_view& delim, const std::string_view& last_delim, const std::string_view& delim_for_two) {
        if (it == end) {
            return "";
        }

        std::stringstream result;
        result << *it++;

        if (it == end) {
            return result.str();
        }

        auto prev = it++;
        if (it == end) {
            result << delim_for_two << *prev;
            return result.str();
        }
        result << delim << *prev;

        prev = it++;
        while (it != end) {
            result << delim << *prev;
            prev = it++;
        }

        result << last_delim << *prev;
        return result.str();
    }

    /**
     * Joins the objects in the given range [it, end) using the given delimiter. The delimiter is used as the delimiter
     * for collections of two objects as well as for the last two objects in collections of more than two objects.
     *
     * @see str_join(I, I, const std::string_view&, const std::string_view&, const std::string_view&)
     *
     * @tparam I the range iterator type
     * @param it the beginning of the range
     * @param end the end of the range
     * @param delim the delimiter to insert
     * @return the joined string
     */
    template <typename I>
    std::string str_join(I it, I end, const std::string_view& delim) {
        return str_join(it, end, delim, delim, delim);
    }

    /**
     * Joins the objects in the given collection using the given delimiters.
     *
     * @see str_join(I, I, const std::string_view&, const std::string_view&, const std::string_view&)
     *
     * @tparam C the collection type
     * @param c the collection of objects to join
     * @param delim the delimiter to insert for collections of size > 2
     * @param last_delim the delimter to insert for collections of size 2
     * @param delim_for_two the delimiter to insert before the last object in collections of size > 2
     * @return the joined string
     */
    template <typename C>
    std::string str_join(const C& c, const std::string_view& delim, const std::string_view& last_delim, const std::string_view& delim_for_two) {
        return str_join(std::begin(c), std::end(c), delim, last_delim, delim_for_two);
    }

    /**
     * Joins the objects in the given collection using the given delimiter. The delimiter is used as the delimiter for
     * collections of two objects as well as for the last two objects in collections of more than two objects.
     *
     * @see str_join(I, I, const std::string_view&, const std::string_view&, const std::string_view&)
     *
     * @tparam C the collection type
     * @param c the collection of objects to join
     * @param delim the delimiter to insert
     * @return the joined string
     */
    template <typename C>
    std::string str_join(const C& c, const std::string_view& delim = ", ") {
        return str_join(std::begin(c), std::end(c), delim, delim, delim);
    }

    /**
     * Replaces every occurence of needle in string haystack with the given replacement, and returns the result.
     *
     * @param haystack the string to modify
     * @param needle the string to search for
     * @param replacement the string to replace needle with
     * @return the modified string
     */
    inline std::string str_replace_every(const std::string_view& haystack, const std::string_view& needle, const std::string_view& replacement) {
        if (haystack.empty() || needle.empty() || needle == replacement) {
            return std::string(haystack);
        }

        std::string result(haystack);

        using diff_type = std::string::iterator::difference_type;
        auto it = std::search(std::begin(result), std::end(result), std::begin(needle), std::end(needle));
        while (it != std::end(result)) {
            // remember the position where the search will continue after replacement
            const auto next_offset = std::distance(std::begin(result), it) + static_cast<diff_type>(replacement.size());
            result.replace(it, std::next(it, static_cast<diff_type>(needle.size())), replacement); // invalidates it

            it = std::search(std::next(std::begin(result), next_offset), std::end(result), std::begin(needle), std::end(needle));
        }
        return result;
    }

    /**
     * Returns a string representation of the given object by means of the stream insertion operator.
     *
     * @tparam T the type of the object
     * @param o the object
     * @return the string representation
     */
    template <typename T>
    std::string str_to_string(const T& o) {
        std::stringstream str;
        str << o;
        return str.str();
    }
}

#endif //TRENCHBROOM_STRING_UTILS_H
