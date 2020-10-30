#include<bgn.h>

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
        for(ssize_t i=v.size()-1;i>=0;i--)
        {
            if(v[i]>b[i])
                return 1;
            else if(v[i]<b[i])
                return -1;
        }
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
    return r;
}

ubgn ubgn::mul_karatsuba(const ubgn &b)const 
{
    auto& a=*this;
    if(a.size()<512 || b.size()<512)
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

    //copy the specified segment directly, the redundant leading 0 needs to be removed
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
    shrink_zero();
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


string bgn::to_string()const
{
    string s;
    if(sign!=0)
        s.push_back('-');
    return s+n.to_string();
}

ostream& operator<<(ostream &out, const bgn &c1)
{
    out<<c1.to_string();
    return out;
}

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
        x=y-(-x);
    else
        x.n+=y.n;
    return x;
}

bgn& bgn::operator-=(const bgn &y)
{
    auto &x=*this;
    auto c=this->n.cmp(y.n);
    if(x.sign==0 && y.sign==0)
    {
        if(c>=0)
            x.n-=y.n;
        else
        {
            x.n=y.n-x.n;
            x.sign=1;
        }
    }
    else if(x.sign!=y.sign)
    {
        x.n+=y.n;
    }
    else 
    {
        auto t=-y;
        t+=*this;
        *this=t;
    }
    return *this;
}

bgn& bgn::operator*=(const bgn &y)
{
    auto &x=*this;
    x.sign^=y.sign;
    x.n*=y.n;
    return x;
}

bgn& bgn::operator*=(const int y)
{
    auto &x=*this;
    x.sign^=static_cast<int>(y<0);
    x.n.mul_base(y);
    return x;
}

bgn& bgn::operator/=(const bgn &y)
{
    return *this;
}

bgn bgn::pow(uint64_t index)const
{
    if(index==0)
        return 1;
    else if(index==1)
        return *this;
    auto x=index/2,y=index-x;
    auto t=this->pow(x);
    if(x==y)
        return t*t;
    else 
        return  t*t*(*this);
}

bgn::operator int32_t()const
{
    if(sign==0)
        return n.v[0].get_value()&0x7fffffff;
    else 
        return -(n.v[0].get_value()&0x7fffffff);
}

bgn::operator int64_t()const
{
    if(n.size()==1)
    {
        int64_t x=n.v[0].get_value();
        if(sign==0)
            return x;
        else 
            return -x;
    }
    else 
    {
        int64_t lo=n.v[0].get_value();
        int64_t hi=static_cast<int64_t>(n.v[1].get_value());
        int64_t x= lo+((hi&0x7fffffff)<<32);
        if(sign==0)
            return x;
        else 
            return -x;
    }
}

bgn operator+(const bgn &x,const bgn &y)
{
    auto r=x;
    r+=y;
    return r;
}

bgn operator-(const bgn &x,const bgn &y)
{
    auto r=x;
    r-=y;
    return r;
}

bgn operator*(const bgn &x,const bgn &y)
{
    auto r=x;
    r*=y;
    return r;
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