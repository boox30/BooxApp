#ifndef __SINOVOICE_TWO_CHAR_WORD_H__
#define __SINOVOICE_TWO_CHAR_WORD_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define	jtHWR_FUNC_CODEPAGE_GBK			936		// GBK (default)
#define jtHWR_FUNC_CODEPAGE_UNICODE		1200	// Unicode (UTF-16) little endien
void jtTwoCharWord_SetParam_CodePage(long iParamValue);

/*
 *  ������:   jtTwoCharWord_GetHightFrequencyWord
 *  �������ܣ���ȡ��Ƶ���б�                                             
 *  ����˵����                                                           
 *  szHightFrequencyWords[out]��	����������Ƶ���б����û����򿪱��ڴ�ռ�  
 *  nNumber[in]:		��ȡ����������Χ����[1��MAX_HIGHTFREQUENCYWORD_NUMBER]֮��   
 *  pContext[in]��	����ģ��                                                   
 *  ����ֵ��			-2��������֤ʧ��
 *						-1���������Ĳ���
 *						 0������ĺ��ֲ���GB2312�еĺ���
 *						������ʵ�ʻ�õĸ�Ƶ��
 */
long jtTwoCharWord_GetHightFrequencyWord(unsigned short *szHightFrequencyWords, long nNumber, const void *pContext);

/*
 *  ������:   jtTwoCharWord_GetSuffixAssociateWord
 *  �������ܣ���ȡ�ֵĺ������                                           
 *  ����˵����                                                           
 *  pwSuffixAssociateWords[out]�����������������ֱ����û����򿪱��ڴ�ռ� 
 *  nNumber[in]:		��ȡ������                                                   
 *  wWord[in]:		���ֵ�GB�����Unicode��                                        
 *  pContext[in]��	����ģ��                                                   
 *  ����ֵ��			-2��������֤ʧ��
 *						-1���������Ĳ���
 *						 0������ĺ��ֲ���GB2312�еĺ���
 *						������ʵ�ʻ�������������������޺������֣����ø�Ƶ�֣�������ʵ�ʵĸ�Ƶ����                                    
 */

long jtTwoCharWord_GetSuffixAssociateWord(unsigned short  *pwSuffixAssociateWords,long nNumber, unsigned short  wWord, const void *pContext);

/*
 *	������:       jtTwoCharWord_AdjustWordFrequency
 *	�������ܣ�	 ��������ʵ�λ��
 *	����˵��:
 *	wWord[in]:	 ���ֵ�GB�����Unicode��
 *	iIndex[in]:	 ��Ҫ�ƶ��ĺ����ں������ֱ��е�����
 *	iForward[in]: ��ǰǰ�ƶ��ĸ���
 *	pContext[in]: ����ģ��
 *	����ֵ:	     1����λ�ɹ�
 *				 0������ĺ��ֲ���GB2312�еĺ��ֻ�û�к�������
 *				-1���������Ĳ���
 *				-2��������֤ʧ��
 *				-3�����������������������������
 */
long jtTwoCharWord_AdjustWordFrequency(
	unsigned short  wWord,
	long iIndex,
	long iForward,
	void *pContext);

#ifdef __cplusplus
}
#endif

#endif