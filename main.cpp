#include<iostream>
#include<bgn.h>

int main(int argc,char *argv[])
{
    auto x=(3bgn).pow(1000)-(2bgn).pow(1000);
    cout<<x<<endl;
    bgn s=1;
    for(int i=1;i<=100000;i++)
        s*=i;
    cout<<s<<endl;
    return 0;
}