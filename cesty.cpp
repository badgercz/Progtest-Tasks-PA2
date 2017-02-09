#ifndef __PROGTEST__
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <map>
#include <set>
#include <list>
#include <algorithm>

#if defined ( __cplusplus ) && __cplusplus > 199711L	/* C++ 11 */
#include <unordered_map>
#include <unordered_set>
#include <memory>
#endif /* C++ 11 */

using namespace std;
#endif /* __PROGTEST__ */

class NoRouteException
{
};

template < typename _T , typename _E> class CZiel
{
public:
	CZiel(_T a, _E b): id(a),hrana(b) {}
	_T id;
	_E hrana;
};

template < typename _T, typename _E > class CRoute
{
public:
	CRoute(){}
	~CRoute(){}

	CRoute & Add(_T node1,  _T node2, _E  edge){
		CZiel<_T,_E> z1 = CZiel<_T,_E>(node2,edge);
		CZiel<_T,_E> z2 = CZiel<_T,_E>(node1,edge);

		if(mapa.find(node1)==mapa.end() ){
			vector<CZiel<_T,_E>> i;
			i.push_back(z1);
			mapa.insert ({node1, i});
		}
		else{
			auto & it = mapa.find(node1)->second;
			it.push_back(z1);
		}

	    if(mapa.find(node2)==mapa.end() ){
			vector<CZiel<_T,_E>> i;
			i.push_back(z2);
			mapa.insert ({node2, i});
		}
		else{
			auto & it = mapa.find(node2)->second;
			it.push_back(z2);
		}

		return *this;
	}

	list<_T> path(_T start, _T target, map<_T,_T> prev){
		list<_T> trace;
		_T x = target;
		while(true){
			trace.push_front(x);
			if(prev.find(x) != prev.end())
				x = prev.find(x)->second;
			else
				break;
		}
		return trace;

	}

	list<_T> Find(_T start, _T target, const function<bool(const _E &)>funkce ){

		if(start==target){
			list<_T> pejsek;
			pejsek.push_back(start);
			return pejsek;
		}

		if(mapa.empty() || mapa.find(start)==mapa.end()  || mapa.find(target)==mapa.end() )
			throw NoRouteException();

		deque<_T> opened;
		deque<_T> closed;
		map<_T,_T> prev;
		_T current = start;

		opened.push_back(start);

		while( !opened.empty() ){

			current = opened.front();
			opened.pop_front();

			if(current == target){
				return path(start,target,prev);
			}

			for(const auto & y : mapa.find(current)->second){
				if( (find(opened.begin(), opened.end(), y.id)==opened.end()) &&
					(find(closed.begin(), closed.end(), y.id)==closed.end()) &&
					 funkce(y.hrana)
					){
						opened.push_back(y.id);
						prev.insert( {y.id,current} );
					}
			}

			closed.push_back(current);

		}

		throw NoRouteException();
	}

	list<_T> Find(_T start, _T target){
		return Find (start, target, [] (const _E &) -> bool {return true;});
	}

private:
	map < _T , vector<CZiel<_T,_E>> > mapa;
};




#ifndef __PROGTEST__
//=================================================================================================
class CTrain
{
public:
  CTrain (const string & company,int speed):m_Company (company), m_Speed (speed)
  {
  }
  //---------------------------------------------------------------------------------------------
  string m_Company;
  int m_Speed;
};

//=================================================================================================
class TrainFilterCompany
{
public:
  TrainFilterCompany (const set < string > &companies):m_Companies (companies)
  {
  }
  //---------------------------------------------------------------------------------------------
  bool operator  () (const CTrain & train) const
  {
    return m_Companies.find (train.m_Company) != m_Companies.end ();
  }
  //---------------------------------------------------------------------------------------------
private:
    set < string > m_Companies;
};

//=================================================================================================
class TrainFilterSpeed
{
public:
  TrainFilterSpeed (int min, int max):m_Min (min), m_Max (max)
  {
  }
  //---------------------------------------------------------------------------------------------
  bool operator  () (const CTrain & train) const
  {
    return train.m_Speed >= m_Min && train.m_Speed <= m_Max;
  }
  //---------------------------------------------------------------------------------------------
private:
  int m_Min;
  int m_Max;
};

//=================================================================================================
bool
NurSchnellzug (const CTrain & zug)
{
  return (zug.m_Company == "OBB" || zug.m_Company == "DB")
    && zug.m_Speed > 100;
}

//=================================================================================================
static string
toText (const list < string > &l)
{
  ostringstream oss;

  auto it = l.cbegin ();
  oss << *it;
  for (++it; it != l.cend (); ++it)
    oss << " > " << *it;
  return oss.str ();
}
//=================================================================================================

int
main (void)
{
  CRoute < string, CTrain > lines;

  lines.Add ("Berlin", "Prague", CTrain ("DB", 120)).
  Add ("Berlin", "Prague", CTrain ("CD", 80)).
    Add ("Berlin", "Dresden", CTrain ("DB", 160)).
    Add ("Dresden", "Munchen",CTrain ("DB", 160)).
    Add ("Munchen", "Prague", CTrain ("CD", 90)).
    Add ("Munchen", "Linz", CTrain ("DB", 200)).
    Add ("Munchen", "Linz", CTrain ("OBB", 90)).
    Add ("Linz", "Prague",CTrain ("CD", 50)).
    Add ("Prague", "Wien", CTrain ("CD", 100)).
    Add ("Linz", "Wien",  CTrain ("OBB", 160)).
    Add ("Paris", "Marseille", CTrain ("SNCF", 300)).
    Add ("Paris", "Dresden", CTrain ("SNCF", 250));


  list < string > r1 = lines.Find ("Berlin", "Linz");
  assert (toText (r1) == "Berlin > Prague > Linz");

  list < string > r2 = lines.Find ("Linz", "Berlin");
  assert (toText (r2) == "Linz > Prague > Berlin");

  list < string > r3 = lines.Find ("Wien", "Berlin");
  assert (toText (r3) == "Wien > Prague > Berlin");

  list < string > r4 = lines.Find ("Wien", "Berlin", NurSchnellzug);
  assert (toText (r4) == "Wien > Linz > Munchen > Dresden > Berlin");

  list < string > r5 =
    lines.Find ("Wien", "Munchen", TrainFilterCompany (set < string >
						       {
						       "CD", "DB"}));
  assert (toText (r5) == "Wien > Prague > Munchen");



  list < string > r6 =
    lines.Find ("Wien", "Munchen", TrainFilterSpeed (120, 200));
  assert (toText (r6) == "Wien > Linz > Munchen");



  list < string > r7 = lines.Find ("Wien", "Munchen",[](const CTrain & x)
				   {
				   return x.m_Company == "CD";
				   }
  );
  assert (toText (r7) == "Wien > Prague > Munchen");



  list < string > r8 = lines.Find ("Munchen", "Munchen");
  assert (toText (r8) == "Munchen");



  list < string > r9 = lines.Find ("Marseille", "Prague");
  assert (toText (r9) == "Marseille > Paris > Dresden > Berlin > Prague"
	  || toText (r9) == "Marseille > Paris > Dresden > Munchen > Prague");



try
  {
    list<string> r10 = lines . Find ( "Marseille", "Prague", NurSchnellzug );
    assert ( "Marseille > Prague connection does not exist!!" == NULL );
  }
  catch ( const NoRouteException & e )
  {
  }



  list<string> r11 = lines . Find ( "Salzburg", "Salzburg" );
  assert ( toText ( r11 ) == "Salzburg" );

  list<string> r12 = lines . Find ( "Salzburg", "Salzburg", [] ( const CTrain & x ) { return x . m_Company == "SNCF"; }  );
  assert ( toText ( r12 ) == "Salzburg" );



  try
  {
    list<string> r13 = lines . Find ( "London", "Oxford" );
    assert ( "London > Oxford connection does not exist!!" == NULL );
  }
  catch ( const NoRouteException & e )
  {
  }


  return 0;
}
#endif  /* __PROGTEST__ */
