#ifndef __PROGTEST__

#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>

using namespace std;
#endif /* __PROGTEST__ */



class Prop {
public:				//struktura obsahující veškeré identifikace věci
	Prop(int invID, const string &name, const string &deptName, const string &city, const string &country){
		p_id = invID;
		p_name = name;
		p_DepthName = deptName;
		p_city = city;
		p_country = country;
	}

	Prop(int invID){
		p_id = invID;
	}

	Prop(const string &deptName){
		p_DepthName = deptName;
	}

	Prop(const string &deptName, const string &city, const string &country){
		p_DepthName = deptName;
		p_city = city;
		p_country = country;
	}


	int p_id;
	string p_name;
	string p_DepthName;
	string p_city;
	string p_country;
};

class CPropListMember {
   public:
   CPropListMember(int invID, const string & name){
   		m_id = invID;
   		m_name = name;
	}
	int m_id;
	string m_name;
};


class CPropList {
public:
	~CPropList(void){
		for (const auto & a : list){
			delete a;
		}
	}
	void add(int invID, const string & name){
		CPropListMember * mem = new CPropListMember(invID,name);
		list.push_back(mem);
	}

	void add(){}

    int InvID(void) const{
    	return list[i]->m_id;

    }
    string Name(void) const{
    	return list[i]->m_name;
    };
    int Remains(void) const{
    	return list.size()-i;
    };
    void Next(void){
    	i++;
    };
private:
	vector<CPropListMember*> list;
    int i = 0;

};


bool compareStr(Prop * a, Prop * b){
	if(a->p_DepthName == b->p_DepthName)
		if(a->p_city == b->p_city)
			return a->p_country < b->p_country;

	if(a->p_DepthName == b->p_DepthName)
		return a->p_city < b->p_city;

	return a->p_DepthName < b->p_DepthName;

	}

bool compareInt(Prop * a, Prop *b){
	return a->p_id < b->p_id;
}

class CInventory {
public:
    CInventory(void){};
    ~CInventory(void){
   		for ( const auto & a : PropById )
  				{
    			delete a ;
    			}


    };

#if defined (__cplusplus) && __cplusplus > 199711L  /* C++ 11 */
    // copying is not required / tested
    CInventory(const CInventory &src) = delete;
    CInventory &operator=(const CInventory &src) = delete;
#endif /* C++ 11 */

    bool AddProp(int invID, const string &name, const string &deptName, const string &city, const string &country){
    	if(find(invID)) return false;
    	Prop * proprieta = new Prop(invID, name, deptName, city, country);
    	PropById.insert(where(invID),proprieta);
    	PropByDepth.insert(where(deptName,city,country),proprieta);
    	return true;
    }

    bool DelProp(int invID){
    	if(!find(invID)) return false;														//jestli existuje dané id, false

    	auto iterator = where(invID);														//iterátor ve vektoru id na zadané id
    	Prop * pointer = * iterator;														//odkaz na strukturu na kterou je ukazováno
    	PropById.erase(iterator);															//vymazání záznamu ve vectoru id
    	for(vector<Prop*>::iterator i = where(pointer->p_DepthName,pointer->p_city,pointer->p_country); i != PropByDepth.end();++i){				//hledání id ve vektorech stejného dept názvu
    		Prop * pointer2 = *i;
    		if(pointer2==pointer){															//jestliže je struktura stejného názvu nalezena, vymaže se záznam s vectoru by dept
    			PropByDepth.erase(i);
    			break;
    		}
    	}
    	delete pointer;																		//konečně dealokace struktury v paměti




       	return true;
    }
    bool Transfer(int invID, const string &deptName, const string &city, const string &country){
    	if(!find(invID)) return false;

    	auto i = where(invID);
    	Prop * pointer = *i;
    	const string name = pointer->p_name;

    	if(deptName == pointer->p_DepthName && city == pointer->p_city && country == pointer->p_country)
    		return false;

    	DelProp(invID);
    	AddProp(invID,name,deptName,city,country);

    	return true;
    }
    CPropList PropList(const string &deptName, const string &city, const string &country) const{
       	int cnt=0;
       	CPropList list;
       	Prop pr = Prop(deptName,city,country);
    	vector<Prop*>::const_iterator l_it = lower_bound( PropByDepth.begin(), PropByDepth.end(),  &pr,compareStr ); //lambda
    	vector<Prop*>::const_iterator u_it = upper_bound( PropByDepth.begin(), PropByDepth.end(),  &pr, compareStr ); //lambda
    	int l_i = l_it - PropByDepth.begin();
    	int u_i = u_it - PropByDepth.begin();
    	for( ; l_i != u_i; ++l_i){
    		if(PropByDepth[l_i]->p_city == city && PropByDepth[l_i]->p_country == country){
    			list.add(PropByDepth[l_i]->p_id, PropByDepth[l_i]->p_name);
    			cnt++;
    		}
    	}
    	if (cnt==0)
    		list.add();

    	return list;

    };

    int PropCount(const string &deptName, const string &city, const string &country) const {
    	int cnt = 0;
    	Prop pr = Prop(deptName,city,country);
    	vector<Prop*>::const_iterator l_it = lower_bound( PropByDepth.begin(), PropByDepth.end(),  &pr, compareStr ); //lambda
    	vector<Prop*>::const_iterator u_it = upper_bound( PropByDepth.begin(), PropByDepth.end(),  &pr, compareStr ); //lambda
    	int l_i = l_it - PropByDepth.begin();
    	int u_i = u_it - PropByDepth.begin();
    	for( ; l_i != u_i; ++l_i){
    		if(PropByDepth[l_i]->p_city == city && PropByDepth[l_i]->p_country == country)
    			cnt++;
    	}
    	return cnt;
    }


private:
	vector<Prop*> PropById;
	vector<Prop*> PropByDepth;

	vector<Prop*>::iterator where(int id){ // umístění iterátoru za prvek s daným id - udržuje seřazené pole
		Prop it = Prop(id);
       	return lower_bound( PropById.begin(), PropById.end(),  &it, compareInt ); //lambda
	}

	vector<Prop*>::iterator where(const string & depth,const string & city, const string & country){ // umístění iterátoru za prvek s daným depth - udržuje seřazené pole
		Prop it = Prop(depth,city,country);
       	return lower_bound( PropByDepth.begin(), PropByDepth.end(),  &it, compareStr ); //lambda
	}


	bool find(int id){
		Prop it = Prop(id);
		return binary_search(PropById.begin(), PropById.end(), &it,compareInt);
	}


};

#ifndef __PROGTEST__

int main(void) {

    CInventory b1;
    assert (b1.AddProp(100, "Notebook", "Accounting", "Prague", "CZ") == true);
    assert (b1.AddProp(101, "Notebook", "Human Resources", "Prague", "CZ") == true);
    assert (b1.AddProp(102, "Notebook", "Accounting", "Brno", "CZ") == true);
    assert (b1.AddProp(103, "Notebook", "Accounting", "Prague", "USA") == true);
    assert (b1.AddProp(104, "Desktop PC", "Accounting", "Prague", "CZ") == true);

   // b1.vypis();
    cout << endl;

    assert (b1.PropCount("Accounting", "Prague", "CZ") == 2);
    assert (b1.PropCount("Ac", "fg", "CZ") == 0);
    assert (b1.Transfer(102, "Accounting", "Brno", "CZ")==false);

    //b1.vypis();
    cout<<endl;

    for (CPropList l = b1.PropList("Accounting", "Prague", "CZ"); l.Remains(); l.Next())
        cout << l.InvID() << ' ' << l.Name() << endl;
    // the following 2 items, in any order:

    // 100 Notebook
    // 104 Desktop PC

    cout << endl;



    assert (b1.PropCount("Human Resources", "Prague", "CZ") == 1);
    for (CPropList l = b1.PropList("Human Resources", "Prague", "CZ"); l.Remains(); l.Next())
        cout << l.InvID() << ' ' << l.Name() << endl;
    // the following property

    // 101 Notebook

    assert (b1.PropCount("Accounting", "Brno", "CZ") == 1);
    for (CPropList l = b1.PropList("Accounting", "Brno", "CZ"); l.Remains(); l.Next())
        cout << l.InvID() << ' ' << l.Name() << endl;
    // the following property

    // 102 Notebook

    assert (b1.PropCount("Accounting", "Prague", "USA") == 1);
    for (CPropList l = b1.PropList("Accounting", "Prague", "USA"); l.Remains(); l.Next())
        cout << l.InvID() << ' ' << l.Name() << endl;
    // the following property

    // 103 Notebook

    assert (b1.Transfer(104, "Accounting", "Prague", "USA") == true);
    assert (b1.PropCount("Accounting", "Prague", "CZ") == 1);
    for (CPropList l = b1.PropList("Accounting", "Prague", "CZ"); l.Remains(); l.Next())
        cout << l.InvID() << ' ' << l.Name() << endl;
    // the following property

    // 100 Notebook
    assert (b1.PropCount("Accounting", "Prague", "USA") == 2);
    for (CPropList l = b1.PropList("Accounting", "Prague", "USA"); l.Remains(); l.Next())
        cout << l.InvID() << ' ' << l.Name() << endl;
    // the following 2 items, in any order:

    // 103 Notebook
    // 104 Desktop PC


   assert (b1.DelProp(104) == true);
   assert (b1.PropCount("Accounting", "Prague", "USA") == 1);
    for (CPropList l = b1.PropList("Accounting", "Prague", "USA"); l.Remains(); l.Next())
       cout << l.InvID() << ' ' << l.Name() << endl;
   //  the following property
    // 103 Notebook



 assert (b1.AddProp(104, "Chair", "Public relations", "Paris", "France") == true);
    assert (b1.PropCount("Public relations", "Paris", "France") == 1);
    for (CPropList l = b1.PropList("Public relations", "Paris", "France"); l.Remains(); l.Next())
        cout << l.InvID() << ' ' << l.Name() << endl;
    // the following property

    // 104 Chair



   CInventory b2;
    assert (b2.AddProp(100, "Notebook", "Accounting", "Prague", "CZ") == true);
    assert (b2.AddProp(101, "Notebook", "Human Resources", "Prague", "CZ") == true);
    assert (b2.AddProp(100, "Table", "CEO Office", "Prague", "CZ") == false);
    assert (b2.DelProp(102) == false);
    assert (b2.Transfer(102, "CEO Office", "Prague", "CZ") == false);
    assert (b2.Transfer(100, "Accounting", "Prague", "CZ") == false);
    assert (b2.DelProp(100) == true);
    assert (b2.DelProp(100) == false);
    assert (b2.AddProp(100, "blbosti__fff  frg  tgtgřřřžžřfregwtg", "Zpfdkdcctaqcca", "Labpnejhippa", "Xouffagfoqhbba") == true);
    assert (b2.PropCount("Facility Services", "Prague", "CZ") == 0);
    for (CPropList l = b2.PropList("CEO Office", "Prague", "USA"); l.Remains(); l.Next())
        cout << l.InvID() << ' ' << l.Name() << endl;
    // empty output

b2.PropList ( "Hqoekfvcupgbba", "Hfbijpeftgfcca", "Vewpoctacnmcca" );


   for (CPropList k = b2.PropList("Zpfdkdcctaqcca", "Labpnejhippa", "Xouffagfoqhbba"); k.Remains(); k.Next())
        cout << k.InvID() << ' ' << k.Name() << endl;
assert(b2.DelProp(100)==true);
 for (CPropList k = b2.PropList("Zpfdkdcctaqcca", "Labpnejhippa", "Xouffagfoqhbba"); k.Remains(); k.Next())
        cout << k.InvID() << ' ' << k.Name() << endl;


    for (size_t i = 0; i < 3000; i++)
		assert(b2.AddProp(10000+i, "Notebook", "Accounting", "Prague"+to_string(i), "CZ") == true);

    for (size_t i = 0; i < 3000; i++)
		assert(b2.Transfer(10000+i, "Accounting", "Dobřív", "CZ") == true);

	assert(b2.PropCount("Accounting", "Dobřív", "CZ")==3000);

//	 for (CPropList k = b2.PropList("Accounting", "Dobřív", "CZ"); k.Remains(); k.Next())
  //      cout << k.InvID() << ' ' << k.Name() << endl;



	for( size_t i = 1000; i < 3000;i++){
	assert (b2.AddProp(100+i, "Notebook", "Accounting", "Prague", "CZ") == true);
    assert (b2.AddProp(100-i, "Notebook", "Human Resources", "Prague", "CZ") == true);
    assert (b2.AddProp(30000+i, "Table", "CEO Office", "Prague", "CZ") == true);
   //b2.DelProp(i);
    assert (b2.Transfer(100+i, "CEO Office", "Prague"+to_string(i), "CZ") == true);
    assert (b2.Transfer(100-i, "Accounting", "Prague", "CZ"+to_string(i)) == true);
	}



     return 0;
}



#endif /* __PROGTEST__ */
