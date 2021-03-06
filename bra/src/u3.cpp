#include <string>
#include <ios>
#include <iomanip>
#include <sstream>

#include <ket/qubit_io.hpp>

#include <bra/gate/gate.hpp>
#include <bra/gate/u3.hpp>
#include <bra/state.hpp>


namespace bra
{
  namespace gate
  {
    std::string const u3::name_ = "U3";

    u3::u3(real_type const phase1, real_type const phase2, real_type const phase3, qubit_type const qubit)
      : ::bra::gate::gate{},
        phase1_{phase1}, phase2_{phase2}, phase3_{phase3}, qubit_{qubit}
    { }

    ::bra::state& u3::do_apply(::bra::state& state) const
    { return state.u3(phase1_, phase2_, phase3_, qubit_); }

    std::string const& u3::do_name() const { return name_; }
    std::string u3::do_representation(
      std::ostringstream& repr_stream, int const parameter_width) const
    {
      repr_stream
        << std::right
        << std::setw(parameter_width) << qubit_
        << std::setw(parameter_width) << phase1_
        << std::setw(parameter_width) << phase2_
        << std::setw(parameter_width) << phase3_;
      return repr_stream.str();
    }
  } // namespace gate
} // namespace bra
