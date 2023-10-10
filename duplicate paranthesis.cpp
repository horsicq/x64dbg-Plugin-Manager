#include <iostream>
#include<string>
#include "stack.cpp"
using namespace std;
bool hasDuplicateParentheses(const string& expression) {
    Stack<char> st;
    bool checkduplicate = false;
    int size = expression.size() - 1;
    int i = 0;
    while (i < size)
    {
        if (expression[i] == ')' && expression[i + 1] != ')')
        {
            while (!st.isEmpty() && st.Top() != '(')
            {
                st.pop();
            }
            if (!st.isEmpty() && st.Top() == '(')
            {
                st.pop();
            }

        }
        else if (expression[i] == ')' && expression[i + 1] == ')')
        {
            while (!st.isEmpty() && st.Top() != '(')
            {
                st.pop();
            }
            if (!st.isEmpty() && st.Top() == '(') {
                st.pop();
                if (!st.isEmpty() && st.Top() == '(')
                {
                    checkduplicate = true;

                }
                else {
                    checkduplicate = false;


                }
            }

        }
        else {
            st.push(expression[i]);

        }
        if (checkduplicate == true)
        {
            return true;
        }
        i++;

    }
    return checkduplicate;
}

int main() {
    //string expression = "(((x)+ a+b)) ";
    // ((((x+y)+a)+z))
    // //((x+y)+a)))
   // string expression = "((x+y))+z ";
    //string expression = "(((x)+ a+b))";
    // ((a+(a+b)))
    //string expression = "(((x)+ a+b)))";
    string expression = "(((x+y))+z)";
    //string expression = "(((x)+ a+b))";
    //string expression = "(((x)+ a+b))";
    bool result = hasDuplicateParentheses(expression);

    if (result) {
        cout << "Duplicate parentheses found." << endl;
    }
    else {
        cout << "No duplicate parentheses found." << endl;
    }

    return 0;
}
