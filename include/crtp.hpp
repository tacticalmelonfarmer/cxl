template<typename Base>
struct crtp_facet
{
  Base& base() { return static_cast<Base&>(*this); }
  const Base& base() const { return static_cast<const Base&>(*this); }
};
