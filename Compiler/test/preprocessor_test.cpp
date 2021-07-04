#include "gtest/gtest.h"
#include "preproc/preprocessor.h"
#include <util/string_compat.h>

typedef AGS::Common::String AGSString;

extern int currentline; // in script/script_common

namespace AGS {
namespace Preprocessor {


std::string last_cc_error_buf;
void clear_error()
{
    last_cc_error_buf.clear();
}

const char *last_seen_cc_error()
{
    return last_cc_error_buf.c_str();
}

// IMPORTANT: the last_seen_cc_error must contain unformatted error message.
// It is being used in test and compared to hard-coded strings.
std::pair<AGSString, AGSString> cc_error_at_line(const char *error_msg)
{
    // printf("error: %s\n", error_msg);
    last_cc_error_buf = ags_strdup(error_msg);
    return std::make_pair(AGSString::FromFormat("Error (line %d): %s", currentline, error_msg), AGSString());
}

AGSString cc_error_without_line(const char *error_msg)
{
    last_cc_error_buf = ags_strdup(error_msg);
    return AGSString::FromFormat("Error (line unknown): %s", error_msg);
}

TEST(Preprocess, Comments) {
    Preprocessor pp = Preprocessor();
    const char *inpl = ""
        "// this is a comment \r\n"
        "// this is another comment \r\n"
        "// 1234 \r\n"
        "//#define invalid 5\r\n"
        "// invalid \r\n"
        "int i;";

    clear_error();
    String res = pp.Preprocess(inpl,"ScriptName");

    ASSERT_TRUE(last_cc_error_buf.empty());

    std::vector<AGSString> lines = res.Split('\n');
    ASSERT_EQ(lines.size(),8);
    ASSERT_STREQ(lines[0].GetCStr(),"\"__NEWSCRIPTSTART_ScriptName\"");
    ASSERT_STREQ(lines[1].GetCStr(),"");
    ASSERT_STREQ(lines[2].GetCStr(),"");
    ASSERT_STREQ(lines[3].GetCStr(),"");
    ASSERT_STREQ(lines[4].GetCStr(),"");
    ASSERT_STREQ(lines[5].GetCStr(),"");
    ASSERT_STREQ(lines[6].GetCStr(),"int i;");
    ASSERT_STREQ(lines[7].GetCStr(),"");
}

} // Preprocessor
} // AGS