#ifndef BRA_UNIT_MPI_3PAGE_STATE_HPP
# define BRA_UNIT_MPI_3PAGE_STATE_HPP

# ifndef BRA_NO_MPI
#   include <vector>

#   include <ket/gate/projective_measurement.hpp>
#   include <ket/utility/parallel/loop_n.hpp>
#   include <ket/mpi/utility/unit_mpi.hpp>
#   include <ket/mpi/state.hpp>

#   include <yampi/allocator.hpp>
#   include <yampi/rank.hpp>
#   include <yampi/communicator.hpp>
#   include <yampi/environment.hpp>

#   include <bra/state.hpp>


namespace bra
{
  class unit_mpi_3page_state final
    : public ::bra::state
  {
    ket::utility::policy::parallel<unsigned int> parallel_policy_;
    using unit_mpi_policy_type
      = ket::mpi::utility::policy::unit_mpi< ::bra::state::state_integer_type, ::bra::state::bit_integer_type, unsigned int >;
    unit_mpi_policy_type mpi_policy_;

    using data_type = ket::mpi::state<complex_type, 2, yampi::allocator<complex_type>>;
    data_type data_;

   public:
    unit_mpi_3page_state(
      ::bra::state::state_integer_type const initial_integer,
      unsigned int const num_local_qubits,
      unsigned int const num_unit_qubits,
      unsigned int const total_num_qubits,
      unsigned int const num_threads_per_process,
      unsigned int const num_processes_per_unit,
      ::bra::state::seed_type const seed,
      yampi::communicator const& communicator,
      yampi::environment const& environment);

    unit_mpi_3page_state(
      ::bra::state::state_integer_type const initial_integer,
      unsigned int const num_local_qubits,
      unsigned int const num_unit_qubits,
      std::vector<qubit_type> const& initial_permutation,
      unsigned int const num_threads_per_process,
      unsigned int const num_processes_per_unit,
      ::bra::state::seed_type const seed,
      yampi::communicator const& communicator,
      yampi::environment const& environment);

    ~unit_mpi_3page_state() = default;
    unit_mpi_3page_state(unit_mpi_3page_state const&) = delete;
    unit_mpi_3page_state& operator=(unit_mpi_3page_state const&) = delete;
    unit_mpi_3page_state(unit_mpi_3page_state&&) = delete;
    unit_mpi_3page_state& operator=(unit_mpi_3page_state&&) = delete;

   private:
    unsigned int do_num_page_qubits() const override;
    unsigned int do_num_pages() const override;

    void do_hadamard(qubit_type const qubit) override;
    void do_adj_hadamard(qubit_type const qubit) override;
    void do_pauli_x(qubit_type const qubit) override;
    void do_adj_pauli_x(qubit_type const qubit) override;
    void do_pauli_y(qubit_type const qubit) override;
    void do_adj_pauli_y(qubit_type const qubit) override;
    void do_pauli_z(qubit_type const qubit) override;
    void do_adj_pauli_z(qubit_type const qubit) override;
    void do_u1(real_type const phase, qubit_type const qubit) override;
    void do_adj_u1(real_type const phase, qubit_type const qubit) override;
    void do_u2(
      real_type const phase1, real_type const phase2,
      qubit_type const qubit) override;
    void do_adj_u2(
      real_type const phase1, real_type const phase2,
      qubit_type const qubit) override;
    void do_u3(
      real_type const phase1, real_type const phase2, real_type const phase3,
      qubit_type const qubit) override;
    void do_adj_u3(
      real_type const phase1, real_type const phase2, real_type const phase3,
      qubit_type const qubit) override;
    void do_phase_shift(
      complex_type const phase_coefficient, qubit_type const qubit) override;
    void do_adj_phase_shift(
      complex_type const phase_coefficient, qubit_type const qubit) override;
    void do_x_rotation_half_pi(qubit_type const qubit) override;
    void do_adj_x_rotation_half_pi(qubit_type const qubit) override;
    void do_y_rotation_half_pi(qubit_type const qubit) override;
    void do_adj_y_rotation_half_pi(qubit_type const qubit) override;
    void do_controlled_not(
      qubit_type const target_qubit,
      control_qubit_type const control_qubit) override;
    void do_adj_controlled_not(
      qubit_type const target_qubit,
      control_qubit_type const control_qubit) override;
    void do_controlled_phase_shift(
      complex_type const phase_coefficient,
      qubit_type const target_qubit,
      control_qubit_type const control_qubit) override;
    void do_adj_controlled_phase_shift(
      complex_type const phase_coefficient,
      qubit_type const target_qubit,
      control_qubit_type const control_qubit) override;
    void do_controlled_v(
      complex_type const phase_coefficient,
      qubit_type const target_qubit,
      control_qubit_type const control_qubit) override;
    void do_adj_controlled_v(
      complex_type const phase_coefficient,
      qubit_type const target_qubit,
      control_qubit_type const control_qubit) override;
    void do_toffoli(
      qubit_type const target_qubit,
      control_qubit_type const control_qubit1,
      control_qubit_type const control_qubit2) override;
    void do_adj_toffoli(
      qubit_type const target_qubit,
      control_qubit_type const control_qubit1,
      control_qubit_type const control_qubit2) override;
    ket::gate::outcome do_projective_measurement(
      qubit_type const qubit, yampi::rank const root) override;
    void do_expectation_values(yampi::rank const root) override;
    void do_measure(yampi::rank const root) override;
    void do_generate_events(yampi::rank const root, int const num_events, int const seed) override;
    void do_shor_box(
      state_integer_type const divisor, state_integer_type const base,
      std::vector<qubit_type> const& exponent_qubits,
      std::vector<qubit_type> const& modular_exponentiation_qubits) override;
    void do_clear(qubit_type const qubit) override;
    void do_set(qubit_type const qubit) override;
  }; // class unit_mpi_3page_state
} // namespace bra


# endif // BRA_NO_MPI

#endif // BRA_UNIT_MPI_3PAGE_STATE_HPP
