#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cassert>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <exception>
#include <typeinfo>

#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */

#include <unordered_map>
#include <unordered_set>
#include <memory>

#endif /* C++ 11 */
using namespace std;


class CSyntaxException : public exception {
public:
    //---------------------------------------------------------------------------------------------
    CSyntaxException(const string &desc)
            : m_Desc(desc) {
    }

    //---------------------------------------------------------------------------------------------
    virtual const char *what(void) const noexcept {
        return m_Desc.c_str();
    }
    //---------------------------------------------------------------------------------------------
private:
    string m_Desc;
};

#endif /* __PROGTEST__ */



class CDataType{
public:
    CDataType(){}
    virtual ~CDataType(){}

    virtual int Sizeof (void) const = 0;
    virtual bool operator == (const CDataType & cmp) const = 0;
    virtual bool operator != (const CDataType & cmp) const = 0;

	string m_Name;
   	friend ostream & operator << (ostream & o, const CDataType & x);
   	virtual void print (ostream & o) const = 0;
};
//==========================================================================
class CDataTypePrimitive : public CDataType{
	virtual bool operator == (const CDataType & cmp) const{
		bool r = (m_Name == cmp.m_Name);
		return r;
	}
    virtual bool operator != (const CDataType & cmp) const{
    	bool r = (m_Name != cmp.m_Name);
    	return r;
    }

};

//==========================================================================
class CDataTypeInt : public CDataTypePrimitive {
public:
	CDataTypeInt(){m_Name="int";}
    virtual int Sizeof (void) const{
        return 4;
    }
protected:
	virtual void print (ostream & os) const{
		os << "int";
	}
};
//=========================================================================

class CDataTypeDouble : public CDataTypePrimitive {
public:
	CDataTypeDouble(){m_Name="double";}
    virtual int Sizeof (void) const{
        return 8;
    }
protected:
	virtual void print (ostream & os) const{
		os << "double";
	}
};

//=========================================================================

class CDataTypeEnum : public CDataType {
public:
	CDataTypeEnum(){m_Name="enum";}

	virtual int Sizeof (void) const {
		return 4;
	}
	CDataTypeEnum & Add(string val){
		if(binary_search (listing.begin(),listing.end(), val)==true)
			throw CSyntaxException("Duplicate enum value: "+val);
		listing.push_back(val);
		return *this;
	}

	virtual bool operator == (const CDataType & cm)const{
		const CDataTypeEnum * cmp = dynamic_cast <const CDataTypeEnum*>(&cm);
		if(!cmp)
			return false;
		if (cmp->listing.size() != listing.size())
			return false;
		for ( unsigned int i = 0 ; i < listing.size(); ++i  ){
			if(listing[i] != cmp->listing[i])
				return false;
		}
		return true;
	}
	virtual bool operator != ( const CDataType & cmp)const{
		return !(*this == cmp);
	}

protected:
	vector<string> listing;
	virtual void print (ostream & os) const{
		unsigned int i = 0;
		os << "enum" << endl;
		os << "{" << endl;
		for(const auto & it : listing){
			if(i==(listing.size()-1))
				os << " " << it << endl;
			else
				os << " " << it << "," << endl;
		++i;
		}


		os << "}";
	}
};

//=========================================================================

class CDataTypeStruct : public CDataType {
public:
	CDataTypeStruct(){
		m_Name = "struct";
	}
	virtual ~CDataTypeStruct(){	}
	CDataTypeStruct & AddField (string name, const  CDataTypeInt  type ){
		if(mapa.find(name)==mapa.end() ){
			shared_ptr<CDataTypeInt> ptr(new CDataTypeInt(type));
			mapa.insert({name,ptr});
			dek.push_back({name, ptr});
			return *this;
		}
		else
			throw CSyntaxException("Duplicate field: "+name);
		}

	CDataTypeStruct & AddField (string name, const  CDataTypeEnum  type ){
		if(mapa.find(name)==mapa.end() ){
			shared_ptr<CDataTypeEnum> ptr(new CDataTypeEnum(type));
			mapa.insert({name,ptr});
			dek.push_back({name, ptr});
			return *this;
		}
		else
			throw CSyntaxException("Duplicate field: "+name);
		}

	CDataTypeStruct & AddField (string name, const  CDataTypeDouble  type ){
		if(mapa.find(name)==mapa.end() ){
			shared_ptr<CDataTypeDouble> ptr(new CDataTypeDouble(type));
			mapa.insert({name,ptr});
			dek.push_back({name, ptr});
			return *this;
		}
		else
			throw CSyntaxException("Duplicate field: "+name);
		}

    CDataType & Field ( string name )const{
    	if (mapa.find(name)==mapa.end() )
    		throw CSyntaxException("Unknown field: "+name);
    	else{
    		auto it = mapa.find(name)->second;
    		return *it;
    	}
    }
    virtual int Sizeof(void) const {
    	int sum = 0;
    	for(const auto & it : mapa){
			sum += it.second->Sizeof();
		}
		return sum;	    	
    }

    virtual bool operator == (const CDataType & cm)const{
 		const CDataTypeStruct * cmp = dynamic_cast <const CDataTypeStruct*>(&cm);
    	if(!cmp)
    		return false;
    	if(Sizeof() != cmp->Sizeof() )
    		return false;
    	else{
    		for(unsigned int i = 0 ; i < dek.size(); i++)
    			if(*cmp->dek[i].second != *dek[i].second){
    				return false;
    			}
    		return true;  
    	}
    }

    virtual bool operator != (const CDataType & cmp)const{
		return !(*this==cmp);
    }
protected:
	vector<pair<string,shared_ptr<CDataType> > > dek;
    unordered_map<string,shared_ptr<CDataType > , hash<string>> mapa;

	virtual void print (ostream & os) const{
		os << "struct" << endl;
		os << "{" << endl;
		for (auto it = dek.begin(); it != dek.end(); ++it)		
			os<< " " << *it->second << " " << it->first <<";"<< endl;			
		os << "}";
	}
};

//=========================================================================

ostream & operator << (ostream & os, const CDataType & x){
	x.print(os);
	return os;
}


//=========================================================================


#ifndef __PROGTEST__

//-------------------------------------------------------------------------------------------------
static bool whitespaceMatch(const string &out,
                            const string &ref) {
    const char *o = out.c_str();
    const char *r = ref.c_str();

    while (1) {
        while (isspace(*o)) o++;
        while (isspace(*r)) r++;
        if (!*r && !*o) return true;
        if (!*r || !*o || *r++ != *o++) return false;
    }
}

//-------------------------------------------------------------------------------------------------
template<typename _T>
static bool whitespaceMatch(const _T &x,
                            const string &ref) {
    ostringstream oss;
    oss << x;
    return whitespaceMatch(oss.str(), ref);
}

//-------------------------------------------------------------------------------------------------
int main(void) {

    CDataTypeStruct a = CDataTypeStruct().
            AddField("m_Length", CDataTypeInt()).
            AddField("m_Status", CDataTypeEnum().
            Add("NEW").
            Add("FIXED").
            Add("BROKEN").
            Add("DEAD")).
            AddField("m_Ratio", CDataTypeDouble());   


    CDataTypeStruct b = CDataTypeStruct().
            AddField("m_Length", CDataTypeInt()).
            AddField("m_Status", CDataTypeEnum().
            Add("NEW").
            Add("FIXED").
            Add("BROKEN").
            Add("READY")).
            AddField("m_Ratio", CDataTypeDouble());

    CDataTypeStruct c = CDataTypeStruct().
            AddField("m_First", CDataTypeInt()).
            AddField("m_Second", CDataTypeEnum().
            Add("NEW").
            Add("FIXED").
            Add("BROKEN").
            Add("DEAD")).
            AddField("m_Third", CDataTypeDouble());

    CDataTypeStruct d = CDataTypeStruct().
            AddField("m_Length", CDataTypeInt()).
            AddField("m_Status", CDataTypeEnum().
            Add("NEW").
            Add("FIXED").
            Add("BROKEN").
            Add("DEAD")).
            AddField("m_Ratio", CDataTypeInt());

    
    assert (whitespaceMatch(a, "struct\n"
            "{\n"
            "  int m_Length;\n"
            "  enum\n"
            "  {\n"
            "    NEW,\n"
            "    FIXED,\n"
            "    BROKEN,\n"
            "    DEAD\n"
            "  } m_Status;\n"
            "  double m_Ratio;\n"
            "}"));


  
    assert (whitespaceMatch(b, "struct\n"
            "{\n"
            "  int m_Length;\n"
            "  enum\n"
            "  {\n"
            "    NEW,\n"
            "    FIXED,\n"
            "    BROKEN,\n"
            "    READY\n"
            "  } m_Status;\n"
            "  double m_Ratio;\n"
            "}"));

    assert (whitespaceMatch(c, "struct\n"
            "{\n"
            "  int m_First;\n"
            "  enum\n"
            "  {\n"
            "    NEW,\n"
            "    FIXED,\n"
            "    BROKEN,\n"
            "    DEAD\n"
            "  } m_Second;\n"
            "  double m_Third;\n"
            "}"));

    assert (whitespaceMatch(d, "struct\n"
            "{\n"
            "  int m_Length;\n"
            "  enum\n"
            "  {\n"
            "    NEW,\n"
            "    FIXED,\n"
            "    BROKEN,\n"
            "    DEAD\n"
            "  } m_Status;\n"
            "  int m_Ratio;\n"
            "}"));    

    assert (a != b);
    assert (a == c);
    assert (a != d);    
    assert (a != CDataTypeInt());
    assert (whitespaceMatch(a.Field("m_Status"), "enum\n"
            "{\n"
            "  NEW,\n"
            "  FIXED,\n"
            "  BROKEN,\n"
            "  DEAD\n"
            "}"));


    b.AddField("m_Other", CDataTypeDouble());

    a.AddField("m_Sum", CDataTypeInt());

    assert (whitespaceMatch(a, "struct\n"
            "{\n"
            "  int m_Length;\n"
            "  enum\n"
            "  {\n"
            "    NEW,\n"
            "    FIXED,\n"
            "    BROKEN,\n"
            "    DEAD\n"
            "  } m_Status;\n"
            "  double m_Ratio;\n"
            "  int m_Sum;\n"
            "}"));

   assert (whitespaceMatch(b, "struct\n"
            "{\n"
            "  int m_Length;\n"
            "  enum\n"
            "  {\n"
            "    NEW,\n"
            "    FIXED,\n"
            "    BROKEN,\n"
            "    READY\n"
            "  } m_Status;\n"
            "  double m_Ratio;\n"
            "  double m_Other;\n"
            "}"));

    assert (a.Sizeof() == 20);
    assert (b.Sizeof() == 24);
    try {
        a.AddField("m_Status", CDataTypeInt());
        assert ("AddField: missing exception!" == NULL);
    }
    catch (const exception &e) {
        assert (string(e.what()) == "Duplicate field: m_Status");
    }

    try {
        cout << a.Field("m_Fail") << endl;
        assert ("Field: missing exception!" == NULL);
    }
    catch (const exception &e) {
        assert (string(e.what()) == "Unknown field: m_Fail");
    }

    try {
        CDataTypeEnum en;
        en.Add("FIRST").
                Add("SECOND").
                Add("FIRST");
        assert ("Add: missing exception!" == NULL);
    }
    catch (const exception &e) {
        assert (string(e.what()) == "Duplicate enum value: FIRST");
    }

    
    return 0;
}

#endif /* __PROGTEST__ */