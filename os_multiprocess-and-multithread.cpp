#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <ctime>
#include <time.h>
#include <math.h>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <chrono>
using namespace std;

vector<int>nonsort ;
bool isexsit( string namestr ) {
  namestr = namestr + ".txt" ;
  fstream fin ;
  fin.open( namestr.c_str(), ios::in ) ;
  if( !fin ) return false ;
  else{
    int temp ;
    while ( fin >> temp ) nonsort.push_back( temp ) ;

    return true ;
  }


  fin.close() ;
  return false ;
}
vector< vector< int > > cutdata( int slice ) {
  vector< vector< int > > acct ;
  int times = nonsort.size() / slice ;
  int cooot = 0 ;
  for( int x = 0 ; x < slice ; x++ ) {
    vector< int > temp ;
    for( int y = 0 ; y < times ; y++ ) {
      temp.push_back( nonsort[cooot] ) ;
      cooot++ ;
    }


    if( x == slice - 1 ) {
      while( cooot != nonsort.size() ) {

        temp.push_back( nonsort[cooot] ) ;
        cooot++ ;
      }
    }
    acct.push_back( temp ) ;
  }
  return acct ;
}
vector<int> singlemerge( vector<int> a, vector<int> b ) {
  vector<int> temp ;
  int nowa = 0, nowb = 0 ;
  while( nowa != a.size() && nowb != b.size() ) {
    if( a[nowa] > b[nowb] ){
      temp.push_back( b[nowb] ) ;
      nowb++ ;
    }
    else {
      temp.push_back( a[nowa] ) ;
      nowa++ ;
    }
  }

  while( nowa != a.size() ) {
    temp.push_back( a[nowa] ) ;
    nowa++ ;
  }

  while( nowb != b.size() ) {
    temp.push_back( b[nowb] ) ;
    nowb++ ;
  }

  return temp ;
}
void mergemiddle ( vector<vector<int>> &middle, vector<int> a, vector<int> b ){
  vector<int> temp =  singlemerge( a, b ) ;
  middle.push_back( temp ) ;
}
bool check( int way, int slice ) {
  if( way == 1 || way == 2 || way == 3 || way == 4 ) {
    if( slice != 0 ) return true ;
    else return false ;
  }
  else return false ;

}
void bubble_sort( vector<int> &nonsort) {
  for( int x = nonsort.size() - 1 ; x > 0 ; x-- ) {
    for( int y = 0 ; y <= x - 1 ; y++ ) {
      if( nonsort[y] > nonsort[y+1] ){
        int temp = nonsort[y] ;
        nonsort[y] = nonsort[y+1] ;
        nonsort[y+1] = temp ;
        //cout << temp << "\n";
      }
    }
  }

}
vector< int > merge_sort( vector< vector< int > >m2_bblist ) {
  vector< vector< int > > middle ;
  for( int i = 0 ; i < m2_bblist.size()  ; i = i + 2 ) {
    vector< int > single ;
    if( i == m2_bblist.size()  - 1 ) single = m2_bblist[i] ;
    else single = singlemerge( m2_bblist[i], m2_bblist[i+1] ) ;
    middle.push_back( single ) ;
  }

  if( middle.size() == 1 ) {
    return middle[0] ;
  }
  else {
    return merge_sort( middle ) ;
  }

}
void bubble_fork( vector< vector< int > > &m3_list ) {
  int slices = m3_list.size() ;
  int pid[ slices ] ;
  int shmget_id = shmget( IPC_PRIVATE, nonsort.size()*sizeof( int ), IPC_CREAT | 0600 ) ;
  int* shmat_id = ( int * )shmat( shmget_id, NULL, 0) ;
  for( int i = 0 ; i < slices ; i++ ) {
    pid[i] = fork() ;
    if( pid[i] == -1 ) {
      perror( "fork error" ) ;
      break ;
    }
    else if( pid[i] == 0 ) {
      bubble_sort( m3_list[i] ) ;
      shmat_id[0] =  m3_list[i].size() ;
      for( int j = 0 ; j < m3_list[i].size() ; j++ ) shmat_id[j + 1] = m3_list[i][j] ;
      exit( 0 ) ;

    }
    else if( pid[i] > 0 ) {
      waitpid( pid[i], NULL, 0 ) ;
      for( int j = 0 ; j < shmat_id[0] ; j++ ) m3_list[i][j] = shmat_id[j + 1] ;
    }
  }
  shmdt( shmat_id ) ;
  shmctl( shmget_id, IPC_RMID, NULL ) ;
}
vector< int > merge_fork( vector< vector< int > > m3_list ) {
  key_t key = IPC_PRIVATE ;
  int slices = m3_list.size() ;
  int pid[ slices ] ;
  int shmget_id = shmget( key, nonsort.size()*sizeof( int ), IPC_CREAT | 0600 ) ;
  int* shmat_id = ( int * )shmat( shmget_id, NULL, 0) ;
  vector< vector< int > > middle ;
  for( int i = 0 ; i < slices ; i = i + 2 ) {
    pid[i] = fork() ;
    if( pid[i] == -1 ) {
      perror( "error" ) ;
      break ;
    }
    else if( pid[i] == 0 ) {
      vector< int > single ;
      if( i == m3_list.size()  - 1 ) single = m3_list[i] ;
      else single = singlemerge( m3_list[i], m3_list[i+1] ) ;
      //middle.push_back( single ) ;
      shmat_id[0] =  single.size() ;
      for( int j = 0 ; j < single.size() ; j++ ) shmat_id[j + 1] = single[j] ;

      exit( 0 ) ;
    }
    else if( pid[i] > 0 ) {
      waitpid( pid[i], NULL, 0 ) ;
      vector< int > temp ;
      for( int j = 0 ; j < shmat_id[0] ; j++ ) temp.push_back( shmat_id[j + 1] ) ;
      middle.push_back(temp) ;
      temp.clear();
    }
  }


  if( middle.size() == 1 ) {
    shmdt( shmat_id ) ;
    shmctl( shmget_id, IPC_RMID, NULL ) ;
    return middle[0] ;
  }
  else {
    shmdt( shmat_id ) ;
    shmctl( shmget_id, IPC_RMID, NULL ) ;
    return merge_fork( middle ) ;
  }





  /*
  for( int i = 0 ; i < m3_list.size() ; i++ ) {
    for( int j = 0 ; j < m3_list[i].size() ; j++ ) cout << m3_list[i][j] << "\n" ;
    cout << "\n"<< i ;
  }
*/
}
void bubble_thread( vector< vector< int > > &m4_list ) {
  vector<thread> thread_bb ;
  for( int i = 0 ; i < m4_list.size() ; i++ ) thread_bb.push_back( thread( bubble_sort, ref( m4_list[i] ) ) ) ;
  for( int i = 0 ; i < m4_list.size() ; i++ ) thread_bb[i].join() ;
}
vector< int > merge_thread( vector< vector< int > > m4_list ) {
  vector<thread> thread_mm ;
  vector< vector< int > > middle ;
  int times = 0 ;
  for( int i = 0 ; i < m4_list.size() ; i = i + 2 ) {
    vector< int > single ;
    vector< int > null ;
    if( i == m4_list.size()  - 1 )  thread_mm.push_back( thread( mergemiddle, ref(middle), m4_list[i], null ) ) ;
    else thread_mm.push_back( thread( mergemiddle, ref(middle), m4_list[i], m4_list[i+1] ) ) ;
    thread_mm[times].join() ;
    times++ ;
  }

  thread_mm.clear() ;
  if( middle.size() == 1 ) {
    return middle[0] ;
  }
  else {
    return merge_thread( middle ) ;
  }

}
string current_DateTime() {
  time_t now = time( 0 ) ;
  tm* now_tm = gmtime( &now ) ;
  char ttt[42] ;
  strftime( ttt, 42, "%Y-%m-%d %X+08:00", now_tm ) ;
  return ttt ;
}
void printall( string inputname, vector<int>sortok, double duration, string way ) {
  string outputnamestr = inputname + "_output" + way + ".txt" ;
  ofstream fout( outputnamestr.c_str() ) ;
  fout << "sort:\n" ;
  for( int i = 0 ; i < sortok.size() ; i++ ) fout << sortok[i] << "\n" ;
  fout << "CPU Time : " << duration << "\n" ;
  cout << "CPU Time : " << duration << "\n" ;
  fout << "Output Time : " << current_DateTime() << "\n" ;
  cout << "Output Time : " << current_DateTime() << "\n" ;
  fout.close() ;

}
void mission1( string filename ) {
  vector<int>m1_list = nonsort ;
  clock_t start, finish ;
  double duration ;
  start = clock() ;
  auto startt = chrono::steady_clock::now() ;
  bubble_sort( m1_list ) ;
  auto endt = chrono::steady_clock::now() ;
  finish = clock() ;
  auto du = chrono::duration_cast<chrono::milliseconds>(endt - startt) ;
  duration = (double) ( finish - start ) / CLOCKS_PER_SEC ;
  printall( filename, m1_list, du.count(), "1" ) ;
}
void mission2( string filename, int slice ) {

  clock_t start, finish ;
  double duration ;
  start = clock() ;
  auto startt = chrono::steady_clock::now() ;
  vector< vector< int > >m2_list = cutdata( slice ) ;
  vector< vector< int > >m2_mergelist ;
  for( int x = 0 ; x < slice ; x++ ) bubble_sort( m2_list[x] ) ;
  vector< int > m2_fflist = merge_sort( m2_list ) ;
  auto endt = chrono::steady_clock::now() ;
  finish = clock() ;
  auto du = chrono::duration_cast<chrono::milliseconds>(endt - startt) ;
  duration = (double) ( finish - start ) / CLOCKS_PER_SEC ;
  printall( filename, m2_fflist, du.count(), "2" ) ;


}
void mission3( string filename, int slice ) {
  clock_t start, finish ;
  double duration ;
  start = clock() ;
  auto startt = chrono::steady_clock::now() ;
  vector< vector< int > >m3_list = cutdata( slice ) ;
  bubble_fork( m3_list ) ;
  vector< int > m3_fflist = merge_fork( m3_list ) ;
  auto endt = chrono::steady_clock::now() ;
  finish = clock() ;
  auto du = chrono::duration_cast<chrono::milliseconds>(endt - startt) ;
  duration = (double) ( finish - start ) / CLOCKS_PER_SEC ;
  printall( filename, m3_fflist, du.count(), "3" ) ;


}
void mission4( string filename, int slice ) {
  clock_t start, finish ;
  double duration ;
  vector<thread> threads ;
  start = clock() ;
  auto startt = chrono::steady_clock::now() ;
  vector< vector< int > >m4_list = cutdata( slice ) ;
  bubble_thread( m4_list ) ;
  vector< int > m4_fflist = merge_thread( m4_list ) ;
  auto endt = chrono::steady_clock::now() ;
  finish = clock() ;
  auto du = chrono::duration_cast<chrono::milliseconds>(endt - startt) ;
  duration = (double) ( finish - start ) / CLOCKS_PER_SEC ;
  printall( filename, m4_fflist, du.count(), "4" ) ;


}
int main()
{
    //Sort sor ;
    string filename ;
    int slice = 0, way = 0 ;

    while(1){
      do {
        cout << "Please enter file name.  no.txt\n" ;
        cin >> filename ;
        cout << "How many do you want to cut (not 0) ?\n" ;
        cin >> slice ;
        cout << "Which way do you want to do ? (1,2,3,4)\n" ;
        cin >> way ;

      } while( !isexsit( filename ) || !check( way, slice ) ) ;

      if( way == 1 ) mission1( filename );
      else if ( way == 2 ) mission2( filename, slice ) ;
      else if ( way == 3 ) mission3( filename, slice );
      else if ( way == 4 ) mission4( filename, slice );;


      nonsort.clear() ;
      slice = 0, way = 0 ;
    }



    return 0;
}
