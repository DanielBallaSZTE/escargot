/*
 * Copyright (c) 2016-present Samsung Electronics Co., Ltd
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

#ifndef __EscargotSandBox__
#define __EscargotSandBox__

#include "parser/ast/Node.h"
#include "runtime/Context.h"

namespace Escargot {

class SandBox : public gc {
    friend class ByteCodeInterpreter;
    friend class ErrorObject;

public:
    SandBox(Context* s)
    {
        m_context = s;
        m_context->m_sandBoxStack.pushBack(this);
    }

    ~SandBox()
    {
        ASSERT(m_context->m_sandBoxStack.back() == this);
        m_context->m_sandBoxStack.pop_back();
    }

    struct StackTraceData : public gc {
        String* fileName;
        String* source;
        ExtendedNodeLOC loc;
        StackTraceData()
            : fileName(String::emptyString)
            , source(String::emptyString)
            , loc(SIZE_MAX, SIZE_MAX, SIZE_MAX)
        {
        }
    };

    typedef Vector<std::pair<ExecutionContext*, StackTraceData>, GCUtil::gc_malloc_allocator<std::pair<ExecutionContext*, StackTraceData>>> StackTraceDataVector;

    struct SandBoxResult {
        Value result;
        Value error;
        String* msgStr;
        Vector<StackTraceData, GCUtil::gc_malloc_allocator<StackTraceData>> stackTraceData;
        SandBoxResult()
            : result(Value::EmptyValue)
            , error(Value::EmptyValue)
            , msgStr(String::emptyString)
        {
        }
    };

    SandBoxResult run(const std::function<Value()>& scriptRunner); // for capsule script executing with try-catch
    void throwException(ExecutionState& state, Value exception);

    Context* context()
    {
        return m_context;
    }

protected:
    void fillStackDataIntoErrorObject(const Value& e);
    Context* m_context;
    StackTraceDataVector m_stackTraceData;
    Value m_exception; // To avoid accidential GC of exception value
};
}

#endif
