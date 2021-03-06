#ifndef KET_MPI_GATE_PAGE_PAULI_Y_HPP
# define KET_MPI_GATE_PAGE_PAULI_Y_HPP

# include <boost/config.hpp>

# include <algorithm>

# include <boost/range/value_type.hpp>

# include <ket/qubit.hpp>
# include <ket/utility/imaginary_unit.hpp>
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
        namespace pauli_y_detail
        {
# ifdef BOOST_NO_CXX14_GENERIC_LAMBDAS
          template <typename Complex>
          struct pauli_y
          {
            template <typename Iterator, typename StateInteger>
            void operator()(Iterator const zero_first, Iterator const one_first, StateInteger const index, int const) const
            {
              auto const zero_iter = zero_first + index;
              auto const one_iter = one_first + index;

              std::iter_swap(zero_iter, one_iter);
              *zero_iter *= -::ket::utility::imaginary_unit<Complex>();
              *one_iter *= ::ket::utility::imaginary_unit<Complex>();
            }
          }; // struct pauli_y<Complex>
# endif // BOOST_NO_CXX14_GENERIC_LAMBDAS
        } // namespace pauli_y_detail

        template <
          typename ParallelPolicy,
          typename RandomAccessRange, typename StateInteger, typename BitInteger>
        inline RandomAccessRange& pauli_y(
          ParallelPolicy const parallel_policy,
          RandomAccessRange& local_state,
          ::ket::mpi::permutated< ::ket::qubit<StateInteger, BitInteger> > const permutated_qubit)
        {
          using complex_type = typename boost::range_value<RandomAccessRange>::type;

# ifndef BOOST_NO_CXX14_GENERIC_LAMBDAS
          return ::ket::mpi::gate::page::detail::one_page_qubit_gate<0u>(
            parallel_policy, local_state, permutated_qubit,
            [](auto const zero_first, auto const one_first, StateInteger const index, int const)
            {
              auto const zero_iter = zero_first + index;
              auto const one_iter = one_first + index;

              std::iter_swap(zero_iter, one_iter);
              *zero_iter *= -::ket::utility::imaginary_unit<complex_type>();
              *one_iter *= ::ket::utility::imaginary_unit<complex_type>();
            });
# else // BOOST_NO_CXX14_GENERIC_LAMBDAS
          return ::ket::mpi::gate::page::detail::one_page_qubit_gate<0u>(
            parallel_policy, local_state, permutated_qubit,
            ::ket::mpi::gate::page::pauli_y_detail::pauli_y<complex_type>{});
# endif // BOOST_NO_CXX14_GENERIC_LAMBDAS
        }

        template <
          typename ParallelPolicy,
          typename RandomAccessRange, typename StateInteger, typename BitInteger>
        inline RandomAccessRange& adj_pauli_y(
          ParallelPolicy const parallel_policy,
          RandomAccessRange& local_state,
          ::ket::mpi::permutated< ::ket::qubit<StateInteger, BitInteger> > const permutated_qubit)
        { return ::ket::mpi::gate::page::pauli_y(parallel_policy, local_state, permutated_qubit); }
      } // namespace page
    } // namespace gate
  } // namespace mpi
} // namespace ket


#endif // KET_MPI_GATE_PAGE_PAULI_Y_HPP
