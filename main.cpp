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
// -------- Global variables ----------
int wordSize = 8; // Controls direct mapping size

//-----FA-------------------
int FApolicy = 2 ; // controls the Fully Associative Policy
int FAI = 0; // The Fully Associative index
  bool fullFlag = false; // Bool to activate the policies
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

// Cache Simulator
cacheResType cacheSim(unsigned int addr)
{
    // This function accepts the memory address for the read/write and returns whether it caused a cache miss or a cache hit
    // The current implementation assumes there is no cache; so, every transaction is a miss
    return MISS;
}
int binToDec(string n1) // converts from binary to decimal based on the mod
{
    long long n = stol(n1);
    cout << "N"<< n << endl;
    
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
//-------------- Fully Associative Parsing -------------
void FAParse (int addr , string & tag)
{
    string strAddr ;

    strAddr = decToBin(to_string(addr));

    tag = strAddr.substr(5,27);
    
}

void getTagIndex( int addr , string & index, string & tag) // parses the string into the different bits for direct mapping
{
    string strAddr ;
    cout <<endl<<addr<<endl;
    strAddr = decToBin(to_string(addr));
    cout << endl<<strAddr<<endl;
    tag = strAddr.substr(2,17);
    cout << tag << "hi";
    
    if (wordSize == 4)
    {
        index = strAddr.substr(19,13);
    }
    else if (wordSize == 8)
    {
        index = strAddr.substr(19,12);
    }
    else if (wordSize == 16)
    {
        index = strAddr.substr(19,11);
    }
    else if (wordSize == 32)
    {
        index = strAddr.substr(19,10);
    }
    else if (wordSize == 64)
    {
        index = strAddr.substr(19,9);
    }
    else if (wordSize == 128)
    {
        index = strAddr.substr(19,8);
    }
    //hi
    
   
    
}
// ------- Hit Ration Function ------------------
float hitRatio ( int hit , int miss) // Gets the hit ratio
{
    float totAc = hit + miss ;
    
    return hit / totAc ;
}
//----------------Fully Associative Function ---------------

void fullyAssociative (int address , int a[][2], int & hit , int & miss )
{
    cout<<hit;
    string strtag;
    int tag;
    int LIFOCounter = 0;
  
    FAParse ( address , strtag);
    tag = binToDec( strtag);
    if ( FAI >= 124)
    {
        fullFlag = true;
        if (FApolicy == 1)
        {
            FAI = 0;
            fullFlag = false;
        }
        else  if (FApolicy == 2)
        {
            FAI = 125;
        }
        
       
    }
    if ( FAI == 0  && fullFlag == true)
    {
        if (FApolicy == 1)
        {
            FAI = 0;
            fullFlag = false;
        }
        else  if (FApolicy == 2)
        {
            FAI = 125;
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
            }
        }
       
        if (hitf == false)
        {
            miss++;
            a[FAI][0] = tag;
            a[FAI][1] = 1;
            FAI++;
        }
        
    } //end of first if
  else if ( fullFlag == true)
  {
       if (FApolicy == 2)
       {
          
           
           bool hitf = false;
           for ( int i =0 ; i < FAI ; i ++)
           {
               if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
               {
                   hitf = true;
                   hit ++;
               }
           }
           
           if (hitf == false)
           {
               miss++;
               a[FAI][0] = tag;
               a[FAI][1] = 1;
               FAI -- ;
           }
         
          
        
           
       }
      else if (FApolicy == 3)
      {
          
      }
      else if (FApolicy == 4)
      {
        
          bool hitf = false;
          for ( int i =0 ; i < FAI ; i ++)
          {
              if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
              {
                  hitf = true;
                  hit ++;
              }
          }
          
          
          
         if (hitf == false)
          {
              int i = rand()%125 - 1 ;
              miss++;
              a[i][0] = tag;
              a[i][1] = 1;
          }
          
      }
  }
    
    
  
    cout << "Hits" << hit << "Miss" << miss << endl;
    
    cout<<"FAI"<<FAI<<endl;

}
// ------------- Direct Mapping Function ----------------------
void directMapping ( int a[][2] ,int n ,  int address ,int & hit , int & miss)
{
    int tag , index , valid;
    string strAddr , strTag , strIndex;
    
    getTagIndex (address , strIndex , strTag);
   
    cout << "Tag:" << strTag << endl << "Index"<< strIndex<< endl;
    tag = binToDec( strTag);
   index = binToDec(strIndex);
    cout << "Index dec:" << index << endl;
    
 
    
    if ((a[index][0] == tag) && (a[index][1] == 1))// tag and valid
    {
        hit++;
    }
        else
        {
            miss++;
            a[index][0] = tag;
            a[index][1] = 1;
        }
    
    cout << " MISS:" <<  miss <<endl <<"hit:" << hit <<endl;
    cout << "Hit ratio : " << hitRatio ( hit , miss) << endl;
}

void nullifyArray( int a[][2] , int n )
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

int main() 
{
    int Hit , Miss = 0 ;
    int iter , mem;
    int byte4 [8000][2],byte8[4000][2],byte16[2000][2] ,byte32[1000][2], byte64[500][2] ,byte128[250][2];
    int FA [125][2] ;
    cacheResType r;
    
    unsigned int addr;
    cout << "Cache Simulator\n";
    for (int i = 0 ; i <130 ; i++)
    {
    mem = memGen2();
      
        
  
        fullyAssociative(mem, FA , Hit , Miss);
    }
    
    // change the number of iterations into a minimum of 1,000,000
//    for(iter=0;iter<100000;iter++)
//    {
//        addr = memGen1();
//        r = cacheSim(addr);
//        cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
//    }
    //commit
}
