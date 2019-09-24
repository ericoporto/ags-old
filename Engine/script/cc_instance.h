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
//
// 'C'-style script interpreter
//
//=============================================================================

#ifndef __CC_INSTANCE_H
#define __CC_INSTANCE_H

#include <memory>
#include <unordered_map>
#include <stdint.h>

#include "script/script_common.h"
#include "script/cc_script.h"  // ccScript
#include "script/nonblockingscriptfunction.h"
#include "util/string.h"

using namespace AGS;

#define INSTF_SHAREDATA     1
#define INSTF_ABORTED       2
#define INSTF_FREE          4
#define INSTF_RUNNING       8   // set by main code to confirm script isn't stuck
#define CC_STACK_SIZE       250
#define CC_STACK_DATA_SIZE  (1000 * sizeof(int32_t))
#define MAX_CALL_STACK      100

// 256 because we use 8 bits to hold instance number
#define MAX_LOADED_INSTANCES 256

#define INSTANCE_ID_SHIFT 24LL
#define INSTANCE_ID_MASK  0x00000000000000ffLL
#define INSTANCE_ID_REMOVEMASK 0x0000000000ffffffLL

struct ccInstance;
struct ScriptImport;

struct ScriptVariable
{
    ScriptVariable()
    {
        ScAddress   = -1; // address = 0 is valid one, -1 means undefined
    }

    int32_t             ScAddress;  // original 32-bit relative data address, written in compiled script;
                                    // if we are to use Map or HashMap, this could be used as Key
    RuntimeScriptValue  RValue;
};

struct ScriptPosition
{
    ScriptPosition()
        : Line(0)
    {
    }

    ScriptPosition(const Common::String &section, int32_t line)
        : Section(section)
        , Line(line)
    {
    }

    Common::String  Section;
    int32_t         Line;
};

struct ccInstance
{
public:
    virtual ~ccInstance() = 0;
    // TOOD: on delete, remove from executor

    // Create a runnable instance of the same script, sharing global memory
    virtual ccInstance *Fork() = 0;

    virtual void OverrideGlobalData(std::vector<char> data) = 0;
    virtual std::vector<char> GetGlobalData() = 0;

    // Specifies that when the current function returns to the script, it
    // will stop and return from CallInstance
    virtual void    Abort() = 0;
    // Aborts instance, then frees the memory later when it is done with
    virtual void    AbortAndDestroy() = 0;

    // Get the address of an exported symbol (function or variable) in the script
    // NOTE: only used to check for existance?
    virtual RuntimeScriptValue GetSymbolAddress(const char *symname) = 0;
};


struct MemWindow
{
    uint32_t virtualaddr;
    const void *addr; 
    size_t size;
};

struct ccImport2
{
    AGS::Common::String name;
    // int offset;
    uint32_t vaddr;
};

#define MAX_FUNC_PARAMS (20)

struct ccStackFrame
{
    std::vector<uint32_t> callstack {};
    uint32_t thisbase = 0;
    uint32_t funcstart = 0;
    // int was_just_callas = -1;
    int num_args_to_func = -1;
    int next_call_needs_object = 0;
};

struct ccExecutor final
{
    public:

    ccExecutor();

    ccInstance *LoadScript(PScript script);
    void UnloadScript(ccInstance *instance);

    // Add real memory address that we just use as a handle essentialy (we never read from it)
    uint32_t AddRealMemoryAddress(const void *addr);
    void RemoveRealMemoryAddress(const void *addr);

    // Call an exported function in the script
    int CallScriptFunction(ccInstance *sci, const char *funcname, int32_t num_params, const RuntimeScriptValue *params);

    int GetReturnValue();

    void Abort();


    // Tells whether this instance is in the process of executing the byte-code
    bool IsInstanceBeingRun(ccInstance *inst);

    // Is the executor still running?
    bool IsBeingRun();


private:

    // std::vector<ccInstance *> loadedInstances;

    // #define SREG_SP           1     // stack pointer
    // #define SREG_MAR          2     // memory address register
    // #define SREG_AX           3     // general purpose
    // #define SREG_BX           4
    // #define SREG_CX           5
    // #define SREG_OP           6    // object pointer for member func calls
    // #define SREG_DX           7

    int line_number;

    uint32_t registers[8];
    uint32_t pc;
    uint32_t stacktop;
    char *stack;
    std::vector<MemWindow> dumbmemory;
    std::vector<ccStackFrame> stackframes;

    // int CallScriptFunctionDirect(uint32_t vaddr, int32_t num_params, const RuntimeScriptValue *params);
    int CallScriptFunctionDirect(uint32_t vaddr, int32_t num_params, const uint32_t *machparams);

    int Run();
    
    uint32_t CallExternalFunction(const ScriptImport *);
    uint32_t CallExternalScriptFunction(const ScriptImport *);

};

extern ccExecutor coreExecutor;

#endif // __CC_INSTANCE_H
