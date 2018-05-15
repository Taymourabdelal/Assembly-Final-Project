//
//  main.cpp
//  Assembly Final Project
//
//  Created by Taymour Abdelal on 5/1/18.
//  Copyright © 2018 Taymour Abdelal. All rights reserved.
//

#include <iostream>
#include  <iomanip>
#include <math.h>
#include <string>
#include <ctime>
 int byte4 [8000][3],byte8[4000][3],byte16[2000][3] ,byte32[1000][3], byte64[500][3] ,byte128[250][3] , DMamat[250][3];
// -------- Global variables ----------
 // Controls direct mapping size

//-----FA-------------------
int FAI = 0;// The Fully Associative index
int FAIamat = 0;
  bool fullFlag = false; // Bool to activate the policies
bool LIFO = false;
//---------END of FA --------
//--------------------------------------
using namespace std;

#define        G                1
#define        DRAM_SIZE        (64*1024*1024)
#define        CACHE_SIZE        (64*1024)


enum cacheResType {MISS=0, HIT=1};

unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */

unsigned int rand_()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}

// ---------- Memory Generating Function ---------
unsigned int memGen1()
{
    static unsigned int addr=0;
    return (addr++)%(128*1024);
}

unsigned int memGen2()
{
    static unsigned int addr=0;
    return  rand_()%(32*1024);
}

unsigned int memGen3()
{
    static unsigned int addr=0;
    return (addr+=(64*1024))%(512*1024);
}

// -------- End of Memory Genrating Functions ---------
string decToBin(string x)
{
    int  num = stoi(x), bin, counter =0;
    string f = "", s= "";
    string temp;
    
    while (num > 0)
    {
        bin = num % 2;
        f +=  to_string(bin);
        num /= 2;
    }
    
    for ( long i= f.length() -1 ; i>=0; i--)
    {
        s += f[i] ;
    }
    
    if  ( s.length() < 32)
    {
        //cout<<endl<< f.length();
        temp = s;
        counter += s.length();
        s = "";
        
        
        for (int i = 0; i < 32 - counter ; i++)
        {
            s += '0';
        } // end of for
    } // end of if
    s += temp;
    
    return s;
}

// ---------------------------------------
int blockaddress(int byteaddressvariable, int blocksize)
{
    int x =0;
    x = byteaddressvariable / blocksize;
    return x;
}

int noofblocks(int cachesize, int blocksize) ///  get no bits of index
{
    int x = cachesize/blocksize;
    return x;
}

int blocknumincache(int blockaddress, int numofblocks) /// gets the index of the block
{
    int x = blockaddress % numofblocks;
    return x;
}



int binToDec(string n1) // converts from binary to decimal based on the mod
{
    long long n = stol(n1);
    //cout << "N"<< n << endl;
    
    int  decno = 0, i = 0, rem;
    while (n!=0)
    {
        rem = n%10;
        n /= 10;
        decno += rem * pow(2,i);
        ++i;
    }
    return decno;
    
}
bool readOrWrite () // Random read write gen , returnf false if write and true if read
{
    int x;
    x = rand()%101;
    {
       if (x <= 30)
       {
           return false;
       }
        else
            return true;
    }
}
//-------------- Fully Associative Parsing -------------
void FAParse (int addr , string & tag)
{
    string strAddr ;

    strAddr = decToBin(to_string(addr));

    tag = strAddr.substr(0,27);
    
}

void getTagIndex( int addr , string & index, string & tag , int wordSize) // parses the string into the different bits for direct mapping
{
    string strAddr ;
   // cout <<endl<<addr<<endl;
    strAddr = decToBin(to_string(addr));
   // cout << endl<<strAddr<<endl;
    tag = strAddr.substr(0,17);
   // cout << tag << "hi";
    
    if (wordSize == 4)
    {
        index = strAddr.substr(17,13);
    }
    else if (wordSize == 8)
    {
        index = strAddr.substr(17,12);
    }
    else if (wordSize == 16)
    {
        index = strAddr.substr(17,11);
    }
    else if (wordSize == 32)
    {
        index = strAddr.substr(17,10);
    }
    else if (wordSize == 64)
    {
        index = strAddr.substr(17,9);
    }
    else if (wordSize == 128)
    {
        index = strAddr.substr(17,8);
    }
    //hi
    
   
    
}


void getTagIndexAmat( int addr , string & index, string & tag , int wordSize) // parses the string into the different bits for direct mapping
{
    string strAddr ;
    // cout <<endl<<addr<<endl;
    strAddr = decToBin(to_string(addr));
    // cout << endl<<strAddr<<endl;
    tag = strAddr.substr(0,19);
    index = strAddr.substr(19,8);
    // cout << tag << "hi";
}

// --------- AMAT WB------------------

void AMATWBFA ( int & misscount , int dirty)
{
    bool row = readOrWrite();
    
    if (row == true)
    {
        if (dirty == 0)
        {
            misscount += 11;
            
        }
        else misscount += 21;
            
        
    }
    else if ( row == false)
    {
        if ( dirty == 0)
        {
            misscount +=1;
        }
        else misscount +=11;
    }
}

float CalculatingAMATWB ( int misses , int hits , int misscount)
{
   float missratio = misses/1000000.0;
    
   float amat;
   amat = missratio * misscount;
  
    amat = amat + hits ;

  
    return amat;
    
}

// ------------ AMAT WT------------
void  AMATWTFA(bool hitf ,int & amath)
{
    bool row =  readOrWrite( );
    
    if ( row  == false)
    {
        if ( hitf == true)
        {
            amath +=1;
        }
    }
    else
    {
        if (hitf == true)
        {
            amath +=11;
        }
    }
    
}

float CalculatingAMATWT (int hits,int misses )
{
   float missratio =  misses / 1000000.0;
float amat;
    amat = hits + (missratio * misses * 11);
    return amat;
}

// ------- Hit Ration Function ------------------
float hitRatio ( int hit , int miss) // Gets the hit ratio
{
    float totAc = 1000000.0 ;
    
    return hit / totAc ;
}
//----------------Fully Associative Function ---------------

void fullyAssociative (int address , int a[][3], int & hit , int & miss , int FApolicy , int & AMATmisscount , int & AMATWTh)
{
   // cout<<hit;
    string strtag;
    int tag;
    
  
    FAParse ( address , strtag );
    tag = binToDec( strtag);
    if ( FAI == 124)
    {
        fullFlag = true;
        if (FApolicy == 1)
        {
            FAI = 0;
            fullFlag = false;
        }
        else  if (FApolicy == 2)
        {
            LIFO = false;
        }
        
       
    }
   
    
    if ( (FAI < 125) && fullFlag == false)
    {
        bool hitf = false;
        for ( int i =0 ; i < FAI ; i ++)
        {
            if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
            {
                hitf = true;
                hit ++;
                AMATWTFA(hitf ,AMATWTh);
            }
        }
       
        if (hitf == false)
        {
            
            miss++;
            a[FAI][0] = tag;
            a[FAI][1] = 1;
            FAI++;
            AMATWBFA (AMATmisscount ,a[FAI][2]);
            a[FAI][2] = 1;
        }
        
    } //end of first if
  else if ( fullFlag == true)
  {
      if (FAI == 0)
          LIFO = true;
      
       if (FApolicy == 2)
       {
          
           if (LIFO ==  false)
           {
           bool hitf = false;
           for ( int i =0 ; i < FAI ; i ++)
            {
                if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
                    {
                       hitf = true;
                       hit ++;
                        AMATWTFA(hitf ,AMATWTh);
                    }
            }
            AMATWTFA(hitf , AMATWTh);
           if (hitf == false)
           {
                   miss++;
                   a[FAI][0] = tag;
                   a[FAI][1] = 1;
                   FAI -- ;
               AMATWBFA (AMATmisscount ,a[FAI][2]);
               a[FAI][2] = 1;
               
            }
               
           }
           
          if (LIFO == true)
           {
               bool hitf = false;
               for ( int i =0 ; i < FAI ; i ++)
               {
                   if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
                   {
                       hitf = true;
                       hit ++;
                       AMATWTFA(hitf ,AMATWTh);
                   }
               }
               
               
               if (hitf == false)
               {
                   miss++;
                   a[FAI][0] = tag;
                   a[FAI][1] = 1;
                   FAI++;
                   AMATWBFA (AMATmisscount ,a[FAI][2]);
                   a[FAI][2] = 1;
                   
               }
           }
         
          
        
           
       }
      else if (FApolicy == 3)
      {
          bool hitf = false;
          for ( int i =0 ; i < 125 ; i ++)
          {
              if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
              {
                  hitf = true;
                      FAI = i;
                  hit ++;
                  AMATWTFA(hitf ,AMATWTh);
                  
              }
          }
          
          if (hitf == false)
          {
              miss++;
              a[FAI][0] = tag;
              a[FAI][1] = 1;
              AMATWBFA (AMATmisscount ,a[FAI][2]);
              a[FAI][2] = 1;
              
           
              
          }
          
      }
      else if (FApolicy == 4)
      {
        
          bool hitf = false;
          for ( int i =0 ; i < 125 ; i ++)
          {
              if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
              {
                  hitf = true;
                  hit ++;
              }
          }
          
           AMATWTFA(hitf , AMATWTh);
          
         if (hitf == false)
          {
              int i = rand()%125;
              miss++;
              a[i][0] = tag;
              a[i][1] = 1;
              AMATWBFA (AMATmisscount ,a[i][2]);
              a[i][2] = 1;
          }
          
      }
  }
    
    
  
   // cout << "Hits" << hit << "Miss" << miss << endl;
    
   // cout<<"FAI"<<FAI<<endl;

}

// "----------------------- FA DM --------------------"

void fullyAssociativeDM (int address , int a[][3], int & hit , int & miss , int FApolicy , int & AMATmisscount , int & AMATWTh)
{
    // cout<<hit;
    string strtag;
    int tag;
    
    
    FAParse ( address , strtag );
    tag = binToDec( strtag);
    if ( FAIamat == 249)
    {
        fullFlag = true;
        if (FApolicy == 1)
        {
            FAIamat = 0;
            fullFlag = false;
        }
        else  if (FApolicy == 2)
        {
            LIFO = false;
        }
        
        
    }
    
    
    if ( (FAIamat < 250) && fullFlag == false)
    {
        bool hitf = false;
        for ( int i =0 ; i < FAIamat ; i ++)
        {
            if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
            {
                hitf = true;
                hit ++;
                AMATWTFA(hitf ,AMATWTh);
            }
        }
        
        if (hitf == false)
        {
            
            miss++;
            a[FAIamat][0] = tag;
            a[FAIamat][1] = 1;
            FAIamat++;
            AMATWBFA (AMATmisscount ,a[FAIamat][2]);
            a[FAIamat][2] = 1;
        }
        
    } //end of first if
    else if ( fullFlag == true)
    {
        if (FAIamat == 0)
            LIFO = true;
        
        if (FApolicy == 2)
        {
            
            if (LIFO ==  false)
            {
                bool hitf = false;
                for ( int i =0 ; i < FAIamat ; i ++)
                {
                    if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
                    {
                        hitf = true;
                        hit ++;
                        AMATWTFA(hitf ,AMATWTh);
                    }
                }
                AMATWTFA(hitf , AMATWTh);
                if (hitf == false)
                {
                    miss++;
                    a[FAIamat][0] = tag;
                    a[FAIamat][1] = 1;
                    FAIamat -- ;
                    AMATWBFA (AMATmisscount ,a[FAIamat][2]);
                    a[FAIamat][2] = 1;
                    
                }
                
            }
            
            if (LIFO == true)
            {
                bool hitf = false;
                for ( int i =0 ; i < FAIamat ; i ++)
                {
                    if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
                    {
                        hitf = true;
                        hit ++;
                        AMATWTFA(hitf ,AMATWTh);
                    }
                }
                
                
                if (hitf == false)
                {
                    miss++;
                    a[FAIamat][0] = tag;
                    a[FAIamat][1] = 1;
                    FAIamat++;
                    AMATWBFA (AMATmisscount ,a[FAIamat][2]);
                    a[FAIamat][2] = 1;
                    
                }
            }
            
            
            
            
        }
        else if (FApolicy == 3)
        {
            bool hitf = false;
            for ( int i =0 ; i < FAIamat ; i ++)
            {
                if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
                {
                    hitf = true;
                    if (hitf = true)
                        FAIamat = i;
                    hit ++;
                    AMATWTFA(hitf ,AMATWTh);
                    
                }
            }
            
            if (hitf == false)
            {
                miss++;
                a[FAIamat][0] = tag;
                a[FAIamat][1] = 1;
                AMATWBFA (AMATmisscount ,a[FAIamat][2]);
                a[FAIamat][2] = 1;
                
                
            }
            
        }
        else if (FApolicy == 4)
        {
            
            bool hitf = false;
            for ( int i =0 ; i < FAIamat ; i ++)
            {
                if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
                {
                    hitf = true;
                    hit ++;
                }
            }
            
            AMATWTFA(hitf , AMATWTh);
            
            if (hitf == false)
            {
                int i = rand()%250;
                miss++;
                a[i][0] = tag;
                a[i][1] = 1;
                AMATWBFA (AMATmisscount ,a[FAIamat][2]);
                a[FAIamat][2] = 1;
            }
            
        }
    }
    
    
    
    // cout << "Hits" << hit << "Miss" << miss << endl;
    
    // cout<<"FAI"<<FAI<<endl;
    
}
// ------------- Direct Mapping Function ----------------------
void directMapping ( int a[][3] ,int n ,  int address ,int & hit , int & miss , int wordsize , int & AMATmisscount , int &  AMATWTh)
{
    int tag , index , valid;
    string strAddr , strTag , strIndex;
    bool hitf = false;
    getTagIndex (address , strIndex , strTag , wordsize);
   
   // cout << "Tag:" << strTag << endl << "Index"<< strIndex<< endl;
    tag = binToDec( strTag);
   index = binToDec(strIndex);
   // cout << "Index dec:" << index << endl;
    
 
    
    if ((a[index][0] == tag) && (a[index][1] == 1))// tag and valid
    {
        hit++;
        hitf = true;
          AMATWTFA(hitf , AMATWTh);
    }
    
    
        else
        {
            miss++;
            a[index][0] = tag;
            a[index][1] = 1;
            AMATWBFA (AMATmisscount ,a[index][2]);
            a[index][2] = 1;
        }
    
//    cout << " MISS:" <<  miss <<endl <<"hit:" << hit <<endl;
//    cout << "Hit ratio : " << hitRatio ( hit , miss) << endl;
}
// -------------- Direct Mapping Amat------------------
void directMappingAmat ( int a[][3] ,int n ,  int address ,int & hit , int & miss , int wordsize , int & AMATmisscount , int &  AMATWTh)
{
    int tag , index , valid;
    string strAddr , strTag , strIndex;
    bool hitf = false;
    getTagIndexAmat (address , strIndex , strTag , wordsize);
    
    // cout << "Tag:" << strTag << endl << "Index"<< strIndex<< endl;
    tag = binToDec( strTag);
    index = binToDec(strIndex);
    // cout << "Index dec:" << index << endl;
    
    
    
    if ((a[index][0] == tag) && (a[index][1] == 1))// tag and valid
    {
        hit++;
        hitf = true;
        AMATWTFA(hitf , AMATWTh);
    }
    
    
    else
    {
        miss++;
        a[index][0] = tag;
        a[index][1] = 1;
        AMATWBFA (AMATmisscount ,a[index][2]);
        a[index][2] = 1;
    }
    
    //    cout << " MISS:" <<  miss <<endl <<"hit:" << hit <<endl;
    //    cout << "Hit ratio : " << hitRatio ( hit , miss) << endl;
}



void nullifyArray( int a[][3] , int n )
{
 

    for (int i = 0 ; i < n ;i++)
    {
        for (int j = 0 ; j < 3 ; j++)
        {
            a[i][j] = NULL;
        }
    }
}

void nullifyArray2( int a[][3] , int n )
{
    
    
    for (int i = 0 ; i < n ;i++)
    {
        for (int j = 0 ; j < 2 ; j++)
        {
            a[i][j] = NULL;
        }
    }
}



char *msg[2] = {"Miss","Hit"};
// ---------------- MAIN------------------------
int main() 
{
    int wordSize = 4;
    int FApolicy = 2;
    int Hit , Miss = 0 ;
    int iter , mem;
   
    int FA [125][3] , FAamat [250][3];
    int amatWBFA = 0 , amatWTFA = 0 ;
    int amatWBmiss = 0 , amatWThits = 0;
    cacheResType r;
    
    srand((unsigned) time (NULL));
    
    unsigned int addr;
    
//    Hit = 0 ;
//
//    cout << "Cache Simulator\n";
//
//    cout << " ------------------ MEMGEN 1---------------------" << endl;
//
//    nullifyArray2( byte4 , 8000 );
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//
//        directMapping(byte4, 8000, mem, Hit, Miss, 4 , amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 4 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    nullifyArray2( byte8 , 4000);
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//
//        directMapping(byte8, 4000, mem, Hit, Miss, 8, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//
//    cout << "Direct Mapping Hits 8 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//
//    nullifyArray2( byte16 , 2000 );
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//
//        directMapping(byte16, 2000, mem, Hit, Miss, 16, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 16 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//
//    nullifyArray2( byte32 , 1000 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//
//        directMapping(byte32, 1000, mem, Hit, Miss, 32, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 32 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    nullifyArray2( byte64 , 500 );
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//
//        directMapping(byte64, 500, mem, Hit, Miss, 64, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 64 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//
//    Hit = 0 ;
//    Miss = 0 ;
//    nullifyArray2( byte128 , 250 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//
//        directMapping(byte128, 250, mem, Hit, Miss, 128, amatWBmiss , amatWThits);
//
//    }
//
//
//
//
//
//    cout << "Direct Mapping Hits 128 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//
//
//   cout <<"----------------- MEM GEN 2----------------------" << endl;
//
//    nullifyArray2( byte4 , 8000 );
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen2();
//
//        directMapping(byte4, 8000, mem, Hit, Miss, 4 , amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//       cout << "Direct Mapping Hits 4 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    nullifyArray2( byte8 , 4000);
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen2();
//
//        directMapping(byte8, 4000, mem, Hit, Miss, 8, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//
//    cout << "Direct Mapping Hits 8 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//
//    nullifyArray2( byte16 , 2000 );
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen2();
//
//        directMapping(byte16, 2000, mem, Hit, Miss, 16, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 16 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//
//    nullifyArray2( byte32 , 1000 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen2();
//
//        directMapping(byte32, 1000, mem, Hit, Miss, 32, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 32 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    nullifyArray2( byte64 , 500 );
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen2();
//
//        directMapping(byte64, 500, mem, Hit, Miss, 64, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 64 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//
//    Hit = 0 ;
//    Miss = 0 ;
//    nullifyArray2( byte128 , 250 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen2();
//
//        directMapping(byte128, 250, mem, Hit, Miss, 128, amatWBmiss , amatWThits);
//
//    }
//
//
//
//
//
//    cout << "Direct Mapping Hits 128 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//
//    cout << " ---------------------MEM GEN 3 -----------------" << endl;
//
//    nullifyArray2( byte4 , 8000 );
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        directMapping(byte4, 8000, mem, Hit, Miss, 4 , amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 4 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    nullifyArray2( byte8 , 4000);
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        directMapping(byte8, 4000, mem, Hit, Miss, 8, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//
//    cout << "Direct Mapping Hits 8 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//
//    nullifyArray2( byte16 , 2000 );
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        directMapping(byte16, 2000, mem, Hit, Miss, 16, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 16 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//
//    nullifyArray2( byte32 , 1000 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        directMapping(byte32, 1000, mem, Hit, Miss, 32, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 32 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    nullifyArray2( byte64 , 500 );
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        directMapping(byte64, 500, mem, Hit, Miss, 64, amatWBmiss , amatWThits);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 64 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//
//    Hit = 0 ;
//    Miss = 0 ;
//    nullifyArray2( byte128 , 250 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        directMapping(byte128, 250, mem, Hit, Miss, 128, amatWBmiss , amatWThits);
//
//    }
//
//
//
//
//
//    cout << "Direct Mapping Hits 128 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;

//    cout << " ----------------- MEM GEN1----------------" <<endl;
//
//    nullifyArray2( FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//
//        fullyAssociative(mem, FA, Hit, Miss, 1, amatWBmiss, amatWThits);
//
//    }
//
//
//
//
//
//
//
//    cout << "fullyAssociative Policy 1 Hits: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    float x;
//    cout << "amat Miss" << amatWBmiss << endl;
//    x= CalculatingAMATWB(Miss, Hit, amatWBmiss);
//    cout << " AMAT Write Back : " << x <<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    FAI=0;

//    nullifyArray2( FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//
//        fullyAssociative(mem, FA, Hit, Miss, 2, amatWBmiss, amatWThits);
//
//    }
//
//
//
//
//
//
//
//    cout << "fullyAssociative Policy 2 Hits" << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    FAI = 0;
//
//    nullifyArray( FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//
//        fullyAssociative(mem, FA, Hit, Miss, 3, amatWBmiss, amatWThits);
//
//    }
//
//
//    cout << "fullyAssociative Policy 3 Hits" << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    FAI = 0;
//
//    nullifyArray2(FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//
//        fullyAssociative(mem, FA, Hit, Miss, 4, amatWBmiss, amatWThits);
//
//    }
//
//
//
//
//
//
//
//    cout << "fullyAssociative Policy 4 Hits" << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    FAI = 0;


//    cout << " ---------------------- MEM Gen 2 -----------------" << endl;
//
//    nullifyArray2( FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen2();
//
//        fullyAssociative(mem, FA, Hit, Miss, 1, amatWBmiss, amatWThits);
//
//    }
//
//
//
//
//
//
//
//    cout << "fullyAssociative Policy 1 Hits " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    FAI = 0;
//
//
//
//    nullifyArray2( FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen2();
//
//        fullyAssociative(mem, FA, Hit, Miss, 2, amatWBmiss, amatWThits);
//
//    }
//
//
//
//
//
//
//
//    cout << "fullyAssociative Policy 2 Hits " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    FAI = 0;
//
//
//
//
//    nullifyArray2( FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen2();
//
//        fullyAssociative(mem, FA, Hit, Miss, 3, amatWBmiss, amatWThits);
//
//    }
//
//
//
//
//
//
//
//    cout << "fullyAssociative Policy 3 Hits " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//    FAI = 0;
//
//
//    nullifyArray2( FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen2();
//
//        fullyAssociative(mem, FA, Hit, Miss, 4, amatWBmiss, amatWThits);
//
//    }
//
//
//
//
//
//
//
//    cout << "fullyAssociative Policy 4 Hits " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;




//    cout << "------------- MEM Gen 3--------------" << endl;
//
//
//    nullifyArray2( FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        fullyAssociative(mem, FA, Hit, Miss, 1, amatWBmiss, amatWThits);
//
//    }
//
//
//
//
//
//
//
//    cout << "fullyAssociative Policy 1 Hits " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//
//
//
//    nullifyArray2( FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        fullyAssociative(mem, FA, Hit, Miss, 2, amatWBmiss, amatWThits);
//
//    }
//
//
//
//
//
//
//
//    cout << "fullyAssociative Policy 2 Hits " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//
//
//    nullifyArray2( FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        fullyAssociative(mem, FA, Hit, Miss, 3, amatWBmiss, amatWThits);
//
//    }
//
//
//
//
//
//
//
//    cout << "fullyAssociative Policy 3 Hits" << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;
//
//
//
//    nullifyArray2( FA, 125 );
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        fullyAssociative(mem, FA, Hit, Miss, 4, amatWBmiss, amatWThits);
//
//    }
//
//
//
//
//
//
//
//    cout << "fullyAssociative Policy 4 Hits " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
//    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
//    amatWBmiss=0;
//    amatWThits=0;
//    Hit = 0 ;
//    Miss = 0 ;

    cout << " -------------------------------- AMAT --------------------------" << endl;



    cout << " ----------------- DM AMAT----------------------"<< endl;

    cout << " ----------------- MEM GEN 1----------------------"<< endl;

    nullifyArray2( DMamat , 250);

    for (int i = 0 ; i <1000000 ; i++)
    {
        mem = memGen1();

        directMappingAmat(DMamat, 250, mem, Hit, Miss, 0, amatWBmiss , amatWThits);



    }



    cout << "Direct Mapping Hits 16 Bytes: " << Hit << " Misses" << Miss << endl;
    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;

    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
    amatWBmiss=0;
    amatWThits=0;
    Hit = 0 ;
    Miss = 0 ;




    cout << " ----------------- MEM GEN 2----------------------"<< endl;

    nullifyArray2( DMamat , 250);

    for (int i = 0 ; i <1000000 ; i++)
    {
        mem = memGen2();

        directMappingAmat(DMamat, 250, mem, Hit, Miss, 0, amatWBmiss , amatWThits);



    }



    cout << "Direct Mapping Hits 16 Bytes: " << Hit << " Misses" << Miss << endl;
    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;

    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
    amatWBmiss=0;
    amatWThits=0;
    Hit = 0 ;
    Miss = 0 ;



     cout << " ----------------- MEM GEN 3----------------------"<< endl;

    nullifyArray2( DMamat , 250);

        for (int i = 0 ; i <1000000 ; i++)
        {
            mem = memGen3();

            directMappingAmat(DMamat, 250, mem, Hit, Miss, 0, amatWBmiss , amatWThits);



        }



        cout << "Direct Mapping Hits 16 Bytes: " << Hit << " Misses" << Miss << endl;
        cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;

        cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
        cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
        amatWBmiss=0;
        amatWThits=0;
        Hit = 0 ;
        Miss = 0 ;


    cout << "---------------------------- FA AMAT -----------------------"<< endl;

    cout << " ----------- MEM GEn 1--------- " << endl;

    nullifyArray2( FAamat, 125 );
    for (int i = 0 ; i <1000000 ; i++)
    {
        mem = memGen1();

        fullyAssociative(mem, FAamat, Hit, Miss, 4, amatWBmiss, amatWThits);

    }







    cout << "fullyAssociative Policy 4 Hits " << Hit << " Misses" << Miss << endl;
    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
    amatWBmiss=0;
    amatWThits=0;
    Hit = 0 ;
    Miss = 0 ;



    cout << " ----------- MEM GEn 2--------- " << endl;

    nullifyArray2( FAamat, 125 );
    for (int i = 0 ; i <1000000 ; i++)
    {
        mem = memGen2();

        fullyAssociative(mem, FAamat, Hit, Miss, 4, amatWBmiss, amatWThits);

    }







    cout << "fullyAssociative Policy 4 Hits " << Hit << " Misses" << Miss << endl;
    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
    amatWBmiss=0;
    amatWThits=0;
    Hit = 0 ;
    Miss = 0 ;






    cout << " ----------- MEM GEn 3--------- " << endl;

    nullifyArray2( FAamat, 125 );
    for (int i = 0 ; i <1000000 ; i++)
    {
        mem = memGen3();

        fullyAssociative(mem, FAamat, Hit, Miss, 4, amatWBmiss, amatWThits);

    }







    cout << "fullyAssociative Policy 4 Hits " << Hit << " Misses" << Miss << endl;
    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
    cout << " AMAT Write Through : " << CalculatingAMATWT(amatWThits,Miss)<<endl;
    cout << " AMAT Write Back : " << CalculatingAMATWB(Miss, Hit, amatWBmiss)<<endl;
    amatWBmiss=0;
    amatWThits=0;
    Hit = 0 ;
    Miss = 0 ;



}
