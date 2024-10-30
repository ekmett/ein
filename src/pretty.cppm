module;

#ifdef EIN_PRELUDE
#include "prelude"
#elifndef EIN_PCH
#include <cstdint>
#include "attribues.hpp"
#include "config.hpp"
#include "concepts.hpp"
#endif

export module ein.pretty;

/// exceptions as control flow. do as i say, not as i do.
namespace ein::pretty {

/// this represents a witdth if n characters at present
/// \note in the future it may represent pixels or em or some other unit
export using width_t = uint32_t;

#if 0
// pretty::colors
export struct colors {
  // todo: color, italic, bold
  // constexpr colors () noexcept = default, comes by default while we are empty
  cosntexpr colors operator +(ein_noescape colors const & other) {}
};
#endif

/// carries the state of our pretty printer
export struct printer;

/// this is punned between the output type and the pretty printer that prints a newline
/// \implements doc
export constexpr struct newline_t {
  /// \note does not \c throw, unlike other \ref pretty::doc types
  ein_inline
  constexpr static void pp(printer & ein_restrict p) noexcept;

  template <typename OStream> constexpr ein_inline
  OStream & operator << (ein_lifetimebound OStream & ein_restrict os) const noexcept {
    return os << std::endl;
  }
} newline;

/// this is punned between the output type and the pretty printer that represents a sequence
/// of \p n spaces.
/// \note in the future this may be a space n units wide instead, when dealing with variable width fonts.
/// \implements doc
export constexpr struct spaces {
  width_t n;

  /// \throw bad backtracking required
  ein_inline
  constexpr void pp(ein_noescape printer & p);

  template <typename OStream> constexpr ein_inline
  friend OStream & operator << (ein_lifetimeound OStream & ein_restrict os, spaces self) const noexcept {
    static constexpr char const SPACES[] =
      "                                                                                                                ";
    constexpr size_t k = sizeof(SPACES)-1;
    for (int i=0;i<self.n/k;++i)
      os << SPACES;
    return os << SPACES + k+self.n/k*k-self.n;
  }
} space{1};

/// a piece corresponds to a unit of text that width, but no height.
using piece = std::variant<string,static_string,spaces>;

/// all pieces can be measured for width
struct width {
  ein_inline ein_nodiscard ein_pure
  static constexpr width_t operator ()(spaces s) noexcept { return s.n; }
  ein_inline ein_nodiscard ein_pure
  static constexpr width_t operator ()(ein_noescape string const & s) noexcept { return s.size(); }
  ein_inline ein_nodiscard ein_pure
  static constexpr width_t operator ()(static_string s) noexcept { return s.size(); }
  ein_inline ein_nodiscard ein_pure
  static constexpr width_t operator ()(piece p) noexcept { std::visit(size{},p); }
};

export struct out;

// struct annotated { ann a; vector<out> body; };

/// a sequence of outputs
export struct ein_nodiscard seq {
  vector<out> body;

  template <typename OStream> friend constexpr ein_inline
  OStream & operator << (ein_lifetimeound OStream & ein_restrict os, seq const & self) noexcept {
    for (auto & o: self.body) os << o;
    return os;
  }
}

/// out is the tree used to return the pretty printed results.
export struct ein_nodiscard out {
  /// \todo: `annotated`
  std::variant<seq,newline,string,static_string,spaces> var;
};

/// control flow exception
export struct bad{};

/// RAII off a lambda
export template <typename F>
struct finally {
  F f;
  ~finally() noexcept { f(); }
};

// forward declaration
struct printer;

/// a doc is anything that has a pp method that can send stuff to the printer
template <typename F>
concept doc = requires (printer & p, F f) {
  f.pp(p);
}

/// a docfun is a pretty printer that returns some other info on the stack.
/// only rudimentary support for these is provided, but they could be used
/// to pass size information or other side-channel info around.
/// if we decide we want a lot more of these then vsep, etc. could be designed
/// to tuple their \ref doc arguments' results to enable users to collect data more broadly
template <typename F, typename A>
concept docfun = requires (printer & p, F f) {
  { f.pp(p) } -> std::same_as<A>;
}

/// A \ref pretty::printer holds the application state used while pretty printing.
/// The backtracking on the other hand is done with exceptions for readable
/// control flow.
struct printer {
  vector<out> output{};
  width_t max_width = 80;
  width_t max_ribbon_width = 60;
  width_t nesting = 0;
  width_t ribbon_width = 0;
  bool flat = false;
  bool can_fail = false;
    // encapsulate this as a static_option<can_fail,> of the parameters for max_width, max_ribbon?
    // alternately we could invert can_fail to 'safe' and use that as an extra parameter to pp<safe>(...) passing
    // it into piece, etc. here then all this conditional logic would be guarded, an the top level call would be
    // showable to be safe

  /// \brief pieces take up width, but have no height
  /// \throws bad to backtrack
  template <typename T>
  requires requires(T c) { piece{c}; width(c); }
  void piece(T c) {
    size_t new_ribbon_width = ribbon_width + width(p);
    if (can_fail && (new_ribbon_width > min(max_width - nesting, max_ribbon_width)))
      throw bad{};
    output.emplace_back({c});
    ribbon_width = new_ribbon_width;
  }

  /// any text placed in the gutter left of our nesting level is not counted against the ribbon width
  /// \note used only for guttering right now.
  void hardline() noexcept {
    output.emplace_back(hardline{});
    ribbon_width = -nesting;
  }

  /// a newine, followed by indentation to restore our nesting level
  void newline() noexcept {
    output.emplace_back(hardline{});
    output.emplace_back(spaces{nesting});
    ribbon_width = 0;
  }

  /// tell subsequent lines that they should be indented by \p w more characters than
  /// current \ref nesting. primarily used by \ref align
  /// \throws bad to backtrack
  auto nest(width_t w, doc auto body) {
    finally remember { [this,old_nesting=nesting] noexcept { nesting = old_nesting } };
    nesting += w;
    return body.pp(this);
  };

  /// intersperse \p delim between \p docs
  /// \throws bad to backtrack
  void intersperse(doc auto delim, doc auto ... docs) {
    auto delim_if_not_first = [delim=std::forward<Delim>(delim),first=true](printer & p) {
      if (first) first = false;
      else delim.pp(p);
    };
    (void((void(delim_if_not_first(this)),void(docs.pp(this)))),...,void());
  }

#if 0
  auto annotate(annotation ann, doc auto body) {
    finally remember {
      [&,this,old_output = exchange(output,{}), old_format = format] noexcept {
        using std::swap;
        old_output.emplace_back(annotated(ann,output);
        output = std::move(old_output);
        format = old_format;
      }
    };
    format += ann.delta();
    return body.pp(this);
  }
#endif
}; // struct printer

/// \ref ein::pretty::doc
struct empty_t {
  ein_inline
  static void pp(ein_noescape printer & ein_restrict) nothrow {}
} empty;

/** \name typesetting

      \details

        overload `typeset` for your types to make them pretty printable
        or define a member function `void pp(printer & p)`
   \{ */

/// you can typeset any doc
/// \throws bad to backtrack
template <doc T> constexpr ein_inline
auto typeset(ein_noescape printer & ein_restrict p, T t) {
  return t.pp(p);
}


/// \throws bad to backtrack
/// you can typeset a string
/// \pre the string doesn't contain spaces or newlines
constexpr ein_inline
void typeset(ein_noescape printer & ein_restrict p, string s) {
  p.piece(s);
}

/// \throws bad to backtrack
/// you can pretty print any static_string (without copying)
///
constexpr ein_inline
void typeset(ein_noescape printer & ein_restrict p, static_string s) {
  p.piece(s);
}

/// \throws bad to backtrack
constexpr ein_inline
void typeset(ein_noescape printer & ein_restrict p, piece pie) {
  p.piece(pie);
}

// you can print anything you can typeset
template <typename T>
concept printable = requires (printer & p, T t) {
  typeset(p,t);
}

template <printable ... Args> constexpr ein_inline
void pp(ein_noescape printer & ein_restrict p, Args && ... args) {
  (void(typeset(p,args)),...,void());
}

/// \}

// make a pretty printer from a lambda
template <typename F>
struct fun {
  F f;
  /// \throws bad to backtrack
  void pp(ein_noescape printer & ein_restrict p) {
    return f(p);
  }
};

/// Usable if you need to cast several docs to the same type
/// rather than pass around functors, but ask yourself if you
/// really should be using this slow path.
using polydoc = fun<std::function<void (printer &)>>;

/// `printable<T> = doc<draft<T>>`
template <printable T>
struct draft {
  T t;
  /// \throws bad to backtrack
  void pp(ein_noescape printer & ein_restrict p) {
    return pp(p,t);
  }
}

/// if the type itself is already a document, just pass it along
template <doc D>
D && document(D && d) noexcept {
  return std::forward<D>(d);
}

/// automatically produce documents given a documentary
template <documented T>
requires !doc<T>
auto document(T && t) -> decltype(auto) noexcept {
  return std::forward_like<T>(draft(std::forward<T>(t)));
}

void newline_t::pp(ein_noescape printer & ein_restrict p) noexcept { p.newline(); }

void spaces::pp(ein_noescape printer & ein_restrict p) { p.spaces(n); }

/// represents a hard carriage return/line feed. (moves to start of next line, regardless of nesting level)
export constexpr struct hardline_t {
  /// \note unlike most other pp definitions this one does not throw
  constexpr static void pp(ein_noescape printer & ein_restrict p) noexcept {
    return p.hardline();
  }
} hardline;

// \implements doc
export template <documented D> constexpr
struct nest {
  width_t w;
  D body;
  // throws bad to backtrack
  void pp(ein_noescape printer & ein_restrict p) {
    p.nest(w,document(body))
  }
};

// auto align(doc auto x) -> doc auto
// auto align(docfun<T> auto x) -> docfun<T> auto

/// \implements doc
export template <doc D>
struct align {
  D body;
  auto pp(ein_noescape printer & ein_restrict p) {
    return p.nest(p.ribbon_width,document(body));
  }
}

/// \returns doc auto
export template <documented Delim, documented ... Args> constexpr
autodoc intersperse(Delim && delim, Args && ... args) noexcept {
  return fun([delim = std::forward<Delim>(delim), ... args=std::forward<Args>(args)](ein_noescape printer & ein_restrict p) {
    p.intersperse(document(delim),document(arg) ...);
  });
}

/// \returns doc auto
export template <documented ... Args> constexpr
autodoc hsep(Args && ... args) noexcept {
  return intersperse(space,std::forward<Args>(args)...)
}

/// \returns doc auto
export template <documented ... Args> constexpr
autodoc vsep(Args && ... args) noexcept {
  return intersperse(newline,std::forward<Args>(args)...)
}

/// \returns doc auto
export template <documented ... Args>
autodoc hvsep(Args && ... args) noexcept {
  return intersperse(fun([] static (ein_noescape printer & ein_restrict p) { p.is_flat ? p.space() : p.newline(); }, std::forward<Args>(args)...);
}

/// \returns doc auto
export template <documented ... Args> constexpr
autodoc hsep_tight(Args && ... args) noexcept {
  return intersperse(fun([] static (ein_noescape printer & ein_restrict p) { if (not p.is_flat) p.space(); }), args...);
}

/// \returns doc auto
export template <documented ... Args> constexpr
autodoc hvsep_tight(Args && ... args) noexcept {
  return intersperse(fun([] static constexpr (ein_nodiscard printer & ein_restrict p) { if (not p.is_flat) p.newline(); }), args...);
}

/// \returns doc auto
constexpr
autodoc guttered(piece pie) noexcept {
  return fun([t](ein_nodiscard printer & ein_restrict p) {
    static constexpr width_t s = 1; // width of a space
    if (p.flat) p.spaces(s);
    else {
       p.hardline();
       auto delta = width(t) + s;
       p.space(p.nesting >= delta ? nesting - delta : nesting);
    }
    p.piece(pie);
    p.spaces(s);
  });
  auto s = 1; // width of a space
  if (p.flat)
}

/// \returns doc auto
export template <documented D>
autodoc grouped(D && body) {
  return fun([body=std::forward<D>(body)](printer & p) {
    if (not flat) {
      finally remember { [&p,could_fail=p.can_fail] noexcept { p.flat = false; p.can_fail = could_fail; } };
      p.can_fail = true;
      p.flat = true;
      return pp(p,body);
    }
    return pp(p,body);
  });
}

/** \brief comma separated list of words, with an oxford comma and stated conjunction
    \details
    \code{.cpp}
      hsep("expected",oxford("or","foo","bar","baz","quux"))
    \encode

    depending on current ribbon width and code position could produce output like

    \code
      expected foo, bar, baz, or quux
    \endcode

    or

    \code
      expected foo,
               bar,
               baz,
            or quux
    \endcode

    \returns doc auto */
export template <documented ... Args> constexpr
autodoc oxford(piece conjunction, Args && ... args) {
  using N = sizeof...(args);
  if constexpr (N == 0)      return document("nothing")
  else if constexpr (N == 1) return document(std::forward<Args...[0]>(args...[0]));
  } else {
    return grouped(align(fun([conjunction,... args = std::forward<Args>(args)](printer &p) {
      val s = 1; // space width
      if constexpr (N == 2) {
        pp(p,std::forward<Args...[0]>(args...[0]), guttered(conjunction), std::forward<Args>(args...[1]));
      } else {
        auto sep = []<size_t i> constexpr static (printer & p, piece & conjunction) {
          if (i == N-1) return;
          p.text(",")
          if (i == N-2) guttered(conjunction).pp(p);
          else if (p.is_flat) p.space(s)
          else p.newline();
        };
        [&piece, ... docs=std::forward<Docs>(docs)]<size_t... i>(std::index_sequence<i...>) {
          (( document(args...[i]).pp(p),sep.template operator()<i>(p,conjunction)) ... );
        }(std::make_index_sequence<T,N>{});
      }
    })));
  }
}

// \ref oxford, taking iterators over some container full of documentable values
// \returns doc auto
export template <typename It> constexpr
autodoc oxford_it(piece conjunction, It s, It e) {
  std::size_t N = e - s;
  return fun([N.conjunction,s,e](ein_noescape printer & ein_restrict p) {
    if (N == 0) p.piece("nothing")
    else if (N == 1) document(*s).pp(p);
    else grouped(align(fun([N,conjunction,s,e](ein_noescape printer & ein_restrict p) {
      if (N == 2) {
        typeset(p,*s++);
        guttered(conjunction).pp(p);
        typeset(p,*s);
      } else {
        for (int i=0;i<N;++i,++s) {
          typeset(p,*s++);
          if (i != N-1) {
            p.text(",");
            if (i == N-2) guttered(conjunction).pp(p);
            else if (p.is_flat) p.space(s);
                 else p.newline();
          }
        }
      }
    }))).pp(p);
  );
}

}; // namespace pretty
