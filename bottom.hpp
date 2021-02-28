#include <array>

namespace bottom
{
    // Helper to deal with compile-time strings.
    template <char... xs>
    struct string_literal
    {
        static constexpr const auto to_array()
        {
            return std::array<char, sizeof...(xs)>({xs...});
        }

        static std::string to_string()
        {
            auto array = to_array();
            return std::string(std::begin(array), std::end(array));
        }
    };

    template <typename CharT, CharT... xs>
    constexpr string_literal<xs...> operator""_C()
    {
        return {};
    }

    // String utils.
    namespace detail
    {
        template <typename Xs, typename Ys>
        struct string_append_impl
        {
        };

        template <typename Xs, typename Ys>
        using string_append = typename string_append_impl<Xs, Ys>::result;

        template <char... xs, char... ys>
        struct string_append_impl<string_literal<xs...>, string_literal<ys...>>
        {
            using result = string_literal<xs..., ys...>;
        };

        template <typename Xs, typename Ys>
        struct string_eq_impl
        {
            static constexpr bool result = false;
        };

        template <typename Xs, typename Ys>
        constexpr bool string_eq()
        {
            return string_eq_impl<Xs, Ys>::result;
        }

        template <char x, char... xs, char y, char... ys>
        struct string_eq_impl<string_literal<x, xs...>, string_literal<y, ys...>>
        {
            static constexpr bool result = x == y && string_eq<string_literal<xs...>, string_literal<ys...>>();
        };

        template <>
        struct string_eq_impl<string_literal<>, string_literal<>>
        {
            static constexpr bool result = true;
        };

    } // namespace detail

    // Encoder.
    namespace detail
    {
        enum struct cond
        {
            gte200,
            gte50,
            gte10,
            gte5,
            gte1,
            zero
        };

        template <cond c, char x>
        struct encode1_cond_impl
        {
        };

        template <cond c, char x>
        using encode1_cond = typename encode1_cond_impl<c, x>::result;

        template <char x>
        struct encode1_impl
        {
            using result = encode1_cond<x >= 200 ? cond::gte200 : x >= 50 ? cond::gte50 : x >= 10 ? cond::gte10 : x >= 5 ? cond::gte5 : x >= 1 ? cond::gte1 : cond::zero, x>;
        };

        template <char x>
        using encode1 = typename encode1_impl<x>::result;

        template <char x>
        struct encode1_cond_impl<cond::gte200, x>
        {
            using result = string_append<decltype("🫂"_C), encode1<x - 200>>;
        };

        template <char x>
        struct encode1_cond_impl<cond::gte50, x>
        {
            using result = string_append<decltype("💖"_C), encode1<x - 50>>;
        };

        template <char x>
        struct encode1_cond_impl<cond::gte10, x>
        {
            using result = string_append<decltype("✨"_C), encode1<x - 10>>;
        };

        template <char x>
        struct encode1_cond_impl<cond::gte5, x>
        {
            using result = string_append<decltype("🥺"_C), encode1<x - 5>>;
        };

        template <char x>
        struct encode1_cond_impl<cond::gte1, x>
        {
            using result = string_append<decltype(","_C), encode1<x - 1>>;
        };

        template <char x>
        struct encode1_cond_impl<cond::zero, x>
        {
            using result = string_literal<>;
        };

        template <typename S>
        struct encode_impl
        {
        };

        template <typename S>
        using encode = typename detail::encode_impl<S>::result;

        template <>
        struct encode_impl<string_literal<>>
        {
            using result = string_literal<>;
        };

        template <char x, char... xs>
        struct encode_impl<string_literal<x, xs...>>
        {
            using result = string_append<string_append<encode1<x>, decltype("👉👈"_C)>, encode<string_literal<xs...>>>;
        };

        template <char... xs>
        struct encode_impl<string_literal<'\x00', xs...>>
        {
            using result = string_append<decltype("❤️👉👈"_C), encode<string_literal<xs...>>>;
        };

    } // namespace detail

    template <typename S>
    using encode = detail::encode<S>;

    // Decoder.
    namespace detail
    {
        template <char c, typename S>
        struct decode_impl
        {
        };

        template <char c, typename S>
        using decode = typename decode_impl<c, S>::result;

        template <>
        struct decode_impl<'\x00', string_literal<>>
        {
            using result = string_literal<>;
        };

        template <char c, char... xs>
        struct decode_impl<c, string_literal<'\xf0', '\x9f', '\xab', '\x82', xs...>> // 🫂
        {
            using result = decode<c + 200, string_literal<xs...>>;
        };

        template <char c, char... xs>
        struct decode_impl<c, string_literal<'\xf0', '\x9f', '\x92', '\x96', xs...>> // 💖
        {
            using result = decode<c + 50, string_literal<xs...>>;
        };

        template <char c, char... xs>
        struct decode_impl<c, string_literal<'\xe2', '\x9c', '\xa8', xs...>> // ✨
        {
            using result = decode<c + 10, string_literal<xs...>>;
        };

        template <char c, char... xs>
        struct decode_impl<c, string_literal<'\xf0', '\x9f', '\xa5', '\xba', xs...>> // 🥺
        {
            using result = decode<c + 5, string_literal<xs...>>;
        };

        template <char c, char... xs>
        struct decode_impl<c, string_literal<',', xs...>> // ,
        {
            using result = decode<c + 1, string_literal<xs...>>;
        };

        template <char c, char... xs>
        struct decode_impl<c, string_literal<'\xf0', '\x9f', '\x91', '\x89', '\xf0', '\x9f', '\x91', '\x88', xs...>> // 👉👈
        {
            using result = string_append<string_literal<c>, decode<'\x00', string_literal<xs...>>>;
        };

        template <char c, char... xs>
        struct decode_impl<c, string_literal<'\xe2', '\x9d', '\xa4', '\xef', '\xb8', '\x8f', '\xf0', '\x9f', '\x91', '\x89', '\xf0', '\x9f', '\x91', '\x88', xs...>> // ❤️👉👈
        {
            using result = string_append<string_literal<'\x00'>, decode<'\x00', string_literal<xs...>>>;
        };
    } // namespace detail

    template <typename S>
    using decode = detail::decode<'\x00', S>;

#ifndef NO_TESTS
    static_assert(detail::string_eq<decode<decltype("💖💖✨🥺,👉👈💖💖,👉👈💖💖✨🥺👉👈💖💖✨🥺,👉👈"_C)>, decltype("test"_C)>());
    static_assert(detail::string_eq<encode<decltype("test"_C)>, decltype("💖💖✨🥺,👉👈💖💖,👉👈💖💖✨🥺👉👈💖💖✨🥺,👉👈"_C)>());
    static_assert(detail::string_eq<decode<decltype("❤️👉👈💖💖✨🥺,👉👈💖💖,👉👈💖💖✨🥺👉👈💖💖✨🥺,👉👈"_C)>, decltype("\x00test"_C)>());
    static_assert(detail::string_eq<encode<decltype("\x00test"_C)>, decltype("❤️👉👈💖💖✨🥺,👉👈💖💖,👉👈💖💖✨🥺👉👈💖💖✨🥺,👉👈"_C)>());
#endif
} // namespace bottom
