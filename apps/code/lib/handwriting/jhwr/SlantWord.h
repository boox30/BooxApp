#ifndef __SINOVOICE_SLANT_WORD_H__
#define __SINOVOICE_SLANT_WORD_H__
/************************************************************************/
/* ����˵������б����                                                   */
/************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif 


/*
 *	�������ܣ�������бУ������б�ıʼ��㰴һ���ĽǶȽ���У�������
 *  ����:
 *  pnStrokeBufferIn[in]	:Ҫ����У���ıʼ��㼯��
 *	pnStrokeBufferOut[out]	:У������ıʼ��㼯��
 * ����ֵ		-2:���ܼ��ʧ��
 *				-1: ��������							
 *				0: У��ʧ�ܣ������У����ıʼ���Ϊԭ�ʼ���
 *				��0: У���ɹ�
 */
long jtSlantWord_Correct(
	short *pnStrokeBufferIn,
	short *pnStrokeBufferOut );
 
#ifdef __cplusplus
} // extern "C"
#endif

#endif	//__SINOVOICE_SLANT_WORD_H__