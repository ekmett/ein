module;

export module ein.pretty;

/// exceptions as control flow. do as i say, not as i do.
namespace ein::pretty {

/// this represents a witdth if n characters at present
/// \note in the future it may represent pixels or em or some other unit
using width_t = uint32_t;

#if 0
// pretty::colors
export struct colors {
  // todo: color, italic, bold
  // constexpr colors () noexcept = default, comes by default while we are empty
  cosntexpr colors operator +(ein_noescape colors const & other) {}
};
#endif

/// carries the state of our pretty printer
struct printer;

/// this is punned between the output type and the pretty printer that prints a newline
export constexpr struct newline_t {
  /// \note does not throw, unlike other pretty printers
  constexpr static void pp(printer & p) noexcept;

  template <typename OStream> ein_inline
  OStream & operator << (ein_lifetimeound OStream & os) const noexcept {
    return os << std::endl;
  }
} newline;

/// this is punned between the output type and the pretty printer that represents a sequence
/// of \p n spaces.
/// \note in the future this may be a space n units wide instead, when dealing with variable width fonts.
export constexpr struct spaces {
  width_t n;

  /// \throws \ref bad
  void pp(printer & p);

  template <typename OStream> ein_inline
  friend OStream & operator << (ein_lifetimeound OStream & os, spaces & self) const noexcept {
    static constexpr char const SPACES[] = "                                                              ";
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
  static width_t operator ()(spaces s) noexcept { return s.n; }
  static width_t operator ()(ein_noescape string const & s) noexcept { return s.size(); }
  static width_t operator ()(static_string s) noexcept { return s.size(); }
  static width_t operator ()(piece p) noexcept { std::visit(size{},p); }
};

struct out;

// struct annotated { ann a; vector<out> body; };

/// a sequence of outputs
export struct seq {
  vector<out> body;

  template <typename OStream>
  ein_inline
  OStream & operator << (ein_lifetimeound OStream & os) const noexcept {
    for (auto & o: body) os << o;
    return os;
  }
}

/// out is the tree used to return the pretty printed results.
struct out {
  /// \todo: `annotated`
  std::variant<seq,newline,string,static_string,spaces> var;
};

/// We can throw pretty::bad.
struct bad{};

/// RAII off a lambda
template <typename F>
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
  width_t max_ribbon = 60;
  width_t nesting = 0;
  width_t ribbon_width = 0;
  bool flat = false;
  bool can_fail = false;

  /// pieces take up width, but have no height
  /// \throws bad
  template <typename T>
  requires requires(T c) { piece{c}; width(c); }
  void piece(T c) {
    size_t new_ribbon_width = ribbon_width + width(p);
    if (can_fail && (new_ribbon_width > min(max_width, nesting + max_ribbon)))
      throw bad{};
    output.emplace_back({c});
    ribbon_width = new_ribbon_width;
  }

  /// used only for guttering right now.
  /// any text placed in the gutter left of our nesting level is not counted against the ribbon width
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

  /// tell subsequent lines that they should be indented by w more characters
  /// \throws \ref bad
  auto nest(width_t w, doc auto body) {
    finally remember { [this,old_nesting=nesting] noexcept { nesting = old_nesting } };
    nesting += w;
    return body.pp(this);
  };

  /// intersperse \p delim between \p docs
  /// \throws \ref bad
  auto intersperse(doc auto delim, doc auto ... docs) {
    auto delim_if_not_first = [delim=std::forward<Delim>(delim),first=true](printer & p) {
      if (first) first = false;
      else delim.pp(p);
    };
    ((delim_if_not_first(this),docs.pp(this)),...);
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

/// documentary

// you can pretty print any doc
template <doc T>
auto pp(printer & p, T t) {
  return t.pp(p);
}

// you can pretty print any string
void pp(printer & p, string s) {
  p.piece(s);
}

// you can pretty print any static_string (without copying)
void pp(printer & p, static_string s) {
  p.piece(s);
}

void pp(printer & p, piece pie) {
  p.piece(pie);
}

// you are printable if pp works on your type
template <typename T>
concept printable = requires (printer & p, T t) {
  pp(p,t);
}

// make a pretty printer from a lambda
template <typename F>
struct make_pretty {
  F f;
  void pp(printer &p) {
    return f(p);
  }
};

// `printable<T> = doc<autodoc<T>>`
template <printable T>
struct autodoc {
  T t;
  /// \throws \ref bad
  void pp(printer & p) {
    return pp(p,t);
  }
}

// if the type itself is already a document, just pass it along
template <doc D>
D && document(D && d) noexcept {
  return std::forward<D>(d);
}

// automatically produce documents given a documentary
template <documented T>
requires !doc<T>
auto document(T && t) -> decltype(auto) noexcept {
  return std::forward_like<T>(autodoc(std::forward<T>(t)));
}

/// represents a hard carriage return/line feed. (moves to start of next line, regardless of nesting level)
export constexpr struct hardline_t {
  /// \note unlike other pp definitions this one does not throw
  constexpr static void pp(printer & p) noexcept {
    return p.hardline();
  }
} hardline;

void newline_t::pp(printer & p) noexcept { p.hardline(); }
void spaces::pp(printer & p) { p.spaces(n); }

// doc
export template <documented D>
struct nest {
  width_t w;
  D body;
  void pp(printer & p) {
    p.nest(w,document(body))
  }
};

export template <documented Delim, documented ... Args>
auto intersperse(Delim && delim, Args && ... args) {
  return make_pretty([delim = std::forward<Delim>(delim), ... args=std::forward<Args>(args)](printer &p) {
    p.intersperse(document(delim),document(arg) ...);
  });
}

export template <documented ... Args>
auto hsep(Args && ... args) {
  return intersperse(space,std::forward<Args>(args)...)
}

export template <documented ... Args>
auto vsep(Args && ... args) {
  return intersperse(newline,std::forward<Args>(args)...)
}

export template <documented ... Args>
auto hsep_tight(Args && ... args) {
  return intersperse(make_pretty([](printer & p) { if (not p.is_flat) p.space() }), args...);
}

export template <documented ... Args>
auto hvsep_tight(Args && ... args) {
  return intersperse(make_pretty([](printer & p) { if (not p.is_flat) p.newline() }), args...);
}

auto guttered(string t) {
  return make_pretty([t](printer & p) {
    width_t s = 1; // width of a space
    if (p.flat) p.spaces(s);
    else {
       p.hardline();
       auto delta = width(t) + s;
       p.space(p.nesting >= delta ? nesting - delta : nesting);
    }
    p.text(t);
    p.spaces(s);
  });
  auto s = 1; // width of a space
  if (p.flat)
}

// auto align(doc auto x) -> doc auto
// auto align(docfun<T> auto x) -> docfun<T> auto

export template <doc D>
struct align {
  D body;
  auto pp(printer & p) {
    return p.nest(p.ribbon_width,document(body));
  }
}

export template <doc D>
autodoc grouped(D && body) {
  return [w,body=std::forward<D>(body)](printer & p) {
    if (not flat) {
      finally remember { [&p,could_fail=p.can_fail] noexcept { p.flat = false; p.can_fail = could_fail; } };
      p.can_fail = true;
      p.flat = true;
      return body(this);
    }
    return body(this);
  };
}

}; // namespace pretty

