#include<iostream>
#include<vector>
#include<string>
#include<cstdint>
#include<algorithm>
#include<exception>
#include<cstdio>

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

    auto get_value()const{return x;}

    pair<bint_t,bint_t> add(const base_data_t n,const bint_t carry=0) const
    {
        //auto y=x+n.x+carry;
        //return {y,static_cast<bint_t>(y<x)};
        uint64_t y=uint64_t(x)+n.x+carry;
        return {static_cast<bint_t>(y),static_cast<bint_t>(y>>32)};
    }

    pair<bint_t,bint_t> sub(const base_data_t n,const bint_t carry=0) const
    {
        //auto y=x-n.x-carry;
        //return {y,static_cast<bint_t>(x<n.x)};
        uint64_t y=uint64_t(x)-n.x-carry;
        return {static_cast<bint_t>(y),static_cast<bint_t>(y>>32)};
    }

    pair<bint_t,bint_t> mul(const base_data_t n,const bint_t carry=0) const
    {
        uint64_t y=static_cast<uint64_t>(x)*static_cast<uint64_t>(n.x)+carry;
        return {static_cast<bint_t>(y&0xffffffff),static_cast<bint_t>(y>>32)};
    }

    auto div(const base_data_t n) const
    {
        return x/n.x;
    }

    auto mod(const base_data_t n) const
    {
        return x%n.x;
    }

    bool operator<(const base_data_t n)const{return x<n.x;}
    bool operator>=(const base_data_t n)const{return x>=n.x;}
    bool operator<=(const base_data_t n)const{return n>=*this;}
    bool operator>(const base_data_t n)const{return n<*this;}
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
    ubgn(string x);
    ubgn(const ubgn &n):v(begin(n.v),end(n.v)){}
    ubgn(ubgn &&n){swap(v,n.v);}
    ubgn()=default;
    ubgn& operator=(const ubgn &n){v=n.v;return *this;}

    ubgn& operator+=(const ubgn &y);
    ubgn& operator-=(const ubgn &y);
    ubgn& operator*=(const ubgn &y);
    ubgn& operator/=(const ubgn &y);

    ubgn operator+(const ubgn &y)const{ubgn r=*this;r+=y;return r;}
    ubgn operator-(const ubgn &y)const{ubgn r=*this;r-=y;return r;}
    ubgn operator*(const ubgn &y)const;
    ubgn operator/(const ubgn &y)const{return this->divmod(y).first;}
    ubgn operator%(const ubgn &y)const{return this->divmod(y).second;}

    int cmp(const ubgn &y)const;

    bool operator==(const ubgn &y)const{return this->cmp(y)==0;}
    bool operator!=(const ubgn &y)const{return this->cmp(y)!=0;}
    bool operator<(const ubgn &y)const{return this->cmp(y)<0;}
    bool operator<=(const ubgn &y)const{return this->cmp(y)<0 || this->cmp(y)==0;}
    bool operator>(const ubgn &y)const{return this->cmp(y)>0;}
    bool operator>=(const ubgn &y)const{return this->cmp(y)>0 || this->cmp(y)==0;}

    ubgn mul_base(const base_data_t x)const;
    ubgn mul_meta(const ubgn &b)const;
    ubgn mul_karatsuba(const ubgn &b)const;

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
            *this=0;
        v.erase(v.begin(),v.begin()+n);
    }

    string to_string() const
    {
        /* string s;
        for(auto i=0;i<v.size();i++)
        {
            char a[20];
            sprintf(a,"%.8x",v[i]);
            s=string(a)+s;
        }
        return s; */
        string s;
        ubgn x=*this;
        while(x!=0)
        {
            cout<<x<<"-<"<<endl;
            auto res=x.divmod(10);
            s.push_back(char(res.second.v[0].get_value())+'0');
            x=res.first;
        }
        reverse(s.begin(),s.end());
        return s;
    }

    friend ostream& operator<<(ostream &out, const ubgn &c1);
    //friend ubgn operator+(const ubgn &a,const ubgn &b);
    //friend ubgn operator-(const ubgn &a,const ubgn &b);
};

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
    while(v.back()==0)
        v.pop_back();
    return *this;
}

ubgn ubgn::mul_base(const base_data_t b)const 
{
    ubgn r;
    r.v.reserve(this->size()+1);
    auto hi=0;
    for(size_t i=0;i<v.size();i++)
    {
        auto x=v[i].mul(b,hi);
        r.push_back(x.first);
        hi=x.second;
    }
    if(hi!=0)
        r.push_back(hi);
    return r;
}

ubgn ubgn::mul_meta(const ubgn &b)const 
{
    ubgn r=0;
    r.v.reserve(this->size()+b.size()+1);
    for(size_t i=0;i<b.size();i++)
    {
        ubgn t=this->mul_base(b[i]);
        t.left_move(i);
        r+=t;
    }
    return r;
}

ubgn ubgn::mul_karatsuba(const ubgn &b)const 
{
    ubgn A,B,C,D;
    A.v.insert(A.v.begin(),this->v.begin()+0,this->v.begin()+1);
    return 0;

}

ubgn ubgn::operator*(const ubgn &y)const 
{
    return this->mul_meta(y);
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
            dividend-=divisor;cout<<dividend<<divisor<<endl;
        }
        ubgn x=count;
        x.left_move(n-1);
        n=n-1;
        r+=x;
        divisor.right_move(1);
        
    }
    return {r,dividend};
}

/* ubgn operator+(const ubgn &a, const ubgn &b)
{
    ubgn r;
    auto x = a[0].add(b[0]);
    r[0] = x.first;
    auto carry = x.second;
    size_t i = 1, j = 1;
    while (i < a.size() && j < b.size())
    {
        auto x = a[i].add(b[j], carry);
        r.push_back(x.first);
        carry = x.second;
        i++;
        j++;
    }
    while (i < a.size())
    {
        auto x = a[i].add(0, carry);
        r.push_back(x.first);
        carry = x.second;
        i++;
    }
    while (j < b.size())
    {
        auto x = b[j].add(0, carry);
        r.push_back(x.first);
        carry = x.second;
        j++;
    }
    if (carry != 0)
        r.push_back(carry);
    return r;
} */

ubgn::ubgn(string x)
{
    for(auto i=begin(x);i!=end(x);i++)
    {
        if(*i<'0' || *i>'9')
            throw std::invalid_argument("ubgn constructor input error");
    }

}

class bgn
{
    bgn uadd(const bgn &b)
    {
        int carry;
        for(size_t i=0;i<base.size();i++)
        {
            
        }
        return bgn();
    }


public:
    vector<int> base;
    int sign;
    bgn()=default;
    bgn(long long x)
    {
        base.push_back(x);
    }

    bgn(const bgn &n)
    {
        
    }

    bgn(bgn &&n)
    {

    }

};


string operator"" bgn(const char *str)
{
    return string(str);
}

void print128(unsigned __int128 x)
{
    if(x!=0)
    {
        print128(x/1000000000);
        cout<<int(x%1000000000);
    }
}

int main(int argc,char *argv[])
{
    ubgn s=1;
    for(int i=1;i<=100;i++)
    {
        s=s*i;
    }
    cout<<"================"<<endl;
    //cout<<s.mul_meta(s);<<endl;
    return 0;
}
