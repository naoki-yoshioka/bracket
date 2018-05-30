#ifndef BRA_GATE_ADJ_U2_HPP
# define BRA_GATE_ADJ_U2_HPP

# include <boost/config.hpp>

# include <string>
# include <iosfwd>

# include <bra/gate/gate.hpp>
# include <bra/state.hpp>

# ifdef BOOST_NO_CXX11_FINAL
#   define final 
#   define override 
# endif // BOOST_NO_CXX11_FINAL


namespace bra
{
  namespace gate
  {
    class adj_u2 final
      : public ::bra::gate::gate
    {
     public:
      typedef ::bra::state::qubit_type qubit_type;
      typedef ::bra::state::real_type real_type;

     private:
      real_type phase1_;
      real_type phase2_;
      qubit_type qubit_;

      static std::string const name_;

     public:
      adj_u2(
        real_type const phase1, real_type const phase2,
        qubit_type const qubit);
# ifndef BOOST_NO_CXX11_DEFAULTED_FUNCTIONS
      ~adj_u2() = default;
# else
      ~adj_u2() { }
# endif

     private:
# ifndef BOOST_NO_CXX11_DELETED_FUNCTIONS
      adj_u2(adj_u2 const&) = delete;
      adj_u2& operator=(adj_u2 const&) = delete;
#   ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
      adj_u2(adj_u2&&) = delete;
      adj_u2& operator=(adj_u2&&) = delete;
#   endif // BOOST_NO_CXX11_RVALUE_REFERENCES
# else // BOOST_NO_CXX11_DELETED_FUNCTIONS
      adj_u2(adj_u2 const&);
      adj_u2& operator=(adj_u2 const&);
#   ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
      adj_u2(adj_u2&&);
      adj_u2& operator=(adj_u2&&);
#   endif // BOOST_NO_CXX11_RVALUE_REFERENCES
# endif // BOOST_NO_CXX11_DELETED_FUNCTIONS

     private:
      ::bra::state& do_apply(::bra::state& state) const override;
      std::string const& do_name() const override;
      std::string do_representation(
        std::ostringstream& repr_stream, int const parameter_width) const override;
    };
  }
}


# ifdef BOOST_NO_CXX11_FINAL
#   undef final 
#   undef override 
# endif // BOOST_NO_CXX11_FINAL

#endif

