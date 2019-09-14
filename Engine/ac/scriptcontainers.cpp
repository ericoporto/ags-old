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
// Containers script API.
//
//=============================================================================
#include "ac/common.h" // quit
#include "ac/string.h"
#include "ac/dynobj/cc_dynamicarray.h"
#include "ac/dynobj/cc_dynamicobject.h"
#include "ac/dynobj/scriptdict.h"
#include "ac/dynobj/scriptset.h"
#include "ac/dynobj/scriptstring.h"
#include "script/script_api.h"
#include "script/script_runtime.h"
#include "util/bbop.h"

extern ScriptString myScriptStringImpl;

//=============================================================================
//
// Dictionary of strings script API.
//
//=============================================================================

ScriptDictBase *Dict_CreateImpl(bool sorted, bool case_sensitive)
{
    ScriptDictBase *dic;
    if (sorted)
    {
        if (case_sensitive)
            dic = new ScriptDict();
        else
            dic = new ScriptDictCI();
    }
    else
    {
        if (case_sensitive)
            dic = new ScriptHashDict();
        else
            dic = new ScriptHashDictCI();
    }
    return dic;
}

ScriptDictBase *Dict_Create(bool sorted, bool case_sensitive)
{
    ScriptDictBase *dic = Dict_CreateImpl(sorted, case_sensitive);
    ccRegisterManagedObject(dic, dic);
    return dic;
}

// TODO: we need memory streams
ScriptDictBase *Dict_Unserialize(int index, const char *serializedData, int dataSize)
{
    if (dataSize < sizeof(int32_t) * 2)
        quit("Dict_Unserialize: not enough data.");
    const char *ptr = serializedData;
    const int sorted = BBOp::Int32FromLE(*((int*)ptr)); ptr += sizeof(int32_t);
    const int cs = BBOp::Int32FromLE(*((int*)ptr)); ptr += sizeof(int32_t);
    ScriptDictBase *dic = Dict_CreateImpl(sorted != 0, cs != 0);
    dic->Unserialize(index, ptr, dataSize -= sizeof(int32_t) * 2);
    return dic;
}

void Dict_Clear(ScriptDictBase *dic)
{
    dic->Clear();
}

bool Dict_Contains(ScriptDictBase *dic, const char *key)
{
    return dic->Contains(key);
}

const char *Dict_Get(ScriptDictBase *dic, const char *key)
{
    auto *str = dic->Get(key);
    return str ? CreateNewScriptString(str) : nullptr;
}

bool Dict_Remove(ScriptDictBase *dic, const char *key)
{
    return dic->Remove(key);
}

bool Dict_Set(ScriptDictBase *dic, const char *key, const char *value)
{
    return dic->Set(key, value);
}

int Dict_GetCompareStyle(ScriptDictBase *dic)
{
    return dic->IsCaseSensitive() ? 1 : 0;
}

int Dict_GetSortStyle(ScriptDictBase *dic)
{
    return dic->IsSorted() ? 1 : 0;
}

int Dict_GetItemCount(ScriptDictBase *dic)
{
    return dic->GetItemCount();
}

void *Dict_GetKeysAsArray(ScriptDictBase *dic)
{
    std::vector<const char*> items;
    dic->GetKeys(items);
    if (items.size() == 0)
        return nullptr;
    DynObjectRef arr = DynamicArrayHelpers::CreateStringArray(items);
    return arr.second;
}

void *Dict_GetValuesAsArray(ScriptDictBase *dic)
{
    std::vector<const char*> items;
    dic->GetValues(items);
    if (items.size() == 0)
        return nullptr;
    DynObjectRef arr = DynamicArrayHelpers::CreateStringArray(items);
    return arr.second;
}


//=============================================================================
//
// Set of strings script API.
//
//=============================================================================

ScriptSetBase *Set_CreateImpl(bool sorted, bool case_sensitive)
{
    ScriptSetBase *set;
    if (sorted)
    {
        if (case_sensitive)
            set = new ScriptSet();
        else
            set = new ScriptSetCI();
    }
    else
    {
        if (case_sensitive)
            set = new ScriptHashSet();
        else
            set = new ScriptHashSetCI();
    }
    return set;
}

ScriptSetBase *Set_Create(bool sorted, bool case_sensitive)
{
    ScriptSetBase *set = Set_CreateImpl(sorted, case_sensitive);
    ccRegisterManagedObject(set, set);
    return set;
}

// TODO: we need memory streams
ScriptSetBase *Set_Unserialize(int index, const char *serializedData, int dataSize)
{
    if (dataSize < sizeof(int32_t) * 2)
        quit("Set_Unserialize: not enough data.");
    const char *ptr = serializedData;
    const int sorted = BBOp::Int32FromLE(*((int*)ptr)); ptr += sizeof(int32_t);
    const int cs = BBOp::Int32FromLE(*((int*)ptr)); ptr += sizeof(int32_t);
    ScriptSetBase *set = Set_CreateImpl(sorted != 0, cs != 0);
    set->Unserialize(index, ptr, dataSize -= sizeof(int32_t) * 2);
    return set;
}

bool Set_Add(ScriptSetBase *set, const char *item)
{
    return set->Add(item);
}

void Set_Clear(ScriptSetBase *set)
{
    set->Clear();
}

bool Set_Contains(ScriptSetBase *set, const char *item)
{
    return set->Contains(item);
}

bool Set_Remove(ScriptSetBase *set, const char *item)
{
    return set->Remove(item);
}

int Set_GetCompareStyle(ScriptSetBase *set)
{
    return set->IsCaseSensitive() ? 1 : 0;
}

int Set_GetSortStyle(ScriptSetBase *set)
{
    return set->IsSorted() ? 1 : 0;
}

int Set_GetItemCount(ScriptSetBase *set)
{
    return set->GetItemCount();
}

void *Set_GetItemsAsArray(ScriptSetBase *set)
{
    std::vector<const char*> items;
    set->GetItems(items);
    if (items.size() == 0)
        return nullptr;
    DynObjectRef arr = DynamicArrayHelpers::CreateStringArray(items);
    return arr.second;
}





/// GENERATED CODE

RuntimeScriptValue Sc_Dict_Create(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_OBJAUTO_PBOOL2(ScriptDictBase, Dict_Create);
}

RuntimeScriptValue Sc_Dict_Clear(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_VOID(ScriptDictBase, Dict_Clear);
}

RuntimeScriptValue Sc_Dict_Contains(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_BOOL_POBJ(ScriptDictBase, Dict_Contains, const char);
}

RuntimeScriptValue Sc_Dict_Get(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_OBJ_POBJ(ScriptDictBase, const char, myScriptStringImpl, Dict_Get, const char);
}

RuntimeScriptValue Sc_Dict_Remove(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_BOOL_POBJ(ScriptDictBase, Dict_Remove, const char);
}

RuntimeScriptValue Sc_Dict_Set(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_BOOL_POBJ2(ScriptDictBase, Dict_Set, const char, const char);
}

RuntimeScriptValue Sc_Dict_GetCompareStyle(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_INT(ScriptDictBase, Dict_GetCompareStyle);
}

RuntimeScriptValue Sc_Dict_GetSortStyle(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_INT(ScriptDictBase, Dict_GetSortStyle);
}

RuntimeScriptValue Sc_Dict_GetItemCount(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_INT(ScriptDictBase, Dict_GetItemCount);
}

RuntimeScriptValue Sc_Dict_GetKeysAsArray(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_OBJ(ScriptDictBase, void, globalDynamicArray, Dict_GetKeysAsArray);
}

RuntimeScriptValue Sc_Dict_GetValuesAsArray(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_OBJ(ScriptDictBase, void, globalDynamicArray, Dict_GetValuesAsArray);
}



RuntimeScriptValue Sc_Set_Create(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_OBJAUTO_PBOOL2(ScriptSetBase, Set_Create);
}

RuntimeScriptValue Sc_Set_Add(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_BOOL_POBJ(ScriptSetBase, Set_Add, const char);
}

RuntimeScriptValue Sc_Set_Clear(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_VOID(ScriptSetBase, Set_Clear);
}

RuntimeScriptValue Sc_Set_Contains(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_BOOL_POBJ(ScriptSetBase, Set_Contains, const char);
}

RuntimeScriptValue Sc_Set_Remove(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_BOOL_POBJ(ScriptSetBase, Set_Remove, const char);
}

RuntimeScriptValue Sc_Set_GetCompareStyle(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_INT(ScriptSetBase, Set_GetCompareStyle);
}

RuntimeScriptValue Sc_Set_GetSortStyle(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_INT(ScriptSetBase, Set_GetSortStyle);
}

RuntimeScriptValue Sc_Set_GetItemCount(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_INT(ScriptSetBase, Set_GetItemCount);
}

RuntimeScriptValue Sc_Set_GetItemAsArray(void *self, const RuntimeScriptValue *params, int32_t param_count)
{
    API_OBJCALL_OBJ(ScriptSetBase, void, globalDynamicArray, Set_GetItemsAsArray);
}



void RegisterContainerAPI()
{
    ccAddExternalStaticFunction("Dictionary::Create", Sc_Dict_Create);
    ccAddExternalObjectFunction("Dictionary::Clear", Sc_Dict_Clear);
    ccAddExternalObjectFunction("Dictionary::Contains", Sc_Dict_Contains);
    ccAddExternalObjectFunction("Dictionary::Get", Sc_Dict_Get);
    ccAddExternalObjectFunction("Dictionary::Remove", Sc_Dict_Remove);
    ccAddExternalObjectFunction("Dictionary::Set", Sc_Dict_Set);
    ccAddExternalObjectFunction("Dictionary::get_CompareStyle", Sc_Dict_GetCompareStyle);
    ccAddExternalObjectFunction("Dictionary::get_SortStyle", Sc_Dict_GetSortStyle);
    ccAddExternalObjectFunction("Dictionary::get_ItemCount", Sc_Dict_GetItemCount);
    ccAddExternalObjectFunction("Dictionary::GetKeysAsArray", Sc_Dict_GetKeysAsArray);
    ccAddExternalObjectFunction("Dictionary::GetValuesAsArray", Sc_Dict_GetValuesAsArray);

    ccAddExternalStaticFunction("Set::Create", Sc_Set_Create);
    ccAddExternalObjectFunction("Set::Add", Sc_Set_Add);
    ccAddExternalObjectFunction("Set::Clear", Sc_Set_Clear);
    ccAddExternalObjectFunction("Set::Contains", Sc_Set_Contains);
    ccAddExternalObjectFunction("Set::Remove", Sc_Set_Remove);
    ccAddExternalObjectFunction("Set::get_CompareStyle", Sc_Set_GetCompareStyle);
    ccAddExternalObjectFunction("Set::get_SortStyle", Sc_Set_GetSortStyle);
    ccAddExternalObjectFunction("Set::get_ItemCount", Sc_Set_GetItemCount);
    ccAddExternalObjectFunction("Set::GetItemsAsArray", Sc_Set_GetItemAsArray);
}
