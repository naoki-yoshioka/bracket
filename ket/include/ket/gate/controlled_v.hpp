#ifndef KET_GATE_CONTROLLED_V_HPP
# define KET_GATE_CONTROLLED_V_HPP

# include <boost/config.hpp>

# include <cassert>
# ifdef KET_PREFER_POINTER_TO_VECTOR_ITERATOR
#   include <vector>
# endif
# include <iterator>
# include <utility>
# ifndef NDEBUG
#   ifndef BOOST_NO_CXX11_HDR_TYPE_TRAITS
#     include <type_traits>
#   else
#     include <boost/type_traits/is_unsigned.hpp>
#     include <boost/type_traits/is_same.hpp>
#   endif
# endif
# ifdef BOOST_NO_CXX11_STATIC_ASSERT
#   include <boost/static_assert.hpp>
# endif
# ifdef KET_PREFER_POINTER_TO_VECTOR_ITERATOR
#   ifndef BOOST_NO_CXX11_ADDRESSOF
#     include <memory>
#   else
#     include <boost/core/addressof.hpp>
#   endif
# endif

# include <boost/math/constants/constants.hpp>
# include <boost/algorithm/minmax.hpp>
# include <boost/tuple/tuple.hpp>
# include <boost/range/begin.hpp>
# include <boost/range/end.hpp>

# include <ket/qubit.hpp>
# include <ket/control.hpp>
# include <ket/utility/loop_n.hpp>
# include <ket/utility/integer_exp2.hpp>
# ifndef NDEBUG
#   include <ket/utility/integer_log2.hpp>
# endif
# include <ket/utility/exp_i.hpp>
# include <ket/utility/meta/real_of.hpp>

# ifndef BOOST_NO_CXX11_HDR_TYPE_TRAITS
#   define KET_is_unsigned std::is_unsigned
#   define KET_is_same std::is_same
# else
#   define KET_is_unsigned boost::is_unsigned
#   define KET_is_same boost::is_same
# endif

# ifdef BOOST_NO_CXX11_STATIC_ASSERT
#   define static_assert(exp, msg) BOOST_STATIC_ASSERT_MSG(exp, msg)
# endif

# ifdef KET_PREFER_POINTER_TO_VECTOR_ITERATOR
#   ifndef BOOST_NO_CXX11_ADDRESSOF
#     define KET_addressof std::addressof
#   else
#     define KET_addressof boost::addressof
#   endif
# endif


namespace ket
{
  namespace gate
  {
    // controlled_v_coeff
    namespace controlled_v_detail
    {
# ifdef BOOST_NO_CXX11_LAMBDAS
      template <
        typename RandomAccessIterator,
        typename Complex, typename StateInteger>
      struct controlled_v_coeff_loop_inside
      {
        RandomAccessIterator first_;
        Complex one_plus_phase_coefficient_; // 1+e^{i\phi}
        Complex one_minus_phase_coefficient_; // 1-e^{i\phi}
        StateInteger target_qubit_mask_;
        StateInteger control_qubit_mask_;
        StateInteger lower_bits_mask_;
        StateInteger middle_bits_mask_;
        StateInteger upper_bits_mask_;

        controlled_v_coeff_loop_inside(
          RandomAccessIterator const first,
          Complex const& one_plus_phase_coefficient, // 1+e^{i\phi}
          Complex const& one_minus_phase_coefficient, // 1-e^{i\phi}
          StateInteger const target_qubit_mask,
          StateInteger const control_qubit_mask,
          StateInteger const lower_bits_mask,
          StateInteger const middle_bits_mask,
          StateInteger const upper_bits_mask)
          : first_(first),
            one_plus_phase_coefficient_(one_plus_phase_coefficient),
            one_minus_phase_coefficient_(one_minus_phase_coefficient),
            target_qubit_mask_(target_qubit_mask),
            control_qubit_mask_(control_qubit_mask),
            lower_bits_mask_(lower_bits_mask),
            middle_bits_mask_(middle_bits_mask),
            upper_bits_mask_(upper_bits_mask)
        { }

        void operator()(StateInteger const value_wo_qubits, int const) const
        {
          // xxx0_txxx0_cxxx
          StateInteger const base_index
            = ((value_wo_qubits bitand upper_bits_mask_) << 2u)
              bitor ((value_wo_qubits bitand middle_bits_mask_) << 1u)
              bitor (value_wo_qubits bitand lower_bits_mask_);
          // xxx0_txxx1_cxxx
          StateInteger const control_on_index
            = base_index bitor control_qubit_mask_;
          // xxx1_txxx1_cxxx
          StateInteger const target_control_on_index
            = control_on_index bitor target_qubit_mask_;
          RandomAccessIterator const control_on_iter = first_+control_on_index;
          RandomAccessIterator const target_control_on_iter
            = first_+target_control_on_index;
          Complex const control_on_iter_value = *control_on_iter;

          typedef
            typename ::ket::utility::meta::real_of<Complex>::type
            real_type;
          using boost::math::constants::half;
          *control_on_iter
            = half<real_type>()
              * (one_plus_phase_coefficient_*control_on_iter_value
                 + one_minus_phase_coefficient_*(*target_control_on_iter));
          *target_control_on_iter
            = half<real_type>()
              * (one_minus_phase_coefficient_*control_on_iter_value
                 + one_plus_phase_coefficient_*(*target_control_on_iter));
        }
      };

      template <
        typename RandomAccessIterator,
        typename Complex, typename StateInteger>
      inline controlled_v_coeff_loop_inside<
        RandomAccessIterator, Complex, StateInteger>
      make_controlled_v_coeff_loop_inside(
        RandomAccessIterator const first,
        Complex const& one_plus_phase_coefficient,
        Complex const& one_minus_phase_coefficient,
        StateInteger const target_qubit_mask,
        StateInteger const control_qubit_mask,
        StateInteger const lower_bits_mask,
        StateInteger const middle_bits_mask,
        StateInteger const upper_bits_mask)
      {
        return controlled_v_coeff_loop_inside<
          RandomAccessIterator, Complex, StateInteger>(
            first, one_plus_phase_coefficient, one_minus_phase_coefficient,
            target_qubit_mask, control_qubit_mask,
            lower_bits_mask, middle_bits_mask, upper_bits_mask);
      }
# endif // BOOST_NO_CXX11_LAMBDAS

      template <
        typename ParallelPolicy, typename RandomAccessIterator,
        typename Complex, typename StateInteger, typename BitInteger>
      void controlled_v_coeff_impl(
        ParallelPolicy const parallel_policy,
        RandomAccessIterator const first, RandomAccessIterator const last,
        Complex const& phase_coefficient,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        static_assert(
          KET_is_unsigned<StateInteger>::value,
          "StateInteger should be unsigned");
        static_assert(
          KET_is_unsigned<BitInteger>::value,
          "BitInteger should be unsigned");
        static_assert(
          (KET_is_same<
             Complex,
             typename std::iterator_traits<RandomAccessIterator>::value_type>::value),
          "Complex must be the same to value_type of RandomAccessIterator");

        assert(
          ::ket::utility::integer_exp2<StateInteger>(target_qubit)
            < static_cast<StateInteger>(last-first)
          and ::ket::utility::integer_exp2<StateInteger>(
                control_qubit.qubit())
                < static_cast<StateInteger>(last-first)
          and target_qubit != control_qubit.qubit());
        assert(
          ::ket::utility::integer_exp2<StateInteger>(
            ::ket::utility::integer_log2<BitInteger>(last-first))
          == static_cast<StateInteger>(last-first));

        typedef typename ::ket::utility::meta::real_of<Complex>::type real_type;
        Complex const one_plus_phase_coefficient = static_cast<real_type>(1)+phase_coefficient;
        Complex const one_minus_phase_coefficient = static_cast<real_type>(1)-phase_coefficient;

        typedef ::ket::qubit<StateInteger, BitInteger> qubit_type;
        boost::tuple<qubit_type, qubit_type> const minmax_qubits
          = boost::minmax(target_qubit, control_qubit.qubit());
        StateInteger const target_qubit_mask
          = ::ket::utility::integer_exp2<StateInteger>(target_qubit);
        StateInteger const control_qubit_mask
          = ::ket::utility::integer_exp2<StateInteger>(
              control_qubit.qubit());
        using boost::get;
        StateInteger const lower_bits_mask
          = ::ket::utility::integer_exp2<StateInteger>(get<0u>(minmax_qubits))
            - static_cast<StateInteger>(1u);
        StateInteger const middle_bits_mask
          = (::ket::utility::integer_exp2<StateInteger>(
               get<1u>(minmax_qubits)-static_cast<qubit_type>(1u))
             - static_cast<StateInteger>(1u))
            xor lower_bits_mask;
        StateInteger const upper_bits_mask
          = compl (lower_bits_mask bitor middle_bits_mask);

        using ::ket::utility::loop_n;
# ifndef BOOST_NO_CXX11_LAMBDAS
        loop_n(
          parallel_policy,
          static_cast<StateInteger>(last-first)/4u,
          [first, &one_plus_phase_coefficient, &one_minus_phase_coefficient,
           target_qubit_mask, control_qubit_mask,
           lower_bits_mask, middle_bits_mask, upper_bits_mask](
            StateInteger const value_wo_qubits, int const)
          {
            // xxx0_txxx0_cxxx
            StateInteger const base_index
              = ((value_wo_qubits bitand upper_bits_mask) << 2u)
                bitor ((value_wo_qubits bitand middle_bits_mask) << 1u)
                bitor (value_wo_qubits bitand lower_bits_mask);
            // xxx0_txxx1_cxxx
            StateInteger const control_on_index = base_index bitor control_qubit_mask;
            // xxx1_txxx1_cxxx
            StateInteger const target_control_on_index
              = control_on_index bitor target_qubit_mask;
            RandomAccessIterator const control_on_iter = first+control_on_index;
            RandomAccessIterator const target_control_on_iter = first+target_control_on_index;
            Complex const control_on_iter_value = *control_on_iter;

            using boost::math::constants::half;
            *control_on_iter
              = half<real_type>()
                * (one_plus_phase_coefficient*control_on_iter_value
                   + one_minus_phase_coefficient*(*target_control_on_iter));
            *target_control_on_iter
              = half<real_type>()
                * (one_minus_phase_coefficient*control_on_iter_value
                   + one_plus_phase_coefficient*(*target_control_on_iter));
          });
# else // BOOST_NO_CXX11_LAMBDAS
        loop_n(
          parallel_policy,
          static_cast<StateInteger>(last-first)/4u,
          ::ket::gate::controlled_v_detail::make_controlled_v_coeff_loop_inside(
            first, one_plus_phase_coefficient, one_minus_phase_coefficient,
            target_qubit_mask, control_qubit_mask,
            lower_bits_mask, middle_bits_mask, upper_bits_mask));
# endif // BOOST_NO_CXX11_LAMBDAS
      }
    } // namespace controlled_v_detail

    template <
      typename RandomAccessIterator, typename Complex,
      typename StateInteger, typename BitInteger>
    inline void controlled_v_coeff(
      RandomAccessIterator const first, RandomAccessIterator const last,
      Complex const& phase_coefficient,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      ::ket::gate::controlled_v_detail::controlled_v_coeff_impl(
        ::ket::utility::policy::make_sequential(),
        first, last, phase_coefficient, target_qubit, control_qubit);
    }

    template <
      typename ParallelPolicy, typename RandomAccessIterator,
      typename Complex, typename StateInteger, typename BitInteger>
    inline void controlled_v_coeff(
      ParallelPolicy const parallel_policy,
      RandomAccessIterator const first, RandomAccessIterator const last,
      Complex const& phase_coefficient,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      ::ket::gate::controlled_v_detail::controlled_v_coeff_impl(
        parallel_policy,
        first, last, phase_coefficient, target_qubit, control_qubit);
    }

    namespace ranges
    {
      template <
        typename RandomAccessRange, typename Complex,
        typename StateInteger, typename BitInteger>
      inline RandomAccessRange& controlled_v_coeff(
        RandomAccessRange& state, Complex const& phase_coefficient,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        ::ket::gate::controlled_v_detail::controlled_v_coeff_impl(
          ::ket::utility::policy::make_sequential(),
          boost::begin(state), boost::end(state), phase_coefficient, target_qubit, control_qubit);
        return state;
      }

      template <
        typename ParallelPolicy, typename RandomAccessRange,
        typename Complex, typename StateInteger, typename BitInteger>
      inline RandomAccessRange& controlled_v_coeff(
        ParallelPolicy const parallel_policy,
        RandomAccessRange& state, Complex const& phase_coefficient,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        ::ket::gate::controlled_v_detail::controlled_v_coeff_impl(
          parallel_policy,
          boost::begin(state), boost::end(state), phase_coefficient, target_qubit, control_qubit);
        return state;
      }

# ifdef KET_PREFER_POINTER_TO_VECTOR_ITERATOR
      template <
        typename Complex, typename Allocator,
        typename StateInteger, typename BitInteger>
      inline std::vector<Complex, Allocator>& controlled_v_coeff(
        std::vector<Complex, Allocator>& state,
        Complex const& phase_coefficient,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        ::ket::gate::controlled_v_detail::controlled_v_coeff_impl(
          ::ket::utility::policy::make_sequential(),
          KET_addressof(state.front()), KET_addressof(state.front()) + state.size(),
          phase_coefficient, target_qubit, control_qubit);
        return state;
      }

      template <
        typename ParallelPolicy,
        typename Complex, typename Allocator,
        typename StateInteger, typename BitInteger>
      inline std::vector<Complex, Allocator>& controlled_v_coeff(
        ParallelPolicy const parallel_policy,
        std::vector<Complex, Allocator>& state,
        Complex const& phase_coefficient,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        ::ket::gate::controlled_v_detail::controlled_v_coeff_impl(
          parallel_policy,
          KET_addressof(state.front()), KET_addressof(state.front()) + state.size(),
          phase_coefficient, target_qubit, control_qubit);
        return state;
      }
# endif // KET_PREFER_POINTER_TO_VECTOR_ITERATOR
    } // namespace ranges


    template <
      typename RandomAccessIterator, typename Complex,
      typename StateInteger, typename BitInteger>
    inline void conj_controlled_v_coeff(
      RandomAccessIterator const first, RandomAccessIterator const last,
      Complex const& phase_coefficient,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      ::ket::gate::controlled_v_coeff(
        first, last, static_cast<Complex>(1)/phase_coefficient, target_qubit, control_qubit);
    }

    template <
      typename ParallelPolicy, typename RandomAccessIterator,
      typename Complex, typename StateInteger, typename BitInteger>
    inline void conj_controlled_v_coeff(
      ParallelPolicy const parallel_policy,
      RandomAccessIterator const first, RandomAccessIterator const last,
      Complex const& phase_coefficient,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      ::ket::gate::controlled_v_coeff(
        parallel_policy, first, last,
        static_cast<Complex>(1)/phase_coefficient, target_qubit, control_qubit);
    }

    namespace ranges
    {
      template <
        typename RandomAccessRange, typename Complex,
        typename StateInteger, typename BitInteger>
      inline RandomAccessRange& conj_controlled_v_coeff(
        RandomAccessRange& state, Complex const& phase_coefficient,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        return ::ket::gate::ranges::controlled_v_coeff(
          state, static_cast<Complex>(1)/phase_coefficient, target_qubit, control_qubit);
      }

      template <
        typename ParallelPolicy, typename RandomAccessRange,
        typename Complex, typename StateInteger, typename BitInteger>
      inline RandomAccessRange& conj_controlled_v_coeff(
        ParallelPolicy const parallel_policy,
        RandomAccessRange& state, Complex const& phase_coefficient,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        return ::ket::gate::ranges::controlled_v_coeff(
          parallel_policy, state,
          static_cast<Complex>(1)/phase_coefficient, target_qubit, control_qubit);
      }
    } // namespace ranges


    template <
      typename RandomAccessIterator, typename Complex,
      typename StateInteger, typename BitInteger>
    inline void adj_controlled_v_coeff(
      RandomAccessIterator const first, RandomAccessIterator const last,
      Complex const& phase_coefficient,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      ::ket::gate::conj_controlled_v_coeff(
        first, last, phase_coefficient, target_qubit, control_qubit);
    }

    template <
      typename ParallelPolicy, typename RandomAccessIterator,
      typename Complex, typename StateInteger, typename BitInteger>
    inline void adj_controlled_v_coeff(
      ParallelPolicy const parallel_policy,
      RandomAccessIterator const first, RandomAccessIterator const last,
      Complex const& phase_coefficient,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      ::ket::gate::conj_controlled_v_coeff(
        parallel_policy, first, last,
        phase_coefficient, target_qubit, control_qubit);
    }

    namespace ranges
    {
      template <
        typename RandomAccessRange, typename Complex,
        typename StateInteger, typename BitInteger>
      inline RandomAccessRange& adj_ontrolled_v_coeff(
        RandomAccessRange& state, Complex const& phase_coefficient,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        return ::ket::gate::ranges::conj_controlled_v_coeff(
          state, phase_coefficient, target_qubit, control_qubit);
      }

      template <
        typename ParallelPolicy, typename RandomAccessRange,
        typename Complex, typename StateInteger, typename BitInteger>
      inline RandomAccessRange& adj_controlled_v_coeff(
        ParallelPolicy const parallel_policy,
        RandomAccessRange& state, Complex const& phase_coefficient,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        return ::ket::gate::ranges::conj_controlled_v_coeff(
          parallel_policy, state,
          phase_coefficient, target_qubit, control_qubit);
      }
    } // namespace ranges


    // controlled_v
    template <
      typename RandomAccessIterator, typename Real,
      typename StateInteger, typename BitInteger>
    inline void controlled_v(
      RandomAccessIterator const first, RandomAccessIterator const last,
      Real const phase,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      typedef typename std::iterator_traits<RandomAccessIterator>::value_type complex_type;
      ::ket::gate::controlled_v_coeff(
        first, last, ::ket::utility::exp_i<complex_type>(phase), target_qubit, control_qubit);
    }

    template <
      typename ParallelPolicy, typename RandomAccessIterator,
      typename Real, typename StateInteger, typename BitInteger>
    inline void controlled_v(
      ParallelPolicy const parallel_policy,
      RandomAccessIterator const first, RandomAccessIterator const last,
      Real const phase,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      typedef typename std::iterator_traits<RandomAccessIterator>::value_type complex_type;
      ::ket::gate::controlled_v_coeff(
        parallel_policy, first, last,
        ::ket::utility::exp_i<complex_type>(phase), target_qubit, control_qubit);
    }

    namespace ranges
    {
      template <
        typename RandomAccessRange, typename Real,
        typename StateInteger, typename BitInteger>
      inline RandomAccessRange& controlled_v(
        RandomAccessRange& state, Real const phase,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        typedef typename boost::range_value<RandomAccessRange>::type complex_type;
        return ::ket::gate::ranges::controlled_v_coeff(
          state, ::ket::utility::exp_i<complex_type>(phase), target_qubit, control_qubit);
      }

      template <
        typename ParallelPolicy, typename RandomAccessRange,
        typename Real, typename StateInteger, typename BitInteger>
      inline RandomAccessRange& controlled_v(
        ParallelPolicy const parallel_policy,
        RandomAccessRange& state, Real const& phase,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        typedef typename boost::range_value<RandomAccessRange>::type complex_type;
        return ::ket::gate::ranges::controlled_v_coeff(
          parallel_policy, state,
          ::ket::utility::exp_i<complex_type>(phase), target_qubit, control_qubit);
      }
    } // namespace ranges


    template <
      typename RandomAccessIterator, typename Real,
      typename StateInteger, typename BitInteger>
    inline void conj_controlled_v(
      RandomAccessIterator const first, RandomAccessIterator const last,
      Real const phase,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      ::ket::gate::controlled_v(
        first, last, -phase, target_qubit, control_qubit);
    }

    template <
      typename ParallelPolicy, typename RandomAccessIterator,
      typename Real, typename StateInteger, typename BitInteger>
    inline void conj_controlled_v(
      ParallelPolicy const parallel_policy,
      RandomAccessIterator const first, RandomAccessIterator const last,
      Real const phase,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      ::ket::gate::controlled_v(
        parallel_policy, first, last, -phase, target_qubit, control_qubit);
    }

    namespace ranges
    {
      template <
        typename RandomAccessRange, typename Real,
        typename StateInteger, typename BitInteger>
      inline RandomAccessRange& conj_controlled_v(
        RandomAccessRange& state, Real const phase,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        return ::ket::gate::ranges::controlled_v(
          state, -phase, target_qubit, control_qubit);
      }

      template <
        typename ParallelPolicy, typename RandomAccessRange,
        typename Real, typename StateInteger, typename BitInteger>
      inline RandomAccessRange& conj_controlled_v(
        ParallelPolicy const parallel_policy,
        RandomAccessRange& state, Real const& phase,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        return ::ket::gate::ranges::controlled_v(
          parallel_policy, state, -phase, target_qubit, control_qubit);
      }
    } // namespace ranges


    template <
      typename RandomAccessIterator, typename Real,
      typename StateInteger, typename BitInteger>
    inline void adj_controlled_v(
      RandomAccessIterator const first, RandomAccessIterator const last,
      Real const phase,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      ::ket::gate::conj_controlled_v(
        first, last, phase, target_qubit, control_qubit);
    }

    template <
      typename ParallelPolicy, typename RandomAccessIterator,
      typename Real, typename StateInteger, typename BitInteger>
    inline void adj_controlled_v(
      ParallelPolicy const parallel_policy,
      RandomAccessIterator const first, RandomAccessIterator const last,
      Real const phase,
      ::ket::qubit<StateInteger, BitInteger> const target_qubit,
      ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
        control_qubit)
    {
      ::ket::gate::conj_controlled_v(
        parallel_policy, first, last, phase, target_qubit, control_qubit);
    }

    namespace ranges
    {
      template <
        typename RandomAccessRange, typename Real,
        typename StateInteger, typename BitInteger>
      inline RandomAccessRange& adj_controlled_v(
        RandomAccessRange& state, Real const phase,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        return ::ket::gate::ranges::conj_controlled_v(
          state, phase, target_qubit, control_qubit);
      }

      template <
        typename ParallelPolicy, typename RandomAccessRange,
        typename Real, typename StateInteger, typename BitInteger>
      inline RandomAccessRange& adj_controlled_v(
        ParallelPolicy const parallel_policy,
        RandomAccessRange& state, Real const& phase,
        ::ket::qubit<StateInteger, BitInteger> const target_qubit,
        ::ket::control< ::ket::qubit<StateInteger, BitInteger> > const
          control_qubit)
      {
        return ::ket::gate::ranges::conj_controlled_v(
          parallel_policy, state, phase, target_qubit, control_qubit);
      }
    } // namespace ranges
  } // namespace gate
} // namespace ket


# ifdef KET_PREFER_POINTER_TO_VECTOR_ITERATOR
#   undef KET_addressof
# endif
# undef KET_is_unsigned
# undef KET_is_same
# ifdef BOOST_NO_CXX11_STATIC_ASSERT
#   undef static_assert
# endif

#endif
