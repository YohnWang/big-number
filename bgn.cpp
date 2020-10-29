#include<iostream>
#include<vector>
#include<string>
#include<cstdint>
#include<algorithm>
#include<exception>
#include<cstdio>
#include<time.h>

using namespace std;

static_assert(static_cast<int>(1<2)==1,"bool true can not convert to 1");
//this type is the base number to construct the big number
//it has base operation [add,sub,mul,div,mod,lt,le,gt,ge,eq,neq]
class base_data_t
{
public:
    using bint_t = uint32_t;
private:
    bint_t x;
public:
    base_data_t(uint32_t x):x(x){}
    base_data_t()=default;
    //base_data_t(base_data_t &&)=default;
    //base_data_t(const base_data_t&)=default;
    //base_data_t& operator=(const base_data_t &)=default;

    auto get_value()const{return x;}

    pair<bint_t,bint_t> add(const base_data_t n,const bint_t carry=0) const
    {
        //auto y=x+n.x+carry;
        //return {y,static_cast<bint_t>(y<x)};
        uint64_t y=uint64_t(x)+n.x+carry;
        return {static_cast<bint_t>(y),static_cast<bint_t>(y>>32)};
        // __uint128_t y=__uint128_t(x)+n.x+carry;
        // return {static_cast<bint_t>(y),static_cast<bint_t>(y>>64)};
    }

    pair<bint_t,bint_t> sub(const base_data_t n,const bint_t carry=0) const
    {
        auto y=x-n.x-carry;
        return {y,static_cast<bint_t>(x<n.x)};
        //uint64_t y=uint64_t(x)-n.x-carry+(1LLU<<32);
        //return {static_cast<bint_t>(y),static_cast<bint_t>(y>>32)};
        // __uint128_t y=__uint128_t(x)-n.x-carry;
        // return {static_cast<bint_t>(y),static_cast<bint_t>(y>>64)};
    }

    pair<bint_t,bint_t> mul(const base_data_t n,const bint_t carry=0) const
    {
        uint64_t y=static_cast<uint64_t>(x)*static_cast<uint64_t>(n.x)+carry;
        return {static_cast<bint_t>(y),static_cast<bint_t>(y>>32)};
        // __uint128_t y=__uint128_t(x)*n.x+carry;
        // return {static_cast<bint_t>(y),static_cast<bint_t>(y>>64)};
    }

    pair<bint_t,bint_t> divmod(const base_data_t n,const bint_t carry=0) const
    {
        uint64_t y=uint64_t(x)+(uint64_t(carry)<<32);
        return {y/n.x,y%n.x};
        // __uint128_t y=__uint128_t(x)+__uint128_t(carry)<<64;
        // return {static_cast<bint_t>(y/n.x),static_cast<bint_t>(y%n.x)};
    }

    bool operator<(const base_data_t n)const{return x<n.x;}
    bool operator>=(const base_data_t n)const{return x>=n.x;}
    bool operator<=(const base_data_t n)const{return x<=n.x;}
    bool operator>(const base_data_t n)const{return x>n.x;}
    bool operator==(const base_data_t n)const{return x==n.x;}
    bool operator!=(const base_data_t n)const{return x!=n.x;}

};


//ubgn is the helper type to construct bgn type
//it has + - * / 
class ubgn
{
public:
    vector<base_data_t> v;
    ubgn(uint32_t x):v{x}{}
    //ubgn(uint64_t x){v.push_back(static_cast<base_data_t::bint_t>(x));if(x>>32)v.push_back(static_cast<base_data_t::bint_t>(x>>32));}
    ubgn(string x);
    //ubgn(const ubgn &n):v(begin(n.v),end(n.v)){}
    //ubgn(ubgn &&n){swap(v,n.v);cout<<"&& c"<<endl;}
    //ubgn(ubgn &&)=default;
    ubgn()=default;
    ubgn& operator=(const ubgn &n)=default;

    ubgn& operator+=(const ubgn &y);
    ubgn& operator-=(const ubgn &y);
    ubgn& operator*=(const ubgn &y){*this=this->mul_karatsuba(y);return *this;}
    ubgn& operator/=(const ubgn &y);

    ubgn operator+(const ubgn &y)const{ubgn r=*this;r+=y;return r;}
    ubgn operator-(const ubgn &y)const{ubgn r=*this;r-=y;return r;}
    ubgn operator*(const ubgn &y)const{return this->mul_karatsuba(y);}
    ubgn operator/(const ubgn &y)const{return this->divmod(y).first;}
    ubgn operator%(const ubgn &y)const{return this->divmod(y).second;}

    int cmp(const ubgn &y)const;

    bool operator==(const ubgn &y)const{return this->cmp(y)==0;}
    bool operator!=(const ubgn &y)const{return this->cmp(y)!=0;}
    bool operator<(const ubgn &y)const{return this->cmp(y)<0;}
    bool operator<=(const ubgn &y)const{return !(*this>y);} 
    bool operator>(const ubgn &y)const{return this->cmp(y)>0;}
    bool operator>=(const ubgn &y)const{return !(*this<y);}

    ubgn& mul_base(const base_data_t x);
    ubgn mul_meta(const ubgn &b)const;
    ubgn mul_karatsuba(const ubgn &b)const;

    pair<ubgn&,base_data_t::bint_t> divmod_base(const base_data_t x);
    pair<ubgn,ubgn> divmod(const ubgn &b)const;

    base_data_t& operator[](size_t i){return v[i];}
    base_data_t operator[](size_t i) const{return v[i];}
    void push_back(base_data_t x){v.push_back(x);}
    auto size() const{return v.size();}

    void left_move(size_t n)
    {
        if(*this!=0)
            v.insert(begin(v),n,0);
    }

    void right_move(size_t n)
    {
        if(n>=v.size())
        {
            v.clear();
            v.push_back(0);
        }
        v.erase(v.begin(),v.begin()+n);
    }

    void shrink_zero()
    {
        while(v.back()==0)
            v.pop_back();
        if(v.size()==0)
            v.push_back(0);
    }
    string to_string() const;

    friend ostream& operator<<(ostream &out, const ubgn &c1);
};

ubgn::ubgn(string x)
{
    ubgn r=0;
    for(auto i:x)
    {
        r.mul_base(10);
        r+=(i-'0');
    }
    swap(v,r.v);
}

string ubgn::to_string() const
{
    if(*this==0)
        return "0";
    string s;
    ubgn x=*this;
    while(x!=0)
    {
        auto res=x.divmod_base(1000000000);
        auto num=res.second;
        for(int i=0;i<9;i++)
        {
            s.push_back(char(num%10)+'0');
            num/=10;
        }
    }
    while(s.back()=='0')
        s.pop_back();
    reverse(s.begin(),s.end());
    return s;
}

ostream& operator<<(ostream &out, const ubgn &n)
{
    out<<n.to_string();
    return out;
}

int ubgn::cmp(const ubgn &b)const
{
    if(v.size()<b.size())
        return -1;
    else if(v.size()>b.size())
        return 1;
    else 
    {
        for(auto i=v.size()-1;i>0;i--)
        {
            if(v[i]>b[i])
                return 1;
            else if(v[i]<b[i])
                return -1;
        }
        if(v[0]>b[0])
            return 1;
        else if(v[0]<b[0])
            return -1;
        else 
            return 0;
    }
}

ubgn& ubgn::operator+=(const ubgn &b)
{
    auto x=v[0].add(b[0]);
    v[0]=x.first;
    auto carry=x.second;
    size_t i=1,j=1;
    while(i<v.size() && j<b.size())
    {
        auto x=v[i].add(b[j],carry);
        v[i]=x.first;
        carry=x.second;
        i++;j++;
    }
    while(i<v.size())
    {
        auto x=v[i].add(0,carry);
        v[i]=x.first;
        carry=x.second;
        i++;
    }
    while(j<b.size())
    {
        auto x=b[j].add(0,carry);
        v.push_back(x.first);
        carry=x.second;
        j++;
    }
    if(carry!=0)
        v.push_back(carry);
    return *this;
}

ubgn& ubgn::operator-=(const ubgn &b)
{
    auto x=v[0].sub(b[0]);
    v[0]=x.first;
    auto carry=x.second;
    size_t i=1,j=1;
    while(i<v.size() && j<b.size())
    {
        auto x=v[i].sub(b[j],carry);
        v[i]=x.first;
        carry=x.second;
        i++;j++;
    }
    while(carry!=0)
    {
        auto x=v[i].sub(0,carry);
        v[i]=x.first;
        carry=x.second;
        i++;
    }
    this->shrink_zero();
    return *this;
}

ubgn& ubgn::mul_base(const base_data_t b)
{
    base_data_t::bint_t hi=0;
    if(b==0)
    {
        v.clear();
        v.push_back(0);
        return *this;
    }
    for(size_t i=0;i<v.size();i++)
    {
        auto x=v[i].mul(b,hi);
        v[i]=x.first;
        hi=x.second;
    }
    if(hi!=0)
        v.push_back(hi);
    return *this;
}

ubgn ubgn::mul_meta(const ubgn &b)const 
{
    ubgn r=0;
    r.v.reserve(this->size()+b.size()+1);
    for(size_t i=0;i<b.size();i++)
    {
        ubgn t=*this;
        t.mul_base(b[i]);
        t.left_move(i);
        r+=t;
    }
    //cout<<r<<endl;
    return r;
}

ubgn ubgn::mul_karatsuba(const ubgn &b)const 
{
    auto& a=*this;
    if(a.size()<32 || b.size()<32)
    {
        return a.mul_meta(b);
    }
    else if(a.size()>b.size())
    {
        ubgn A,B;
        A.v.insert(begin(A.v),begin(a.v)+b.size(),end(a.v));
        B.v.insert(begin(B.v),begin(a.v),begin(a.v)+b.size());
        A=A.mul_karatsuba(b);
        A.left_move(b.size());
        return A+B.mul_karatsuba(b);
    }
    else if(a.size()<b.size())
        return b.mul_karatsuba(a);
    
    ubgn A,B,C,D;
    size_t low_size=a.size()/2;

    A.v.insert(begin(A.v),begin(a.v)+low_size,end(a.v));
    B.v.insert(begin(B.v),begin(a.v),begin(a.v)+low_size);
    
    C.v.insert(begin(C.v),begin(b.v)+low_size,end(b.v));
    D.v.insert(begin(D.v),begin(b.v),begin(b.v)+low_size);

    A.shrink_zero();B.shrink_zero();C.shrink_zero();D.shrink_zero();

    auto AC=A.mul_karatsuba(C);
    auto BD=B.mul_karatsuba(D);
    
    auto X=A+B;
    auto Y=C+D;
    auto XY=X.mul_karatsuba(Y);
    
    XY-=AC;
    XY-=BD;
    AC.left_move(low_size*2);
    XY.left_move(low_size);

    return AC+XY+BD;
}

pair<ubgn&,base_data_t::bint_t> ubgn::divmod_base(const base_data_t b)
{
    base_data_t::bint_t carry=0;
    for(ssize_t i=v.size()-1;i>=0;i--)
    {
        auto x=v[i].divmod(b,carry);
        v[i]=x.first;
        carry=x.second;
    }
    this->shrink_zero();
    return {*this,carry};
}

pair<ubgn,ubgn> ubgn::divmod(const ubgn &b)const
{
    if(*this<b)
        return {0,*this};
    if(b==0)
        return {0,0};
    ubgn dividend=*this,divisor=b;
    ubgn r=0;
    size_t n=this->size();
    divisor.left_move(this->size()-divisor.size());
    while(dividend>=b)
    {
        int count=0;
        while(dividend>=divisor)
        {
            count++;
            dividend-=divisor;
        }
        ubgn x=count;
        x.left_move(n-1);
        n=n-1;
        r+=x;
        divisor.right_move(1);
    }
    return {r,dividend};
}


class bgn
{
private:
    int sign;
    ubgn n;
public:
    bgn():sign(0),n(0){}
    bgn(int64_t x):bgn(to_string(x)){}
    bgn(string x);

    bgn& operator+=(const bgn &y);
    bgn& operator-=(const bgn &y);
    bgn& operator*=(const bgn &y);
    bgn& operator/=(const bgn &y);
    bgn& operator%=(const bgn &y);

    bgn operator-()const{bgn x=*this;x.sign=!x.sign;return x;}

    int cmp(const bgn &y)const;

    bool operator==(const bgn &y)const{return this->cmp(y)==0;}
    bool operator!=(const bgn &y)const{return this->cmp(y)!=0;}
    bool operator<(const bgn &y)const{return this->cmp(y)<0;}
    bool operator<=(const bgn &y)const{return !(*this>y);}
    bool operator>(const bgn &y)const{return this->cmp(y)>0;}
    bool operator>=(const bgn &y)const{return !(*this<y);}

    friend bgn operator+(const bgn &x,const bgn &y);
    friend bgn operator-(const bgn &x,const bgn &y);
};

int bgn::cmp(const bgn &y)const
{
    auto &x=*this;
    if(x.sign==0 && y.sign!=0)
        return 1;
    else if(x.sign!=0 && y.sign==0)
        return -1;
    else if(x.sign==0 && y.sign==0)
        return x.n.cmp(y.n);
    else 
        return -x.n.cmp(y.n);
}

bgn& bgn::operator+=(const bgn &y)
{
    auto &x=*this;
    if(x.sign==0 && y.sign!=0)
        x-=-y;
    else if(x.sign!=0 && y.sign==0)
        x=y-x;
    else
        x.n+=y.n;
    return x;
}

bgn& bgn::operator-=(const bgn &y)
{

}
bgn& bgn::operator*=(const bgn &y);
bgn& bgn::operator/=(const bgn &y);
bgn& bgn::operator%=(const bgn &y);

bgn operator-(const bgn &x,const bgn &y)
{
    // if(x.sign==0 && y.sign!=0)
    //     return x+y;
    // else if(x.sign!=0 && y.sign==0)
    //     return x-y;
    // else
    //     x.n+=y.n;
    // return x;
}


bgn::bgn(string x)
{
    string s;
    int sign=0;
    int state=0;
    for(auto i:x)
    {
        if(state==0&&i=='-')
            sign=!sign;
        else if(i>='0'&&i<='9')
        {
            s.push_back(i);
            state=1;
        }
        else 
            throw invalid_argument("bgn string construct error\n");
    }
    this->sign=sign;
    n=s;
}

string operator"" bgn(const char *str)
{
    return string(str);
}



int main(int argc,char *argv[])
{
    auto start_time=clock();
    ubgn s=1,t=1;
    for(int i=1;i<=10000;i++)
    {
        s*=2;
        t*=3;
    }
    cout<<"================"<<endl;
    //cout<<s<<endl;
    cout<<"================"<<endl;
    cout<<s.mul_karatsuba(t)<<endl;

    cout<<"using time="<<clock()-start_time<<endl;

    // ubgn x=string("1000000000000000000000000000000000000000000000000");
    // ubgn y=string("000000000000000000000000000000000000000000000000");
    // cout<<x.mul_meta(y)<<endl;

    
    return 0;
}
