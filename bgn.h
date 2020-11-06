#ifndef _BGN_H
#define _BGN_H

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
        uint64_t y=uint64_t(x)+n.x+carry;
        return {static_cast<bint_t>(y),static_cast<bint_t>(y>>32)};
    }

    pair<bint_t,bint_t> sub(const base_data_t n,const bint_t carry=0) const
    {
        auto y=uint64_t(x)-n.x-carry;
        return {y,static_cast<bint_t>(y>>32)&1};
    }

    pair<bint_t,bint_t> mul(const base_data_t n,const bint_t carry=0) const
    {
        uint64_t y=static_cast<uint64_t>(x)*static_cast<uint64_t>(n.x)+carry;
        return {static_cast<bint_t>(y),static_cast<bint_t>(y>>32)};
    }

    pair<bint_t,bint_t> divmod(const base_data_t n,const bint_t carry=0) const
    {
        uint64_t y=uint64_t(x)+(uint64_t(carry)<<32);
        return {y/n.x,y%n.x};
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

    ubgn& left_move(size_t n)
    {
        if(*this!=0)
            v.insert(begin(v),n,0);
        return *this;
    }

    ubgn& right_move(size_t n)
    {
        if(n>=v.size())
        {
            v.clear();
            v.push_back(0);
        }
        v.erase(v.begin(),v.begin()+n);
        return *this;
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



class bgn
{
private:
    int sign;
    ubgn n;
public:
    bgn():sign(0),n(0){}
    bgn(int64_t x):bgn(std::to_string(x)){}
    bgn(int32_t x):bgn(std::to_string(x)){}
    bgn(string x);
    bgn(const char *x):bgn(string(x)){}

    bgn& operator+=(const bgn &y);
    bgn& operator-=(const bgn &y);
    bgn& operator*=(const bgn &y);
    bgn& operator*=(const int y);
    bgn& operator/=(const bgn &y);
    bgn& operator/=(const uint32_t y){n.divmod_base(y);return *this;}
    bgn& operator%=(const bgn &y);

    bgn operator-()const{bgn x=*this;x.sign=!x.sign;return x;}
    bgn operator/(const uint32_t y)const{bgn r=*this;r.n.divmod_base(y);return r;}

    bgn pow(uint64_t index)const;

    int cmp(const bgn &y)const;

    bool operator==(const bgn &y)const{return this->cmp(y)==0;}
    bool operator!=(const bgn &y)const{return this->cmp(y)!=0;}
    bool operator<(const bgn &y)const{return this->cmp(y)<0;}
    bool operator<=(const bgn &y)const{return !(*this>y);}
    bool operator>(const bgn &y)const{return this->cmp(y)>0;}
    bool operator>=(const bgn &y)const{return !(*this<y);}

    operator int32_t()const;
    operator int64_t()const;

    string to_string()const;

    friend bgn operator+(const bgn &x,const bgn &y);
    friend bgn operator-(const bgn &x,const bgn &y);
    friend bgn operator*(const bgn &x,const bgn &y);
    friend ostream& operator<<(ostream &out, const bgn &c1);
};

static auto operator"" bgn(const char *str)
{
    return bgn(str);
}

#endif