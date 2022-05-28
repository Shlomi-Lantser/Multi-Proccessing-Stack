#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sys/mman.h>

using namespace std;

typedef struct Stack
{
    int top;
    char data[4096];
} Mystack, *stack_ptr;

void push(string str, Stack *stack_ptr)
{
     for (int i = 0 ; i < str.length(); i++)
    {
        stack_ptr->data[stack_ptr->top + 1] = str[i];
        stack_ptr->top++;
    }
    stack_ptr->data[stack_ptr->top + 1] = '\0';
    stack_ptr->top++;
}

string top(Stack *stack_ptr)
{
    if (stack_ptr->top == 1)
    {
        return "";
    }
    string temp = "OUTPUT : ";
    int i = stack_ptr->top - 1;
    while (stack_ptr->data[i] != '\0')
    {
        i--;
    }
    
    for (int j = 0; stack_ptr->data[i + 1] != '\0' ; j++)
    {
        temp+= stack_ptr->data[i+1];
        i++;
    }
    return temp;
}

 string pop(Stack *stack_ptr)
{
    string temp = top(stack_ptr);

    stack_ptr->top--;
    while (stack_ptr->data[stack_ptr->top] != '\0')
    {
        stack_ptr->top--;
    }
    return temp;
}