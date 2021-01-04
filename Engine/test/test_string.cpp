//=============================================================================
//
// Adventure Game Studio (AGS)
//
// Copyright (C) 1999-2011 Chris Jones and 2011-20xx others
// The full list of copyright holders can be found in the Copyright.txt
// file, which is part of this source code distribution.
//
// The AGS source code is provided under the Artistic License 2.0.
// A copy of this license can be found in the file License.txt and at
// http://www.opensource.org/licenses/artistic-license-2.0.php
//
//=============================================================================

#include "core/platform.h"
#if defined(AGS_RUN_TESTS) && defined(_DEBUG)

#include "gtest/gtest.h"
#include <string.h>
#include <vector>
#include "util/path.h"
#include "util/string.h"

using namespace AGS::Common;

TEST(String, FilePath)
{
    ASSERT_TRUE(Path::IsSameOrSubDir(".", "dir1/") == true);
    ASSERT_TRUE(Path::IsSameOrSubDir(".", "dir1/dir2/dir3/") == true);
    ASSERT_TRUE(Path::IsSameOrSubDir(".", "dir1/../") == true);
    ASSERT_TRUE(Path::IsSameOrSubDir(".", "dir1/dir2/../../") == true);
    ASSERT_TRUE(Path::IsSameOrSubDir(".", "dir1/../dir2/../dir3/") == true);
    ASSERT_TRUE(Path::IsSameOrSubDir(".", "..dir/") == true);

    ASSERT_TRUE(Path::IsSameOrSubDir(".", "../") == false);
    ASSERT_TRUE(Path::IsSameOrSubDir(".", "../") == false);
    ASSERT_TRUE(Path::IsSameOrSubDir(".", "/dir1/") == false);
    ASSERT_TRUE(Path::IsSameOrSubDir(".", "dir1/../../") == false);
    ASSERT_TRUE(Path::IsSameOrSubDir(".", "dir1/../dir2/../../dir3/") == false);
}

// Test string's internal work
TEST(String, Internals)
{
    String s1 = "abcdefghijklmnop";
    String s2 = s1;
    String s3 = s1;
    ASSERT_TRUE(s1.GetRefCount() == 3);
    ASSERT_TRUE(s1.GetBuffer() == s2.GetBuffer());
    ASSERT_TRUE(s2.GetBuffer() == s3.GetBuffer());

    int cap1 = s1.GetCapacity();
    ASSERT_TRUE(cap1 == s1.GetLength());

    s2.TruncateToLeft(10);
    ASSERT_TRUE(cap1 == s2.GetCapacity());
    s3.TruncateToRight(10);
    ASSERT_TRUE(cap1 == s3.GetCapacity());
    ASSERT_TRUE(s1.GetRefCount() == 1);

    s2.AppendChar('z');
    ASSERT_TRUE(cap1 == s2.GetCapacity());

    s3.Append("1234");
    ASSERT_TRUE(cap1 == s3.GetCapacity());
    s3.Append("1234567890123");
    ASSERT_TRUE(27 == s3.GetCapacity());
    s3.Append("1234567890123");
    ASSERT_TRUE(40 == s3.GetCapacity());
    s3.Append("1234567890123");
    ASSERT_TRUE(60 == s3.GetCapacity());

    String s4 = "12345678901234567890";
    const char *cstr = s4.GetCStr();
    s4.ClipLeft(10);
    ASSERT_TRUE(s4.GetCStr() == cstr + 10);
    s4.Prepend("12345");
    ASSERT_TRUE(s4.GetCStr() == cstr + 5);
    s4.Append("12345");
    ASSERT_TRUE(s4.GetCStr() == cstr);
    ASSERT_TRUE(strcmp(s4, "12345123456789012345") == 0);
}

// Test Compare
TEST(String, Compare)
{
    String s1 = "abcdabcdabcd";
    String s2 = "abcdbfghijklmn";
    int cmp1 = s1.Compare(s2);
    int cmp2 = s1.CompareLeft("abcd");
    int cmp3 = s1.CompareLeft("abcdxxx");
    int cmp4 = s1.CompareLeft("abcdxxx", 4);
    int cmp5 = s1.CompareMid(s2, 2, 4);
    int cmp6 = s1.CompareMid(s2, 8, 4);
    int cmp7 = s1.CompareMid(s2, 8, 9);
    int cmp8 = s1.CompareLeft("abcdabcdabcdxxxx");
    int cmp9 = s1.CompareMid("ab", 8);
    int cmp10 = s1.CompareMid("ab", 8, 4);
    int cmp11 = s1.CompareRight("abcd");
    int cmp12 = s1.CompareRight("bcdxxx", 3);
    int cmp13 = s1.CompareRight("abc", 4);
    int cmp14 = s1.CompareRight("abcdxxxx");
    ASSERT_TRUE(cmp1 < 0);
    ASSERT_TRUE(cmp2 == 0);
    ASSERT_TRUE(cmp3 < 0);
    ASSERT_TRUE(cmp4 == 0);
    ASSERT_TRUE(cmp5 > 0);
    ASSERT_TRUE(cmp6 == 0);
    ASSERT_TRUE(cmp7 < 0);
    ASSERT_TRUE(cmp8 < 0);
    ASSERT_TRUE(cmp9 == 0);
    ASSERT_TRUE(cmp10 > 0);
    ASSERT_TRUE(cmp11 == 0);
    ASSERT_TRUE(cmp12 == 0);
    ASSERT_TRUE(cmp13 > 0);
    ASSERT_TRUE(cmp14 < 0);
}

// Test FindChar
TEST(String, FindChar)
{
    String s1 = "findsomethinginhere";
    String s2 = "stringtofindsomethinginside";
    String s3 = "findsomethinginherex";
    String s4 = "xstringtofindsomethinginside";
    String s5;
    size_t find1 = s1.FindChar('o');
    size_t find2 = s2.FindCharReverse('o');
    size_t find3 = s1.FindChar('x');
    size_t find4 = s2.FindCharReverse('x');
    size_t find5 = s3.FindChar('x');
    size_t find6 = s4.FindCharReverse('x');
    size_t find7 = s5.FindChar('x');
    size_t find8 = s5.FindCharReverse('x');
    size_t find9 = s1.FindChar('i', 2);
    size_t find10 = s1.FindCharReverse('i', 12);
    ASSERT_TRUE(find1 == 5);
    ASSERT_TRUE(find2 == 13);
    ASSERT_TRUE(find3 == -1);
    ASSERT_TRUE(find4 == -1);
    ASSERT_TRUE(find5 == 19);
    ASSERT_TRUE(find6 == 0);
    ASSERT_TRUE(find7 == -1);
    ASSERT_TRUE(find8 == -1);
    ASSERT_TRUE(find9 == 10);
    ASSERT_TRUE(find10 == 10);
}

// Test GetAt
TEST(String, GetAt)
{
    String s1 = "abcdefghijklmnop";
    String s2;
    char c1 = s1.GetAt(0);
    char c2 = s1.GetAt(15);
    char c3 = s1.GetAt(-10);
    char c4 = s1.GetAt(16);
    char c5 = s2.GetAt(0);
    ASSERT_TRUE(c1 == 'a');
    ASSERT_TRUE(c2 == 'p');
    ASSERT_TRUE(c3 == 0);
    ASSERT_TRUE(c4 == 0);
    ASSERT_TRUE(c5 == 0);
}

// Test ToInt
TEST(String, ToInt)
{
    String s1;
    String s2 = "100";
    String s3 = "202aaa";
    String s4 = "aaa333";
    int i1 = s1.ToInt();
    int i2 = s2.ToInt();
    int i3 = s3.ToInt();
    int i4 = s4.ToInt();
    ASSERT_TRUE(i1 == 0);
    ASSERT_TRUE(i2 == 100);
    ASSERT_TRUE(i3 == 202);
    ASSERT_TRUE(i4 == 0);
}

// Test Left/Right/Mid
TEST(String, LeftRightMid)
{
    String s1 = "this is a string to be split";
    String s2 = s1.Left(4);
    String s3 = s1.Left(100);
    String s4 = s1.Mid(10);
    String s5 = s1.Mid(10, 6);
    String s6 = s1.Mid(0, 200);
    String s7 = s1.Right(5);
    String s8 = s1.Right(100);
    String s9 = s1.Left(0);
    String s10 = s1.Mid(-1, 0);
    String s11 = s1.Right(0);

    ASSERT_TRUE(strcmp(s2, "this") == 0);
    ASSERT_TRUE(strcmp(s3, "this is a string to be split") == 0);
    ASSERT_TRUE(strcmp(s4, "string to be split") == 0);
    ASSERT_TRUE(strcmp(s5, "string") == 0);
    ASSERT_TRUE(strcmp(s6, "this is a string to be split") == 0);
    ASSERT_TRUE(strcmp(s7, "split") == 0);
    ASSERT_TRUE(strcmp(s8, "this is a string to be split") == 0);
    ASSERT_TRUE(strcmp(s9, "") == 0);
    ASSERT_TRUE(strcmp(s10, "") == 0);
    ASSERT_TRUE(strcmp(s11, "") == 0);
}

// Test Section
TEST(String, Section)
{
    String s = "_123_567_";
    size_t from;
    size_t to;
    ASSERT_TRUE(s.FindSection('_', 0, 0, true, true, from, to));
    ASSERT_TRUE(from == 0 && to == 0);
    ASSERT_TRUE(s.FindSection('_', 0, 0, false, true, from, to));
    ASSERT_TRUE(from == 0 && to == 0);
    ASSERT_TRUE(s.FindSection('_', 0, 0, true, false, from, to));
    ASSERT_TRUE(from == 0 && to == 1);
    ASSERT_TRUE(s.FindSection('_', 0, 0, false, false, from, to));
    ASSERT_TRUE(from == 0 && to == 1);
    ASSERT_TRUE(s.FindSection('_', 3, 3, true, true, from, to));
    ASSERT_TRUE(from == 9 && to == 9);
    ASSERT_TRUE(s.FindSection('_', 3, 3, false, true, from, to));
    ASSERT_TRUE(from == 8 && to == 9);
    ASSERT_TRUE(s.FindSection('_', 3, 3, true, false, from, to));
    ASSERT_TRUE(from == 9 && to == 9);
    ASSERT_TRUE(s.FindSection('_', 3, 3, false, false, from, to));
    ASSERT_TRUE(from == 8 && to == 9);
    ASSERT_TRUE(s.FindSection('_', 1, 1, true, true, from, to));
    ASSERT_TRUE(from == 1 && to == 4);
    ASSERT_TRUE(s.FindSection('_', 1, 1, false, true, from, to));
    ASSERT_TRUE(from == 0 && to == 4);
    ASSERT_TRUE(s.FindSection('_', 1, 1, true, false, from, to));
    ASSERT_TRUE(from == 1 && to == 5);
    ASSERT_TRUE(s.FindSection('_', 1, 1, false, false, from, to));
    ASSERT_TRUE(from == 0 && to == 5);
}

// Test Append
TEST(String, Append)
{
    String s1 = "a string to enlarge - ";
    s1.Append("make it bigger");
    ASSERT_TRUE(strcmp(s1, "a string to enlarge - make it bigger") == 0);
    s1.AppendChar('!');
    ASSERT_TRUE(strcmp(s1, "a string to enlarge - make it bigger!") == 0);
    s1.AppendChar(' ');
    ASSERT_TRUE(strcmp(s1, "a string to enlarge - make it bigger! ") == 0);
    s1.Append("much much bigger!");
    ASSERT_TRUE(strcmp(s1, "a string to enlarge - make it bigger! much much bigger!") == 0);
}

// Test Clip
TEST(String, Clip)
{
    String str1 = "long truncateable string";
    String str2 = str1;
    String str3 = str1;
    String str4 = str1;
    String str5 = str1;

    str1.ClipLeft(4);
    str2.ClipRight(6);
    str3.ClipMid(5, 12);
    str4.ClipMid(5, 0);
    str5.ClipMid(0);
    ASSERT_TRUE(strcmp(str1, " truncateable string") == 0);
    ASSERT_TRUE(strcmp(str2, "long truncateable ") == 0);
    ASSERT_TRUE(strcmp(str3, "long  string") == 0);
    ASSERT_TRUE(strcmp(str4, "long truncateable string") == 0);
    ASSERT_TRUE(strcmp(str5, "") == 0);
}

// Test ClipSection
TEST(String, ClipSection)
{
    String str1 = "C:\\Games\\AGS\\MyNewGame";
    String str2 = str1;
    String str3 = str1;
    String str4 = str1;
    String str5 = str1;
    String str6 = str1;
    String str7 = str1;
    String str8 = str1;
    String str9 = str1;
    String str10 = str1;
    String str11 = str1;

    str1.ClipLeftSection('\\');
    str2.ClipLeftSection('\\', false);
    str3.ClipRightSection('\\');
    str4.ClipRightSection('\\', false);
    str5.ClipSection('\\', 1, 2);
    str6.ClipSection('\\', 1, 2, false, false);
    str7.ClipSection('|', 1, 2);
    str8.ClipSection('\\', 0, 2);
    str9.ClipSection('\\', 1, 3);
    str10.ClipSection('\\', 3, 1);
    str11.ClipSection('\\', 0, 4);
    ASSERT_TRUE(strcmp(str1, "Games\\AGS\\MyNewGame") == 0);
    ASSERT_TRUE(strcmp(str2, "\\Games\\AGS\\MyNewGame") == 0);
    ASSERT_TRUE(strcmp(str3, "C:\\Games\\AGS") == 0);
    ASSERT_TRUE(strcmp(str4, "C:\\Games\\AGS\\") == 0);
    ASSERT_TRUE(strcmp(str5, "C:MyNewGame") == 0);
    ASSERT_TRUE(strcmp(str6, "C:\\\\MyNewGame") == 0);
    ASSERT_TRUE(strcmp(str7, "C:\\Games\\AGS\\MyNewGame") == 0);
    ASSERT_TRUE(strcmp(str8, "MyNewGame") == 0);
    ASSERT_TRUE(strcmp(str9, "C:") == 0);
    ASSERT_TRUE(strcmp(str10, "C:\\Games\\AGS\\MyNewGame") == 0);
    ASSERT_TRUE(strcmp(str11, "") == 0);
}

// Test making new string
TEST(String, NewString)
{
    String s1 = "we have some string here";
    ASSERT_TRUE(strcmp(s1, "we have some string here") == 0);
    s1.Empty();
    ASSERT_TRUE(strcmp(s1, "") == 0);
    s1.FillString('z', 10);
    ASSERT_TRUE(strcmp(s1, "zzzzzzzzzz") == 0);
    s1.FillString('a', 0);
    ASSERT_TRUE(strcmp(s1, "") == 0);
    s1.Format("this %d is %9ld a %x formatted %0.2f string %s", 1,2,100,22.55F,"abcd");
    ASSERT_TRUE(strcmp(s1, "this 1 is         2 a 64 formatted 22.55 string abcd") == 0);
    s1.SetString("some string");
    ASSERT_TRUE(strcmp(s1, "some string") == 0);
    s1.SetString("some string", 4);
    ASSERT_TRUE(strcmp(s1, "some") == 0);
}

// Test Upper/Lower case
TEST(String, UpperLowerCase)
{
    String s1 = "ThIs StRiNg Is TwIsTeD";
    String s2 = s1;
    String s3 = s1;
    s2.MakeLower();
    s3.MakeUpper();
    ASSERT_TRUE(strcmp(s2, "this string is twisted") == 0);
    ASSERT_TRUE(strcmp(s3, "THIS STRING IS TWISTED") == 0);
}

// Test Prepend
TEST(String, Prepend)
{
    String s1 = "- a string to enlarge";
    s1.Prepend("make it bigger ");
    ASSERT_TRUE(strcmp(s1, "make it bigger - a string to enlarge") == 0);
    s1.PrependChar('!');
    ASSERT_TRUE(strcmp(s1, "!make it bigger - a string to enlarge") == 0);
    s1.PrependChar(' ');
    ASSERT_TRUE(strcmp(s1, " !make it bigger - a string to enlarge") == 0);
    s1.Prepend("much much bigger!");
    ASSERT_TRUE(strcmp(s1, "much much bigger! !make it bigger - a string to enlarge") == 0);
}

// Test ReplaceMid
TEST(String, ReplaceMid)
{
    String s1 = "we need to replace PRECISELY THIS PART in this string";
    String s2 = s1;
    String new_long = "WITH A NEW TAD LONGER SUBSTRING";
    String new_short = "SMALL STRING";
    s1.ReplaceMid(19, 19, new_long);
    ASSERT_TRUE(strcmp(s1, "we need to replace WITH A NEW TAD LONGER SUBSTRING in this string") == 0);
    s2.ReplaceMid(19, 19, new_short);
    ASSERT_TRUE(strcmp(s2, "we need to replace SMALL STRING in this string") == 0);
    String s3 = "insert new string here: ";
    s3.ReplaceMid(s3.GetLength(), 0, "NEW STRING");
    ASSERT_TRUE(strcmp(s3, "insert new string here: NEW STRING") == 0);
}

// Test SetAt
TEST(String, SetAt)
{
    String s1 = "strimg wiyh typos";
    s1.SetAt(-1, 'a');
    ASSERT_TRUE(strcmp(s1, "strimg wiyh typos") == 0);
    s1.SetAt(100, 'a');
    ASSERT_TRUE(strcmp(s1, "strimg wiyh typos") == 0);
    s1.SetAt(1, 0);
    ASSERT_TRUE(strcmp(s1, "strimg wiyh typos") == 0);
    s1.SetAt(4, 'n');
    s1.SetAt(9, 't');
    ASSERT_TRUE(strcmp(s1, "string with typos") == 0);
}

// Test Trim
TEST(String, Trim)
{
    String str1 = "\t   This string is quite long and should be cut a little bit\r\n    ";
    String str2 = str1;
    String str3 = str1;
    String str4 = str1;
    String str5 = "There's nothing to trim here";

    str1.TrimLeft();
    str2.TrimRight();
    str3.Trim();
    str4.Trim('|');
    str5.Trim();

    ASSERT_TRUE(strcmp(str1, "This string is quite long and should be cut a little bit\r\n    ") == 0);
    ASSERT_TRUE(strcmp(str2, "\t   This string is quite long and should be cut a little bit") == 0);
    ASSERT_TRUE(strcmp(str3, "This string is quite long and should be cut a little bit") == 0);
    ASSERT_TRUE(strcmp(str4, "\t   This string is quite long and should be cut a little bit\r\n    ") == 0);
    ASSERT_TRUE(strcmp(str5, "There's nothing to trim here") == 0);
}

// Test Truncate
TEST(String, Truncate)
{
    String str1 = "long truncateable string";
    String str2 = str1;
    String str3 = str1;
    String str4 = str1;
    String str5 = str1;

    str1.TruncateToLeft(4);
    str2.TruncateToRight(6);
    str3.TruncateToMid(5, 12);
    str4.TruncateToMid(5, 0);
    str5.TruncateToMid(0);
    ASSERT_TRUE(strcmp(str1, "long") == 0);
    ASSERT_TRUE(strcmp(str2, "string") == 0);
    ASSERT_TRUE(strcmp(str3, "truncateable") == 0);
    ASSERT_TRUE(strcmp(str4, "") == 0);
    ASSERT_TRUE(strcmp(str5, "long truncateable string") == 0);
}

// Test TruncateToSection
TEST(String, TruncateToSection)
{
    String str1 = "C:\\Games\\AGS\\MyNewGame";
    String str2 = str1;
    String str3 = str1;
    String str4 = str1;
    String str5 = str1;
    String str6 = str1;
    String str7 = str1;
    String str8 = str1;
    String str9 = str1;
    String str10 = str1;
    String str11 = str1;
    String str12 = str1;

    str1.TruncateToLeftSection('\\');
    str2.TruncateToLeftSection('\\', false);
    str3.TruncateToRightSection('\\');
    str4.TruncateToRightSection('\\', false);
    str5.TruncateToSection('\\', 1, 2);
    str6.TruncateToSection('\\', 1, 2, false, false);
    str7.TruncateToSection('|', 1, 3);
    str8.TruncateToSection('\\', 0, 2);
    str9.TruncateToSection('\\', 1, 3);
    str10.TruncateToSection('\\', 3, 1);
    str11.TruncateToSection('\\', 3, 3);
    str12.TruncateToSection('\\', 3, 3, false, false);
    ASSERT_TRUE(strcmp(str1, "C:") == 0);
    ASSERT_TRUE(strcmp(str2, "C:\\") == 0);
    ASSERT_TRUE(strcmp(str3, "MyNewGame") == 0);
    ASSERT_TRUE(strcmp(str4, "\\MyNewGame") == 0);
    ASSERT_TRUE(strcmp(str5, "Games\\AGS") == 0);
    ASSERT_TRUE(strcmp(str6, "\\Games\\AGS\\") == 0);
    ASSERT_TRUE(strcmp(str7, "") == 0);
    ASSERT_TRUE(strcmp(str8, "C:\\Games\\AGS") == 0);
    ASSERT_TRUE(strcmp(str9, "Games\\AGS\\MyNewGame") == 0);
    ASSERT_TRUE(strcmp(str10, "") == 0);
    ASSERT_TRUE(strcmp(str11, "MyNewGame") == 0);
    ASSERT_TRUE(strcmp(str12, "\\MyNewGame") == 0);
}

// Test Split
TEST(String, Split)
{
    String str1 = "C:\\Games\\AGS\\MyNewGame\\";
    std::vector<String> result = str1.Split('\\');
    ASSERT_TRUE(result.size() == 5);
    ASSERT_TRUE(strcmp(result[0], "C:") == 0);
    ASSERT_TRUE(strcmp(result[1], "Games") == 0);
    ASSERT_TRUE(strcmp(result[2], "AGS") == 0);
    ASSERT_TRUE(strcmp(result[3], "MyNewGame") == 0);
    ASSERT_TRUE(strcmp(result[4], "") == 0);
    String str2 = "test,,,test";
    result = str2.Split(',');
    ASSERT_TRUE(result.size() == 4);
    ASSERT_TRUE(strcmp(result[0], "test") == 0);
    ASSERT_TRUE(strcmp(result[1], "") == 0);
    ASSERT_TRUE(strcmp(result[2], "") == 0);
    ASSERT_TRUE(strcmp(result[3], "test") == 0);
    String str3 = ",,test,,";
    result = str3.Split(',');
    ASSERT_TRUE(result.size() == 5);
    ASSERT_TRUE(strcmp(result[0], "") == 0);
    ASSERT_TRUE(strcmp(result[1], "") == 0);
    ASSERT_TRUE(strcmp(result[2], "test") == 0);
    ASSERT_TRUE(strcmp(result[3], "") == 0);
    ASSERT_TRUE(strcmp(result[4], "") == 0);
}

// Test Wrap
TEST(String, Wrap)
{
    const char *cstr = "This is a string literal";
    String str1 = String::Wrapper(cstr);
    String str2 = str1;
    ASSERT_TRUE(str1.GetCStr() == cstr);
    ASSERT_TRUE(str2.GetCStr() == cstr);
    ASSERT_TRUE(str1.GetRefCount() == 0);
    ASSERT_TRUE(str2.GetRefCount() == 0);
    str2.SetAt(0, 'A');
    ASSERT_TRUE(str2.GetCStr() != cstr);
    ASSERT_TRUE(str2.GetRefCount() == 1);
}

#endif // AGS_RUN_TESTS
