// BEGIN_BasicSlot 10
    /**
        BasicSlot is a base type for various "slot" types.
    */
template < typename R, typename A1 = Void,  typename A2 = Void,  typename A3 = Void,  typename A4 = Void,  typename A5 = Void,  typename A6 = Void,  typename A7 = Void,  typename A8 = Void,  typename A9 = Void,  typename A10 = Void>
    class BasicSlot : public Slot {
        public:
            /**
            Creates a copy of this object and returns it. The caller
            owns the returned object.
            */
            virtual Slot* clone() const = 0;
    };
// END_BasicSlot 10
