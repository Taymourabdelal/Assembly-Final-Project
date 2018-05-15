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
 int byte4 [8000][2],byte8[4000][2],byte16[2000][2] ,byte32[1000][2], byte64[500][2] ,byte128[250][2];
// -------- Global variables ----------
 // Controls direct mapping size

#include <time.h>



//-----FA-------------------
int FAI = 0; // The Fully Associative index

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
    
    
    
}

void Setassociative_tagindex(int setSize,int addr, string &index, string &tag)
{
    string strAddr ;
    cout <<endl<<addr<<endl;
    strAddr = decToBin(to_string(addr));
    cout << endl<<strAddr<<endl;
    cout << tag << "hi";
    
    if(setSize==4)
    {
        index=strAddr.substr(19,10);
        tag = strAddr.substr(2,18);

    }
    else  if(setSize==8)
    {
        index=strAddr.substr(20,9);
        tag = strAddr.substr(1,19);
    }
    else  if(setSize==16)
    {
        index=strAddr.substr(21,8);
        tag = strAddr.substr(1,20);
    }

    else  if(setSize==2)
    {
        index=strAddr.substr(18,11);
        tag = strAddr.substr(1,17);

    }

    

    
}
// ------- Hit Ration Function ------------------
float hitRatio ( int hit , int miss) // Gets the hit ratio
{
    float totAc = hit + miss ;
    
    return hit / totAc ;
}

// ------- Set associative Function ------------------

void SetAssociative2 (int address , int a[][6], int & hit , int & miss )
{
    int tag , index , valid;
    string strAddr , strTag , strIndex;
    int FIFOCounter = 0;
    time_t timer;
    bool hitten = false;
    int rep=0;
    int sizes=2;
    Setassociative_tagindex (sizes, address , strIndex , strTag);
    
    cout << "Tag:" << strTag << endl << "Index"<< strIndex<< endl;
    tag = binToDec( strTag);
    index = binToDec(strIndex);
    cout << "Index dec:" << index << endl;
    

    
    
    
    if ((a[index][0] == tag) && (a[index][1] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
             if ((a[index][3] == tag) && (a[index][4] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    
    if(hitten == false)
        {
            
            for(int i=1; i<6;i+=3)
            {
                if(a[index][i]==0)
                    
                {
                    miss++;
                    timer = time(NULL);
                    a[index][i-1] = tag;
                    a[index][i] = 1;
                    a[index][i+1] = timer;
                    rep=1;
                    break;
                }
            }
        }
        
        if(hitten==false && rep==0)
        {
            
            miss++;
            
            for(int i=2;i+3<6;i+=3)
            {
                if(a[index][i]<a[index][i+3])
                    
                    FIFOCounter=i;
                else
                    
                    FIFOCounter=i+3;
            }
            {
                timer = time(NULL);
                
                a[index][FIFOCounter-2] = tag;
                a[index][FIFOCounter-1] = 1;
                a[index][FIFOCounter] = timer;
            }
            
        }

    
    
    
    cout << " MISS:" <<  miss <<endl <<"hit:" << hit <<endl;
    cout << "Hit ratio : " << hitRatio ( hit , miss) << endl;
}


void SetAssociative4 (int address , int a[][12], int & hit , int & miss )
{
    int tag , index , valid;
    string strAddr , strTag , strIndex;
    int FIFOCounter = 0;
    time_t timer;
    bool hitten =false;
    int sizes = 4;
    int rep=0;
    Setassociative_tagindex (sizes, address , strIndex , strTag);
    
    cout << "Tag:" << strTag << endl << "Index"<< strIndex<< endl;
    tag = binToDec( strTag);
    index = binToDec(strIndex);
    cout << "Index dec:" << index << endl;
    
    
    
        
        
        if ((a[index][0] == tag) && (a[index][1] == 1))// tag and valid
        {
            hit++;
            hitten =true;
            rep=1;
        }
        if ((a[index][3] == tag) && (a[index][4] == 1))// tag and valid
        {
            hit++;
            hitten =true;
            rep=1;
        }
        if ((a[index][6] == tag) && (a[index][7] == 1))// tag and valid
        {
            hit++;
            hitten =true;
            rep=1;
        }
        if ((a[index][9] == tag) && (a[index][10] == 1))// tag and valid
        {
            hit++;
            hitten =true;
            rep=1;
        }
    
        if(hitten == false)
        {
            for(int i=1; i<12;i+=3)
        {
            if(a[index][i]==0)
                
            {
                miss++;
                timer = time(NULL);
                a[index][i-1] = tag;
                a[index][i] = 1;
                a[index][i+1] = timer;
                rep=1;
                break;
            }
        }
        }
    
     if(hitten==false && rep==0)
           {
               
               miss++;
               
               for(int i=2;i+3<12;i+=3)
               {
                   if(a[index][i]<a[index][i+3])
                       
                       FIFOCounter=i;
                   else
                       
                       FIFOCounter=i+3;
               }
               {
                   timer = time(NULL);
                   
                   a[index][FIFOCounter-2] = tag;
                   a[index][FIFOCounter-1] = 1;
                   a[index][FIFOCounter] = timer;
               }
               
           }

    
    cout << " MISS:" <<  miss <<endl <<"hit:" << hit <<endl;
    cout << "Hit ratio : " << hitRatio ( hit , miss) << endl;
}
void SetAssociative8 (int address , int a[][24], int & hit , int & miss )
{
    int tag , index , valid;
    string strAddr , strTag , strIndex;
    int FIFOCounter = 0;
    time_t timer;
    bool hitten = false;
    int sizes = 8;
    int rep=0;
    
    Setassociative_tagindex (sizes, address , strIndex , strTag);
    
    cout << "Tag:" << strTag << endl << "Index"<< strIndex<< endl;
    tag = binToDec( strTag);
    index = binToDec(strIndex);
    cout << "Index dec:" << index << endl;
    
    
    
    
    
    if ((a[index][0] == tag) && (a[index][1] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][3] == tag) && (a[index][4] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][6] == tag) && (a[index][7] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][9] == tag) && (a[index][10] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][12] == tag) && (a[index][13] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][15] == tag) && (a[index][16] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][18] == tag) && (a[index][19] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][21] == tag) && (a[index][22] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if(hitten == false)
    {
        for(int i=1; i<24;i+=3)
        {
            if(a[index][i]==0)
                
            {
                miss++;
                timer = time(NULL);
                a[index][i-1] = tag;
                a[index][i] = 1;
                a[index][i+1] = timer;
                rep=1;
                break;
            }
        }
    }

    
    if(hitten==false && rep==0)
    {
        miss++;
        for(int i=2;i+3<24;i+=3)
        {
            if(a[index][i]<a[index][i+3])
                
                FIFOCounter=i;
            else
                
                FIFOCounter=i+3;
        }
        {
            timer = time(NULL);
            
            a[index][FIFOCounter-2] = tag;
            a[index][FIFOCounter-1] = 1;
            a[index][FIFOCounter] = timer;
        }
        
    }
    
    
    cout << " MISS:" <<  miss <<endl <<"hit:" << hit <<endl;
    cout << "Hit ratio : " << hitRatio ( hit , miss) << endl;
}

void SetAssociative16 (int address , int a[][48], int & hit , int & miss )
{
    int tag , index , valid;
    string strAddr , strTag , strIndex;
    int FIFOCounter = 0;
    time_t timer;
    bool hitten = false;
    int sizes = 16;
    int rep=0;
    Setassociative_tagindex (sizes, address , strIndex , strTag);
    
    cout << "Tag:" << strTag << endl << "Index"<< strIndex<< endl;
    tag = binToDec( strTag);
    index = binToDec(strIndex);
    cout << "Index dec:" << index << endl;
    
    
    
    
    
    if ((a[index][0] == tag) && (a[index][1] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    
    if ((a[index][3] == tag) && (a[index][4] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][6] == tag) && (a[index][7] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][9] == tag) && (a[index][10] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][12] == tag) && (a[index][13] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][15] == tag) && (a[index][16] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][18] == tag) && (a[index][19] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][21] == tag) && (a[index][22] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][24] == tag) && (a[index][25] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][27] == tag) && (a[index][28] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][30] == tag) && (a[index][31] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][33] == tag) && (a[index][34] == 1))// tag and valid
    {
        hit++;
        rep=1;
        hitten =true;
    }
    if ((a[index][36] == tag) && (a[index][37] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][39] == tag) && (a[index][40] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    
    if ((a[index][42] == tag) && (a[index][43] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    if ((a[index][45] == tag) && (a[index][46] == 1))// tag and valid
    {
        hit++;
        hitten =true;
        rep=1;
    }
    
    if(hitten == false)
    {
        for(int i=1; i<48;i+=3)
        {
            if(a[index][i]==0)
                
            {
                miss++;
                timer = time(NULL);
                a[index][i-1] = tag;
                a[index][i] = 1;
                a[index][i+1] = timer;
                rep=1;
                break;
                
            }
        }
    }

    if(hitten==false && rep==0)
    {
        miss++;
        for(int i=2;i+3<48;i+=3)
        {
            if(a[index][i]<a[index][i+3])
                
                FIFOCounter=i;
            else
                
                FIFOCounter=i+3;
        }
        {
            timer = time(NULL);
            
            a[index][FIFOCounter-2] = tag;
            a[index][FIFOCounter-1] = 1;
            a[index][FIFOCounter] = timer;
        }
        
    }
    
    
    cout << " MISS:" <<  miss <<endl <<"hit:" << hit <<endl;
    cout << "Hit ratio : " << hitRatio ( hit , miss) << endl;
}


//----------------Fully Associative Function ---------------

void fullyAssociative (int address , int a[][3], int & hit , int & miss , int FApolicy , int & AMATWT , int & AMATWB)
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
                    } // hahahah
            }
           
           if (hitf == false)
           {
                   miss++;
                   a[FAI][0] = tag;
                   a[FAI][1] = 1;
                   FAI -- ;
               
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
                   }
               }
               
               if (hitf == false)
               {
                   miss++;
                   a[FAI][0] = tag;
                   a[FAI][1] = 1;
                   FAI++;
                   
               }
           }
         
          
        
           
       }
      else if (FApolicy == 3)
      {
          bool hitf = false;
          for ( int i =0 ; i < FAI ; i ++)
          {
              if ((a[i][0] == tag) && (a[i][1] == 1)) // tag and valid
              {
                  hitf = true;
                  if (hitf = true)
                      FAI = i;
                  hit ++;
                  
              }
          }
          
          if (hitf == false)
          {
              miss++;
              a[FAI][0] = tag;
              a[FAI][1] = 1;
           
              
          }
          
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
              int i = rand()%125;
              miss++;
              a[i][0] = tag;
              a[i][1] = 1;
          }
          
      }
  }
    
    
  
   // cout << "Hits" << hit << "Miss" << miss << endl;
    
   // cout<<"FAI"<<FAI<<endl;


   
}
// ------------- Direct Mapping Function ----------------------
void directMapping ( int a[][2] ,int n ,  int address ,int & hit , int & miss , int wordsize)
{
    int tag , index , valid;
    string strAddr , strTag , strIndex;
    

    getTagIndex (address , strIndex , strTag , wordsize);
   
   // cout << "Tag:" << strTag << endl << "Index"<< strIndex<< endl;
    tag = binToDec( strTag);
   index = binToDec(strIndex);
   // cout << "Index dec:" << index << endl;

    
    
    
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

void nullifyArray2( int a[][2] , int n )
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

   
    int FA [125][3] ;
    int amatWBFA = 0 , amatWTFA = 0 ;

    int byte4 [8000][2],byte8[4000][2],byte16[2000][2] ,byte32[1000][2], byte64[500][2] ,byte128[250][2];
    
    
    int set4 [1024][12],set8[512][24],set16[256][48] ,set2[2048][6];
    

    cacheResType r;
    
    srand((unsigned) time (NULL));
    
    unsigned int addr;
    
    
    
    cout << "Cache Simulator\n";

  
//     cout <<"----------------- MEM GEN 1----------------------" << endl;
//    
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//        
//        directMapping(byte4, 8000, mem, Hit, Miss, 4);
//        
//        
//        
//    }
//    
//    
//    
//    cout << "Direct Mapping Hits 4 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    Hit = 0 ;
//    Miss = 0 ;
//    
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//        
//        directMapping(byte8, 4000, mem, Hit, Miss, 8);
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
//    Hit = 0 ;
//    Miss = 0 ;
//    
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//        
//        directMapping(byte16, 2000, mem, Hit, Miss, 16);
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
//    
//    Hit = 0 ;
//    Miss = 0 ;
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//        
//        directMapping(byte32, 1000, mem, Hit, Miss, 32);
//        
//        
//        
//    }
//    
//    
//    
//    cout << "Direct Mapping Hits 32 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    Hit = 0 ;
//    Miss = 0 ;
//    
//    
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//        
//        directMapping(byte64, 500, mem, Hit, Miss, 64);
//        
//        
//        
//    }
//    
//    
//    
//    cout << "Direct Mapping Hits 64 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    
//    Hit = 0 ;
//    Miss = 0 ;
//    
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen1();
//        
//        directMapping(byte128, 250, mem, Hit, Miss, 128);
//        
//    }
//    
//    
//    
//    
//    
//    cout << "Direct Mapping Hits 128 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    
//    Hit = 0 ;
//    Miss = 0 ;
    
   cout <<"----------------- MEM GEN 2----------------------" << endl;
    
    nullifyArray2( byte4 , 8000 );
    
    for (int i = 0 ; i <1000000 ; i++)
    {
        mem = memGen2();
        
        directMapping(byte4, 8000, mem, Hit, Miss, 4);
        
     
        
    }
    

    
       cout << "Direct Mapping Hits 4 Bytes: " << Hit << " Misses" << Miss << endl;
    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
    Hit = 0 ;
    Miss = 0 ;
    nullifyArray2( byte8 , 4000);
    for (int i = 0 ; i <1000000 ; i++)
    {
        mem = memGen2();
        
        directMapping(byte8, 4000, mem, Hit, Miss, 8);
        
        
        
    }
    
  
    
    
    cout << "Direct Mapping Hits 8 Bytes: " << Hit << " Misses" << Miss << endl;
    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
    Hit = 0 ;
    Miss = 0 ;
    nullifyArray2( byte16 , 2000 );
    
    for (int i = 0 ; i <1000000 ; i++)
    {
        mem = memGen2();
        
        directMapping(byte16, 2000, mem, Hit, Miss, 16);
        
        
        

        
        SetAssociative2(mem , set2, Hit , Miss);

    }
    
  
    
    cout << "Direct Mapping Hits 16 Bytes: " << Hit << " Misses" << Miss << endl;
    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
    
    
    Hit = 0 ;
    Miss = 0 ;
    
    nullifyArray2( byte32 , 1000 );
    for (int i = 0 ; i <1000000 ; i++)
    {
        mem = memGen2();
        
        directMapping(byte32, 1000, mem, Hit, Miss, 32);
        
        
        
    }
    

    
    cout << "Direct Mapping Hits 32 Bytes: " << Hit << " Misses" << Miss << endl;
    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
    Hit = 0 ;
    Miss = 0 ;
    nullifyArray2( byte64 , 500 );
    
    for (int i = 0 ; i <1000000 ; i++)
    {
        mem = memGen2();
        
        directMapping(byte64, 500, mem, Hit, Miss, 64);
        
        
        
    }

    
    
    cout << "Direct Mapping Hits 64 Bytes: " << Hit << " Misses" << Miss << endl;
    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
    
    Hit = 0 ;
    Miss = 0 ;
    nullifyArray2( byte128 , 250 );
    for (int i = 0 ; i <1000000 ; i++)
    {
        mem = memGen2();
        
        directMapping(byte128, 250, mem, Hit, Miss, 128);
        
    }
    


    
    
    cout << "Direct Mapping Hits 128 Bytes: " << Hit << " Misses" << Miss << endl;
    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
    
    Hit = 0 ;
    Miss = 0 ;
    
//
//    cout <<"----------------- MEM GEN 3----------------------" << endl;
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        directMapping(byte4, 8000, mem, Hit, Miss, 4);
//
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 4 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    Hit = 0 ;
//    Miss = 0 ;
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        directMapping(byte8, 4000, mem, Hit, Miss, 8);
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
//    Hit = 0 ;
//    Miss = 0 ;
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//        
//        directMapping(byte16, 2000, mem, Hit, Miss, 16);
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
//
//    Hit = 0 ;
//    Miss = 0 ;
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        directMapping(byte32, 1000, mem, Hit, Miss, 32);
//
//
//
//    }
//    
//
//
//    cout << "Direct Mapping Hits 32 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//    Hit = 0 ;
//    Miss = 0 ;
//
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//
//        directMapping(byte64, 500, mem, Hit, Miss, 64);
//        
//
//
//    }
//
//
//
//    cout << "Direct Mapping Hits 64 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//
//    Hit = 0 ;
//    Miss = 0 ;
//
//    for (int i = 0 ; i <1000000 ; i++)
//    {
//        mem = memGen3();
//        
//        directMapping(byte128, 250, mem, Hit, Miss, 128);
//
//    }
//
//
//
//
//    
//    cout << "Direct Mapping Hits 128 Bytes: " << Hit << " Misses" << Miss << endl;
//    cout <<"Hit Ratio " << hitRatio(Hit, Miss) <<endl;
//
//    Hit = 0 ;
//    Miss = 0 ;
//
//    for (int i = 0 ; i <10000 ; i++)
//    {
//    mem = memGen2();
//
//        mem = rand()%10;
//        fullyAssociative(mem, FA , Hit , Miss ,1 , amatWBFA , amatWTFA );
//        
//    }
//    
//    cout << " FIFO MISS:" << Miss << " HIT: " << Hit <<endl;
//    Hit = 0 ;
//    Miss = 0;
//    nullifyArray( FA , 125 );
//    FAI = 0;
//     fullFlag = false;
//   
//    for (int i = 0 ; i <10000 ; i++)
//    {
//        mem = memGen2();
// 
//        mem = rand()%10;
//        fullyAssociative(mem, FA , Hit , Miss ,2 ,amatWBFA , amatWTFA);
//    }
//      cout << "  LIFO MISS:" << Miss << " HIT: " << Hit << endl;
//    Hit = 0 ;
//    Miss = 0;
//     nullifyArray( FA , 125 );
//     FAI = 0;
//    fullFlag = false;
//    
//    for (int i = 0 ; i <10000 ; i++)
//    {
//        mem = memGen2();
//  mem = rand()%10;
//        fullyAssociative(mem, FA , Hit , Miss ,3 , amatWBFA , amatWTFA );
//    }
//      cout << " MRU MISS:" << Miss << " HIT: " << Hit <<endl;
//    Hit = 0 ;
//    Miss = 0;
//     nullifyArray( FA , 125 );
//   FAI = 0;
//     fullFlag = false;
//    
//    for (int i = 0 ; i <10000 ; i++)
//    {
//        mem = memGen2();
//
//        fullyAssociative(mem, FA , Hit , Miss ,4 , amatWBFA , amatWTFA);
//    }
//      cout << " Random MISS:" << Miss << " HIT: " << Hit <<endl;
//    

    
    // change the number of iterations into a minimum of 1,000,000
    //    for(iter=0;iter<100000;iter++)
    //    {
    //        addr = memGen1();
    //        r = cacheSim(addr);
    //        cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
    //    }
    //commit
}
