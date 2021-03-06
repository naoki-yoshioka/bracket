#ifndef KET_MPI_GATE_PAGE_PAULI_Z_HPP
# define KET_MPI_GATE_PAGE_PAULI_Z_HPP

# include <boost/config.hpp>

# include <boost/range/value_type.hpp>

# include <ket/qubit.hpp>
# include <ket/utility/meta/real_of.hpp>
# include <ket/mpi/permutated.hpp>
# include <ket/mpi/gate/page/detail/one_page_qubit_gate.hpp>


namespace ket
{
  namespace mpi
  {
    namespace gate
    {
      namespace page
      {
        namespace pauli_z_detail
        {
# ifdef BOOST_NO_CXX14_GENERIC_LAMBDAS
          template <typename Real>
          struct pauli_z
          {
            template <typename Iterator, typename StateInteger>
            void operator()(Iterator const, Iterator const one_first, StateInteger const index, int const) const
            { *(one_first + index) *= Real{-1}; }
          }; // struct pauli_z<Real>
# endif // BOOST_NO_CXX14_GENERIC_LAMBDAS
        } // namespace pauli_z_detail

        template <
          typename ParallelPolicy,
          typename RandomAccessRange, typename StateInteger, typename BitInteger>
        inline RandomAccessRange& pauli_z(
          ParallelPolicy const parallel_policy,
          RandomAccessRange& local_state,
          ::ket::mpi::permutated< ::ket::qubit<StateInteger, BitInteger> > const permutated_qubit)
        {
          using real_type = typename ::ket::utility::meta::real_of<typename boost::range_value<RandomAccessRange>::type>::type;

# ifndef BOOST_NO_CXX14_GENERIC_LAMBDAS
          return ::ket::mpi::gate::page::detail::one_page_qubit_gate<1u>(
            parallel_policy, local_state, permutated_qubit,
            [](auto const, auto const one_first, StateInteger const index, int const)
            { *(one_first + index) *= real_type{-1}; });
# else // BOOST_NO_CXX14_GENERIC_LAMBDAS
          return ::ket::mpi::gate::page::detail::one_page_qubit_gate<1u>(
            parallel_policy, local_state, permutated_qubit,
            ::ket::mpi::gate::page::pauli_z_detail::pauli_z<real_type>{});
# endif // BOOST_NO_CXX14_GENERIC_LAMBDAS
        }

        template <
          typename ParallelPolicy,
          typename RandomAccessRange, typename StateInteger, typename BitInteger>
        inline RandomAccessRange& adj_pauli_z(
          ParallelPolicy const parallel_policy,
          RandomAccessRange& local_state,
          ::ket::mpi::permutated< ::ket::qubit<StateInteger, BitInteger> > const permutated_qubit)
        { return ::ket::mpi::gate::page::pauli_z(parallel_policy, local_state, permutated_qubit); }
      } // namespace page
    } // namespace gate
  } // namespace mpi
} // namespace ket


#endif // KET_MPI_GATE_PAGE_PAULI_Z_HPP
