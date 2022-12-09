
#ifndef ISOLATOR_H
#define ISOLATOR_H

#if !defined IPP_LICENSE_COMPANY
	#define IPP_LICENSE_COMPANY ""
#endif

#if !defined IPP_LICENSE_KEY
	#define IPP_LICENSE_KEY ""
#endif

#include "IsolatorInternals.h"
#include <exception>

/*!
 * \brief
 * Defines fake objects default behavior FakeOptions.
 * 
 * FakeOptions supported:
 * CallOriginal - use this option to cause faked member functions call their original implementation by default.
 * otherwise, calling to faked member functions will have a default recursive fake behavior
 * 
 * \remarks
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>(FakeOptions::CallOriginal); // Creates a faked instance of ConcreteClass.
 * fakeConcrete->GetString(); // This will call the original implementation by default
 * \endcode 
 *
 * \see
 * FAKE.
 */
namespace FakeOptions
{
	/*!
	 * \brief
	 * Faked methods will return fake objects, this is the default
	 * 
	 * \remarks
	 * Example:
	 * \code
	 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>(FakeOptions::Recursive); // Creates a faked instance of ConcreteClass.
	 * fakeConcrete->GetString(); // This will return a fake string
	 * \endcode 
	 *
	 * \see
	 * FAKE.
	 */
	static const int Recursive = 0x0;

	/*!
	 * \brief
	 * Faked methods will call their original implementation by default.
	 * 
	 * \remarks
	 * Example:
	 * \code
	 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>(FakeOptions::CallOriginal); // Creates a faked instance of ConcreteClass.
	 * fakeConcrete->GetString(); // This will call the original implementation by default
	 * \endcode 
	 *
	 * \see
	 * FAKE.
	 */	
	static const int CallOriginal = 0x01;

#ifdef __Windows__
        /*!
         * \brief
         * Initially create a fake object only. Object's methods will call their original implementation by defaults
         * until WHEN_CALLED is called on one or more of the object methods. Note that Abstract classes will ignore the lean flag
         * NOTE: This option is Windows specific
         *
         * \remarks
         * Example:
         * \code
         * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>(FakeOptions::Lean); // Creates a faked instance of ConcreteClass.
         * fakeConcrete->GetString(); // This will call the original implementation by default
         * \endcode
         *
         * \see
         * FAKE.
         */	
	static const int Lean = 0x02;
#endif
};

// Note: to use deprecated API from version 1 define ISOLATOR_USE_DEPRECATED_V1_API
namespace Typemock
{

#pragma push_macro("DoStaticOrGlobalInstead")
#undef DoStaticOrGlobalInstead

#pragma push_macro("DoMemberFunctionInstead")
#undef DoMemberFunctionInstead


struct ISOLATOR_API IsolatorConfiguration
{
    DEPRECATED static bool DontFakeCtorOnInit;

	 /*!
	 * \brief
	 * Fake methods only on the same thread that the fake was created. Defaults to all threads
	 * 
	 * \param Value
	 * Boolean.
	 * 
	 * \remarks
	 * The default behavior is to fake all methods on all threads. 
	 * To override this behavior and to fake methods on the test thread set FakeOnlyOnTestThread to true.
	 * \code
	 * Typemock::IsolatorConfiguration::FakeOnlyOnTestThread = true;
	 * \endcode
	 * \see
	 * FAKE | WHEN_CALLED
	 */
	static bool FakeOnlyOnTestThread;
};

#ifdef __Windows__
struct
{		
    /*!
	 * \brief
	 * Set the return value from a faked method.
	 * 
	 * \param Value
	 * The faked value to return.
	 * 
	 * \throws IsolatorException
	 * 
	 * \remarks
	 * Use Return to return a faked value from your faked method.
	 * Use it as a completing statement to WHEN_CALLED macro.
	 * Example:
	 * \code
	 * //return faked int value:
	 * 
	 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
	 * WHEN_CALLED(fakeConcrete->GetInt()).Return(100);	 
	 * ASSERT_EQ(100, fakeConcrete->GetInt());
	 *
	 * \endcode
	 * \see
	 * FAKE | WHEN_CALLED
	 */
        void	Return(short Value) {}
        void	Return(unsigned short Value) {}
        void	Return(int Value) {}	    
        void	Return(unsigned int Value) {}
	    void	Return(long Value) {}
	    void	Return(unsigned long Value) {}
	    void	Return(long long Value) {}
	    void	Return(unsigned long long Value) {}
	    void	Return(float Value) {}
	    void	Return(double Value) {}        
        void	Return(void* Value) {}
        void	Return(const char* Value) {}
        void	Return(const wchar_t* Value) {}

	/*!
	 * \brief
	 * Specify that an intercepted call will throw the given exception.
	 * 
	 * \param
	 * Support general, message and exception objects
	 * 
	 * \remarks
	 * Use Throw method as completing statement to WHEN_CALLED macro.
	 * Example:
	 * \code
	 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
	 * WHEN_CALLED(fakeConcrete->GetString()).Throw(new ::std::exception("Exception occured"));
	 * \endcode
	 * \see
	 * FAKE | WHEN_CALLED
	 */
		void	Throw() {}
		void	Throw(PCSTR Message) {}
#if (_MSC_VER > 1400)
		void	Throw(PVOID Excpt) {}
#endif
	/*!
	 * \brief
	 * Specify that the faked method should return a faked instance.
	 * 
	 * \throws IsolatorException
	 * 
	 * Use it when you don't care about the return value behavior.
	 * The returned faked instance will have a default recursive behavior.
	 * 
	 * \remarks
	 * When using ReturnFake on a void method will cause the intercepted call to be ignored without executing any logic.
	 * You can verify that the method has been called by using ASSERT_WAS_CALLED macro.
	 * Use ReturnFake method as completing statement to WHEN_CALLED macro.
	 * Example:
	 * \code
	 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
	 * WHEN_CALLED(fakeConcrete->GetInt()).ReturnFake();
	 * ASSERT_EQ(0, fakeConcrete->GetInt());
	 * \endcode
	 *
	 * \see
	 * FAKE | WHEN_CALLED | ASSERT_WAS_CALLED
	 */
		void	ReturnFake(int FakeOptions = FakeOptions::Recursive) {}
		
	/*!
	 * \brief
	 * Specify that an intercepted call will try to execute the real logic of the intercepted instance. 
     * You can also set the return value from the original function by using CallOriginal().Return(value)
	 * 
	 * \throws IsolatorException 
	 * 
	 * When using CallOriginal on a faked instance the original implementation of the method will be called.
	 * 
	 * \remarks
	 * Use CallOriginal method as completing statement to WHEN_CALLED macro.
	 * Example:
	 * \code
	 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
	 * WHEN_CALLED(fakeConcrete->GetInt()).CallOriginal();
	 * \endcode
	 * 
	 * \see
	 * FAKE | WHEN_CALLED
	 */
		IReturn&	CallOriginal() {}

	/*!
	 * \brief
	 * Run a custom function instead of the faked method of a faked class.
	 * 
	 * \param Instance
	 * The context (this) of your callback method
	 *
	 * \param Function
	 * The function that will run instead of the faked method. 
	 * 
	 * DoStaticOrGlobalInstead will run your custom logic when a faked method has been called.
	 * 
	 * \remarks
	 * Use DoMemberFunctionInstead method as completing statement to WHEN_CALLED macro.
	 * Example:
	 * \code
	 * AlternativeImplementation* instance = new AlternativeImplementation();
	 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
	 * WHEN_CALLED(fakeConcrete->GetInt()).DoMemberFunctionInstead(instance, AlternativeImplementation::DoThis);
	 * \endcode 
	 * \see
	 * FAKE | WHEN_CALLED
	 */
		void	DoMemberFunctionInstead(PVOID Instance, PVOID Function) {}

	/*!
	 * \brief
	 * Run a custom function instead of the faked global or static method.
	 *
	 * \param Function
	 * The function that will run instead of the faked method. 
	 * 
	 * \param Instance
	 * Can be any data that you want to send to your callback method from the test.
	 * You can get a hold of this parameter inside the callback using the GET_EXTRA_DATA macro.
	 * This is an optional parameter for your callback method.	 
	 * 
	 * DoStaticOrGlobalInstead will run your custom logic when a faked method has been called.
	 * 
	 * \remarks
	 * Use DoMemberFunctionInstead method as completing statement to WHEN_CALLED macro.
	 * Example:
	 * \code
	 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
	 * WHEN_CALLED(ConcreteClass::staticGetInt()).DoStaticOrGlobalInstead(AlternativeImplementation::StaticDoThis);
	 * \endcode 
	 * \see
	 * FAKE | WHEN_CALLED | GET_EXTRA_DATA
	 */
		void	DoStaticOrGlobalInstead(PVOID Function, PVOID ExtraData) {}
		
	/*!
	 * \brief
	 * Will cause the method to be ignored.
	 * 
	 * \throws IsolatorException 
	 * 
	 * This is the default behavior of a method on a faked object.
	 * Usually you'll use it when you create fake with a default behavior of FakeOptions::CallOriginal
	 * and than you specify explicitly the methods you want to ignore.
	 * \remarks
	 * Use Ignore method as completing statement to WHEN_CALLED macro.
	 * Example:
	 * \code
	 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>(FakeOptions::CallOriginal);
	 * WHEN_CALLED(fakeConcrete->VoidCall()).Ignore();
	 * \endcode
	 * 
	 * \see
	 * FAKE | WHEN_CALLED
	 */		
        void	Ignore() {}
};      
#endif

#pragma pop_macro("DoMemberFunctionInstead")
#pragma pop_macro("DoStaticOrGlobalInstead")
}

/*!	
 * \brief
 * Exception thrown when misusing the API or when one of the Isolator++ asserts fails.
 * \see
 * ASSERT_WAS_CALLED | ASSERT_NOT_CALLED
 */
class ISOLATOR_API IsolatorException : public std::exception
{
public:
    IsolatorException(PCSTR	what);

    virtual ~IsolatorException() throw();

	PCSTR ErrorMessage() const;

    const char* what() const NOEXCEPT;

private:
	PCSTR m_szWhat;
};

/*!
 * \brief
 * When you are calling a method inside WHEN_CALLED macro Isolator++ ignores the method arguments.
 * Since there is no need to send real arguments you can replace any arguments with _ (underscore)
 * or ANY macro
 * 
 * \remarks
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>(); // Creates a faked instance of ConcreteClass.
 * WHEN_CALLED(fakeConcrete->Method(_,_)).Return(5);
 * \endcode 
 *
 * \see
 * WHEN_CALLED.
 */
#define _ \
	__INTERNAL_ANY

#define ANY \
	__INTERNAL_ANY

#define ANY_VAL(...) \
	__INTERNAL_ANY_VAL(__VA_ARGS__)

#define ANY_REF(...) \
	__INTERNAL_ANY_REF(__VA_ARGS__)

#define ANY_METHOD(...) \
	__INTERNAL_ANY_METHOD(__VA_ARGS__)


/*!
 * \brief
 * Used to fake overload private method inside PRIVATE_WHEN_CALLED macro.
 * Pass the overloaded method argumement types 
 * 
 * \remarks
 * Example:
 * \code
 * MyClass myClass;
 * PRIVATE_WHEN_CALLED(&myClass, PrivateMethod, TYPEOF(char*)).Return(3);
 * \endcode 
 *
 * \see
 * PRIVATE_WHEN_CALLED.
 */
#define TYPEOF(...) \
	__INTERNAL_TYPEOF(__VA_ARGS__)

/*!
 * \brief
 * The following are a set of predicates that are used in conditional matching
 * 
 * \remarks
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>(); // Creates a faked instance of ConcreteClass.
 * WHEN_CALLED(fakeConcrete->Method(EQ(2))).Return(5); // wil lreturn 5 if the passed argument is 2
 * \endcode 
 *
 * \see
 * WHEN_CALLED.
 */
#define EQ(param) \
    __INTERNAL_PREDICATE(param)

#define NE(param) \
    __INTERNAL_PREDICATE(param)

#define LT(param) \
    __INTERNAL_PREDICATE(param)

#define LE(param) \
    __INTERNAL_PREDICATE(param)

#define GT(param) \
    __INTERNAL_PREDICATE(param)

#define GE(param) \
    __INTERNAL_PREDICATE(param)


/*!
 * \brief
 * Fakes a new instance of the specified type.
 * 
 * \param type
 * The type of the faked object. See 'FakeOptions'
 * 
 * Use the class name as an argument. The FAKE macro will return a new instance of the specified type.
 * You can set the behavior of the new faked instance using WHEN_CALLED macro and its completing statements. See WHEN_CALLED for details.
 * 
 * \remarks
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>(); // Creates a faked instance of ConcreteClass.
 * WHEN_CALLED(fakeConcrete->GetString()).ReturnPtr("hello");
 * \endcode 
 *
 * \see
 * WHEN_CALLED.
 */
template <typename T>
T* FAKE(int FakeOptions = FakeOptions::Recursive)
{
    typedef T TemplateObject;
	return __INTERNAL_FAKE(TemplateObject, FakeOptions);
}

#define ISOLATOR_FORCE_SYMBOL   0x10

/*!
 * \brief
 * Fakes a new instance of the specified type and forces creation of the class symbols.
 * 
 * \param flag
 * A symbols flag. must be ISOLATOR_FORCE_SYMBOL
 * \param type
 * The type of the faked object. See 'FakeOptions'
 * 
 * Use the class name as an argument. The FAKE macro will return a new instance of the specified type.
 * You can set the behavior of the new faked instance using WHEN_CALLED macro and its completing statements. See WHEN_CALLED for details.
 * 
 * \remarks
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>(ISOLATOR_FORCE_SYMBOL, FakeOptions::Recursive); // Creates a faked instance of ConcreteClass with symbols generation.
 * WHEN_CALLED(fakeConcrete->GetString()).ReturnPtr("hello");
 * \endcode 
 *
 * \see
 * WHEN_CALLED.
 */
template <typename T, typename S>
T* FAKE(S s, int FakeOptions)
{
    if (s != ISOLATOR_FORCE_SYMBOL) return NULL;
    if (false) { T obj; } 
    return FAKE<T>(FakeOptions);
}

/*!
 * \brief
 * Fakes all current and instances of the specified type.
 * 
 * \param type
 * The type to be faked. See 'FakeOptions'
 * 
 * Use the class name as an argument. The FAKE_ALL macro will return an instance of the specified type.
 * by setting the behavior of this instance, you essentially control the behavior of all current and future instances.
 * You can set the behavior of the instance using the WHEN_CALLED macro and its completing statements. 
 * See WHEN_CALLED for details.
 * 
 * \remarks
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE_ALL<ConcreteClass>(); // Creates a faked instance of ConcreteClass.
 * WHEN_CALLED(fakeConcrete->GetString()).ReturnPtr("hello");
 * \endcode 
 *
 * \see
 * WHEN_CALLED.
 */
template <typename T>
T* FAKE_ALL(int FakeOptions = FakeOptions::Recursive)
{
	typedef T TemplateObject;    
	return __INTERNAL_FAKE_ALL(TemplateObject, FakeOptions);
}

/*!
 * \brief
 * Fakes all current and instances of the specified type and forces creation of the class symbols.
 * 
 * \param flag
 * A symbols flag. must be ISOLATOR_FORCE_SYMBOL
 * \param type
 * The type of the faked object. See 'FakeOptions'
 * 
 * Use the class name as an argument. The FAKE_ALL macro will return an instance of the specified type.
 * by setting the behavior of this instance, you essentially control the behavior of all current and future instances.
 * You can set the behavior of the instance using the WHEN_CALLED macro and its completing statements. 
 * See WHEN_CALLED for details.
 * 
 * \remarks
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE_ALL<ConcreteClass>(ISOLATOR_FORCE_SYMBOL, FakeOptions::Recursive); // Creates a faked instance of ConcreteClass with symbols generation..
 * WHEN_CALLED(fakeConcrete->GetString()).ReturnPtr("hello");
 * \endcode 
 *
 * \see
 * WHEN_CALLED.
 */
template <typename T, typename S>
T* FAKE_ALL(S s, int FakeOptions)
{
    if (s != ISOLATOR_FORCE_SYMBOL) return NULL;
    if (false) { T obj; } 
    return FAKE_ALL<T>(FakeOptions);
}

/*!
 * \brief
 * Fakes all static functions on a type, to return default values or recursive fakes.
 * 
 * \param type
 * The type on which to fake the static methods. See 'FakeOptions'
 * 
 * Use the class name as an argument. 
 * You must use this macro if you'd like to later change the behavior of any of the static methods using WHEN_CALLED.
 * You can set the behavior of each specific method later by using WHEN_CALLED macro and its completing statements. See WHEN_CALLED for details.
 * 
 * \remarks
 * Example:
 * \code
 * FAKE_STATICS<ConcreteClass>(); // Creates a faked instance of ConcreteClass.
 * WHEN_CALLED(fakeConcrete::StaticGetString()).ReturnPtr("hello");
 * \endcode 
 *
 * \see
 * WHEN_CALLED.
 */
template <typename T>
void FAKE_STATICS(int FakeOptions = FakeOptions::Recursive)
{
	typedef T TemplateObject; 
	__INTERNAL_FAKE_STATICS(TemplateObject, FakeOptions);
}

/*!
 * \brief
 * Use the FAKE_GLOBAL macro to fake global function that is not a class member
 * 
 * \param function
 * The function that you want to fake.
 * 
 * Use FAKE_GLOBAL macro to declare the global function that you want to fake.
 * This should be followed with a WHEN_CALLED macro.
 * 
 * \remarks
 * Example:
 * \code
 * FAKE_GLOBAL(GlobalMethod);
 * WHEN_CALLED(GlobalMethod(_)).ReturnVal(5);
 * \endcode 
 *
 * \see
 * WHEN_CALLED.
 */
#define FAKE_GLOBAL(...) \
	__INTERNAL_FAKE_GLOBAL(__VA_ARGS__)

/*!
 * \brief
 * Use the ISOLATOR_INVOKE_CONSTRUCTOR macro to invoke one of its constructors
 * 
 * \param instance
 * pointer to the faked object
 * \param optional arguments 
 * arguments to be passed to the constructor
 * 
 * \remarks
 * The example below creates a faked object of class ConcreteClass and then
 * invokes its constructor that accepts an integer, passing a value of 1.
 * Typically, constructors can be called only once during the fake object lifetime
 * \code
 * auto fake = FAKE<ConcreteClass>();
 * ISOLATOR_INVOKE_CONSTRUCTOR(fake, 1);
 * \endcode 
 *
 * \see
 * FAKE.
 */
#define ISOLATOR_INVOKE_CONSTRUCTOR(instance, ...) \
	__INTERNAL_ISOLATOR_INVOKE_CONSTRUCTOR(instance, ##__VA_ARGS__)
    
/*!
 * \brief
 * Set the method that you want to set the faked behavior on.
 * 
 * \param function
 * The function to set the faked behavior on.
 * 
 * Use WHEN_CALLED to set the method that you want to define the faked behavior on. 
 * 
 * \remarks
 * You can then call behavior completing statements (e.g. ReturnVal(), ReturnPtr(), Throw() etc.) to define the desired method behavior.
 * Example:
 * \code
 * fakeConcrete = FAKE<ConcreteClass>();
 * WHEN_CALLED(fakeConcrete->GetString()).ReturnPtr("hello");
 * ASSERT_EQ("hello", fakeConcrete->GetString()); 
 * \endcode
 *
 * \see
 * FAKE
 */
#define WHEN_CALLED(...) \
	__INTERNAL_WHEN_CALLED(__VA_ARGS__, #__VA_ARGS__)


/*!
 * \brief
 * Use this macro to fake private methods.
 * 
 * \param instance
 * Instance that was returned from previous call to FAKE
 * \param function 
 * Private method that you want to fake. You can also specify an overloaded method 
 * by passing the types of the arguments using TYPEOF() macro
 * 
 * \remarks 
 * Example:
 * \code
 * ConcreteClass* fake = FAKE<ConcreteClass>();
 * PRIVATE_WHEN_CALLED(fake, PrivateMethod).CallOriginal(); 
 * PRIVATE_WHEN_CALLED(fake, OverloadedPrivateMethod, TYPEOF(char*)).Ignore(); // will isolate the specific overloded function in the class
 * \endcode
 *
 * \see
 * FAKE
 */
#define PRIVATE_WHEN_CALLED(instance, function, ...) \
	__INTERNAL_WHEN_CALLED_INDIRECT(instance, function, #__VA_ARGS__, ##__VA_ARGS__)

/*!
 * \brief
 * Verifies that a method was called.
 * 
 * \param function
 * The method we want to verify if was called.
 * 
 * 
 * \remarks
 * Use it to check interaction between your tested code to the faked types.
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
 * fakeConcrete->VoidCall(); // this call is ignored
 * ASSERT_WAS_CALLED(fakeConcrete->VoidCall());
 * \endcode
 * \see
 * ASSERT_WAS_CALLED
 */
#define ASSERT_WAS_CALLED(...) \
    __INTERNAL_ASSERT_WAS_CALLED(__VA_ARGS__, #__VA_ARGS__)	

/*!
 * \brief
 * Verifies that a method was not called.
 * 
 * \param function
 * The method we want to do the verification on.
 * 
 * 
 * \remarks
 * Use ASSERT_NOT_CALLED to check interaction between your tested code to the faked types.
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
 * ASSERT_NOT_CALLED(fakeConcrete->VoidCall());
 * \endcode
 * \see
 * ASSERT_NOT_CALLED
 */
#define ASSERT_NOT_CALLED(...) \
    __INTERNAL_ASSERT_NOT_CALLED(__VA_ARGS__, #__VA_ARGS__)	


/*!
 * \brief
 * Return the amount of times a method has been called.
 * 
 * \param function
 * The function we want to do the call count on.
 * 
 * 
 * \remarks
 * Use TIMES_CALLED macro when you want to verify how many times a method has been called through the test.
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
 * fakeConcrete->VoidCall();
 * fakeConcrete->VoidCall();
 * int num = TIMES_CALLED(fakeConcrete->VoidCall());
 * ASSERT_EQ(2, num);
 * \endcode
 *
 * \see
 * FAKE | WHEN_CALLED
 */
#define TIMES_CALLED(...) \
    __INTERNAL_TIMES_CALLED(__VA_ARGS__, #__VA_ARGS__)			

/*!
 * \brief
 * Verifies that a method was called.
 * 
 * \param instance
 * Instance that was returned from previous call to FAKE
 * \param function 
 * Private method that you want to fake.
 * 
 * 
 * \remarks
 * Use it to check interaction between your tested non-public methods to the faked types.
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
 * PRIVATE_WHEN_CALLED(fake, PrivateMethod).CallOriginal(); 
 * PRIVATE_ASSERT_WAS_CALLED(fake, PrivateMethod);
 * \endcode
 * \see
 * PRIVATE_ASSERT_WAS_CALLED
 */
#define PRIVATE_ASSERT_WAS_CALLED(instance, function, ...) \
	__INTERNAL_ASSERT_WAS_CALLED_INDIRECT(instance, function, #__VA_ARGS__, ##__VA_ARGS__)

/*!
 * \brief
 * Verifies that a method was called.
 * 
 * \param instance
 * Instance that was returned from previous call to FAKE
 * \param function 
 * Private method that you want to fake.
 * 
 * 
 * \remarks
 * Use it to check interaction between your tested non-public methods to the faked types.
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
 * PRIVATE_ASSERT_NOT_CALLED(fake, PrivateMethod);
 * \endcode
 * \see
 * PRIVATE_ASSERT_NOT_CALLED
 */
#define PRIVATE_ASSERT_NOT_CALLED(instance, function, ...) \
	__INTERNAL_ASSERT_NOT_CALLED_INDIRECT(instance, function, #__VA_ARGS__, ##__VA_ARGS__)

/*!
 * \brief
 * Verifies that a method was called.
 * 
 * \param instance
 * Instance that was returned from previous call to FAKE
 * \param function 
 * Private method that you want to fake.
 * 
 * 
 * \remarks
 * Use this macro when you want to verify how many times a non-public method has been called through the test.
 * Example:
 * \code
 * ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
 * PRIVATE_WHEN_CALLED(fake, PrivateMethod).CallOriginal(); 
 * fakeConcrete->CallPrivateMethod();
 * fakeConcrete->CallPrivateMethod();
 * int num = PRIVATE_TIMES_CALLED(fakeConcrete, PrivateMethod);
 * ASSERT_EQ(2, num);
 * \endcode
 * \see
 * PRIVATE_TIMES_CALLED
 */
#define PRIVATE_TIMES_CALLED(instance, function, ...) \
	__INTERNAL_TIMES_CALLED_INDIRECT(instance, function, #__VA_ARGS__, ##__VA_ARGS__)


/*!
 * \brief
 * Cleans Isolator++ fakes at the end of the test.
 * 
 * \remarks
 * Use ISOLATOR_CLEANUP macro at the end of the test to clean up any faked behavior you set in the test.
 * Note that if you will not do that the faked behavior will carry on to the next test.
 * Example:
 * \code
 * 
 * WHEN_CALLED(fakeConcrete->GetString()).ReturnPtr("hello");
 * ASSERT_EQ("hello", fakeConcrete->GetString());
 * ISOLATOR_CLEANUP()
 * \endcode
 *
 * \see
 * FAKE | WHEN_CALLED
 */
#define ISOLATOR_CLEANUP() \
	__INTERNAL_ISOLATOR_CLEANUP()	


/*!
 * \brief
 * Real call to a static/global/local/member function regardless of its accessibility.
 * 
 * \remarks
 * Use ISOLATOR_INVOKE_FUNCTION macro anytime you want to call to a static/global/local/member function directly. 
 * The macro will call to the function even if it's private/protected in a class or locally defined as static in a module. 
 * Example:
 * \code
 * class ClassWithPrivates 
 * {
 * private:
 *   std::string PrivateCall(int param1, int param2)
 *   {
 *       return "abc";
 *   }
 *
 *   static std::string PrivateStaticCall(int param1, int param2)
 *   {
 *       return "abc";
 *   }
 *
 *   void DoPrivate()
 *   {
 *   }
 *};
 *
 * static int StaticFunctionInModule(int x) { return 10; }
 *
 * std::string ret;
 * ClassWithPrivates obj;
 * ISOLATOR_INVOKE_FUNCTION(ret, &obj, PrivateCall, 1, 2);  // return value passed by value
 * ISOLATOR_INVOKE_FUNCTION(_, &obj, DoPrivate);            // return value isn't passed: implies a void function
 * ISOLATOR_INVOKE_FUNCTION(ret, _, ClassWithPrivates::PrivateStaticCall, 1, 2);  // return value passed by value to static function
 * ISOLATOR_INVOKE_FUNCTION(ret, _, StaticFunctionInModule, 1); // call static local/global methods   
 * \endcode
 *
 */
#define ISOLATOR_INVOKE_FUNCTION(ret, instance, function, ...) \
	__INTERNAL_ISOLATOR_INVOKE_FUNCTION(ret, instance, function, ##__VA_ARGS__)

/*!
 * \brief
 * Set private/static/global/member variable directly
 * 
 * \remarks
 * Use ISOLATOR_SET_VARIABLE macro anytime you want to access a private/static/global/member variable directly. 
 * You can use the macros to set values directly even if it's private/protected in a class. 
 * Example:
 * \code
 * class ClassWithPrivates 
 * {
 * private:
 *    int _private;
 *    static int _privateStatic;
 * };
 *
 * static int staticVariable = 0;
 *
 * ClassWithPrivates obj;
 * ISOLATOR_SET_VARIABLE(&obj, _private, 1);  // Set the value of the private data member directly
 * ISOLATOR_SET_VARIABLE(_, ClassWithPrivates::_privateStatic, 1);  // Set the value of the private static data member directly
 * ISOLATOR_SET_VARIABLE(_, staticVariable, 1);  // Set the value of some global static variable directly
 * \endcode
 *
 */
#define ISOLATOR_SET_VARIABLE(obj, data, value) \
    __INTERNAL_ISOLATOR_SET_VARIABLE(obj, data, value)

/*!
 * \brief
 * Get private/static/global/member variable directly
 * 
 * \remarks
 * Use ISOLATOR_GET_VARIABLE macro anytime you want to access a private/static/global/member variable directly. 
 * You can use the macros to get values directly even if it's private/protected in a class. 
 * Example:
 * \code
 * class ClassWithPrivates 
 * {
 * private:
 *    int _private;
 *    static int _privateStatic;
 * };
 *
 * static int staticVariable = 0;
 *
 * ClassWithPrivates obj;
 * int value = 0;
 * ISOLATOR_GET_VARIABLE(&obj, _private, value);  // return the value of the private data member
 * ISOLATOR_GET_VARIABLE(_, ClassWithPrivates::_privateStatic, value);  // return the value of the private static data member
 * ISOLATOR_GET_VARIABLE(_, staticVariable, value);  // Get the value of some global static variable directly
 * \endcode
 *
 */
#define ISOLATOR_GET_VARIABLE(obj, data, value) \
    __INTERNAL_ISOLATOR_GET_VARIABLE(obj, data, value)
    
/*!
 * \brief
 * Gets the first parameter to DoMemberFunctionInstead inside the callback method.
 * 
 * \remarks
 * Use it when you want to check the state of the class of your callback method.
 * Example:
 * \code
 *class AlternativeImplementation
 *{
 *	static int DoThis()
 *  {
 *		AlternativeImplementation* instance = (AlternativeImplementation*)GET_EXTRA_DATA();
 *  }
 *};
 * TEST_F(DoInstead, GetContextExample)
 *{
 *   AlternativeImplementation* instance = new AlternativeImplementation();
 *   ConcreteClass* fakeConcrete = FAKE<ConcreteClass>();
 *	 WHEN_CALLED(fakeConcrete->GetInt()).DoMemberFunctionInstead(instance, AlternativeImplementation::DoThis);
 *	 fakeConcrete->GetInt();
 *} 
 * \endcode
 *
 * \see
 * FAKE | WHEN_CALLED
 */
#define GET_EXTRA_DATA() \
	__INTERNAL_GET_EXTRA_DATA();

#ifdef BITS_32
/*!
 * \brief
 * Use this macro to fake an out parameter.
 * 
 * \remarks
 * Use it when you want to fake a method with an out parameter.
 * Example:
 * \code
 *bool Product::IsExpired()
 *{
 *	SYSTEMTIME now;
 *  ::GetSystemTime(&now);
 *	if (now.wYear > 2012)
 *		return true;
 *  return false;
 *  }
 *}
 *
 * TEST_F(IsolatorPPTests, IsExpired_YearIs2013_ReturnTrue)
 *{
 *	 Product product;
 *   SYSTEMTIME fakeTime;
 *   fakeTime.wYear = 2013;
 *	 FAKE_GLOBAL(GetSystemTime);
 *	 WHEN_CALLED(GetSystemTime(RET(&fakeTime))).Ignore();
 *	 ASSERT_TRUE(product.IsExpired());
 *} 
 * \endcode
 *
 * \see
 * FAKE_GLOBAL | WHEN_CALLED
 */
template <typename T> __declspec(naked) 
T* RET(T* OutArg, size_t RetSize = 0) __INTERNAL_RET(T)

template <typename T> __declspec(naked) 
T& RET(T& OutArg, size_t RetSize = 0) __INTERNAL_RET(T)

template <typename T> __declspec(naked)
T& RET_REF(T& OutArg, size_t RetSize = 0) __INTERNAL_RET(T)

/*!
 * \brief
 * Use this macro to fake an out parameter with condition.
 * 
 * \remarks
 * Use it when you want to fake a method with an out parameter with parameter only if parameter satisfies the condition.
 * Example:
 * \code
 * TEST_F(FakingAnOutParams, InPrivateMethodCallWithCondition)
 *{
 *	 Person* person = FAKE<Person>(FakeOptions::CallOriginal);
 *   char* out = "Smith";
 *   PRIVATE_WHEN_CALLED(person, ChangeLastName, RET_IF(EQ(BY_REF("Johns")), &out)).Return(false);
 *	 Person* otherPerson = new Person();
 *	 otherPerson->lastName = "Johns";
 *	 ASSERT_FALSE(person->GetMarried(otherPerson));
 *	 ASSERT_EQ(otherPerson->lastName, out);
 *} 
 * \endcode
 *
 * \see
 * FAKE | PRIVATE_WHEN_CALLED
 */
template <typename T, typename TCondition>
T* RET_IF(TCondition condition, T* OutArg, size_t RetSize = 0) __INTERNAL_RET_IF(T*)

template <typename T, typename TCondition>
T& RET_IF(TCondition condition, T& OutArg, size_t RetSize = 0) __INTERNAL_RET_IF(T&)

/*!
 * \brief
 * Use this macro for custom argument assertion.
 * 
 * \remarks
 * Use it when you want change beahavior of the method with arguments pass the lambda function, or assert the method was called with specific arguments.
 * Example:
 * \code
 * TEST_F(AssertingNonPublicInteraction, AssertPrivateWithCondition)
 *{
 *	 Person* person = new Person();
 *   PRIVATE_WHEN_CALLED(person, IsValidName).Return(true);
 *   PRIVATE_WHEN_CALLED(person, SaveAll).Ignore();
 *	 person->Save("foo");
 *	 PRIVATE_ASSERT_WAS_CALLED(person, SaveAll,  IS<std::string>([](std::string& val) {return val == "foo";}));
 *} 
 * \endcode
 *
 * \see
 * PRIVATE_ASSERT_WAS_CALLED | PRIVATE_WHEN_CALLED
 */
template<typename T, typename TLambda>
T IS(TLambda predicate) __INTERNAL_IS(ANY_VAL(T))

/*!
 * \brief
 * Use this macro for conditional behavior faking of methods with out parameters.
 * 
 * \remarks
 * Use it when you want change beahavior of the method with an out arguments pass the lambda function, without changing them.
 * Example:
 * \code
 * TEST_F(FakingAnOutParams, InPrivateMethodCallWithConditionWithoutAssigment)
 *{
 *	 Person* person = FAKE<Person>(FakeOptions::CallOriginal);
 *   PRIVATE_WHEN_CALLED(person, ChangeLastName, IS_REF<char**>([](char** s) {return !strcmp(*s, "Smith");})).Return(false);
 *	 Person* otherPerson = new Person();
 *	 otherPerson->lastName = "Smith";
 *	 char* neededLastName = "Smith";
 *	 ASSERT_FALSE(person->GetMarried(otherPerson));
 *	 ASSERT_EQ(otherPerson->lastName, neededLastName);
 *} 
 * \endcode
 *
 * \see
 * FAKE | PRIVATE_WHEN_CALLED
 */
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4172)
#endif
template<typename T, typename TLambda>
T& IS_REF(TLambda predicate) __INTERNAL_IS(ANY_REF(T))
#ifdef _MSC_VER
#pragma warning (pop)
#endif

#else
/*!
 * \brief
 * Use this macro to fake an out parameter.
 * 
 * \remarks
 * Use it when you want to fake a method with an out parameter.
 * Example:
 * \code
 *bool Product::IsExpired()
 *{
 *	SYSTEMTIME now;
 *  ::GetSystemTime(&now);
 *	if (now.wYear > 2012)
 *		return true;
 *  return false;
 *  }
 *}
 *
 * TEST_F(IsolatorPPTests, IsExpired_YearIs2013_ReturnTrue)
 *{
 *	 Product product;
 *   SYSTEMTIME fakeTime;
 *   fakeTime.wYear = 2013;
 *	 FAKE_GLOBAL(GetSystemTime);
 *	 WHEN_CALLED(GetSystemTime(RET(&fakeTime))).Ignore();
 *	 ASSERT_TRUE(product.IsExpired());
 *} 
 * \endcode
 *
 * \see
 * FAKE_GLOBAL | WHEN_CALLED
 */

template <typename T>
T* RET(T* outArg, size_t RetSize = 0) __INTERNAL_PTR_RET64()

template <typename T>
T& RET(T& outArg, size_t RetSize = 0) __INTERNAL_REF_RET64()

template <typename T>
T& RET_REF(T& outArg, size_t RetSize = 0) __INTERNAL_REF_RET64()

/*!
 * \brief
 * Use this macro to fake an out parameter with condition.
 * 
 * \remarks
 * Use it when you want to fake a method with an out parameter with parameter only if parameter satisfies the condition.
 * Example:
 * \code
 * TEST_F(FakingAnOutParams, InPrivateMethodCallWithCondition)
 *{
 *	 Person* person = FAKE<Person>(FakeOptions::CallOriginal);
 *   char* out = "Smith";
 *   PRIVATE_WHEN_CALLED(person, ChangeLastName, RET_IF(EQ(BY_REF("Johns")), &out)).Return(false);
 *	 Person* otherPerson = new Person();
 *	 otherPerson->lastName = "Johns";
 *	 ASSERT_FALSE(person->GetMarried(otherPerson));
 *	 ASSERT_EQ(otherPerson->lastName, out);
 *} 
 * \endcode
 *
 * \see
 * FAKE | PRIVATE_WHEN_CALLED
 */
template <typename T, typename TCondition>
T* RET_IF(TCondition condition, T* outArg, size_t RetSize = 0) __INTERNAL_RET_IF64()

template <typename T, typename TCondition>
T& RET_IF(TCondition condition, T& outArg, size_t RetSize = 0) __INTERNAL_RET_IF64()

/*!
 * \brief
 * Use this macro for custom argument assertion.
 * 
 * \remarks
 * Use it when you want change beahavior of the method with arguments pass the lambda function, or assert the method was called with specific arguments.
 * Example:
 * \code
 * TEST_F(AssertingNonPublicInteraction, AssertPrivateWithCondition)
 *{
 *	 Person* person = new Person();
 *   PRIVATE_WHEN_CALLED(person, IsValidName).Return(true);
 *   PRIVATE_WHEN_CALLED(person, SaveAll).Ignore();
 *	 person->Save("foo");
 *	 PRIVATE_ASSERT_WAS_CALLED(person, SaveAll,  IS<std::string>([](std::string& val) {return val == "foo";}));
 *} 
 * \endcode
 *
 * \see
 * PRIVATE_ASSERT_WAS_CALLED | PRIVATE_WHEN_CALLED
 */
template<typename T, typename TLambda>
T IS(TLambda predicate) __INTERNAL_IS64(ANY_VAL(T))
/*!
 * \brief
 * Use this macro for conditional behavior faking of methods with out parameters.
 * 
 * \remarks
 * Use it when you want change beahavior of the method with an out arguments pass the lambda function, without changing them.
 * Example:
 * \code
 * TEST_F(FakingAnOutParams, InPrivateMethodCallWithConditionWithoutAssigment)
 *{
 *	 Person* person = FAKE<Person>(FakeOptions::CallOriginal);
 *   PRIVATE_WHEN_CALLED(person, ChangeLastName, IS_REF<char**>([](char** s) {return !strcmp(*s, "Smith");})).Return(false);
 *	 Person* otherPerson = new Person();
 *	 otherPerson->lastName = "Smith";
 *	 char* neededLastName = "Smith";
 *	 ASSERT_FALSE(person->GetMarried(otherPerson));
 *	 ASSERT_EQ(otherPerson->lastName, neededLastName);
 *} 
 * \endcode
 *
 * \see
 * FAKE | PRIVATE_WHEN_CALLED
 */
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4172)
#endif
template<typename T, typename TLambda>
T& IS_REF(TLambda predicate) __INTERNAL_IS64(ANY_REF(T))
#ifdef _MSC_VER
#pragma warning (pop)
#endif
#endif


/*!
 * \brief
 * Accept an object by value 
 * 
 * \remarks
 * Use BY_VAL within a 'Return' call when you want to return an object by value.
 *
 * Example:
 * \code
 * class CFoo
 * {
 * public:
 *   std::vector<std::string> ReturnVectorByVal() { throw std::exception(); }
 * };
 *
 * auto fake = FAKE<CFoo>();
 * std::vector<std::string> vectorByVal;
 * vectorByVal.push_back("blah");
 * WHEN_CALLED(fake->ReturnVectorByVal()).Return(BY_VAL(vectorByVal));
 *
 * \endcode
 *
 * \see
 * FAKE | WHEN_CALLED
 */
template <class T> 
#ifdef __linux__
Typemock::__ByValRetObj
#else
void* 
#endif
BY_VAL(T& Value) __INTERNAL_BY_VAL(Value)

/*!
 * \brief
 * Macro to assign value directly 
 * 
 * \remarks
 * Use in condition macros to assign value directly
 * Example:
 * \code
 * WHEN_CALLED(fake->Foo(RET_IF(EQ(BY_REF("typemock")), &out)).Return(1);
 * \endcode
 *
 * \see
 * RET_IF | WHEN_CALLED
 */
template <class T> 
T* BY_REF(T Value) __INTERNAL_BY_REF(Value)

/*!
 * \brief
 * Make a class/function testable even if it not called by any code (and the compiler optimizes it out)
 * 
 * \remarks
  *
 * \see
 * FAKE | WHEN_CALLED
 */
#define ISOLATOR_TESTABLE \
    __INTERNAL_TESTABLE

/*!
 * \brief
 * Disable inlining of methods
 * 
 * \remarks
 * Use for code coverage tools like BullsEye that inline methods 
 *
 */
#define ISOLATOR_FORCE_MODULE() \
	__INTERNAL_FORCE_MODULE

#ifdef ISOLATOR_USE_DEPRECATED_V1_API
	#define TESTABLE     __INTERNAL_TESTABLE
	#define FAKED
	#ifndef PURE
	#define PURE    = 0
	#endif
#endif

PRAGMA_REGION(deprecated)
// ISOLATOR_INVOKE_STATIC_MEMBER is deprecated. Use ISOLATOR_INVOKE_FUNCTION instead
#define ISOLATOR_INVOKE_STATIC_MEMBER(ret, function, ...) \
	__DEPRECATED::_ISOLATOR_INVOKE_STATIC_MEMBER(); \
	__INTERNAL_ISOLATOR_INVOKE_FUNCTION(ret, _, function, ##__VA_ARGS__)

// ISOLATOR_INVOKE_MEMBER is deprecated. Use ISOLATOR_INVOKE_FUNCTION instead
#define ISOLATOR_INVOKE_MEMBER(ret, instance, function, ...) \
    /*__DEPRECATED::_ISOLATOR_INVOKE_MEMBER();*/ \
	__INTERNAL_ISOLATOR_INVOKE_FUNCTION(ret, instance, function, ##__VA_ARGS__)

// ISOLATOR_SET_MEMBER is deprecated. Use ISOLATOR_SET_VARIABLE instead
#define ISOLATOR_SET_MEMBER(obj, data, value) \
    /*__DEPRECATED::_ISOLATOR_SET_MEMBER(); */ \
    __INTERNAL_ISOLATOR_SET_VARIABLE(obj, data, value)

// ISOLATOR_GET_MEMBER is deprecated. Use ISOLATOR_GET_VARIABLE instead
#define ISOLATOR_GET_MEMBER(obj, data, value) \
    /*__DEPRECATED::_ISOLATOR_GET_MEMBER(); */ \
	__INTERNAL_ISOLATOR_GET_VARIABLE(obj, data, value)
PRAGMA_ENDREGION(deprecated)

 #endif //ifndef ISOLATOR_H
