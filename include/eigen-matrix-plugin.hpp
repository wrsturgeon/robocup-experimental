// No #include guards: we assert that it's never manually included in check.sh

// Included at Eigen/src/Core/MatrixBase.h:132 (WITHIN Eigen namespace)



template <typename Packet> EIGEN_DEVICE_FUNC INLINE Packet
static prshift(Packet const& a, Packet const& b) { return a >> b; }

template <typename lhs_t>
struct scalar_rshift_op : internal::binary_op_base<lhs_t, uint8_t> {
  typedef typename ScalarBinaryOpTraits<lhs_t, uint8_t, scalar_rshift_op>::ReturnType result_t;
#ifdef EIGEN_SCALAR_BINARY_OP_PLUGIN
  scalar_rshift_op() { EIGEN_SCALAR_BINARY_OP_PLUGIN }
#endif
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE result_t operator() (lhs_t const& a, uint8_t const& b) const { return a >> b; }
  template <typename Packet> EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE Packet packetOp(Packet const& a, Packet const& b) const { return prshift(a, b); }
  template <typename Packet> EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE result_t predux(Packet const& a) const { return Eigen::internal::predux(a); }
};



template <typename Packet> EIGEN_DEVICE_FUNC INLINE Packet
static pbitinv(Packet const& a) { return ~a; }

template <typename rhs_t>
struct scalar_bitinv_op {
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE const rhs_t operator() (const rhs_t& a) const { return ~a; }
  template<typename Packet>
  EIGEN_DEVICE_FUNC EIGEN_STRONG_INLINE const Packet packetOp(const Packet& a) const { return pbitinv(a); }
};



using RShiftReturnType = CwiseBinaryOp<scalar_rshift_op<Scalar>, Derived const, ConstantReturnType const>;
EIGEN_DEVICE_FUNC MEMBER_INLINE RShiftReturnType const operator>>(Scalar const& scalar) const {
  return RShiftReturnType(derived(), Derived::Constant(rows(), cols(), scalar));
}



using BitInvReturnType = CwiseUnaryOp<scalar_bitinv_op<Scalar>, Derived const>;
EIGEN_DEVICE_FUNC MEMBER_INLINE BitInvReturnType const operator~() const {
  return BitInvReturnType(derived());
}
