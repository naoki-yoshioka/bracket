#ifndef BRA_MAKE_GENERAL_MPI_STATE_HPP
# define BRA_MAKE_GENERAL_MPI_STATE_HPP

# ifndef BRA_NO_MPI
#   include <vector>
#   include <memory>

#   include <yampi/communicator.hpp>
#   include <yampi/environment.hpp>

#   include <bra/state.hpp>
#   include <bra/gates.hpp>


namespace bra
{
  std::unique_ptr< ::bra::state > make_general_mpi_state(
    unsigned int const num_page_qubits,
    ::bra::state::state_integer_type const initial_integer,
    ::bra::state::bit_integer_type const num_local_qubits,
    ::bra::state::bit_integer_type const total_num_qubits,
    unsigned int const num_threads_per_process,
    ::bra::state::seed_type const seed,
    yampi::communicator const& communicator,
    yampi::environment const& environment);

  std::unique_ptr< ::bra::state > make_general_mpi_state(
    unsigned int const num_page_qubits,
    ::bra::state::state_integer_type const initial_integer,
    ::bra::state::bit_integer_type const num_local_qubits,
    std::vector< ::bra::state::permutated_qubit_type > const& initial_permutation,
    unsigned int const num_threads_per_process,
    ::bra::state::seed_type const seed,
    yampi::communicator const& communicator,
    yampi::environment const& environment);
} // namespace bra


# endif // BRA_NO_MPI

#endif // BRA_MAKE_GENERAL_MPI_STATE_HPP
