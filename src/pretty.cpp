
#include "pretty.hpp"

// output

template constexpr void newline_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
template constexpr void newline_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
template constexpr std::ostream & operator<<(ein_lifetimebound std::ostream & ein_restrict, newline_t) noexcept;
constexpr newline_t newline;

template constexpr void spaces::pp<false>(this spaces, ein_noescape printer & ein_restrict) noexcept(true);
template constexpr void spaces::pp<true>(this spaces, ein_noescape printer & ein_restrict) noexcept(false);
template constexpr std::ostream & operator<<(ein_lifetimebound std::ostream & ein_restrict, spaces) noexcept;
constexpr spaces space{1};

template constexpr std::ostream & operator<<(ein_lifetimebound std::ostream & ein_restrict, seq) noexcept;

template constexpr std::ostream & operator<<(ein_lifetimebound std::ostream & ein_restrict, out) noexcept;

// output that puns as documents

template constexpr void hardline_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
template constexpr void hardline_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
constexpr hardline_t hardline;
constexpr empty_t empty;

// documents

template constexpr void hv_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
template constexpr void hv_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
constexpr hv_t hv;

template constexpr void h_tight_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
template constexpr void h_tight_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
constexpr h_tight_t h_tight;

template constexpr void hv_tight_t::pp<true>(ein_noescape printer & ein_restrict) noexcept(false);
template constexpr void hv_tight_t::pp<false>(ein_noescape printer & ein_restrict) noexcept(true);
constexpr hv_tight_t hv_tight;

