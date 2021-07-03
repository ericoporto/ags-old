#include <script/cc_error.h>
#include <script/cs_parser_common.h>
#include <script/cc_macrotable.h>

using namespace AGS::Common;

extern int currentline; // in script/script_common

namespace AGS::Preprocessor {

    enum class ErrorCode {
        MacroNameMissing = 1,
        MacroDoesNotExist,
        MacroAlreadyExists,
        MacroNameInvalid,
        LineTooLong,
        UnknownPreprocessorDirective,
        UserDefinedError,
        EndIfWithoutIf,
        IfWithoutEndIf,
    };

    class Preprocessor {
    private:
        bool _inMultiLineComment = false;
        MacroTable _macros = MacroTable();

        static void Error(ErrorCode error, ...) {
            switch (error) {
                case ErrorCode::MacroNameMissing:
                    cc_error("missing macro name");
                    break;
                case ErrorCode::MacroDoesNotExist:
                    cc_error("message");
                    break;
                case ErrorCode::MacroAlreadyExists:
                    cc_error("message");
                    break;
                case ErrorCode::MacroNameInvalid:
                    cc_error("message");
                    break;
                case ErrorCode::LineTooLong:
                    cc_error("message");
                    break;
                case ErrorCode::UnknownPreprocessorDirective:
                    cc_error("message");
                    break;
                case ErrorCode::UserDefinedError:
                    cc_error("!message");
                    break;
                case ErrorCode::EndIfWithoutIf:
                    cc_error("message");
                    break;
                case ErrorCode::IfWithoutEndIf:
                    cc_error("#endif");
                    break;
            }
        }

        String GetNextWord(String &text, bool trimText = true, bool includeDots = false) {
            int i = 0;
            while ((i < text.GetLength()) &&
                   (is_alphanum(text[i]) ||
                    (includeDots && (text[i] == '.')))
                    ) {
                i++;
            }
            String word = text.Left(i);
            text.ClipLeft(i);
            if (trimText) {
                text.Trim();
            }
            return word;
        }

    public:
        void DefineMacro(const String& name, const String& value)
        {
            _macros.add(name.GetCStr(), value.GetCStr());
        }



    };

}