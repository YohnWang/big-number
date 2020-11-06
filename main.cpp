#include<iostream>
#include<bgn.h>


template<typename T>
class matrix
{
public:
    vector<vector<T>> v;
    size_t m,n;
    matrix(size_t m,size_t n,T x=T()):v(m,vector<T>(n,x)),m(m),n(n){}
    matrix()=default;
    matrix(const matrix<T> &)=default;

    matrix<T>& operator=(const matrix<T> &)=default;
    vector<T>& operator[](size_t i){return v[i];}
    matrix<T> operator*(matrix<T> &v2);
    matrix<T> pow(uint64_t index);
};

template<typename T> 
ostream& operator<<(ostream &out, matrix<T> &c)
{
    for(size_t i=0;i<c.m;i++)
    {
        for(size_t j=0;j<c.n;j++)
        {
            cout<<c[i][j]<<",";
        }
        cout<<endl;
    }
    return out;
}

template<typename T>
matrix<T> matrix<T>::operator*(matrix<T> &v2)
{
    auto &v1=*this;
    matrix<T> r(v1.m,v2.n);
    //cout<<m<<","<<n<<endl;
    for(size_t i=0;i<v1.m;i++)
    {
        for(size_t j=0;j<v2.n;j++)
        {
            //cout<<i<<","<<j<<endl;
            for(size_t k=0;k<v1.n;k++)
                r[i][j]+=v1[i][k]*v2[k][j];
        }
    }
    return r;
}

template<typename T>
matrix<T> matrix<T>::pow(uint64_t index)
{
    if(index==0)
    {
        matrix<T> r(m,n);
        for(size_t i=0;i<m;i++)
            r[i][i]=1;
    }
    else if(index==1)
        return *this;
    auto x=index/2,y=index-x;
    auto t=this->pow(x);
    if(x==y)
        return t*t;
    else 
        return  t*t*(*this);
}

auto f(uint64_t n)
{
    if(n==0)
        return pair<bgn,bgn>(1,0);
    else if(n==1)
        return pair<bgn,bgn>(1,1);
    else if(n%2==0)
    {
        auto [x,y]=f(n/2);
        auto xx=x*x;
        auto yy=y*y;
        auto xy=xx-yy;
        if(n/2%2==0)
            xy-=1;
        else 
            xy+=1;
        return pair<bgn,bgn>(xx+yy,xy+(xy-yy));
    }
    else 
    {
        auto [x,y]=f(n/2);
        auto xx=x*x;
        auto yy=y*y;
        auto xy=x*y;
        if(n/2%2==0)
            xy-=1;
        else 
            xy+=1;
        return pair<bgn,bgn>(xx+xy+xy,xx+yy);
    }
}


int main(int argc,char *argv[])
{
    auto start=clock();
    // matrix<bgn> x(2,2),f(2,1);
    // x[0][0]=1;x[0][1]=1;x[1][0]=1;x[1][1]=0;
    // f[0][0]=1;f[1][0]=0;
    //
    //y=y*y;
    //for(int i=0;i<100;i++)
    //    y=y*x;
    //auto y=x.pow(10000000)*f;
    //cout<<y<<endl;
    //auto t=ubgn(1).mul_base(1);
    //cout<<t<<endl;
    
    //bgn a=(11bgn).pow(100000),b=(12bgn).pow(100000);
    //cout<<a*b<<endl;
    
    auto x=f(10000000);
    cout<<clock()-start<<endl;
    cout<< x.first<<endl<<x.second<<endl;
    return 0;
}
