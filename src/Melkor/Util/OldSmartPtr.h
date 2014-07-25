#ifndef SMARTPTR_H
#define SMARTPTR_H

#pragma warning( disable : 4786 )

using namespace std;

class SmartPtrInterface {
    int crefs;
public:
	SmartPtrInterface(void) 
	{
		crefs = 0; 
	}
	~SmartPtrInterface() {};
	void upcount(void)
	{
		++crefs;
	}
	void downcount(void)
	{
		if (--crefs == 0)
			delete this;
	}
};

template <class T> class SmartPtr {
    T* p;
public:
    SmartPtr(T* p_) : p(p_) 
	{
		if(p!=NULL) 
			p->upcount(); 
	}
//    SmartPtr(const T* p_) : p(p_) { if(p!=NULL) p->upcount(); }
    SmartPtr(const SmartPtr& smp) : p(smp.p) 
	{
		if(p!=NULL) 
			p->upcount(); 
	}
    virtual ~SmartPtr(void) 
	{ 
		if(p!=NULL) 
			p->downcount(); 
	}
    operator T*(void) { return p; }
    operator const T*(void) const { return p; }
    T& operator*(void) { return *p; }
    const T& operator*(void) const { return *p; }
    T* operator->(void) { return p; }
    const T* operator->(void) const { return p; }

    SmartPtr& operator=(const SmartPtr<T> &p_)
	{
		if(p!=p_.p)
		{
			if(p!=NULL) p->downcount();
			p = p_.p;
			if(p!=NULL) p->upcount();
		}
		return *this;
	}
	   
	SmartPtr& operator=(T* p_) {
		if(p!=p_)
		{
			if(p!=NULL) p->downcount();
			p = p_;
			if(p!=NULL) p->upcount();
		}
		return *this;
	}
};

#endif