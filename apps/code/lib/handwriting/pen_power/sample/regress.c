#define C_SOURCE
#include <stdio.h>
#include "hwr_api.h"
#include "ink.c"
#define NO_CANDIDATE 10
POINT_TYPE RawStroke[1024];

unsigned short code_array[20];
unsigned char WorkBuf[8192];
HWRData theHWRData;
HWRBOX  box;

int main(void)
{
  short i,no_point,flag,no_cand;
  short *ptr_ink;
  struct Global_area1 *ptr_global_area1;
  unsigned short code;
  short char_count;
  unsigned long value;
  int RamSize;

  i=0;
  char_count=0;

  /* ��l�Ʈ֤ߤ��e, �@�w�n�եΥ���� */
  RamSize=PPHWRGetRamSize();
  /*
     �Q�έ�~���o��RamSize�Ӱt�m�u�@�O����;
     �p�G�O�ĥ��R�A�ŧi���}�C, �h�i�H�Ψ����Ҥj�p
  */
  theHWRData.pPrivate=(signed char*)WorkBuf; //WorkBuf���R�A�}�C

  /*
      �p�G�ƾڮw�O�P�{�ǽX���}�N�����ɭ�,
      �������w�ƾڮw���}�l�a�}.
      �ثe���Ϋ��w
  */
  /* �֤ߪ�l�� */
  if(PPHWRInit(&theHWRData)!=STATUS_OK){
    return -2;
  }
  value=ALL_TYPE;
  /* �]�w�ѧO�r�� */
  PPHWRSetType(&theHWRData,(DWORD)value);

  /* �]�w�Կ�r�Ӽ� */
  PPHWRSetCandidateNum(&theHWRData,10);

  /* �w�]���Ѽg��j�p�Φ�m�� (0,0,60,60)
     �p�G�z�����O�o�Ӽˤl, �������ܥH�U�o�Ө�ƪ��Ѽ�
  */
  box.left=box.top=0;
  box.right=box.bottom=60;
  PPHWRSetBox(&theHWRData,
	      &box);


  ptr_ink=(short *)&ink_data[0];
  flag=1;
  while(flag)
  {

      no_point=*ptr_ink++;
      if(no_point<=0)break;
      for(i=0;i<no_point;i++)
      {
          RawStroke[i].x=*ptr_ink++;
          RawStroke[i].y=*ptr_ink++;
      }

      if(PPHWRRecognize(&theHWRData,
          (WORD *)RawStroke,
          code_array)!=STATUS_OK){
              printf("Cannot Recognize!\n");
              return -3;
      }
      no_cand=0;
      while(code_array[no_cand]) no_cand++;

      if(no_cand!=*ptr_ink++)
      {
          printf("Candidate Count miss-match %d %d!\n",
              no_cand, *(ptr_ink-1));
          flag=0;
          break;
      }

      for(i=0;i<no_cand;i++)
      {
          code = *ptr_ink++;
          if(code_array[i]!=code)
          {
              printf("Candidate code miss-match %d %04x %04x!\n",
                  i,code, code_array[i]);
              flag=0;
              break;
          }
      }
      if(i!=no_cand)break;

      char_count++;
      printf("Regressioned Character=%d\n", char_count);
  }

  printf("Total Regressioned Character=%d\n",
         char_count);
  return(char_count);
}

