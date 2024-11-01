#include "ein/static_string.hpp"

namespace ein {

extern template class basic_static_string<char,std::char_traits<char>>;
extern template class basic_static_string<wchar_t,std::char_traits<wchar_t>>;
extern template class basic_static_string<char8_t,std::char_traits<char8_t>>;
extern template class basic_static_string<char16_t,std::char_traits<char16_t>>;
extern template class basic_static_string<char32_t,std::char_traits<char32_t>>;

}
