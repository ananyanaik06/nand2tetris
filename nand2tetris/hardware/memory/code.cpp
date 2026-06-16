#include<iostream>

int main()
{
    char ch = 'a';
    for(int i = 0 ; i < 4 ;i++)
    {
        printf("myRAM4K(in = in , load = %c , address = address[0..11] , out = out%c);\n",ch,ch);
        ch++;
    }
}
