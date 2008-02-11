// BEGIN_Method 10
/**
The Method class wraps member functions for use with the signals/slots framework.
*/
template < typename R,typename ClassT,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class Method : public Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
{
    public:
        /** Convenience typedef of the wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10);

        /** Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        /** Deeply copies rhs. */
        Method(const Method& rhs) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>()
        { this->operator=(rhs); }


        /** Returns a reference to this object's wrapped ClassT object. */
        ClassT& object()
        { return *_object;}

        /** Returns a const reference to this object's wrapped ClassT object. */
        const ClassT& object() const
        { return *_object;}

        /** Returns a reference to this object's wrapped MemFuncT. */
        const MemFuncT& method() const
        { return _memFunc;}

        /**
          Passes on all arguments to the wrapped object/member pair and returns
          the result of that member.
        */
        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); }

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Method<R, ClassT, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>* clone() const
        { return new Method(*this); }

        /** Deeply copies rhs. */
        Method& operator=(const Method& rhs)
        {
            if( this != &rhs ) {
                _object = rhs._object;
                _memFunc = rhs._memFunc;
            }
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

/**
Creates and returns a Method object for the given object/method pair.
*/
template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>(obj,ptr);
}
/**
  MethodSlot wraps Method objects so that they can act as Slots.
*/
template < typename R, typename ClassT,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class MethodSlot : public BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
{
    public:
        /** Wraps the given Method object. */
        MethodSlot(const Method<R, ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& method)
        : _method( method )
        {}

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Slot* clone() const
        { return new MethodSlot(*this); }

        /** Returns a pointer to this object's internal Callable. */
        virtual const void* callable() const
        { return &_method; }

        virtual bool opened(const Connection& c)
        {
            Connectable& connectable = _method.object();
            return connectable.opened(c);
        }

        virtual void closed(const Connection& c) 
        {
            Connectable& connectable = _method.object();
            connectable.closed(c);
        }

    private:
        Method<R, ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> _method;
};
/** Creates and returns a MethodSlot object for the given object/member pair. */
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>( callable( obj, memFunc ) );
}

// END_Method 10
// BEGIN_Method 9
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class Method<R,ClassT, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void>
{
    public:
        /** Convenience typedef of the wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8,A9);

        /** Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        /** Deeply copies rhs. */
        Method(const Method& rhs) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void>()
        { this->operator=(rhs); }


        /** Returns a reference to this object's wrapped ClassT object. */
        ClassT& object()
        { return *_object;}

        /** Returns a const reference to this object's wrapped ClassT object. */
        const ClassT& object() const
        { return *_object;}

        /** Returns a reference to this object's wrapped MemFuncT. */
        const MemFuncT& method() const
        { return _memFunc;}

        /**
          Passes on all arguments to the wrapped object/member pair and returns
          the result of that member.
        */
        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5,a6,a7,a8,a9); }

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Method<R, ClassT, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void>* clone() const
        { return new Method(*this); }

        /** Deeply copies rhs. */
        Method& operator=(const Method& rhs)
        {
            if( this != &rhs ) {
                _object = rhs._object;
                _memFunc = rhs._memFunc;
            }
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

/**
Creates and returns a Method object for the given object/method pair.
*/
template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9>(obj,ptr);
}
/** Creates and returns a MethodSlot object for the given object/member pair. */
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7,A8,A9> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8,A9>( callable( obj, memFunc ) );
}

// END_Method 9
// BEGIN_Method 8
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class Method<R,ClassT, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void>
{
    public:
        /** Convenience typedef of the wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7,A8);

        /** Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        /** Deeply copies rhs. */
        Method(const Method& rhs) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void>()
        { this->operator=(rhs); }


        /** Returns a reference to this object's wrapped ClassT object. */
        ClassT& object()
        { return *_object;}

        /** Returns a const reference to this object's wrapped ClassT object. */
        const ClassT& object() const
        { return *_object;}

        /** Returns a reference to this object's wrapped MemFuncT. */
        const MemFuncT& method() const
        { return _memFunc;}

        /**
          Passes on all arguments to the wrapped object/member pair and returns
          the result of that member.
        */
        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5,a6,a7,a8); }

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Method<R, ClassT, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void>* clone() const
        { return new Method(*this); }

        /** Deeply copies rhs. */
        Method& operator=(const Method& rhs)
        {
            if( this != &rhs ) {
                _object = rhs._object;
                _memFunc = rhs._memFunc;
            }
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

/**
Creates and returns a Method object for the given object/method pair.
*/
template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8>(obj,ptr);
}
/** Creates and returns a MethodSlot object for the given object/member pair. */
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7,A8> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7,A8>( callable( obj, memFunc ) );
}

// END_Method 8
// BEGIN_Method 7
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class Method<R,ClassT, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void>
{
    public:
        /** Convenience typedef of the wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6,A7);

        /** Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        /** Deeply copies rhs. */
        Method(const Method& rhs) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void>()
        { this->operator=(rhs); }


        /** Returns a reference to this object's wrapped ClassT object. */
        ClassT& object()
        { return *_object;}

        /** Returns a const reference to this object's wrapped ClassT object. */
        const ClassT& object() const
        { return *_object;}

        /** Returns a reference to this object's wrapped MemFuncT. */
        const MemFuncT& method() const
        { return _memFunc;}

        /**
          Passes on all arguments to the wrapped object/member pair and returns
          the result of that member.
        */
        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5,a6,a7); }

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Method<R, ClassT, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void>* clone() const
        { return new Method(*this); }

        /** Deeply copies rhs. */
        Method& operator=(const Method& rhs)
        {
            if( this != &rhs ) {
                _object = rhs._object;
                _memFunc = rhs._memFunc;
            }
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

/**
Creates and returns a Method object for the given object/method pair.
*/
template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5,A6,A7>(obj,ptr);
}
/** Creates and returns a MethodSlot object for the given object/member pair. */
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6,A7> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6,A7) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6,A7>( callable( obj, memFunc ) );
}

// END_Method 7
// BEGIN_Method 6
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
class Method<R,ClassT, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void>
{
    public:
        /** Convenience typedef of the wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5,A6);

        /** Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        /** Deeply copies rhs. */
        Method(const Method& rhs) throw()
        : Callable<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void>()
        { this->operator=(rhs); }


        /** Returns a reference to this object's wrapped ClassT object. */
        ClassT& object()
        { return *_object;}

        /** Returns a const reference to this object's wrapped ClassT object. */
        const ClassT& object() const
        { return *_object;}

        /** Returns a reference to this object's wrapped MemFuncT. */
        const MemFuncT& method() const
        { return _memFunc;}

        /**
          Passes on all arguments to the wrapped object/member pair and returns
          the result of that member.
        */
        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5,a6); }

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Method<R, ClassT, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        /** Deeply copies rhs. */
        Method& operator=(const Method& rhs)
        {
            if( this != &rhs ) {
                _object = rhs._object;
                _memFunc = rhs._memFunc;
            }
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

/**
Creates and returns a Method object for the given object/method pair.
*/
template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
Method<R,ClassT,A1,A2,A3,A4,A5,A6> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5,A6>(obj,ptr);
}
/** Creates and returns a MethodSlot object for the given object/member pair. */
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5,A6> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5,A6) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5,A6>( callable( obj, memFunc ) );
}

// END_Method 6
// BEGIN_Method 5
template < typename R, typename ClassT,class A1, class A2, class A3, class A4, class A5>
class Method<R,ClassT, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void>
{
    public:
        /** Convenience typedef of the wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4,A5);

        /** Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        /** Deeply copies rhs. */
        Method(const Method& rhs) throw()
        : Callable<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void>()
        { this->operator=(rhs); }


        /** Returns a reference to this object's wrapped ClassT object. */
        ClassT& object()
        { return *_object;}

        /** Returns a const reference to this object's wrapped ClassT object. */
        const ClassT& object() const
        { return *_object;}

        /** Returns a reference to this object's wrapped MemFuncT. */
        const MemFuncT& method() const
        { return _memFunc;}

        /**
          Passes on all arguments to the wrapped object/member pair and returns
          the result of that member.
        */
        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
        { return (_object->*_memFunc)(a1,a2,a3,a4,a5); }

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Method<R, ClassT, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        /** Deeply copies rhs. */
        Method& operator=(const Method& rhs)
        {
            if( this != &rhs ) {
                _object = rhs._object;
                _memFunc = rhs._memFunc;
            }
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

/**
Creates and returns a Method object for the given object/method pair.
*/
template <class R, class ClassT,class A1, class A2, class A3, class A4, class A5>
Method<R,ClassT,A1,A2,A3,A4,A5> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4,A5>(obj,ptr);
}
/** Creates and returns a MethodSlot object for the given object/member pair. */
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5>
MethodSlot<R,BaseT,A1,A2,A3,A4,A5> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4,A5) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4,A5>( callable( obj, memFunc ) );
}

// END_Method 5
// BEGIN_Method 4
template < typename R, typename ClassT,class A1, class A2, class A3, class A4>
class Method<R,ClassT, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void>
{
    public:
        /** Convenience typedef of the wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)(A1,A2,A3,A4);

        /** Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        /** Deeply copies rhs. */
        Method(const Method& rhs) throw()
        : Callable<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void>()
        { this->operator=(rhs); }


        /** Returns a reference to this object's wrapped ClassT object. */
        ClassT& object()
        { return *_object;}

        /** Returns a const reference to this object's wrapped ClassT object. */
        const ClassT& object() const
        { return *_object;}

        /** Returns a reference to this object's wrapped MemFuncT. */
        const MemFuncT& method() const
        { return _memFunc;}

        /**
          Passes on all arguments to the wrapped object/member pair and returns
          the result of that member.
        */
        inline R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
        { return (_object->*_memFunc)(a1,a2,a3,a4); }

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Method<R, ClassT, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        /** Deeply copies rhs. */
        Method& operator=(const Method& rhs)
        {
            if( this != &rhs ) {
                _object = rhs._object;
                _memFunc = rhs._memFunc;
            }
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

/**
Creates and returns a Method object for the given object/method pair.
*/
template <class R, class ClassT,class A1, class A2, class A3, class A4>
Method<R,ClassT,A1,A2,A3,A4> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3, A4 a4)) throw()
{
    return Method<R,ClassT,A1,A2,A3,A4>(obj,ptr);
}
/** Creates and returns a MethodSlot object for the given object/member pair. */
template <class R, class BaseT, class ClassT,class A1, class A2, class A3, class A4>
MethodSlot<R,BaseT,A1,A2,A3,A4> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3,A4) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3,A4>( callable( obj, memFunc ) );
}

// END_Method 4
// BEGIN_Method 3
template < typename R, typename ClassT,class A1, class A2, class A3>
class Method<R,ClassT, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        /** Convenience typedef of the wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)(A1,A2,A3);

        /** Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        /** Deeply copies rhs. */
        Method(const Method& rhs) throw()
        : Callable<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(rhs); }


        /** Returns a reference to this object's wrapped ClassT object. */
        ClassT& object()
        { return *_object;}

        /** Returns a const reference to this object's wrapped ClassT object. */
        const ClassT& object() const
        { return *_object;}

        /** Returns a reference to this object's wrapped MemFuncT. */
        const MemFuncT& method() const
        { return _memFunc;}

        /**
          Passes on all arguments to the wrapped object/member pair and returns
          the result of that member.
        */
        inline R operator()(A1 a1, A2 a2, A3 a3) const
        { return (_object->*_memFunc)(a1,a2,a3); }

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Method<R, ClassT, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        /** Deeply copies rhs. */
        Method& operator=(const Method& rhs)
        {
            if( this != &rhs ) {
                _object = rhs._object;
                _memFunc = rhs._memFunc;
            }
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

/**
Creates and returns a Method object for the given object/method pair.
*/
template <class R, class ClassT,class A1, class A2, class A3>
Method<R,ClassT,A1,A2,A3> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2, A3 a3)) throw()
{
    return Method<R,ClassT,A1,A2,A3>(obj,ptr);
}
/** Creates and returns a MethodSlot object for the given object/member pair. */
template <class R, class BaseT, class ClassT,class A1, class A2, class A3>
MethodSlot<R,BaseT,A1,A2,A3> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2,A3) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2,A3>( callable( obj, memFunc ) );
}

// END_Method 3
// BEGIN_Method 2
template < typename R, typename ClassT,class A1, class A2>
class Method<R,ClassT, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        /** Convenience typedef of the wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)(A1,A2);

        /** Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        /** Deeply copies rhs. */
        Method(const Method& rhs) throw()
        : Callable<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(rhs); }


        /** Returns a reference to this object's wrapped ClassT object. */
        ClassT& object()
        { return *_object;}

        /** Returns a const reference to this object's wrapped ClassT object. */
        const ClassT& object() const
        { return *_object;}

        /** Returns a reference to this object's wrapped MemFuncT. */
        const MemFuncT& method() const
        { return _memFunc;}

        /**
          Passes on all arguments to the wrapped object/member pair and returns
          the result of that member.
        */
        inline R operator()(A1 a1, A2 a2) const
        { return (_object->*_memFunc)(a1,a2); }

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Method<R, ClassT, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        /** Deeply copies rhs. */
        Method& operator=(const Method& rhs)
        {
            if( this != &rhs ) {
                _object = rhs._object;
                _memFunc = rhs._memFunc;
            }
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

/**
Creates and returns a Method object for the given object/method pair.
*/
template <class R, class ClassT,class A1, class A2>
Method<R,ClassT,A1,A2> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1, A2 a2)) throw()
{
    return Method<R,ClassT,A1,A2>(obj,ptr);
}
/** Creates and returns a MethodSlot object for the given object/member pair. */
template <class R, class BaseT, class ClassT,class A1, class A2>
MethodSlot<R,BaseT,A1,A2> slot( ClassT & obj, R (BaseT::*memFunc)(A1,A2) ) throw()
{
    return MethodSlot<R,ClassT,A1,A2>( callable( obj, memFunc ) );
}

// END_Method 2
// BEGIN_Method 1
template < typename R, typename ClassT,class A1>
class Method<R,ClassT, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        /** Convenience typedef of the wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)(A1);

        /** Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        /** Deeply copies rhs. */
        Method(const Method& rhs) throw()
        : Callable<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(rhs); }


        /** Returns a reference to this object's wrapped ClassT object. */
        ClassT& object()
        { return *_object;}

        /** Returns a const reference to this object's wrapped ClassT object. */
        const ClassT& object() const
        { return *_object;}

        /** Returns a reference to this object's wrapped MemFuncT. */
        const MemFuncT& method() const
        { return _memFunc;}

        /**
          Passes on all arguments to the wrapped object/member pair and returns
          the result of that member.
        */
        inline R operator()(A1 a1) const
        { return (_object->*_memFunc)(a1); }

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Method<R, ClassT, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        /** Deeply copies rhs. */
        Method& operator=(const Method& rhs)
        {
            if( this != &rhs ) {
                _object = rhs._object;
                _memFunc = rhs._memFunc;
            }
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

/**
Creates and returns a Method object for the given object/method pair.
*/
template <class R, class ClassT,class A1>
Method<R,ClassT,A1> callable( ClassT & obj, R (ClassT::*ptr)(A1 a1)) throw()
{
    return Method<R,ClassT,A1>(obj,ptr);
}
/** Creates and returns a MethodSlot object for the given object/member pair. */
template <class R, class BaseT, class ClassT,class A1>
MethodSlot<R,BaseT,A1> slot( ClassT & obj, R (BaseT::*memFunc)(A1) ) throw()
{
    return MethodSlot<R,ClassT,A1>( callable( obj, memFunc ) );
}

// END_Method 1
// BEGIN_Method 0
template < typename R, typename ClassT>
class Method<R,ClassT, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public Callable<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void>
{
    public:
        /** Convenience typedef of the wrapped member function signature. */
        typedef R (ClassT::*MemFuncT)();

        /** Wraps the given object/member pair. */
        explicit Method(ClassT& object, MemFuncT ptr) throw()
        : _object(&object), _memFunc(ptr)
        { }

        /** Deeply copies rhs. */
        Method(const Method& rhs) throw()
        : Callable<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void>()
        { this->operator=(rhs); }


        /** Returns a reference to this object's wrapped ClassT object. */
        ClassT& object()
        { return *_object;}

        /** Returns a const reference to this object's wrapped ClassT object. */
        const ClassT& object() const
        { return *_object;}

        /** Returns a reference to this object's wrapped MemFuncT. */
        const MemFuncT& method() const
        { return _memFunc;}

        /**
          Passes on all arguments to the wrapped object/member pair and returns
          the result of that member.
        */
        inline R operator()() const
        { return (_object->*_memFunc)(); }

        /**
        Creates a copy of this object and returns it. The caller
        owns the returned object.
        */
        Method<R, ClassT, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void>* clone() const
        { return new Method(*this); }

        /** Deeply copies rhs. */
        Method& operator=(const Method& rhs)
        {
            if( this != &rhs ) {
                _object = rhs._object;
                _memFunc = rhs._memFunc;
            }
            return (*this);
        }

    private:
        ClassT* _object;
        MemFuncT _memFunc;
};

/**
Creates and returns a Method object for the given object/method pair.
*/
template <class R, class ClassT>
Method<R,ClassT> callable( ClassT & obj, R (ClassT::*ptr)()) throw()
{
    return Method<R,ClassT>(obj,ptr);
}
/** Creates and returns a MethodSlot object for the given object/member pair. */
template <class R, class BaseT, class ClassT>
MethodSlot<R,BaseT> slot( ClassT & obj, R (BaseT::*memFunc)() ) throw()
{
    return MethodSlot<R,ClassT>( callable( obj, memFunc ) );
}

// END_Method 0
