#include<iostream>
using namespace std;
int main(){
    cout<<"Basic Calculator Program"<<endl;
    int a,b;
    cout<<"Enter value of 1st number"<<endl;
    cin>>a;
    cout<<"Enter value of 2nd number"<<endl;
    cin>>b;
    char op;
    cout<< " Enter which operation to perform ( +, - , * , / , % ) " <<endl;
    cin>>op;
    
    switch(op){
        case '+': cout<<"The addition of numbers is " << (a+b) << endl;
                  break;
        case '-': cout<<"The subtraction of numbers is " << (a-b) << endl;
                  break;
         case '*': cout<<"The multiplication of numbers is " << (a*b) << endl;
                  break;
         case '/': cout<<"The division of numbers is " << (a/b) << endl;
                  break;
        case '%': cout<<"The modulus of numbers is " << (a%b) << endl;
                  break;
        default: cout<<"Invalid Operation"<< endl;                                                
    }
}
