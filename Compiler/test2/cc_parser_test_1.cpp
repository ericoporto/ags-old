#include <string>
#include <fstream>
#include <streambuf>

#include "gtest/gtest.h"

#include "script/cc_options.h"
#include "script/cc_error.h"

#include "script2/cc_symboltable.h"
#include "script2/cc_internallist.h"
#include "script2/cs_parser.h"

#include "cc_parser_test_lib.h"


// The vars defined here are provided in each test that is in category "Compile1"
class Compile1 : public ::testing::Test
{
protected:
    AGS::ccCompiledScript scrip = AGS::ccCompiledScript(); // Note: calls Init();

    Compile1()
    {
        // Initializations, will be done at the start of each test
        ccSetOption(SCOPT_NOIMPORTOVERRIDE, false);
        ccSetOption(SCOPT_LINENUMBERS, true);
        clear_error();
    }
};


TEST_F(Compile1, Sections) {

    // Errors in globalscript must be reported for globalscript
    // "__NEWSCRIPTSTART..." begins a line #0,
    // so the error must be reported on line 3.

    char *inpl = "\
        \"__NEWSCRIPTSTART_globalscript.ash\"   \n\
        int main()                              \n\
        {                                       \n\
            return 2 < 1 ? ;                    \n\
                    break;                      \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : last_seen_cc_error());
    EXPECT_EQ(3, currentline);
    ASSERT_STREQ("globalscript.ash", ccCurScriptName);
}

TEST_F(Compile1, Autoptr) {

    // String is autoptr so should not print as "String *"

    char *inpl = "\
        managed autoptr builtin struct String   \n\
        {};                                     \n\
        int main()                              \n\
        {                                       \n\
            String var = 15;                    \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    ASSERT_EQ(std::string::npos, msg.find("String *"));
}

TEST_F(Compile1, BinaryNot)
{    

    // '!' can't be binary

    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            int Var = 15 ! 2;                   \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("inary or postfix op"));
}

TEST_F(Compile1, UnaryDivideBy) { 

    // '/' can't be unary

    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            int Var = (/ 2);                    \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("refix operator"));
}

TEST_F(Compile1, UnaryPlus) {

    // '/' can't be unary

    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            return +42;                         \n\
        }                                       \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, FloatInt1) {  

    // Can't mix float and int

    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            int Var = 4 / 2.0;                  \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'/'"));
}

TEST_F(Compile1, FloatInt2) {

    // Can't negate float

    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            int Var = !2.0;                     \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("after '!'"));
}

TEST_F(Compile1, StringInt1) {    

    // Can't mix string and int

    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            int Var = (\"Holzschuh\" == 2);     \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("compare"));
}

TEST_F(Compile1, ExpressionVoid) {  

    // Can't mix void

    char *inpl = "\
        import void Func();                     \n\
        int main()                              \n\
        {                                       \n\
            int Var = 1 + Func() * 3;           \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("ype mismatch"));
}

TEST_F(Compile1, ExpressionLoneUnary1) { 

    // Unary -, nothing following

    char *inpl = "\
        import void Func();                     \n\
        int main()                              \n\
        {                                       \n\
            int Var;                            \n\
            Var = -;                            \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("xpected a term"));
}

TEST_F(Compile1, ExpressionLoneUnary2) {

    // Unary ~, nothing following

    char *inpl = "\
        import void Func();                     \n\
        int main()                              \n\
        {                                       \n\
            int Var;                            \n\
            Var = ~;                            \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("xpected a term"));
}

TEST_F(Compile1, ExpressionBinaryWithoutRHS) {

    // Binary %, nothing following

    char *inpl = "\
        import void Func();                     \n\
        int main()                              \n\
        {                                       \n\
            int Var;                            \n\
            Var = 5 %;                          \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("ollowing"));
}

TEST_F(Compile1, LocalTypes1)
{
    char *inpl = "\
        void Test1()            \n\
        {                       \n\
            struct MyStruct     \n\
            {                   \n\
                int a;          \n\
            };                  \n\
        }                       \n\
        ";
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("struct type"));
}

TEST_F(Compile1, LocalTypes2)
{
    char *inpl = "\
        void Test1()            \n\
        {                       \n\
            enum Foo            \n\
            {                   \n\
                a,              \n\
            };                  \n\
        }                       \n\
        ";
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("enum type"));
}

TEST_F(Compile1, StaticArrayIndex1) {

    // Constant array index, is out ouf bounds

    char *inpl = "\
        enum E                          \n\
        {                               \n\
            MinusFive = -5,             \n\
        };                              \n\
        int main()                      \n\
        {                               \n\
            int Var[5];                 \n\
            Var[-MinusFive];            \n\
        }                               \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("oo high"));
}

TEST_F(Compile1, StaticArrayIndex2) {

    // Constant array index, is out ouf bounds

    char *inpl = "\
        enum E                          \n\
        {                               \n\
            MinusFive = -5,             \n\
        };                              \n\
        int main()                      \n\
        {                               \n\
            int Var[5];                 \n\
            Var[MinusFive];             \n\
        }                               \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("oo low"));
}

TEST_F(Compile1, ExpressionArray1) {    

    // Can't mix void

    char *inpl = "\
        import void Func();                     \n\
        int main()                              \n\
        {                                       \n\
            int Var[8];                         \n\
            Var++;                              \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("ype mismatch"));
}

TEST_F(Compile1, FuncTypeClash1) {

    // Can't use func except in a func call

    char *inpl = "\
        int Func()                              \n\
        {                                       \n\
        }                                       \n\
                                                \n\
        int main()                              \n\
        {                                       \n\
            int Var = Func;                     \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'('"));
}

TEST_F(Compile1, FloatOutOfBounds) {

    // Too small

    char *inpl = "\
        int Func()                              \n\
        {                                       \n\
        }                                       \n\
                                                \n\
        int main()                              \n\
        {                                       \n\
            int Var = -77E7777;                 \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("ut of bounds"));
}

TEST_F(Compile1, DoWhileSemicolon) { 

    // ';' missing

    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            int I = 1;                          \n\
            do {                                \n\
                I *= 2;                         \n\
            } while (I < 10)                    \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("';'"));
}

TEST_F(Compile1, ExtenderExtender1) {    

    // No extending a struct with a compound function

    char *inpl = "\
        struct Struct1                      \n\
        {                                   \n\
            void Func();                    \n\
        };                                  \n\
        struct Struct2                      \n\
        {                                   \n\
        };                                  \n\
        void Struct1::Func(static Struct2)  \n\
        {                                   \n\
        }                                   \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'::'"));
}

TEST_F(Compile1, ExtenderExtender2) {    

    // No extending a struct with a compound function

    char *inpl = "\
        struct Struct1                      \n\
        {                                   \n\
        };                                  \n\
        struct Struct2                      \n\
        {                                   \n\
            void Struct1::Func(short i);    \n\
        };                                  \n\
    ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'::'"));
}

TEST_F(Compile1, NonManagedStructParameter) {

    // Can't pass a non-managed struct as a function parameter

    char *inpl = "\
        struct Struct                           \n\
        {                                       \n\
        };                                      \n\
        int Func(Struct S)                      \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("non-managed"));
}

TEST_F(Compile1, StrangeParameterName) {   

    // Can't use keyword as parameter name

    char *inpl = "\
        void Func(int while)                    \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("while"));
}

TEST_F(Compile1, DoubleParameterName) { 

    // Can't use keyword as parameter name

    char *inpl = "\
        void Func(int PI, float PI)             \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("PI"));
}

TEST_F(Compile1, FuncParamDefaults1) {

    // Can't give a parameter a default here, not a default there

    char *inpl = "\
        void Func(int i = 5, float j = 6.0);    \n\
        void Func(int i = 5, float j)           \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("#2"));
}

TEST_F(Compile1, FuncParamDefaults2) {

    // Can't give a parameter a default here, not a default there

    char *inpl = "\
        void Func(int i, float j);          \n\
        void Func(int i, float j = 6.0)     \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("#2"));
}

TEST_F(Compile1, FuncParamDefaults3) {   

    // Can't give a parameter differing defaults

    char *inpl = "\
        void Func(float J = -6.0);              \n\
        void Func(float J = 6.0)                \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("#1"));
}

TEST_F(Compile1, FuncParamNumber1) {  

    // Differing number of parameters

    char *inpl = "\
        void Func(int, float);                  \n\
        void Func(int I, float J, short K)      \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("parameters"));
}

TEST_F(Compile1, FuncParamNumber2) {  

    char *inpl = "\
        struct Test                                         \n\
        {                                                   \n\
            import void Func(int a, int b, int c, int d);   \n\
        };                                                  \n\
                                                            \n\
        void Test::Func(int a)                              \n\
        {                                                   \n\
        }                                                   \n\
        ";

    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, FuncVariadicCollision) {    

    // Variadic / non-variadic

    char *inpl = "\
        void Func(int, float, short, ...);      \n\
        void Func(int I, float J, short K)      \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("additional"));
}

TEST_F(Compile1, FuncReturnTypes) {   

    // Return types

    char *inpl = "\
        int Func(int, float, short);            \n\
        short Func(int I, float J, short K)     \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("eturn"));
}

TEST_F(Compile1, FuncReturnStruct1) {  

    // Return vartype must be managed when it is a struct

    char *inpl = "\
        struct Struct {  };                     \n\
        Struct Func()                           \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("managed"));
}

TEST_F(Compile1, FuncReturnStruct2) { 

    // Should work -- Compiler will imply the '*'

    char *inpl = "\
        managed struct Struct {  };             \n\
        Struct Func()                           \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, FuncReturnStruct3) {

    // Should work

    char *inpl = "\
        managed struct Struct {  };             \n\
        Struct[] Func()                         \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, FuncDouble) {

    // No two equally-named functions with body

    char *inpl = "\
        void Func()                             \n\
        {                                       \n\
        }                                       \n\
        void Func()                             \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("with body"));
}

TEST_F(Compile1, FuncProtected) {

    // Protected functions must be part of a struct

    char *inpl = "\
        protected void Func(int I = 6)          \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("protected"));
}

TEST_F(Compile1, FuncNameClash1) {

    // Can't give a parameter differing defaults

    char *inpl = "\
        int Func;                               \n\
        void Func(int I = 6)                    \n\
        {                                       \n\
        }                                       \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("declared as a function"));
}

TEST_F(Compile1, TypeEqComponent) {

    // A struct component may have the same name as a type.

    char *inpl = "\
        builtin managed struct Room             \n\
        {                                       \n\
        };                                      \n\
                                                \n\
        builtin managed struct Character        \n\
        {                                       \n\
            readonly import attribute int Room; \n\
        };                                      \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, ExtenderFuncClash) {

    // Don't remember the struct of extender functions past their definition (and body, if applicable)

    char *inpl = "\
        builtin struct Maths 							\n\
        { 												\n\
        }; 												\n\
        import int Abs(static Maths, int value);		\n\
        import int Max(static Maths, int a, int b);		\n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, MissingSemicolonAfterStruct1) {

    // Missing ";" after struct declaration; isn't a var decl either

    char *inpl = "\
        enum bool { false = 0, true = 1 };      \n\
        struct CameraEx                         \n\
        {                                       \n\
            import static readonly attribute bool StaticTarget;  \n\
        }                                       \n\
                                                \n\
        bool get_StaticTarget(static CameraEx)  \n\
        {                                       \n\
            return 0;                           \n\
        }                                       \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("orget a"));
}

TEST_F(Compile1, NewBuiltin1) {    

    // Cannot do "new X;" when X is a builtin type

    char *inpl = "\
        builtin managed struct DynamicSprite            \n\
        {                                               \n\
        };                                              \n\
                                                        \n\
        struct SpriteFont                               \n\
        {                                               \n\
            DynamicSprite *Glyph;                       \n\
            import void    CreateFromSprite();          \n\
        };                                              \n\
                                                        \n\
        void SpriteFont::CreateFromSprite()             \n\
        {                                               \n\
            this.Glyph = new DynamicSprite;             \n\
        }                                               \n\
        ";
   
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("built-in"));
}

TEST_F(Compile1, NewArrayBuiltin1) { 

    // Can do "new X[77];" when X is a builtin type because this will only
    // allocate a dynarray of pointers, not of X chunks

    char *inpl = "\
        builtin managed struct DynamicSprite            \n\
        {                                               \n\
        };                                              \n\
                                                        \n\
        struct SpriteFont                               \n\
        {                                               \n\
            DynamicSprite *Glyphs[];                    \n\
            import void    CreateFromSprite();          \n\
        };                                              \n\
                                                        \n\
        void SpriteFont::CreateFromSprite()             \n\
        {                                               \n\
            this.Glyphs = new DynamicSprite[77];        \n\
        }                                               \n\
        ";
 
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, MissingFunc) {   

    // Must either import or define a function with body if you want to call it.
    // Also, check that the section is set correctly.

    char *inpl = "\
\"__NEWSCRIPTSTART_HauntedHouse\"                       \n\
        int main()                                      \n\
        {                                               \n\
            return GhostFunc();                         \n\
        }                                               \n\
                                                        \n\
        int GhostFunc(float f = 0.0);                   \n\
        ";
    
    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_STREQ("HauntedHouse", ccCurScriptName);
}

TEST_F(Compile1, FixupMismatch) {  

    // Code cells that have an "import" fixup must point to the corresponding imports.
    // (This used to fail in combination with linenumbers turned on)

    char *inpl = "\
        builtin managed struct InventoryItem    \n\
        {                                       \n\
            readonly int reserved[2];           \n\
        };                                      \n\
        import InventoryItem i400Mirror;        \n\
        import InventoryItem i400Key;           \n\
        import InventoryItem inventory[3];      \n\
                                                \n\
        int main()                              \n\
        {                                       \n\
            switch (inventory[1])               \n\
            {                                   \n\
            case i400Mirror:                    \n\
                break;                          \n\
            case i400Key:                       \n\
                break;                          \n\
            }                                   \n\
        }                                       \n\
        ";

    ccSetOption(SCOPT_LINENUMBERS, true);

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());

    // Sanity check for import fixups: The corresponding Bytecode must
    // point into the imports[] array, and the corresponding slot must
    // contain a non-empty string.
    for (size_t fixup_idx = 0; fixup_idx < static_cast<size_t>(scrip.numfixups); fixup_idx++)
    {
        if (FIXUP_IMPORT != scrip.fixuptypes[fixup_idx])
            continue;
        int const code_idx = scrip.fixups[fixup_idx];
        EXPECT_TRUE(code_idx < scrip.codesize);

        int const cv = scrip.code[code_idx];
        EXPECT_TRUE(cv >= 0);
        EXPECT_TRUE(cv < scrip.numimports);
        EXPECT_FALSE('\0' == scrip.imports[cv][0]);
    }
}

TEST_F(Compile1, ComponentOfNonStruct1) {

    // If a '.' follows something other than a struct then complain about that fact.
    // Do not complain about expecting and not finding a component.

    char *inpl = "\
        struct MyStruct             \n\
        {                           \n\
            int i;                  \n\
        };                          \n\
                                    \n\
        void TestArray()            \n\
        {                           \n\
            MyStruct arr[100];      \n\
            arr.i = 0;              \n\
        }                           \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'.'"));
}

TEST_F(Compile1, ComponentOfNonStruct2) {

    // If a '.' follows something other than a struct then complain about that fact.
    // Do not complain about expecting and not finding a component.

    char *inpl = "\
        void Test()     \n\
        {               \n\
            int i;      \n\
            i.j = 0;    \n\
        }               \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'.'"));
}

TEST_F(Compile1, EmptySection) {

    // An empty last section should not result in an endless loop.

    char *inpl = "\
\"__NEWSCRIPTSTART_FOO\"     \n\
\"__NEWSCRIPTSTART_BAR\"      \n\
        ";

    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, AutoptrDisplay) {

    // Autopointered types should not be shown with trailing ' ' in messages
    char *inpl = "\
        internalstring autoptr builtin      \n\
            managed struct String           \n\
        {                                   \n\
        };                                  \n\
        void main() {                       \n\
            String s = 17;                  \n\
        }                                   \n\
        ";

    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    ASSERT_EQ(std::string::npos, msg.find("'String '"));
}

TEST_F(Compile1, ReadonlyObjectWritableAttribute)
{
    // player is readonly, but player.InventoryQuantity[...] can be written to.

    char *inpl = "\
        builtin managed struct Character                \n\
        {                                               \n\
            import attribute int InventoryQuantity[];   \n\
        };                                              \n\
                                                        \n\
        import readonly Character *player;              \n\
                                                        \n\
        void main()                                     \n\
        {                                               \n\
            player.InventoryQuantity[15] = 0;           \n\
        }                                               \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, ImportAutoptr1) {

    // Import decls of funcs with autopointered returns must be processed correctly.

    char *inpl = "\
        internalstring autoptr builtin      \n\
            managed struct String           \n\
        {                                   \n\
        };                                  \n\
                                            \n\
        import String foo(int);             \n\
                                            \n\
        String foo(int bar)                 \n\
        {                                   \n\
            return null;                    \n\
        }                                   \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, ImportAutoptr2) {

    // Import decls of autopointered variables must be processed correctly.

    char *inpl = "\
        internalstring autoptr builtin      \n\
            managed struct String           \n\
        {                                   \n\
        };                                  \n\
                                            \n\
        import String foo;                  \n\
        String foo;                         \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, DynptrDynarrayMismatch1)
{
    // It is an error to assign a Dynpointer to a Dynarray variable

    char *inpl = "\
        managed struct Strct                \n\
        {                                   \n\
            int Payload;                    \n\
        };                                  \n\
                                            \n\
        int foo ()                          \n\
        {                                   \n\
            Strct *o[] = new Strct;         \n\
        }                                   \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("assign"));
}

TEST_F(Compile1, DynptrDynarrayMismatch1a)
{
    // It is an error to assign a Dynpointer to a Dynarray variable

    char *inpl = "\
        managed struct Strct                \n\
        {                                   \n\
            int Payload;                    \n\
        };                                  \n\
                                            \n\
        int foo ()                          \n\
        {                                   \n\
            Strct *o[];                     \n\
            o= new Strct;                   \n\
        }                                   \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("assign"));
}

TEST_F(Compile1, DynptrDynarrayMismatch2)
{
    // It is an error to assign a Dynarray to a Dynpointer variable

    char *inpl = "\
        builtin managed struct Object       \n\
        {                                   \n\
            int Payload;                    \n\
        };                                  \n\
                                            \n\
        int foo ()                          \n\
        {                                   \n\
            Object *o = new Object[10];     \n\
        }                                   \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("assign"));
}

TEST_F(Compile1, ZeroMemoryAllocation1)
{

    // If a struct type doesn't contain any variables then there are zero bytes 
    // to allocate. However, it _is_ legal to allocate a dynarray for the
    // struct. (Its elements could be initialized via other means than new.)

    char *inpl = "\
        managed struct Strct                \n\
        {                                   \n\
        };                                  \n\
                                            \n\
        int foo ()                          \n\
        {                                   \n\
            Strct *o[] = new Strct[10];     \n\
        }                                   \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, ZeroMemoryAllocation2)
{
    // If a struct type doesn't contain any variables then there are zero
    // bytes to allocate. The Engine really doesn't like allocating 0 bytes

    char *inpl = "\
        managed struct Strct                \n\
        {                                   \n\
        };                                  \n\
                                            \n\
        int foo ()                          \n\
        {                                   \n\
            Strct *o = new Strct;           \n\
        }                                   \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'Strct'"));
}


TEST_F(Compile1, AttribInc) {

    // Import decls of autopointered variables must be processed correctly.

    char *inpl = "\
        builtin managed struct Object       \n\
        {                                   \n\
            import attribute int  Graphic;  \n\
        } obj;                              \n\
                                            \n\
        int foo ()                          \n\
        {                                   \n\
            obj.Graphic++;                  \n\
        }                                   \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1,ForwardStructManaged)
{
    // Forward-declared structs must be 'managed', so the
    // actual declaration must have the 'managed' keyword

    char *inpl = "\
        managed struct Object;              \n\
        struct Object                       \n\
        {                                   \n\
            import attribute int Graphic;   \n\
        } obj;                              \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'managed'"));
}

TEST_F(Compile1, ForwardStructBuiltin)
{
    // Either the forward decl and the actual decl must both be 'builtin'
    // or both be non-'builtin'.

    char *inpl1 = "\
        managed struct Object;              \n\
        managed builtin struct Object       \n\
        {                                   \n\
        };                                  \n\
        ";
    int compile_result1 = cc_compile(inpl1, scrip);
    std::string msg1 = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result1 >= 0) ? "Ok" : msg1.c_str());
    EXPECT_NE(std::string::npos, msg1.find("'builtin'"));

    char *inpl2 = "\
        builtin managed struct Object;      \n\
        managed struct Object               \n\
        {                                   \n\
        };                                  \n\
        ";
    int compile_result2 = cc_compile(inpl2, scrip);
    std::string msg2 = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result2 >= 0) ? "Ok" : msg2.c_str());
    EXPECT_NE(std::string::npos, msg2.find("'builtin'"));
}

TEST_F(Compile1, ForwardStructAutoptr)
{
    // Either the forward decl and the actual decl must both be 'autoptr'
    // or both be non-'autoptr'.

    char *inpl1 = "\
        managed struct Object;              \n\
        managed builtin struct Object       \n\
        {                                   \n\
        };                                  \n\
        ";
    int compile_result1 = cc_compile(inpl1, scrip);
    std::string msg1 = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result1 >= 0) ? "Ok" : msg1.c_str());
    EXPECT_NE(std::string::npos, msg1.find("'builtin'"));

    char *inpl2 = "\
        builtin managed struct Object;      \n\
        managed struct Object               \n\
        {                                   \n\
        };                                  \n\
        ";
    int compile_result2 = cc_compile(inpl2, scrip);
    std::string msg2 = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result2 >= 0) ? "Ok" : msg2.c_str());
    EXPECT_NE(std::string::npos, msg2.find("'builtin'"));
}

TEST_F(Compile1, FuncThenAssign)
{
    // A function symbol in front of an assignment
    // The compiler should complain about a missing '('  

    char *inpl2 = "\
        import int GetTextHeight                    \n\
            (const string text, int, int width);    \n\
                                                    \n\
        builtin managed struct Character            \n\
        {                                           \n\
            readonly import attribute int Baseline; \n\
        };                                          \n\
                                                    \n\
        import readonly Character *player;          \n\
                                                    \n\
        int game_start()                            \n\
        {                                           \n\
            GetTextHeight                           \n\
            player.Baseline = 1;                    \n\
        }                                           \n\
        ";
    int compile_result2 = cc_compile(inpl2, scrip);
    std::string msg2 = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result2 >= 0) ? "Ok" : msg2.c_str());
    EXPECT_NE(std::string::npos, msg2.find("'('"));
}

TEST_F(Compile1, BuiltinForbidden)
{
    // Function names must not start with '__Builtin_'.

    char *inpl = "\
        void __Builtin_TestFunc()   \n\
        {                           \n\
            return;                 \n\
        }                           \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("__Builtin_"));
}

TEST_F(Compile1, ReadonlyParameters1) {

    // Parameters may be declared "readonly" so that they cannot be
    // assigned to within the function.

    char *inpl = "\
        int foo(readonly int bar)           \n\
        {                                   \n\
            bar++;                          \n\
            return bar;                     \n\
        }                                   \n\
                                            \n\
        int main ()                         \n\
        {                                   \n\
            return foo(5);                  \n\
        }                                   \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_EQ(std::string::npos, msg.find("parameter list"));
    EXPECT_NE(std::string::npos, msg.find("readonly"));
}

TEST_F(Compile1, ReadonlyParameters2) {

    // "readonly" parameters can be assigned to other variables,
    // but they may not be modified themselves.
    // Contrast this to "const" parameters, they
    // may only assigned to variables that are "const" and 
    // may not be returned unless the return vartype is "const".
    // "Readonly" does NOT imply "const".
    // All the assignments in the function should be allowed.

    char *inpl = "\
        int ReadonlyTest2(readonly int ReadOnly)    \n\
        {                                   \n\
            readonly int A = ReadOnly;      \n\
            int B;                          \n\
            B = ReadOnly;                   \n\
            int C = ReadOnly;               \n\
            return ReadOnly;                \n\
        }                                   \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, BinaryCompileTimeEval1) {

    // Checks binary compile time evaluations for integers.

    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            return (4 + 3) / 0;                 \n\
        }                                       \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'7 /"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (1073741824 + 1073741824);   \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("Overflow"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (1073741824 + 1073741823);   \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compileResult >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, CTEvalIntPlus) {


    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            return (4 + 3) / 0;                 \n\
        }                                       \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok1", (compileResult >= 0) ? "Ok1" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'7 /"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (1073741824 + 1073741824);   \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok2", (compileResult >= 0) ? "Ok2" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("Overflow"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (-1073741824 + -1073741823); \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STREQ("Ok3", (compileResult >= 0) ? "Ok3" : msg.c_str());
}

TEST_F(Compile1, CTEvalIntMinus) {


    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            return (83 - 95) / 0;               \n\
        }                                       \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok1", (compileResult >= 0) ? "Ok1" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'-12 /"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (-1073741824 - 1073741824);  \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok2", (compileResult >= 0) ? "Ok2" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("Overflow"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (-1073741824 - 1073741823);  \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STREQ("Ok3", (compileResult >= 0) ? "Ok3" : msg.c_str());
}

TEST_F(Compile1, CTEvalIntMultiply) {


    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            return (33 * -39) / 0;              \n\
        }                                       \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok1", (compileResult >= 0) ? "Ok1" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'-1287 /"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (46341 * 46341);             \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok2", (compileResult >= 0) ? "Ok2" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("Overflow"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (46341 * 46340);             \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STREQ("Ok3", (compileResult >= 0) ? "Ok3" : msg.c_str());
}

TEST_F(Compile1, CTEvalIntDivide) {


    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            return (52 / 8) / 0;                \n\
        }                                       \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok1", (compileResult >= 0) ? "Ok1" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'6 /"));
}

TEST_F(Compile1, CTEvalIntModulo) {


    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            return (95 % 17) / 0;               \n\
        }                                       \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok1", (compileResult >= 0) ? "Ok1" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'10 /"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (46341 % -0);                \n\
        }                                       \n\
        ";
    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok2", (compileResult >= 0) ? "Ok2" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("Modulo zero"));
}

TEST_F(Compile1, CTEvalIntShiftLeft) {


    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            return (60 << 3) / 0;               \n\
        }                                       \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok1", (compileResult >= 0) ? "Ok1" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'480 /"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return 536870912 << 2;              \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok2", (compileResult >= 0) ? "Ok2" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("Overflow"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (-1073 << 4) / 0; \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok3", (compileResult >= 0) ? "Ok3" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'-17168 /"));
    
    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (1073 << 0) / 0; \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok4", (compileResult >= 0) ? "Ok4" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'1073 /"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return 1073 << -5;                  \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok5", (compileResult >= 0) ? "Ok5" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("egative shift"));
}

TEST_F(Compile1, CTEvalIntShiftRight) {


    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            return (60 >> 3) / 0;               \n\
        }                                       \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok1", (compileResult >= 0) ? "Ok1" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'7 /"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (-10730 >> 4) / 0; \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok3", (compileResult >= 0) ? "Ok3" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'-671 /"));
    
    inpl = "\
        int main()                              \n\
        {                                       \n\
            return (1073 >> 0) / 0; \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok4", (compileResult >= 0) ? "Ok4" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'1073 /"));

    inpl = "\
        int main()                              \n\
        {                                       \n\
            return 1073 >> -5;                  \n\
        }                                       \n\
        ";

    compileResult = cc_compile(inpl, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok5", (compileResult >= 0) ? "Ok5" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("egative shift"));
}

TEST_F(Compile1, CTEvalIntComparisons) {

    // Will fail as soon as any one of those comparisons go awry

    char *inpl = "\
        int main()                          \n\
        {                                   \n\
            return                          \n\
                (          (7 == 77)        \n\
                + 2 *      (7 >= 77)        \n\
                + 4 *      (7 >  77)        \n\
                + 8 *      (7 <= 77)        \n\
                + 16 *     (7 <  77)        \n\
                + 32 *     (7 != 77)        \n\
                + 64 *     (77 == 7)        \n\
                + 128 *    (77 >= 7)        \n\
                + 256 *    (77 >  7)        \n\
                + 512 *    (77 <= 7)        \n\
                + 1024 *   (77 <  7)        \n\
                + 2048 *   (77 != 7)        \n\
                + 4096 *   (77 == 77)       \n\
                + 81928 *  (77 >= 77)       \n\
                + 16384 *  (77 >  77)       \n\
                + 32768 *  (77 <= 77)       \n\
                + 65536 *  (77 <  77)       \n\
                + 131072 * (77 != 77)) / 0; \n\
        }                                   \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok1", (compileResult >= 0) ? "Ok1" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'121280 /"));
}

TEST_F(Compile1, CTEvalBitOps) {

    char *inpl = "\
        int main()                        \n\
        {                                 \n\
            return                        \n\
                (          (0 & 0)        \n\
                + 2 *      (0 | 0)        \n\
                + 4 *      (0 ^ 0)        \n\
                + 8 *      (0 & 3)        \n\
                + 16 *     (0 | 3)        \n\
                + 32 *     (0 ^ 3)        \n\
                + 64 *     (3 & 0)        \n\
                + 128 *    (3 | 0)        \n\
                + 256 *    (3 ^ 0)        \n\
                + 512 *    (3 & 3)        \n\
                + 1024 *   (3 | 3)        \n\
                + 2048 *   (3 ^ 3)) / 0;  \n\
        }                                 \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok1", (compileResult >= 0) ? "Ok1" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'5904 /"));
}

TEST_F(Compile1, CTEvalBitNeg) {

    char *inpl = "\
        int main()                        \n\
        {                                 \n\
            return (~660753869) / 0;      \n\
        }                                 \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok1", (compileResult >= 0) ? "Ok1" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'-660753870 /"));
}

TEST_F(Compile1, CTEvalLogicalOps) {

    char *inpl = "\
        int main()                              \n\
        {                                       \n\
            return (  100000000 *   (!!7) +     \n\
                      10000000 *    (! 7) +     \n\
                      1000000 *     (! 0) +     \n\
                      100000 *      (5 || 7) +  \n\
                      10000 *       (7 || 0) +  \n\
                      1000 *        (0 || 7) +  \n\
                      100 *         (5 && 7) +  \n\
                      10 *          (7 && 0) +  \n\
                                    (0 && 7) ) / 0;   \n\
        }                                       \n\
        ";

    int compileResult = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok1", (compileResult >= 0) ? "Ok1" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'101577700 /"));
}

TEST_F(Compile1, EnumConstantExpressions)
{
    // Enum values to be evaluated at compile time

    char *inpl = "\
        enum Bytes              \n\
        {                       \n\
            zero = 1 << 0,      \n\
            one = 1 << 1,       \n\
            two = 1 << 2,       \n\
        };                      \n\
                                \n\
        int main() {            \n\
            int i = two / 0;    \n\
        }                       \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'4 /"));
}

TEST_F(Compile1, IncrementReadonly)
{
    // No incrementing readonly vars

    char *inpl = "\
        readonly int I;         \n\
                                \n\
        int main() {            \n\
            return ++I;         \n\
        }                       \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("eadonly"));
}

TEST_F(Compile1, SpuriousExpression)
{
    char *inpl = "\
        int main() {            \n\
            77;                 \n\
        }                       \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, CompileTimeConstant1)
{
    char *inpl = "\
        const int CI = 4711;                    \n\
        const float Euler = 2.718281828459045;  \n\
        const float AroundOne = Euler / Euler;  \n\
        float Array[CI];                        \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, CompileTimeConstant2)
{
    char *inpl = "\
        int main() {                            \n\
            while (1)                           \n\
            {                                   \n\
                const int CI2 = 4712;           \n\
            }                                   \n\
            float CI2;                          \n\
        }                                       \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, CompileTimeConstant3)
{
    char *inpl = "\
        struct Str                          \n\
        {                                   \n\
            const int foo = 17;             \n\
            static const int foo_squared =  \n\
                Str.foo * Str.foo;          \n\
        } s;                                \n\
                                            \n\
        int main() {                        \n\
            return s.foo;                   \n\
        }                                   \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STREQ("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
}

TEST_F(Compile1, CompileTimeConstant4)
{
    char *inpl = "\
        import const int C = 42; \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("import"));

    char *inpl2 = "\
        readonly const int C = 42; \n\
        ";
    compile_result = cc_compile(inpl2, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("readonly"));
}

TEST_F(Compile1, CompileTimeConstant5)
{
    char *inpl = "\
        const short S = 42; \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'int'"));

    char *inpl2 = "\
        const int C[]; \n\
        ";
    compile_result = cc_compile(inpl2, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("rray"));

    char *inpl3 = "\
        const int[] C; \n\
        ";
    compile_result = cc_compile(inpl3, scrip);
    msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("Expected '('"));
}

TEST_F(Compile1, CompileTimeConstant6)
{
    char *inpl = "\
            const float pi = 3.14;  \n\
        int main() {                \n\
            float pi = 3.141;       \n\
        }                           \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("in use"));
}

TEST_F(Compile1, StaticThisExtender)
{
    // A 'static' extender function cannot have a 'this'.
    // This declaration should be written
    //     "import int foo (static Struct);"

    char *inpl = "\
        managed struct Struct                   \n\
        {                                       \n\
            int Payload;                        \n\
        };                                      \n\
        import static int Foo(this Struct *);   \n\
        ";
    int compile_result = cc_compile(inpl, scrip);
    std::string msg = last_seen_cc_error();
    ASSERT_STRNE("Ok", (compile_result >= 0) ? "Ok" : msg.c_str());
    EXPECT_NE(std::string::npos, msg.find("'static'"));
}
