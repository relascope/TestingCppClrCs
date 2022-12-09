
#ifndef ISOLATOR_INTERNALS_H
#define ISOLATOR_INTERNALS_H

// This word width architecture portion
#ifdef __linux__
# ifndef __i386__
#  ifndef __linux64__ 
#   define __linux64__
#  endif
#  ifndef BITS_64
#   define BITS_64
#  endif
#  ifdef __linux32__
#   undef __linux32__
#  endif
#  ifdef BITS_32
#   undef BITS_32
#  endif
# else
#  ifndef BITS_32
#   define BITS_32
#  endif
#  ifndef __linux32__
#   define __linux32__
#  endif
#  ifdef BITS_64
#   undef BITS_64
#  endif
#  ifdef __linux64__
#   undef __linux64__
#  endif
# endif
# ifdef __Windows__
#  undef __Windows__
# endif
#else
# ifndef __Windows__
#  define __Windows__
# endif
# ifdef _WIN64
#  ifndef __Windows64__
#   define __Windows64__
#  endif
#  ifndef BITS_64
#   define BITS_64
#  endif
#  ifdef __Windows32__
#   undef __Windows32__
#  endif
#  ifdef BITS_32
#   undef BITS_32
#  endif
# else
#  ifndef __Windows32__
#   define __Windows32__
#  endif
#  ifndef BITS_32
#   define BITS_32
#  endif
#  ifdef __Windows64__
#   undef __Windows64__
#  endif
#  ifdef BITS_64
#   undef BITS_64
#  endif
# endif
#endif

#ifdef __Windows__ 
#define __IPP_FUNC__ __FUNCTION__
#elif __linux__
#define __IPP_FUNC__ __PRETTY_FUNCTION__
#endif

#ifdef __Windows__ 
#include <wtypes.h>
#include <intrin.h>
#endif

// Force empty string default c'tors
#define _DEFINE_EMPTY_STRING_CONSTRUCTOR
#include <string>
#include <list>
#include <functional>
#include <exception>
#include <fstream>
// TODO: can't have extra includes here

enum EApi
{
    // _ (underscore) - use API_ANY
    API_ANY,
    API_ANY_VAL,
    API_ANY_REF,
    API_ANY_METHOD,
    API_TYPEOF,
    API_EQ,
    API_NE,
    API_LT,
    API_LE,
    API_GT,
    API_GE,
    API_FAKE,
    API_FAKE_ALL,
    API_FAKE_STATICS,
    API_FAKE_GLOBAL,
    API_WHEN_CALLED,
    API_PRIVATE_WHEN_CALLED,
    API_ASSERT_WAS_CALLED,
    API_ASSERT_NOT_CALLED,
    API_TIMES_CALLED,
    API_PRIVATE_ASSERT_WAS_CALLED,
    API_PRIVATE_ASSERT_NOT_CALLED,
    API_PRIVATE_TIMES_CALLED,
    API_ISOLATOR_CLEANUP,
    API_ISOLATOR_SET_VARIABLE,
    API_ISOLATOR_GET_VARIABLE,
    API_GET_EXTRA_DATA,
    API_IS,         // Also API_IS_REF.
    API_RET,
    API_RET_IF,
    API_BY_VAL,
    API_BY_REF,
    API_ISOLATOR_TESTABLE,
    API_ISOLATOR_FORCE_MODULE,
    API_ISOLATOR_INVOKE_CONSTRUCTOR,
    API_ISOLATOR_INVOKE_FUNCTION,
    API_Return,
    API_Throw,
    API_ReturnFake,
    API_CallOriginal,
    API_DoMemberFunctionInstead,
    API_DoStaticOrGlobalInstead,
    API_Ignore,
    // Add to the end.
};



#ifdef __linux__
#include <stdarg.h>	// for va_arg
#include "windefs.h"

//
// An implementation of a template functor definition similar to std::function
// to be used on gcc and clang that do not support std::function in their std library
// it is closed in func_functor namespace for avoiding colleasions.
// This should be changed to std::function when it is implemented by the standard libraries
// Amit
//
namespace func_functor {

#if __cplusplus >= 201103L	// as stated in C++11 standard
	#define OVERRIDE override
#else
	#define OVERRIDE
	#define nullptr 0
#endif

    template<typename RES, typename ...ARGS>
    struct AbstractFunc {
        virtual RES operator()(ARGS&&... args) = 0;
        virtual AbstractFunc *clone() const = 0;
        virtual ~AbstractFunc() = 0;
    };	// AbstractFunc

    template<typename FUNC, typename RES, typename ...ARGS>
    class ConcreteFunc: public AbstractFunc<RES, ARGS...> {
        FUNC f;
    public:
        ConcreteFunc(const FUNC &x): f(x) {}
        RES operator()(ARGS&&... args) OVERRIDE { return f(static_cast<ARGS&&>(args)...); }
        ConcreteFunc *clone() const OVERRIDE { return new ConcreteFunc{f}; }
    };	// ConcreteFunc

    template<typename FUNC>
    struct FuncFilter {
        typedef FUNC type;
    };

    template<typename RES, typename ...ARGS>
    struct FuncFilter<RES(ARGS...)> {
        typedef RES (*type)(ARGS ...);
    };

    template<typename signature>
    class function;

    template<typename RES, typename ...ARGS>
    class function<RES(ARGS...)> {
        AbstractFunc<RES, ARGS...> *f;

		template <typename F>
		struct RetVoidWrapper {};

		template <typename RVW_RES, typename... RVW_ARGS>
		struct RetVoidWrapper<RVW_RES(RVW_ARGS...)> {
			static RVW_RES wrap(AbstractFunc<RVW_RES, RVW_ARGS...> *f, RVW_ARGS&& ... args){
				//std::cout << "RetVoidWrapper: Res wrap" << std::endl;
				if (f){
					RVW_RES r = (*f)(static_cast<RVW_ARGS&&>(args)...);
					return r;
				}
				return RVW_RES();
			}
		};

		template <typename... RVW_ARGS>
		struct RetVoidWrapper<void(RVW_ARGS...)> {
			static void wrap(AbstractFunc<void, RVW_ARGS...> *f, RVW_ARGS&& ... args){
				//std::cout << "RetVoidWrapper: void wrap" << std::endl;
				if (f)
					(*f)(static_cast<RVW_ARGS&&>(args)...);
			}
		};

    public:
        function(): f(nullptr) {}

        template<typename FUNC>
		function(const FUNC &x): f(new ConcreteFunc<typename FuncFilter<FUNC>::type, RES, ARGS...>(x)) {}

        function(const function &rhs): f(rhs.f? rhs.f->clone(): nullptr) {}

        function& operator=(const function &rhs){
            if(&rhs != this && rhs.f){
                AbstractFunc<RES, ARGS...> *temp = rhs.f->clone();
                delete f;
                f = temp;
            }
            return *this;
        }

        template<typename FUNC>
		function& operator=(const FUNC &x) {
            AbstractFunc<RES, ARGS...> *temp = new ConcreteFunc<typename FuncFilter<FUNC>::type, RES, ARGS...>(x);
            delete f;
            f = temp;
            return *this;
        }

        RES operator()(ARGS&& ... args) {
			return RetVoidWrapper<RES(ARGS...)>::wrap(f, static_cast<ARGS&&>(args)...);
        }

        ~function(){
            delete f;
        }
    };
}	// namespace func_functor

#endif

#ifdef _MSC_VER
#define DEPRECATED __declspec(deprecated)
#elif defined(__GNUC__)
#define DEPRECATED __attribute__((deprecated))
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif

#ifdef __Windows__
extern "C" int __cdecl _purecall();
extern "C" IMAGE_DOS_HEADER __ImageBase;
#else
extern "C" void __cxa_pure_virtual();
extern "C" uint64_t __ImageBase;
#endif

#ifndef _MSC_VER
#define NOEXCEPT throw()
#else
#define NOEXCEPT
#endif

#ifdef __clang__
#define __ARG_EVALUATION_ORDER 0
#else
#define __ARG_EVALUATION_ORDER 1
#endif

#ifdef BITS_64
#ifdef __cplusplus
extern "C" 
{
#endif
	void  SaveFunctionCallAddress();
    void  SaveControlActionInRbx(PVOID controlAction);
    void  StoreOutInStaticVar(); 
    void  LoadStaticVarIntoOut();
    void  LoadRbxIntoOut();
    PVOID RestoreRbx();
    int   FakeResultToRax();
    void  RetAsm(size_t cbBufSize, PVOID condition, PCSTR conditionType, BYTE argEvalOrder = __ARG_EVALUATION_ORDER);
    bool  IsStackAddress(PVOID address);
    bool  IsAsm(PCSTR predicateType, BYTE argEvalOrder = __ARG_EVALUATION_ORDER, void * ptr = NULL);
    PBYTE CheckDynamicCallImpl64();
    void* GetSavedControlData();
#ifdef __cplusplus
};
#endif
#endif

#ifdef ISOLATOR_EXPORTS
 #ifdef __Windows__ 
  #define ISOLATOR_API __declspec(dllexport)
 #else // __linux__
  // GCC exports everything by default
  //#define ISOLATOR_API
  // Explicitly:
  #define ISOLATOR_API __attribute__ ((__visibility__("default")))
 #endif //#ifdef __Windows__ 
#else
 #ifdef __Windows__ 
  #define ISOLATOR_API __declspec(dllimport)
 #else // __linux__
  // GCC imports everything by default
  //#define ISOLATOR_API
  // Explicitly:
  #define ISOLATOR_API __attribute__ ((__visibility__("default")))
 #endif

 #ifdef _cpp_min
 #undef min
 #define min _cpp_min
 #endif

 #ifdef _cpp_max
 #undef max
 #define max _cpp_max
 #endif

#endif //#ifdef ISOLATOR_EXPORTS


#ifdef __Windows__ 
#ifndef _CPPRTTI
#error Typemock Isolator++ requires C++ RTTI enabled (Compiler flag /GR)
#endif
#endif //#ifdef __Windows__ 

#ifndef NOP_FUNCTION
#ifdef __Windows__ 
#define NOP_FUNCTION	__noop
#else
// GCC __noop macro
#define NOP_FUNCTION	(0)
#endif
#endif

#ifdef __Windows__ 
#pragma warning(disable: 4101)
#pragma warning(disable: 4141)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#pragma warning(disable: 4748)
#pragma warning(disable: 4715)
#pragma warning(disable: 4716)
#pragma warning(disable: 4094)
#endif

#ifdef __Windows__ 
#define PRAGMA_REGION(arg)      __pragma(region arg)
#define PRAGMA_ENDREGION(arg)   __pragma(endregion arg)
#else
#define PRAGMA_REGION(arg)
#define PRAGMA_ENDREGION(arg)
#endif

namespace Typemock
{

#ifdef __linux__

#define ISOLATOR_INLINE static inline
    void emptyFunc();
    extern void __gen_nop();
    struct __ByValRetObj
    {
        void* m_retObjPtr;
        __ByValRetObj(const __ByValRetObj &obj) : m_retObjPtr(obj.m_retObjPtr) {}
        __ByValRetObj(void* ptr) : m_retObjPtr(ptr) {}
    };
#define POSSIBLY_THROW_EXCEPTION Typemock::emptyFunc();
#else
    static inline void __gen_nop() { __nop(); };
#define ISOLATOR_INLINE inline
#define POSSIBLY_THROW_EXCEPTION Typemock::__gen_nop();
#endif

#ifdef __linux__
    extern void * allocate_exception(size_t size);
    extern PCSTR __demangle(PCSTR str);
#endif

#ifndef ISOLATOR_EXPORTS
#ifndef ISOLATOR_NO_CRT
    static	std::string  __sz;
#endif
#endif

    enum FakeType
    {
        typeFake,
        typeAll,
        typeStatics,
        typeGlobal,
    };

    struct ControlAction
    {
        ISOLATOR_API ControlAction(INT_PTR Data, PCSTR szCall, PCSTR szName = NULL);
        ISOLATOR_API void AddOutSize(UINT nSize, BYTE argEvalOrder);
        ISOLATOR_API std::string CreateSignature(BOOL isGlobal);

        INT_PTR	 m_Data;
        PCSTR	 m_szCall;
        PCSTR	 m_szName;
        UINT	 m_nOutIndex;        

#ifdef __linux__
        ControlAction() :
            m_Data(0),
            m_szCall(NULL),
			m_pOutSize(NULL),
            m_nOutIndex(0)
        {
        }

        ~ControlAction()
		{
			if (m_pOutSize)
			{
				delete m_pOutSize;
			}
		}

        UINT*    m_pOutSize;
#else
        UINT	 m_pOutSize[64];
#endif
    };

#ifdef __linux__
#define CONTROL_ACTION_LOCAL_CONTEXT (*GetControlAction())=
#else
#define CONTROL_ACTION_LOCAL_CONTEXT
#endif

    class ISOLATOR_API RecordingStatus
    {
        bool m_inRecording;
        bool m_inControlAction;

    public:
        RecordingStatus();

        void SetRecordingMode(bool recordingMode);
        bool IsInRecordingMode();
        
        void SetControlAction(bool controlAction);
        bool IsInControlAction();
		bool IsInOurAPI();	
    };

    typedef void (__stdcall *PF_ASSERT_FAIL)(const WORD*, const PVOID);
    typedef void (*PureFreeFunc)(PVOID);

    ISOLATOR_API extern const UINT_PTR __WHEN_CALLED__;
    ISOLATOR_API extern const UINT_PTR __WHEN_CALLED_GLOB_;
    ISOLATOR_API extern const UINT_PTR __WAS_CALLED__;
    ISOLATOR_API extern const UINT_PTR __TIMES_CALLED__;
    ISOLATOR_API extern const UINT_PTR __PARAM_EOL__;	
    
    ISOLATOR_API extern RecordingStatus g_RecordingStatus;
    ISOLATOR_API extern ControlAction* g_ControlAction;
    ISOLATOR_API extern PF_ASSERT_FAIL g_pfAssertFail;

    ISOLATOR_API PVOID  __fake(PCSTR szObj, size_t nSize, int nType, PVOID pTestModule, va_list& vl);
    ISOLATOR_API PVOID  __when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, PVOID pTestModule, va_list& vl);
    ISOLATOR_API PVOID  __get_extra_data();
    ISOLATOR_API void   __isolator_cleanup();
    ISOLATOR_API void   __chk_fake(PCSTR szFunction, PVOID pTestModule);
    ISOLATOR_API void   __chk_called(BOOL fWasCalled, BOOL fActual, PCSTR szFunction);
    ISOLATOR_API bool   __add_predicate(PVOID pCode, PCSTR predicateType, BYTE argEvalOrder);
    ISOLATOR_API void   __add_ret_condition(PVOID pCondition, PCSTR szConditionType, BYTE argEvalOrder);
    ISOLATOR_API ULONG64 __get_data_member_offset(PCSTR szClass, PCSTR szValueType, PCSTR szDataMember, PVOID pObj, bool fGet, PVOID pTestModule, ULONG64& size);
	ISOLATOR_API PVOID TrackLocalAllocation(PVOID ptr);
    ISOLATOR_API void  __isolator_cleanup_locals(PureFreeFunc);
	ISOLATOR_API PVOID TrackLocalAllocation(PVOID ptr);
	ISOLATOR_API void __server_license(PCSTR lic, PCSTR com);

#ifdef __linux__
//#pragma interface - comment since clang doesn't support it 
#endif
    __interface IReturn
    {
        virtual void Return(short Value) = 0;
        virtual void Return(unsigned short Value) = 0;
        virtual void Return(int Value) = 0;
        virtual void Return(unsigned int Value) = 0;
        virtual void Return(long Value) = 0;
        virtual void Return(unsigned long Value) = 0;
        virtual void Return(long long Value) = 0;
        virtual void Return(unsigned long long Value) = 0;
        virtual void Return(float Value) = 0;
        virtual void Return(double Value) = 0;
        virtual void Return(void* Value) = 0;
#ifdef __linux__
        virtual void Return(Typemock::__ByValRetObj) = 0;
#endif
        virtual void Return(const char* Value) = 0;
        virtual void Return(const wchar_t* Value) = 0;
    };

    __interface __IBehavior_Internal : IReturn
    {
        virtual PSTR GetBehaviorError() = 0;
        virtual void Throw() = 0;
        virtual void Throw(PCSTR Message) = 0;
#ifdef __Windows__
#if (_MSC_VER > 1400)
        void Throw(PVOID Excpt);
#endif
#else
        virtual void Throw(PVOID, const std::type_info*) = 0;
#endif
        virtual void ReturnFake(int FakeOptions) = 0;
        virtual void Ignore() = 0;
        virtual IReturn& CallOriginal() = 0;

        virtual void DoStaticOrGlobalInstead(PVOID Function, PVOID ExtraData) = 0;
        virtual void DoMemberFunctionInstead(PVOID Instance, PVOID Function) = 0;

PRAGMA_REGION(deprecated)
        virtual void ReturnVal(short Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnVal(unsigned short Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnVal(int Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnVal(unsigned int Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnVal(long Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnVal(unsigned long Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnVal(long long Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnVal(unsigned long long Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnVal(float Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnVal(double Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnVal(void* Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnVal(void* Value, size_t size, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnPtr(void* Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnString(const char* Value, INT_PTR FakeOptions = 0) = 0;
        virtual void ReturnStringW(const wchar_t* Value, INT_PTR FakeOptions = 0) = 0;
PRAGMA_ENDREGION(deprecated)
    };
#ifdef __linux__
//#pragma implementation - comment since clang doesn't support it
#endif

    class __NoRetType {};

    template <typename ret>
    ret __ifretexist(ret&) {}

    template <typename ret>
    Typemock::__NoRetType __ifretexist(const ret&) {}

    template <typename retType>
    struct CFuncTemplates
    {
        retType &m_retVal;

        CFuncTemplates(retType &retVal) : m_retVal(retVal) {}

        template <typename ret>
        ret __funcTemplate() { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0> 
        ret __funcTemplate(arg0) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1> 
        ret __funcTemplate(arg0, arg1) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2> 
        ret __funcTemplate(arg0, arg1, arg2) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3> 
        ret __funcTemplate(arg0, arg1, arg2, arg3) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4> 
        ret __funcTemplate(arg0, arg1, arg2, arg3, arg4) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5> 
        ret __funcTemplate(arg0, arg1, arg2, arg3, arg4, arg5) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6> 
        ret __funcTemplate(arg0, arg1, arg2, arg3, arg4, arg5, arg6) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7> 
        ret __funcTemplate(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8> 
        ret __funcTemplate(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8, typename arg9> 
        ret __funcTemplate(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) { POSSIBLY_THROW_EXCEPTION }	

        template <typename ret>
        void __invokeFuncTemplate()
        {
            m_retVal = __funcTemplate<ret>();
        }

        template <typename ret, typename arg0>
        void __invokeFuncTemplate(arg0 a0)
        {
            m_retVal = __funcTemplate<ret>(a0);
        }

        template <typename ret, typename arg0, typename arg1>
        void __invokeFuncTemplate(arg0 a0, arg1 a1)
        {
            m_retVal = __funcTemplate<ret>(a0, a1);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2)
        {
            m_retVal = __funcTemplate<ret>(a0, a1, a2);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3)
        {
            m_retVal = __funcTemplate<ret>(a0, a1, a2, a3);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4)
        {
            m_retVal = __funcTemplate<ret>(a0, a1, a2, a3, a4);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5)
        {
            m_retVal = __funcTemplate<ret>(a0, a1, a2, a3, a4, a5);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6)
        {
            m_retVal = __funcTemplate<ret>(a0, a1, a2, a3, a4, a5, a6);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7)
        {
            m_retVal = __funcTemplate<ret>(a0, a1, a2, a3, a4, a5, a6, a7);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7, arg8 a8)
        {
            m_retVal = __funcTemplate<ret>(a0, a1, a2, a3, a4, a5, a6, a7, a8);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8, typename arg9>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7, arg8 a8, arg9 a9)
        {
            m_retVal = __funcTemplate<ret>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }
    };

    template <>
    struct CFuncTemplates<Typemock::__NoRetType>
    {
        template <class tp>
        CFuncTemplates(const tp &) {}

        CFuncTemplates() {}

        template <typename ret>
        void __funcTemplate() { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0>
        void __funcTemplate(arg0) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1>
        void __funcTemplate(arg0, arg1) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2>
        void __funcTemplate(arg0, arg1, arg2) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3>
        void __funcTemplate(arg0, arg1, arg2, arg3) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4>
        void __funcTemplate(arg0, arg1, arg2, arg3, arg4) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5>
        void __funcTemplate(arg0, arg1, arg2, arg3, arg4, arg5) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6>
        void __funcTemplate(arg0, arg1, arg2, arg3, arg4, arg5, arg6) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7>
        void __funcTemplate(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8>
        void __funcTemplate(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8, typename arg9>
        void __funcTemplate(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) { POSSIBLY_THROW_EXCEPTION }

        template <typename ret>
        void __invokeFuncTemplate()
        {
            __funcTemplate<ret>();
        }

        template <typename ret, typename arg0>
        void __invokeFuncTemplate(arg0 a0)
        {
            __funcTemplate<ret>(a0);
        }

        template <typename ret, typename arg0, typename arg1>
        void __invokeFuncTemplate(arg0 a0, arg1 a1)
        {
            __funcTemplate<ret>(a0, a1);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2)
        {
            __funcTemplate<ret>(a0, a1, a2);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3)
        {
            __funcTemplate<ret>(a0, a1, a2, a3);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4)
        {
            __funcTemplate<ret>(a0, a1, a2, a3, a4);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5)
        {
            __funcTemplate<ret>(a0, a1, a2, a3, a4, a5);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6)
        {
            __funcTemplate<ret>(a0, a1, a2, a3, a4, a5, a6);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7)
        {
            __funcTemplate<ret>(a0, a1, a2, a3, a4, a5, a6, a7);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7, arg8 a8)
        {
            __funcTemplate<ret>(a0, a1, a2, a3, a4, a5, a6, a7, a8);
        }

        template <typename ret, typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8, typename arg9>
        void __invokeFuncTemplate(arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7, arg8 a8, arg9 a9)
        {
            __funcTemplate<ret>(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }
    };

    ISOLATOR_INLINE void __assert_fail(PCSTR message)
    {
        std::string szMsg(message);
        std::wstring wzMsg(szMsg.begin(), szMsg.end());
        g_pfAssertFail(reinterpret_cast<const WORD*>(wzMsg.data()), NULL);
    }

#define HANDLE_TRAP catch (std::exception& e) { if (::Typemock::g_pfAssertFail) { ::Typemock::__assert_fail(e.what()); } else throw; }
#define TRAP(block) try { block; } HANDLE_TRAP

    ISOLATOR_INLINE PVOID __try_fake(PCSTR szObj, size_t nSize, int nType, ...)
    {
        TRAP( va_list vl; va_start(vl, nType); PVOID res = __fake(szObj, nSize, nType, &__ImageBase, vl); va_end(vl); return res; ) return NULL;
    }

    template <typename Arg> 
    inline INT64 __align_arg(Arg* pRrg)
    {
        INT64 value = 0;
        switch (sizeof(Arg))
        {
        case sizeof(INT8):
            value = *reinterpret_cast<INT8*>(pRrg);
            break;

        case sizeof(INT16):
            value = *reinterpret_cast<INT16*>(pRrg);
            break;

        case sizeof(INT32):
            value = *reinterpret_cast<INT32*>(pRrg);
            break;

        case sizeof(INT64):
            value = *reinterpret_cast<INT64*>(pRrg);
            break;
        }
        return value;
    }

    ISOLATOR_INLINE PVOID __try_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, ...)
    {
        PVOID result = NULL; 
        TRAP( va_list vl; va_start(vl, function); result = ::Typemock::__when_called_indirect(instance, controlAction, called, name, function, &__ImageBase, vl); ); 
        return result;
    }

    inline PVOID __internal_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function)
    {
        return __try_when_called_indirect(instance, controlAction, called, name, function, 0LL);
    }

    template <typename arg0> 
    inline PVOID __internal_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, arg0 a0)
    {
        return __try_when_called_indirect(instance, controlAction, called, name, function, 1LL, __align_arg(&a0));
    }

    template <typename arg0, typename arg1> 
    inline PVOID __internal_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, arg0 a0, arg1 a1)
    {
        return __try_when_called_indirect(instance, controlAction, called, name, function, 2LL, __align_arg(&a0), __align_arg(&a1));
    }

    template <typename arg0, typename arg1, typename arg2> 
    inline PVOID __internal_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, arg0 a0, arg1 a1, arg2 a2)
    {
        return __try_when_called_indirect(instance, controlAction, called, name, function, 3LL, __align_arg(&a0), __align_arg(&a1), __align_arg(&a2));
    }

    template <typename arg0, typename arg1, typename arg2, typename arg3> 
    inline PVOID __internal_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, arg0 a0, arg1 a1, arg2 a2, arg3 a3)
    {
        return __try_when_called_indirect(instance, controlAction, called, name, function, 4LL, __align_arg(&a0), __align_arg(&a1), __align_arg(&a2), 
            __align_arg(&a3));
    }

    template <typename arg0, typename arg1, typename arg2, typename arg3, typename arg4> 
    inline PVOID __internal_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4)
    {
        return __try_when_called_indirect(instance, controlAction, called, name, function, 5LL, __align_arg(&a0), __align_arg(&a1), __align_arg(&a2), 
            __align_arg(&a3), __align_arg(&a4));
    }

    template <typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5> 
    inline PVOID __internal_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5)
    {
        return __try_when_called_indirect(instance, controlAction, called, name, function, 6LL, __align_arg(&a0), __align_arg(&a1), __align_arg(&a2), 
            __align_arg(&a3), __align_arg(&a4), __align_arg(&a5));
    }

    template <typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6> 
    inline PVOID __internal_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6)
    {
        return __try_when_called_indirect(instance, controlAction, called, name, function, 7LL, __align_arg(&a0), __align_arg(&a1), __align_arg(&a2), 
            __align_arg(&a3), __align_arg(&a4), __align_arg(&a5), __align_arg(&a6));
    }

    template <typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7> 
    inline PVOID __internal_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7)
    {
        return __try_when_called_indirect(instance, controlAction, called, name, function, 8LL, __align_arg(&a0), __align_arg(&a1), __align_arg(&a2), 
            __align_arg(&a3), __align_arg(&a4), __align_arg(&a5), __align_arg(&a6), __align_arg(&a7));
    }

    template <typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8> 
    inline PVOID __internal_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7, arg8 a8)
    {
        return __try_when_called_indirect(instance, controlAction, called, name, function, 9LL, __align_arg(&a0), __align_arg(&a1), __align_arg(&a2), 
            __align_arg(&a3), __align_arg(&a4), __align_arg(&a5), __align_arg(&a6), __align_arg(&a7), __align_arg(&a8));
    }

    template <typename arg0, typename arg1, typename arg2, typename arg3, typename arg4, typename arg5, typename arg6, typename arg7, typename arg8, typename arg9> 
    inline PVOID __internal_when_called_indirect(PVOID instance, PVOID controlAction, BOOL called, PCSTR name, PCSTR function, arg0 a0, arg1 a1, arg2 a2, arg3 a3, arg4 a4, arg5 a5, arg6 a6, arg7 a7, arg8 a8, arg9 a9)
    {
        return __try_when_called_indirect(instance, controlAction, called, name, function, 10LL, __align_arg(&a0), __align_arg(&a1), __align_arg(&a2), 
            __align_arg(&a3), __align_arg(&a4), __align_arg(&a5), __align_arg(&a6), __align_arg(&a7), __align_arg(&a8), __align_arg(&a9));
    }

    class __lambda_wrapper 
    {
#if (_MSC_VER > 1400) || defined(__linux__)
        typedef PVOID (*LambdaOp)(PVOID);
    public:
        __lambda_wrapper() { _Init(); }
        template<typename T> __lambda_wrapper(T&& lambda)
        {
            _Init();
			if (_pFunctor) *_pFunctor = lambda; 
#ifdef __Windows__ 
#if (_MSC_VER > 1600)
            else
            {
                _invoke = [](PVOID pLambda) -> PVOID { (*static_cast<T*>(pLambda))(); };
                _delete = [](PVOID pLambda) -> PVOID { delete static_cast<T*>(pLambda); };
                _copy = [](PVOID pLambda) -> PVOID { return new T(*static_cast<T*>(pLambda)); };
                _pLambda = _copy(&lambda);
            }
#endif
#endif
        }

#ifdef __linux__        
        ~__lambda_wrapper();
#else
        ~__lambda_wrapper(){
            if (_pFunctor) delete _pFunctor; if (_delete) _delete(_pLambda);
        }
#endif

        __lambda_wrapper& operator=(__lambda_wrapper const &other)
        {
            if (other._pFunctor) { *_pFunctor = *other._pFunctor; }
            else { _pLambda = other._copy(other._pLambda); _invoke = other._invoke; _delete = other._delete; _copy = other._copy; }            
            return *this;
        }

#ifdef __linux__
        void operator()();
#else
        void operator()() { if (_pLambda) _invoke(_pLambda); else (*_pFunctor)(); }
#endif

#ifdef __Windows__ 
		typedef std::function<void()> VOID_FUNCTOR;

	#if (_MSC_VER > 1600)
        void _Init() { _pLambda = _invoke = _copy = _delete = NULL; _pFunctor = NULL; }
	#else
		void _Init() { _pLambda = _invoke = _copy = _delete = NULL; _pFunctor = new VOID_FUNCTOR; }
	#endif
#else
		typedef func_functor::function<void()> VOID_FUNCTOR;

		void _Init() { _pLambda = NULL; _invoke = _copy = _delete = NULL; _pFunctor = new VOID_FUNCTOR; }
#endif

	private:
        PVOID _pLambda;
        LambdaOp _invoke;
        LambdaOp _copy;
        LambdaOp _delete;
        VOID_FUNCTOR *_pFunctor; // using std::function only on versions <= VS2010
#endif
    };

    class __behavior_wrapper
    {	
        __IBehavior_Internal* m_pBehavior;

    public:	
        void Return(short Value) { TRAP(m_pBehavior->Return(Value)); }
        void Return(unsigned short Value) { TRAP(m_pBehavior->Return(Value)); }
        void Return(unsigned int Value) { TRAP(m_pBehavior->Return(Value)); } 
        void Return(long Value) { TRAP(m_pBehavior->Return(Value)); } 
        void Return(unsigned long Value) { TRAP(m_pBehavior->Return(Value)); }
        void Return(long long Value) { TRAP(m_pBehavior->Return(Value)); }
        void Return(unsigned long long Value) { TRAP(m_pBehavior->Return(Value)); }
        void Return(float Value) { TRAP(m_pBehavior->Return(Value)); }
        void Return(double Value) { TRAP(m_pBehavior->Return(Value)); }
        void Return(void* Value) { TRAP(m_pBehavior->Return(Value)); }
#ifdef __linux__
        void Return(Typemock::__ByValRetObj Value) { TRAP(m_pBehavior->Return(Value)); }
#endif
        void Return(const char* Value) { TRAP(m_pBehavior->Return(Value)); }
        void Return(const wchar_t* Value) { TRAP(m_pBehavior->Return(Value)); }

        void Throw() { TRAP(m_pBehavior->Throw()); }
        void Throw(PCSTR Message) { TRAP(m_pBehavior->Throw(Message)); }	
#ifdef __Windows__
#if (_MSC_VER > 1400)
        //@^^!+: should find a way to defin TRAP in linux
        // in wimdws there was a problem to throw any type of exception thus this lambda / pointer workaround.
        // maybe in linux we don't need this
        template <class T> void Throw(T Excpt) { TRAP(m_pBehavior->Throw(&__lambda_wrapper([Excpt]() { throw Excpt; }))); }
#endif
#else
        template <class T> void Throw(T Excpt) {
            void * ptr = allocate_exception(sizeof(T));
            T* exc_obj = new (ptr) T(Excpt);
            TRAP(m_pBehavior->Throw(exc_obj, &typeid(Excpt)));
        }

        template <class T> void Throw(T *Excpt) {
            void * ptr = allocate_exception(sizeof(T*));
            *reinterpret_cast<UINT_PTR*>(ptr) = reinterpret_cast<UINT_PTR>(Excpt);
            TRAP(m_pBehavior->Throw(ptr, &typeid(Excpt)));
        }
#endif

#ifdef __linux__
        __behavior_wrapper(__IBehavior_Internal* behavior);
        IReturn& CallOriginal();
        void Ignore();
        void ReturnFake(int FakeOptions = 0);
        void Return(int Value);
        void DoStaticOrGlobalInstead(PVOID Function, PVOID ExtraData);
        void DoMemberFunctionInstead(PVOID Instance, PVOID Function);
#else
        __behavior_wrapper(__IBehavior_Internal* behavior) : m_pBehavior(behavior) {}
        void ReturnFake(int FakeOptions = 0) { TRAP(m_pBehavior->ReturnFake(FakeOptions)); }
        IReturn& CallOriginal() { TRAP(return m_pBehavior->CallOriginal()); }
        void Ignore() { TRAP(m_pBehavior->Ignore()); }
        void Return(int Value) { TRAP(m_pBehavior->Return(Value)); }
        void DoStaticOrGlobalInstead(PVOID Function, PVOID ExtraData){
            TRAP(m_pBehavior->DoStaticOrGlobalInstead(Function, ExtraData));
        }   

        void DoMemberFunctionInstead(PVOID Instance, PVOID Function){
            TRAP(m_pBehavior->DoMemberFunctionInstead(Instance, Function));
        }
#endif

PRAGMA_REGION(deprecated)
#ifdef ISOLATOR_USE_DEPRECATED_V1_API
        ISOLATOR_API void ReturnVal(short Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnVal(unsigned short Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnVal(int Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnVal(unsigned int Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnVal(long Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnVal(unsigned long Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnVal(long long Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnVal(unsigned long long Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnVal(float Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnVal(double Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnVal(void* Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnVal(void* Value, size_t size, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnPtr(void* Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnString(const char* Value, INT_PTR FakeOptions = 0);
        ISOLATOR_API void ReturnStringW(const wchar_t* Value, INT_PTR FakeOptions = 0);
#else
        __declspec(deprecated("==> this function is deprecated, use Return(short)"))
            ISOLATOR_API void ReturnVal(short Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(unsigned short)"))
            ISOLATOR_API void ReturnVal(unsigned short Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(int)")) 
            ISOLATOR_API void ReturnVal(int Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(unsigned int)")) 
            ISOLATOR_API void ReturnVal(unsigned int Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(long)")) 
            ISOLATOR_API void ReturnVal(long Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(unsigned long)")) 
            ISOLATOR_API void ReturnVal(unsigned long Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(long long)")) 
            ISOLATOR_API void ReturnVal(long long Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(unsigned long long)")) 
            ISOLATOR_API void ReturnVal(unsigned long long Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(float)")) 
            ISOLATOR_API void ReturnVal(float Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(double)")) 
            ISOLATOR_API void ReturnVal(double Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(void*)")) 
            ISOLATOR_API void ReturnVal(void* Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(void*)")) 
            ISOLATOR_API void ReturnVal(void* Value, size_t size, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(void*)")) 
            ISOLATOR_API void ReturnPtr(void* Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(const char*)")) 
            ISOLATOR_API void ReturnString(const char* Value, INT_PTR FakeOptions = 0);
        __declspec(deprecated("==> this function is deprecated, use Return(const wchar_t*)")) 
            ISOLATOR_API void ReturnStringW(const wchar_t* Value, INT_PTR FakeOptions = 0);
#endif
PRAGMA_ENDREGION(deprecated)
    };

    template <typename T>
    void __fake_global(PCSTR szName, T function, ...)
    {
        va_list vl; va_start(vl, function);
        INT_PTR option = va_arg(vl, INT_PTR);
        if (false) // currently no arguments
        {
            std::string szFunction = szName;
            szFunction.erase(szFunction.rfind(','));
            __try_fake(typeid(*function).name(), 0, ::Typemock::typeGlobal, szFunction.data(), 
                reinterpret_cast<void(*)()>(function), option, _purecall, ::Typemock::__PARAM_EOL__);
        }    
        else
        {
            __try_fake(typeid(*function).name(), 0, ::Typemock::typeGlobal, szName, 
                reinterpret_cast<void(*)()>(function), _purecall, ::Typemock::__PARAM_EOL__);
        }    
    }

	struct ISOLATOR_API IsolatorConfigurationInternal
    {
        static bool UseMicrosoftSymbolServer;
    };
} // namespace Typemock

ISOLATOR_API void ApiRegister(EApi);
ISOLATOR_API void ApiRegister(const char*);
#define LIC_REG (::Typemock::__server_license(IPP_LICENSE_KEY, IPP_LICENSE_COMPANY))
#define API_REG1(a) (ApiRegister(a), LIC_REG)
#define API_REG2(a, b) (API_REG1(a), b)

#define __INTERNAL_GET_EXTRA_DATA() ::Typemock::__get_extra_data()

#define __FAKE(obj, size, type, ...) \
    ::Typemock::__try_fake(typeid(obj).name(), size, type, ##__VA_ARGS__, _purecall, ::Typemock::__PARAM_EOL__)

#define __INTERNAL_FAKE(obj, ...) \
    API_REG2(API_FAKE, static_cast<obj*>(__FAKE(obj, sizeof(obj), ::Typemock::typeFake, ##__VA_ARGS__)));

#define __INTERNAL_FAKE_ALL(obj, ...) \
    API_REG2(API_FAKE_ALL, static_cast<obj*>(__FAKE(obj, sizeof(obj), ::Typemock::typeAll, ##__VA_ARGS__)));

#define __INTERNAL_FAKE_STATICS(obj, ...) \
    API_REG2(API_FAKE_STATICS, __FAKE(obj, sizeof(obj), ::Typemock::typeStatics, ##__VA_ARGS__));

#define __INTERNAL_FAKE_GLOBAL(...) \
    API_REG2(API_FAKE_GLOBAL, ::Typemock::__fake_global(#__VA_ARGS__, __VA_ARGS__, ::Typemock::__PARAM_EOL__));

#ifdef __Windows32__

#define __WHEN_CALLED_DIRECT2(__behavior)		\
    PVOID __behavior;				\
    __asm { mov	[__behavior], ebx } 		\
    __asm { pop ebx } 				\
    ::Typemock::__behavior_wrapper(reinterpret_cast<::Typemock::__IBehavior_Internal*>(__behavior))
#endif

#if defined(__linux32__)

#define __WHEN_CALLED_DIRECT2(__esi,__behavior)				\
    PVOID __behavior;							\
    __asm__ __volatile__("mov	%%esi,%0" : "=m" (__behavior)); 	\
    __asm__ __volatile__("mov	%0,%%esi" : : "m" (__esi) : "esi");	\
    ::Typemock::__behavior_wrapper(reinterpret_cast<__Typemock_IBehavior_Internal *>(__behavior))

#define __WHEN_CALLED_DIRECT1(a, x, y)	\
    __WHEN_CALLED_DIRECT2(a,x##y)

#define __WHEN_CALLED_DIRECT(a, x, y)	\
    __WHEN_CALLED_DIRECT1(a, x, y)

#else 

#if defined(__linux64__)

#define __WHEN_CALLED_DIRECT2(__behavior)   \
       ::Typemock::__behavior_wrapper(reinterpret_cast<__Typemock_IBehavior_Internal *>(RestoreRbx()))

#endif

#define __WHEN_CALLED_DIRECT1(x, y)     \
    __WHEN_CALLED_DIRECT2(x##y)

#define __WHEN_CALLED_DIRECT(x, y)      \
    __WHEN_CALLED_DIRECT1(x, y)

#endif

#ifdef __linux__
// @^^!+: ToDo: see if linux & windows code can be the unified
// This is here due to a bug in gcc 4.4.7 that causes _reinterpret_cast to fail on a global scope namespace
typedef ::Typemock::__IBehavior_Internal __Typemock_IBehavior_Internal;
#endif


#ifdef __Windows64__ 

#if _MSC_VER >= 1915
#define SAVE_FUNCTION_CALL_ADDRESS SaveFunctionCallAddress();
#else
#define SAVE_FUNCTION_CALL_ADDRESS
#endif

#define __WHEN_CALLED_DIRECT2(__behavior)   \
    ::Typemock::__behavior_wrapper(reinterpret_cast<::Typemock::__IBehavior_Internal*>(RestoreRbx()))

#define __INTERNAL_WHEN_CALLED(function, function_call) { TRAP( 					\
    API_REG1(API_WHEN_CALLED);                                                      \
    ::Typemock::__chk_fake(function_call, &__ImageBase);						    \
    ::Typemock::__gen_nop();								                        \
    ::Typemock::g_RecordingStatus.SetRecordingMode(true);				            \
    ::Typemock::ControlAction __action(::Typemock::__WHEN_CALLED__, function_call);	\
    ::Typemock::g_ControlAction = &__action;										\
    SaveControlActionInRbx((PVOID)&__action);										\
    SAVE_FUNCTION_CALL_ADDRESS														\
    function;																		\
    ::Typemock::__gen_nop();								                        \
    ::Typemock::g_RecordingStatus.SetRecordingMode(false);) }			            \
    __WHEN_CALLED_DIRECT(__behavior, __COUNTER__)

#define __ASSERT_CALLED(function, function_call, called) {	TRAP(				    \
    UINT __actual;														            \
    ::Typemock::__chk_fake(function_call, &__ImageBase);					        \
    ::Typemock::__gen_nop();								                        \
    ::Typemock::g_RecordingStatus.SetControlAction(true);				            \
    ::Typemock::ControlAction __action(::Typemock::__WAS_CALLED__, function_call);	\
    SaveControlActionInRbx((PVOID)&__action); 									    \
    function;																	    \
    ::Typemock::__gen_nop();								                        \
    ::Typemock::g_RecordingStatus.SetControlAction(false);				            \
    __actual = static_cast<UINT>(reinterpret_cast<UINT64>(RestoreRbx()));           \
    ::Typemock::__chk_called(called, __actual, function_call);) }

#endif //#ifdef __Windows64__

#ifdef __Windows32__ 
#define __INTERNAL_WHEN_CALLED(function, function_call) { try {	    \
    API_REG1(API_WHEN_CALLED);                                      \
    ::Typemock::__chk_fake(function_call, &__ImageBase);		    \
    ::Typemock::__gen_nop();								        \
    ::Typemock::g_RecordingStatus.SetRecordingMode(true);		    \
    ::Typemock::ControlAction __action(::Typemock::__WHEN_CALLED__, function_call);	\
    __asm { push ebx } 								                \
    __asm { lea ebx, __action } 							        \
    function;									                    \
    ::Typemock::__gen_nop();								        \
    ::Typemock::g_RecordingStatus.SetRecordingMode(false); }        \
    HANDLE_TRAP } \
    __WHEN_CALLED_DIRECT(__behavior, __COUNTER__)
#endif





#if defined(__linux__)

#ifdef __linux32__ 

#define __INTERNAL_WHEN_CALLED(function, function_call) \
    API_REG1(API_WHEN_CALLED);                          \
    __INTERNAL_WHEN_CALLED1(function, function_call, __COUNTER__)

#define __INTERNAL_WHEN_CALLED1(x,y,z) \
    __INTERNAL_WHEN_CALLED2(a,b,x,y,z)

#define __INTERNAL_WHEN_CALLED2(a,b,x,y,z) \
    __INTERNAL_WHEN_CALLED3(a##z,b##z,x,y)

#define __INTERNAL_WHEN_CALLED3(__esi,__action,function, function_call) 		\
    ::Typemock::__chk_fake(function_call, NULL);						\
    ::Typemock::__gen_nop();								\
    ::Typemock::ControlAction __action(::Typemock::__WHEN_CALLED__, function_call);	\
    PVOID __esi;									\
    __asm__ __volatile__ ("mov  %%esi,%0" : "=m" (__esi));     				\
    __asm__ __volatile__ ("lea  %0,%%esi" : : "m" (__action) : "%esi");  		\
    /* Dummy push/pop, so we can find this place later */				\
    __asm__ __volatile__ ("push %ebx");				     		\
    __asm__ __volatile__ ("pop %ebx");				     		\
    function;										\
    __WHEN_CALLED_DIRECT(__esi,__behavior, __COUNTER__)

# else	// __linux64__ 

# define __INTERNAL_WHEN_CALLED(function, function_call) { TRAP( 					\
    API_REG1(API_WHEN_CALLED);                                                      \
    ::Typemock::__chk_fake(function_call, NULL);											\
    ::Typemock::__gen_nop();								                        \
    ::Typemock::g_RecordingStatus.SetRecordingMode(true);				            \
    ::Typemock::ControlAction __action(::Typemock::__WHEN_CALLED__, function_call);	\
    ::Typemock::g_ControlAction = &__action;										\
    SaveControlActionInRbx((PVOID)&__action);										\
    function;																		\
    ::Typemock::__gen_nop(); \
    ::Typemock::g_RecordingStatus.SetRecordingMode(false);) }						\
    __WHEN_CALLED_DIRECT(__behavior, __COUNTER__)

# endif	// __linux64__ 

#endif	// __linux__

#ifdef BITS_64
#define __SAVE_ACTION_IN_EBX(__action)
#define __RESTORE_EBX() RestoreRbx();
#elif defined(__linux__)
#warning need to adapt the code below for linux
#define __SAVE_ACTION_IN_EBX(__action)
#define __RESTORE_EBX() RestoreRbx();
#else
#define __SAVE_ACTION_IN_EBX(__action) \
    __asm { push ebx } \
    __asm { lea ebx, __action }
#define __RESTORE_EBX() __asm { pop ebx }
#endif

#define __WHEN_CALLED_INDIRECT2(instance, __call, function, ...) \
    PVOID __call = NULL;  \
    PVOID __call##_inst = NULL; \
    PCSTR __call##_name = NULL; \
    PVOID __call##_behavior = NULL; \
    ::Typemock::ControlAction __call##_action(::Typemock::__WHEN_CALLED__, NULL);	\
    ::Typemock::g_ControlAction = &__call##_action;										\
    if (instance) { __call##_inst = instance; __call##_name = typeid(instance).name(); } \
    __SAVE_ACTION_IN_EBX(__call##_action) \
    __call##_behavior = ::Typemock::__internal_when_called_indirect( \
    __call##_inst, &__call##_action, NULL, __call##_name, #function, ##__VA_ARGS__); \
    __RESTORE_EBX() \
    (*((::Typemock::__IBehavior_Internal*)__call##_behavior))


#define __WHEN_CALLED_INDIRECT1(w, x, y, z, ...) \
    __WHEN_CALLED_INDIRECT2(x, y##w, z, ##__VA_ARGS__)

#define __WHEN_CALLED_INDIRECT(w, x, y, z, ...)	\
    __WHEN_CALLED_INDIRECT1(w, x, y, z, ##__VA_ARGS__)

#define __INTERNAL_WHEN_CALLED_INDIRECT(instance, function, ...) \
    API_REG1(API_PRIVATE_WHEN_CALLED);                                              \
    __WHEN_CALLED_INDIRECT(__COUNTER__, instance, __call, function, ##__VA_ARGS__)

#ifdef __Windows32__ 
#define __ASSERT_CALLED(function, function_call, called) {	try {		        \
    BOOL __actual;								                \
    ::Typemock::__chk_fake(function_call, &__ImageBase);					        \
    ::Typemock::__gen_nop();							        \
    ::Typemock::g_RecordingStatus.SetControlAction(true);				            \
    ::Typemock::ControlAction __action(::Typemock::__WAS_CALLED__, function_call);	\
    __asm { push ebx } 								            \
    __asm { lea ebx, __action } 						        \
    function;									                \
    ::Typemock::__gen_nop();								    \
    __asm { mov	[__actual], ebx } 						        \
    __asm { pop ebx } 								            \
    ::Typemock::g_RecordingStatus.SetControlAction(false);				            \
    ::Typemock::__chk_called(called, __actual, function_call); }    \
    HANDLE_TRAP }
#endif

#ifdef __linux__

# ifdef __linux32__ 

#define __ASSERT_CALLED(function, called) {					\
    BOOL  __actual;								\
    PVOID __esi;								\
    ::Typemock::__chk_fake(#function, NULL);					\
    ::Typemock::__gen_nop();							\
    ::Typemock::ControlAction __action(::Typemock::__WAS_CALLED__, #function);	\
    __asm__ __volatile__ (" mov  %%esi,%0" : "=m"  (__esi));    		\
    __asm__ __volatile__ (" lea  %0,%%esi" : : "m" (__action) : "esi" );	\
    /* Dummy push/pop, so we can find this place later */			\
    __asm__ __volatile__ ("push %ebx");					\
    __asm__ __volatile__ ("pop %ebx");					     	\
    function;									\
    __asm__ __volatile__ (" mov	%%esi,%0" : "=m"  (__actual) );			\
    __asm__ __volatile__ (" mov	%0,%%esi" : : "m" (__esi) : "esi");		\
    ::Typemock::__chk_called(called, __actual, #function); }

# else // __linux64__
 
#define __ASSERT_CALLED(function, function_call, called) {	TRAP(				    \
    UINT __actual;														            \
    ::Typemock::__chk_fake(function_call, NULL);									        \
    ::Typemock::__gen_nop(); \
	::Typemock::g_RecordingStatus.SetControlAction(true);	\
	::Typemock::ControlAction __action(::Typemock::__WAS_CALLED__, function_call);	\
    SaveControlActionInRbx((PVOID)&__action); 									    \
    function;																	    \
    ::Typemock::__gen_nop();                                                                                    \
    __actual = static_cast<UINT>(reinterpret_cast<UINT64>(RestoreRbx()));           \
	::Typemock::g_RecordingStatus.SetControlAction(false); \
    ::Typemock::__chk_called(called, __actual, function_call);) }

# endif	// __linux64__ 

#endif // __linux__

#ifdef __linux32__ 

#define __INTERNAL_ASSERT_WAS_CALLED(function, function_call) \
    API_REG1(API_ASSERT_WAS_CALLED);                          \
    __ASSERT_CALLED(function, TRUE)

#define __INTERNAL_ASSERT_NOT_CALLED(function, function_call) \
    API_REG1(API_ASSERT_NOT_CALLED);                          \
    __ASSERT_CALLED(function, FALSE)

#else

#define __INTERNAL_ASSERT_WAS_CALLED(function, function_call) \
    API_REG1(API_ASSERT_WAS_CALLED);                          \
    __ASSERT_CALLED(function, function_call, TRUE)							
#define __INTERNAL_ASSERT_NOT_CALLED(function, function_call) \
    API_REG1(API_ASSERT_NOT_CALLED);                          \
    __ASSERT_CALLED(function, function_call, FALSE)

#endif


#define __PRIVATE_ASSERT_CALLED(instance, function, type, called, ...) \
    static_cast<UINT>(reinterpret_cast<UINT64>( \
    ::Typemock::__internal_when_called_indirect(instance, &(CONTROL_ACTION_LOCAL_CONTEXT ::Typemock::ControlAction(type, NULL)), called, typeid(instance).name(), #function, ##__VA_ARGS__)))

#define __INTERNAL_ASSERT_WAS_CALLED_INDIRECT(instance, function, ...) \
    API_REG1(API_PRIVATE_ASSERT_WAS_CALLED);                            \
    __PRIVATE_ASSERT_CALLED(instance, function, ::Typemock::__WAS_CALLED__, TRUE, ##__VA_ARGS__)

#define __INTERNAL_ASSERT_NOT_CALLED_INDIRECT(instance, function, ...) \
    API_REG1(API_PRIVATE_ASSERT_NOT_CALLED);                            \
    __PRIVATE_ASSERT_CALLED(instance, function, ::Typemock::__WAS_CALLED__, FALSE, ##__VA_ARGS__)

#define __INTERNAL_TIMES_CALLED_INDIRECT(instance, function, ...) \
    API_REG2(API_PRIVATE_TIMES_CALLED, __PRIVATE_ASSERT_CALLED(     \
        instance, function, ::Typemock::__TIMES_CALLED__, 0, ##__VA_ARGS__))

#ifdef __Windows32__
#define __INTERNAL_RET(type) {					\
    Typemock::ControlAction* pAction;			\
    UINT nSize;								    \
    __asm { push ebx }							\
    __asm { push ebp }							\
    __asm { mov  ebp, esp }						\
    __asm { sub  esp, __LOCAL_SIZE }			\
    __asm { push esi }							\
    __asm { push edi }							\
    __asm { mov  [pAction], ebx }				\
    __asm { mov  eax, [ebp + 0x10] }			\
    __asm { mov  [nSize], eax }					\
    pAction->AddOutSize(nSize ? nSize : sizeof(type), __ARG_EVALUATION_ORDER); \
    __asm { mov  eax, [ebp + 0x0C] }			\
    __asm { pop  edi }							\
    __asm { pop  esi }							\
    __asm { mov  esp, ebp }						\
    __asm { pop  ebp }							\
    __asm { pop  ebx }							\
    __asm { ret } }

#else

#define __INTERNAL_RET(type) {						\
    Typemock::ControlAction* pAction;					\
    UINT nSize;								\
    /*Dummy push/pop, so we can find this place later*/                 \
    __asm__ __volatile__ (" push %ebx ");				\
    __asm__ __volatile__ (" pop  %ebx ");			\
    __asm__ __volatile__ (" mov	%%esi,%0 " : "=m" (pAction)  );		\
    __asm__ __volatile__ (" mov	0x0C(%ebp),%eax ");			\
    __asm__ __volatile__ (" mov	%%eax,%0 " : "=m" (nSize) );		\
    pAction->AddOutSize(nSize ? nSize : sizeof(type), __ARG_EVALUATION_ORDER); \
    __asm__ __volatile__ (" mov	0x08(%ebp),%eax");	}

#endif

#define __INTERNAL_RET_IF(type) { \
    Typemock::__add_ret_condition(condition, typeid(condition).name(), __ARG_EVALUATION_ORDER); \
    API_REG1(API_RET_IF); \
    return RET<type>(OutArg, RetSize); }

#ifdef __linux__

/*
** When function parameter by value is used, copy constructor should be accessible.
** Most coding conventions require both copy constructor and assignment operator to be either
** user-defined or to be as default (generated by compiler).
** NOTE: Below copy constructor is explicitly required for _tmp. It allows to avoid of
** optimizing out necessary methods
*/
#define __INTERNAL_PTR_RET64() { \
    RetAsm(RetSize ? RetSize : sizeof(T), NULL, NULL); \
    T _tmp(*outArg); \
    API_REG1(API_RET); \
    return outArg; }

#define __INTERNAL_REF_RET64() { \
    RetAsm(RetSize ? RetSize : sizeof(T), NULL, NULL); \
    T _tmp(outArg); \
    API_REG1(API_RET); \
    return outArg; }

#else

#define __INTERNAL_PTR_RET64() { \
    RetAsm(RetSize ? RetSize : sizeof(T), NULL, NULL); \
    API_REG1(API_RET); \
    return outArg; }

#define __INTERNAL_REF_RET64() __INTERNAL_PTR_RET64()

#endif

#define __INTERNAL_RET_IF64() { \
    RetAsm(RetSize ? RetSize : sizeof(T), &condition, typeid(condition).name()); \
    API_REG1(API_RET_IF); \
    return outArg; }

#ifdef __Windows32__
#define __MOV_OUT_TO_EAX2(get_eip, next, assign, set) NULL;     \
    __asm { call get_eip }                                      \
    __asm { jmp next }			                                \
    __asm { get_eip: mov eax, [esp] }	                        \
    __asm { ret }				                                \
    __asm { next: sub eax, 0Bh }		                        \
    __asm { cmp byte ptr[eax], 45h }                            \
    __asm { je assign }                                         \
    __asm { sub eax, 02h }                                      \
    __asm { mov  eax, [eax] }	                                \
    __asm { jmp set }                                           \
    __asm { assign: movsx  eax, [eax + 1] }                     \
    __asm { set: add  eax, ebp }		                        \
    __asm { push eax } \

#define __MOV_OUT_TO_EAX1(a, b, c, d, e) \
    __MOV_OUT_TO_EAX2(a##e, b##e, c##e, d##e)

#define __MOV_OUT_TO_EAX(a, b, c, d, e) \
    __MOV_OUT_TO_EAX1(a, b, c, d, e)

#endif

#ifdef __Windows__ 
#ifdef __Windows32__ 
#define __TIMES_CALLED2(function, action, function_call)                       \
    __MOV_OUT_TO_EAX(get_eip, next, assign, set, __COUNTER__)   \
    try { ::Typemock::__chk_fake(#function, &__ImageBase);					\
    ::Typemock::__gen_nop();							        \
    ::Typemock::g_RecordingStatus.SetControlAction(true);				            \
    ::Typemock::ControlAction action(::Typemock::__TIMES_CALLED__, function_call);	\
    __asm { pop  eax }					    			        \
    __asm { push ebx } 								            \
    __asm { lea ebx, action }    						        \
    __asm { push eax }								            \
    function;									                \
    ::Typemock::__gen_nop();								    \
    __asm { pop  eax }								            \
    __asm { mov  [eax], ebx }							        \
    __asm { pop  ebx }                                          \
    ::Typemock::g_RecordingStatus.SetControlAction(false);		\
    } HANDLE_TRAP
#else
#define __TIMES_CALLED2(function, action, function_call)							    \
    (API_REG1(API_TIMES_CALLED), NULL); { TRAP(                                         \
    StoreOutInStaticVar();					                                            \
    ::Typemock::__chk_fake(#function, &__ImageBase);									\
    ::Typemock::g_RecordingStatus.SetControlAction(true);				                \
    ::Typemock::ControlAction __action(::Typemock::__TIMES_CALLED__, function_call);    \
    SaveControlActionInRbx((PVOID)&__action); 										    \
    function;																		    \
    ::Typemock::__gen_nop();								                            \
    ::Typemock::g_RecordingStatus.SetControlAction(false);				                \
    LoadRbxIntoOut();) }
#endif
#endif


#define __INTERNAL_ISOLATOR_INVOKE_CONSTRUCTOR(instance, ...) { \
    API_REG1(API_ISOLATOR_INVOKE_CONSTRUCTOR); \
    Typemock::CFuncTemplates<Typemock::__NoRetType> __func; \
    WHEN_CALLED(__func.__funcTemplate<void>(__VA_ARGS__)).DoMemberFunctionInstead(instance, ::Typemock::__ctor__); \
    __func.__invokeFuncTemplate<void>(__VA_ARGS__); \
    }

#define __INTERNAL_ISOLATOR_INVOKE_FUNCTION(ret, instance, function, ...) { \
    API_REG1(API_ISOLATOR_INVOKE_FUNCTION); \
    Typemock::CFuncTemplates<decltype(Typemock::__ifretexist(ret))> templates(ret); if (instance) { \
    WHEN_CALLED(templates.__funcTemplate<decltype(ret)>(__VA_ARGS__)).DoMemberFunctionInstead(instance, function); \
    templates.__invokeFuncTemplate<decltype(ret)>(__VA_ARGS__); \
    } else { \
    WHEN_CALLED(templates.__funcTemplate<decltype(ret)>(__VA_ARGS__)).DoStaticOrGlobalInstead(#function, NULL); \
    templates.__invokeFuncTemplate<decltype(ret)>(__VA_ARGS__); } } \

#ifdef __linux__

template <typename T>
void __INTERNAL_COPY_VAR_SIZE(T** dest, T* src, ULONG64 size, ULONG64 val_size)
{
    // check if it is an array
    if (size > sizeof(UINT_PTR))
    {
       size_t count = size / sizeof(T);
       while (count--)
       {   
           const size_t shift = count*sizeof(T);
           *(T*)((PBYTE)dest + shift) = *(T*)((PBYTE)src + shift);
       }
    }
    else
    {  
       *dest = src;
    }
}

template <typename T>
void __INTERNAL_GET_VAR_REF(T*& dest, T** src, ULONG64 size, ULONG64 val_size)
{
    // check if it is an array
    if (val_size > sizeof(UINT_PTR))
    {
       size_t count = val_size / sizeof(T);
       while (count--)
       {
           const size_t shift = count*sizeof(T);
           *(T*)((PBYTE)dest + shift) = *(T*)((PBYTE)src + shift);
       }
    }
    else
    if (size > sizeof(UINT_PTR))
    {
       dest = reinterpret_cast<T*>(src);
    }
    else
    {
       dest = *src;
    }
}

#endif

template <typename T>
void __INTERNAL_COPY_VAR_SIZE(T* dest, T src, ULONG64 size, ULONG64 val_size)
{
#ifdef __linux__
    *dest = src;
#else
    memcpy(dest, *reinterpret_cast<PVOID*>(&src), static_cast<size_t>(size));
#endif
}

template <typename T>
void __INTERNAL_GET_VAR_REF(T& dest, T* src, ULONG64 size, ULONG64 val_size)
{
#ifdef __linux__
    dest = *src;
#else
    dest = reinterpret_cast<T&>(src);
#endif
}

#define __INTERNAL_ISOLATOR_SET_VARIABLE(obj, data, value) \
    TRAP(API_REG1(API_ISOLATOR_SET_VARIABLE);               \
    ULONG64 size = 0; auto offset = Typemock::__get_data_member_offset(obj ? typeid(obj).name() : NULL, \
    typeid(decltype(value)).name(), #data, obj, false, &__ImageBase, size); \
    auto dest = reinterpret_cast<decltype(value)*>((obj ? size_t(obj) : 0LL) + offset); \
    if (size) { __INTERNAL_COPY_VAR_SIZE(dest, value, size, sizeof(value)); } else { *(dest) = value; })

#define __INTERNAL_ISOLATOR_GET_VARIABLE(obj, data, value) \
    TRAP(API_REG1(API_ISOLATOR_GET_VARIABLE);               \
    ULONG64 size = 0; auto offset = Typemock::__get_data_member_offset(obj ? typeid(obj).name() : NULL, \
    typeid(decltype(value)).name(), #data, obj, true, &__ImageBase, size); \
    auto src = reinterpret_cast<decltype(value)*>((obj ? size_t(obj) : 0LL) + offset); \
    if (size) { __INTERNAL_GET_VAR_REF(value, src, size, sizeof(value)); } else { value = *(src); })

#ifdef __linux__

# ifdef __linux32__ 

#define __TIMES_CALLED2(function, next, action) 				\
    NULL;										\
{											\
    PVOID __esi;									\
    PVOID __eax;									\
    __asm__ __volatile__ (" call 1f ");							\
    __asm__ __volatile__ (" jmp 0f ");							\
    __asm__ __volatile__ (" 1: mov (%esp), %eax  ");					\
    __asm__ __volatile__ (" ret ");							\
    __asm__ __volatile__ (" 0:  sub %0,%%eax " : : "i" (0x0A));				\
/* GCC:      __asm__ __volatile__ (" movsx (%eax),  %eax "); 2DO: check         */      \
/* CLANG:    __asm__ __volatile__ (" movsw (%eax),  %eax "); 2DO: check 	*/		\
    __asm__ __volatile__ (" add	  %ebp,%eax ");						\
    __asm__ __volatile__ (" mov    %%eax,%0" : "=m"  (__eax)); 				\
    ::Typemock::__chk_fake(#function, &__ImageBase);						\
    ::Typemock::__gen_nop();								\
    ::Typemock::ControlAction __action(::Typemock::__TIMES_CALLED__, #function);	        \
    __asm__ __volatile__ (" mov	%0,%%eax" : : "m" (__eax) : "eax");			\
    __asm__ __volatile__ (" mov  %%esi,%0" : "=m"  (__esi)); 				\
    __asm__ __volatile__ (" lea  %0,%%esi" : : "m" (__action) : "esi" );			\
    __asm__ __volatile__ (" mov  %%eax,%0" : "=m"  (__eax)); 				\
    /* Dummy push/pop, so we can find this place later */				\
    __asm__ __volatile__ (" push %ebx ");						\
    __asm__ __volatile__ (" pop %ebx ");						\
    function;										\
    __asm__ __volatile__ (" mov	%0,%%eax" : : "m" (__eax) : "eax");			\
    __asm__ __volatile__ (" mov	%esi,(%eax) ");						\
    __asm__ __volatile__ (" mov	%0,%%esi" : : "m" (__esi) : "esi");			\
}

# else // __linux64__

//2do: support on TRAP exceptions, port TRAP with __TIMES_CALLED2 to linux
#define __TIMES_CALLED2(function, action, function_call)                                                    \
    (::Typemock::__chk_fake(#function, NULL),                                                                     \
	::Typemock::g_RecordingStatus.SetControlAction(true), \
    SaveControlActionInRbx((PVOID)&(CONTROL_ACTION_LOCAL_CONTEXT ::Typemock::ControlAction(::Typemock::__TIMES_CALLED__, function_call))), \
    function, ::Typemock::__gen_nop(), \
	::Typemock::g_RecordingStatus.SetControlAction(true), *(uint64_t*)GetSavedControlData() )

# endif

#endif //#ifdef __linux__

#define __TIMES_CALLED1(x, y, z, function_call) \
    __TIMES_CALLED2(x, y##z, function_call)

#define __TIMES_CALLED(x, y, z, function_call) \
    __TIMES_CALLED1(x, y, z, function_call)

#define __INTERNAL_TIMES_CALLED(function, function_call) \
    __TIMES_CALLED(function, __action, __COUNTER__, function_call)

#define __INTERNAL_ISOLATOR_CLEANUP() \
    API_REG1(API_ISOLATOR_CLEANUP);   \
    ::Typemock::__isolator_cleanup(); \
    ::Typemock::__isolator_cleanup_locals(free);

#define __INTERNAL_ANY				            NOP_FUNCTION
#define __INTERNAL_ANY_VAL(type)                type()
#define __INTERNAL_ANY_METHOD(...)              (__VA_ARGS__)

#ifdef __linux__

template<typename _Tp>
struct ANY_REF2
{
    static _Tp& GET() { return (_Tp&)*reinterpret_cast<_Tp*>(::Typemock::__gen_nop);  }
};

template<typename _Tp>
struct ANY_REF2<_Tp&>
{
    static _Tp& GET() { return (_Tp&)*reinterpret_cast<_Tp*>(::Typemock::__gen_nop);  }
};

#define __INTERNAL_ANY_REF(T) ANY_REF2<T>::GET()

#else
#define __INTERNAL_ANY_REF(type)		(type&)(const int&)__INTERNAL_ANY
#endif

#ifdef __linux__
#define __INTERNAL_TYPEOF(type)         API_REG2(API_TYPEOF, Typemock::__demangle(typeid(type).name()))
#else
#define __INTERNAL_TYPEOF(type)         API_REG2(API_TYPEOF, typeid(type).name())
#endif

#define __INTERNAL_PREDICATE(param)    param

#ifdef __Windows32__ 

  #define __INTERNAL_IS(param) { \
    PVOID code;                         \
    __asm { call get_eip }              \
    __asm { jmp set_eip }			    \
    __asm { get_eip: mov eax, [esp] }	\
    __asm { ret }                       \
    __asm { set_eip: mov [code], eax }	\
    if (Typemock::__add_predicate(code, typeid(param).name(), __ARG_EVALUATION_ORDER)) predicate(param); \
    API_REG1(API_IS);                   \
    return param; }

#elif defined(__linux32__)

  #warning - linux inline 32 bit asm should be implemented here
  #define __INTERNAL_IS(param) { \
    PVOID code;                  \
    if (Typemock::__add_predicate(code, typeid(param).name(), __ARG_EVALUATION_ORDER)) predicate(param); \
    API_REG1(API_IS);            \
    return param; }

#else

#ifndef NO_CPP_LAMBDA_SUPPORT

 #define __INTERNAL_IS64(param) { \
    if (IsAsm(typeid(param).name(), __ARG_EVALUATION_ORDER, &predicate)) predicate(param); \
    API_REG1(API_IS);             \
    return param; }

#else

 #define __INTERNAL_IS64(param) { \
    if (IsAsm(typeid(param).name() \
              , __ARG_EVALUATION_ORDER \
              , ::Typemock::TrackLocalAllocation(new TLambda(predicate)))) predicate(param); \
    API_REG1(API_IS);             \
    return param; }

#endif

#endif

#ifdef __linux__

#define __INTERNAL_BY_VAL(Value) { \
    API_REG1(API_BY_VAL); \
    T local(Value); \
    return Typemock::__ByValRetObj(&Value); }

extern "C" Typemock::ControlAction* GetControlAction();

#else

#define __INTERNAL_BY_VAL(Value) { \
    API_REG1(API_BY_VAL); \
    T local(Value); \
    return &Value; }

#endif

#define __INTERNAL_BY_REF(Value) { \
    API_REG1(API_BY_REF); \
    return static_cast<T*>(::Typemock::TrackLocalAllocation(new T(Value))); }

#ifdef __linux__
#define __INTERNAL_TESTABLE         __attribute__((used))
#else
#define __INTERNAL_TESTABLE         __declspec(dllexport)
#endif

#define __INTERNAL_FORCE_MODULE     __pragma(inline_depth(0))

#ifndef ISOLATOR_EXPORTS
#define DoStaticOrGlobalInstead(PVOID_Function,PVOID_ExtraData) \
    DoStaticOrGlobalInstead(&(CONTROL_ACTION_LOCAL_CONTEXT ::Typemock::ControlAction( \
    (INT_PTR)(void *)PVOID_Function, typeid(PVOID_Function).name(), #PVOID_Function)), PVOID_ExtraData)

#define DoMemberFunctionInstead(PVOID_Instance,PVOID_Member) \
    DoMemberFunctionInstead(&(CONTROL_ACTION_LOCAL_CONTEXT ::Typemock::ControlAction( \
    (INT_PTR)PVOID_Instance, typeid(PVOID_Instance).name(), NULL)), (void *)(#PVOID_Member))
#endif

PRAGMA_REGION(deprecated)
class __IS_DEPRECATED
{
public:

#ifndef ISOLATOR_USE_DEPRECATED_V1_API
    __declspec(deprecated("==> this macro is deprecated, use ISOLATOR_INVOKE_FUNCTION")) 
#endif
        inline static void _ISOLATOR_INVOKE_STATIC_MEMBER() {};

#ifndef ISOLATOR_USE_DEPRECATED_V1_API
    __declspec(deprecated("==> this macro is deprecated, use ISOLATOR_INVOKE_FUNCTION")) 
#endif
        inline static void _ISOLATOR_INVOKE_MEMBER() {};

#ifndef ISOLATOR_USE_DEPRECATED_V1_API
    __declspec(deprecated("==> this macro is deprecated, use ISOLATOR_SET_VARIABLE")) 
#endif
        inline static void _ISOLATOR_SET_MEMBER() {};

#ifndef ISOLATOR_USE_DEPRECATED_V1_API
    __declspec(deprecated("==> this macro is deprecated, use ISOLATOR_GET_VARIABLE")) 
#endif
        inline static void _ISOLATOR_GET_MEMBER() {};
};
PRAGMA_ENDREGION(deprecated)

#ifdef __Windows__ 
#pragma warning(default: 4716)
#endif

#endif //#ifndef ISOLATOR_INTERNALS_H
