# Not filling out now because this is public
# NAME
# ID
# 2025-04-03
# Ubuntu 24.04

import os

curr = os.path.dirname(os.path.abspath(__file__))
inp = os.path.join(curr, "input_RPN.txt")

with open(inp, "r") as f:
    lines = f.read().splitlines()

def eval_rpn(expr):
    stack = []
    tokens = expr.split()
    for token in tokens:
        if token in ('+', '-', '*', '/'):
            b = stack.pop()
            a = stack.pop()
            if token == '+':
                stack.append(a + b)
            elif token == '-':
                stack.append(a - b)
            elif token == '*':
                stack.append(a * b)
            elif token == '/':
                stack.append(a / b)
        else:
            stack.append(int(token))
    return stack[0]

for line in lines:
    if line.strip():
        result = eval_rpn(line.strip())
        print(result)