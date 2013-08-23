#ifndef __SINOVOICE_SPLIT_CHAR_H__
#define __SINOVOICE_SPLIT_CHAR_H__
/************************************************************************/
/*	����˵�����ַ��и�                                                  */
/************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif //__CPLUSPLUS



/*
 *	jtSplitCharRect	
 *	���ο����Ͷ���
 */
typedef struct 
{
	short left;			//���
    short top;			//�ϲ�
    short right;		//�Ҳ�
    short bottom;		//�²�
}jtSplitCharRect;


/*
 *	jtSplitCharBox
 *  �и�洢���ݽṹ
 */
typedef struct 
{
	jtSplitCharRect CB_bound;			//�ʻ����ο�
	unsigned char IsMerged;				//�Ƿ�ϲ�
	long  nPos;					//�и�λ��
	long  nstrIdxFirst;					//��ʼ�ʻ���
	long  nstrIdxLast;					//�����ʻ���
	long  nstrNum;						//ÿ�����ֵĵ�ĸ���
}jtSplitCharBox;

/* 
 * ��  ��: ����д�������黮�ֳ����ɸ���������
 * ��  ��: 
 * pPointArray[in]	:�洢��д��������Ļ�����
 * iPointSize[in]	:��д���󻺳����ĳ���(��ĸ���)
 * pSlitArray[out]	:��д����ָ��ָ��, �����洢���ֽ�� �ڴ����ⲿ����
 * iSplitSize[out]	:���ֵĸ���
 * ����ֵ			:-2 : û��ͨ�����ܼ��
 *					:-1	: ����Ĳ�������
 *					:0	:û�н����и�
 *					: >0:������ε��������и�õ�����
 *
 * ʾ��:
 * short pPointArray[iPointSize * 2]; 
 * Charbox pSplitArray[iSplitSize]; 
 * SplitPointArray(pPointArray, iPointSize, pSplitArray,iSplitSize);
 */
long jtSplitChar_SplitPointArray(
	short* pPointArray, 
	long iPointSize, 
	jtSplitCharBox* pSplitArray,
	long iSplitSize); 


#ifdef __cplusplus
};
#endif //__CPLUSPLUS
#endif //__SINOVOICE_SPLIT_CHAR_H__