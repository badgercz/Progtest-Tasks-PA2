
#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#if defined ( __cplusplus ) && __cplusplus > 199711L /* C++ 11 */
#include <unordered_map>
#include <unordered_set>
#include <memory>
#endif /* C++ 11 */
using namespace std;
#endif /* __PROGTEST__ */

class CDate
{
 	public:
    	// CDate ( y, m, d )
    	CDate(){}
    	CDate(int y, int m, int d){
    		m_y = y;
    		m_m = m;
	    	m_d = d;
    	}

		int m_y;
		int m_m;
		int m_d;

		bool operator > (CDate a){
			if(m_y == a.m_y && m_m == a.m_m)
        		return m_d > a.m_d;
      		if(m_y == a.m_y)
        		return m_m > a.m_m;

      		return m_y > a.m_y;
		}
	    bool operator < (CDate a){
			if(m_y == a.m_y && m_m == a.m_m)
        		return m_d < a.m_d;
      		if(m_y == a.m_y)
        		return m_m < a.m_m;

      		return m_y < a.m_y;

		}

	    bool operator <= (CDate a){
			if(m_y == a.m_y && m_m == a.m_m)
        		return m_d <= a.m_d;
      		if(m_y == a.m_y)
        		return m_m <= a.m_m;

      		return m_y <= a.m_y;

		}

};

struct CGoodsInfo{
	CGoodsInfo(CDate expiryDate, int count){
		g_expiry = expiryDate;
		g_cnt = count;
	}

	CDate g_expiry;
	int g_cnt;
};

bool cmpEx(CGoodsInfo * a, CGoodsInfo * b){
	return a->g_expiry < b->g_expiry;
}

bool cmpPair(pair<string,int> & a, pair<string,int> & b){
	return a.second > b.second ;
}

class CSupermarket
{
  public:
    CSupermarket(){}
    ~CSupermarket(){
    	for ( const auto & kocicka : inventary){
    		for ( const auto & pejsek : kocicka.second)
    			delete pejsek;
    		}

    }


    CSupermarket & Store(string name, CDate expiryDate, int count){
    	CGoodsInfo * info = new CGoodsInfo(expiryDate,count);
    	if( inventary.find(name) == inventary.end() ){
    		vector<CGoodsInfo*> i;
    		i.insert(where(i,expiryDate),info);
    		inventary.insert( {name,i} );
    	}

    	else{
    		vector<CGoodsInfo*> & it = inventary.find(name)->second;
    		it.insert(where(it,expiryDate),info);
    	}
    	return *this;

    }

    list<pair<string,int>> Expired ( const CDate & datum ){
    	list<pair<string,int>> invList;
    	int sum=0;
    	for ( const auto & kocicka : inventary){
    		for ( const auto & pejsek : kocicka.second){
    			if(pejsek->g_expiry < datum){
    				sum+=pejsek->g_cnt;
    			}
    		}
    		if(sum!=0){
    			invList.push_back({kocicka.first, sum});
    			sum=0;
    		}
    	}
    	invList.sort(cmpPair);
    	return invList;
    }

    void delFirst(string name){
   		vector<CGoodsInfo*> & it = inventary.find(name)->second;
   		delete it.front();
   		it.erase(it.begin());
   	}

   	void ElementSell(pair<string,int> & polozka, string name){

   		int tmp;
   		vector<CGoodsInfo*> & infos = inventary.find(name)->second;
   		while(inventary.find(name) != inventary.end() && polozka.second != 0 ){
   			tmp = infos.front()->g_cnt - polozka.second;
   			if(tmp==0){
   				delFirst(name);
   				polozka.second = -1;
   				break;
   			}
   			else if(tmp > 0){
   				infos.front()->g_cnt = tmp;
   				polozka.second = -1;
   				break;
   			}
   			else{
   				delFirst(name);
   				if(infos.size()==0)
   					inventary.erase(name);
   				polozka.second = (-1) * tmp;
   			}
   		}


   	}

   	void Sell ( list<pair<string,int>> & nakup){
   		auto copy = inventary;
   		for(auto & polozka : nakup){
   			if(inventary.find(polozka.first) != inventary.end() ){
   				ElementSell(polozka,polozka.first);
   				continue;
   			}
        	string tmp = CompareMistake(polozka.first,copy);
        	if(tmp != "")
        		ElementSell(polozka,tmp);
        }
		for(auto i = nakup.begin(); i != nakup.end();){
			if(i->second==(-1) )
				i = nakup.erase(i);
			else
				++i;
		}
   	} //konec fce


 void vypis(){
   		for ( const auto & kocicka : inventary){
    		for ( const auto & pejsek : kocicka.second)
    			cout << kocicka.first << " "<<pejsek->g_cnt  << " " << pejsek->g_expiry.m_y << "-" << pejsek->g_expiry.m_m  <<"-" << pejsek->g_expiry.m_d << endl;
    		}
    	}



  private:
  	unordered_map< string , vector<CGoodsInfo*>, hash<string> > inventary;

  	vector<CGoodsInfo*>::iterator where(vector<CGoodsInfo*> & v, CDate ex){
  		CGoodsInfo pejsek = CGoodsInfo(ex,0);
  		return lower_bound(v.begin(), v.end(), &pejsek, cmpEx);
  	}

  	string CompareMistake(string candidate, unordered_map< string , vector<CGoodsInfo*>, hash<string> > copy ){
  		string tmp;
  		int match = 0;
  		int mistake;
  		unsigned int i =  0;
  		for(auto k = copy.begin(); k != copy.end(); ++k){

  			mistake = 0;
  			string comp = k->first;
  			if(comp.length() != candidate.length()){
  				continue;
  			}
  			else{
				for(i=0; i < candidate.length(); ++i ){
					if(candidate[i]==comp[i]){
						continue;
					}
					else{
						mistake++;
						continue;
					}
  				}
  				if(mistake < 2){
  					tmp = comp;
  					match++;
  				}
  				else{
  					continue;
  				}
  			}
  		}
  		if(match > 1){
  			return "";
  		}
  		else if(match == 0){
  			return "";
  		}
  		else{
  		return tmp;
  		}
  	}

};
#ifndef __PROGTEST__
int main ( void )
{
  CSupermarket s;
  s . Store ( "bread", CDate ( 2016, 4, 30 ), 100 ).
      Store ( "butter", CDate ( 2016, 5, 10 ), 10 ).
      Store ( "beer", CDate ( 2016, 8, 10 ), 50 ) .
      Store ( "bread", CDate ( 2016, 4, 25 ), 100 ) .
      Store ( "okey", CDate ( 2016, 7, 18 ), 5 );



  list<pair<string,int> > l0 = s . Expired ( CDate ( 2018, 4, 30 ) );


  assert ( l0 . size () == 4 );
  assert ( ( l0 == list<pair<string,int> > { { "bread", 200 }, { "beer", 50 }, { "butter", 10 }, { "okey", 5 } } ) );


  list<pair<string,int> > l1 { { "bread", 2 }, { "Coke", 5 },{ "butter", 20 } };
 s . Sell ( l1 );


 assert ( l1 . size () == 2 );
 assert ( ( l1 == list<pair<string,int> > { { "Coke", 5 }, { "butter", 10 } } ) );



  list<pair<string,int> > l2 = s . Expired ( CDate ( 2016, 4, 30 ) );


  assert ( l2 . size () == 1 );
  assert ( ( l2 == list<pair<string,int> > { { "bread", 98 } } ) );






  list<pair<string,int> > l3 = s . Expired ( CDate ( 2016, 5, 20 ) );
  assert ( l3 . size () == 1 );
  assert ( ( l3 == list<pair<string,int> > { { "bread", 198 } } ) );




  list<pair<string,int> > l4 { { "bread", 105 } };
  s . Sell ( l4 );
  assert ( l4 . size () == 0 );
  assert ( ( l4 == list<pair<string,int> > {  } ) );



  list<pair<string,int> > l5 = s . Expired ( CDate ( 2017, 1, 1 ) );
  assert ( l5 . size () == 3 );
  assert ( ( l5 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 } } ) );



  s . Store ( "Coke", CDate ( 2016, 12, 31 ), 10 );
  list<pair<string,int> > l6 { { "Cake", 1 }, { "Coke", 1 }, { "cake", 1 }, { "coke", 1 }, { "cuke", 1 }, { "Cokes", 1 } };
  s . Sell ( l6 );
  assert ( l6 . size () == 3 );
  assert ( ( l6 == list<pair<string,int> > { { "cake", 1 }, { "cuke", 1 }, { "Cokes", 1 } } ) );



  list<pair<string,int> > l7 = s . Expired ( CDate ( 2017, 1, 1 ) );
  assert ( l7 . size () == 4 );
  assert ( ( l7 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 7 }, { "okey", 5 } } ) );

  s . Store ( "cake", CDate ( 2016, 11, 1 ), 5 );

  list<pair<string,int> > l8 { { "Cake", 1 }, { "Coke", 1 }, { "cake", 1 }, { "coke", 1 }, { "cuke", 1 } };
  s . Sell ( l8 );
  assert ( l8 . size () == 2 );
  assert ( ( l8 == list<pair<string,int> > { { "Cake", 1 }, { "coke", 1 } } ) );

  list<pair<string,int> > l9 = s . Expired ( CDate ( 2017, 1, 1 ) );
  assert ( l9 . size () == 5 );
  assert ( ( l9 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 }, { "cake", 3 } } ) );

  list<pair<string,int> > l10 { { "cake", 15 }, { "Cake", 2 } };
  s . Sell ( l10 );
  assert ( l10 . size () == 2 );
  assert ( ( l10 == list<pair<string,int> > { { "cake", 12 }, { "Cake", 2 } } ) );


  list<pair<string,int> > l11 = s . Expired ( CDate ( 2017, 1, 1 ) );
  assert ( l11 . size () == 4 );
  assert ( ( l11 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "Coke", 6 }, { "okey", 5 } } ) );


  list<pair<string,int> > l12 { { "Cake", 4 } };
  s . Sell ( l12 );
  assert ( l12 . size () == 0 );
  assert ( ( l12 == list<pair<string,int> > {  } ) );

  list<pair<string,int> > l13 = s . Expired ( CDate ( 2017, 1, 1 ) );
  assert ( l13 . size () == 4 );
  assert ( ( l13 == list<pair<string,int> > { { "bread", 93 }, { "beer", 50 }, { "okey", 5 }, { "Coke", 2 } } ) );


  list<pair<string,int> > l14 { { "Beer", 20 }, { "Coke", 1 }, { "bear", 25 }, { "beer", 10 } };
  s . Sell ( l14 );

  assert ( l14 . size () == 1 );
  assert ( ( l14 == list<pair<string,int> > { { "beer", 5 } } ) );



  return 0;
}
#endif /* __PROGTEST__ */
