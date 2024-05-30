#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>

#include "hash.h"

LLVMValueRef build_number(float val) {
    return LLVMConstReal(LLVMFloatType(), val);
}

LLVMValueRef build_binop(
    LLVMValueRef lhs,
    LLVMValueRef rhs,
    char op,
    LLVMBuilderRef builder
) {
    if (LLVMIsUndef(lhs) || LLVMIsUndef(rhs)) {
        return LLVMGetUndef(LLVMFloatType());
    }
    switch (op) {
        case '+':
            return LLVMBuildFAdd(builder, lhs, rhs, "add_result");
        case '-':
            return LLVMBuildFSub(builder, lhs, rhs, "sub_result");
        case '*':
            return LLVMBuildFMul(builder, lhs, rhs, "mul_result");
        case '/':
            return LLVMBuildFDiv(builder, lhs, rhs, "div_result");
        default:
            fprintf(stderr, "Error: invalid operator: %c\n", op);
            return LLVMGetUndef(LLVMFloatType());
    }
}

int main() {
    LLVMModuleRef module = LLVMModuleCreateWithName("foo.code");
    LLVMBuilderRef builder = LLVMCreateBuilder();

    LLVMTypeRef foo_type = LLVMFunctionType(
        LLVMFloatType(),
        NULL,
        0,
        0
    );
    LLVMValueRef foo_fn = LLVMAddFunction(module, "foo", foo_type);
    LLVMBasicBlockRef entry_blk = LLVMAppendBasicBlock(foo_fn, "entry");
    LLVMPositionBuilderAtEnd(builder, entry_blk);

    LLVMValueRef expr1 = build_binop(
        build_number(4),
        build_number(2),
        '*',
        builder
    );
    LLVMValueRef expr2 = build_binop(
        build_number(8),
        expr1,
        '+',
        builder
    );

    LLVMBuildRet(builder, expr2);

    LLVMVerifyModule(module, LLVMAbortProcessAction, NULL);
    char* out = LLVMPrintModuleToString(module);
    printf("%s\n", out);

    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
    return 0;
}
