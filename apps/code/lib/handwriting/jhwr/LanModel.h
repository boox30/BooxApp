#ifndef __LANMODEL_H__
#define __LANMODEL_H__

#define MAX_SELECT 10					//��౸ѡ���ֵĸ���
#define	jtHWR_FUNC_CODEPAGE_GBK			936		// GBK (default)
#define jtHWR_FUNC_CODEPAGE_UNICODE		1200	// Unicode (UTF-16) little endien

//�ڵ㣬ÿ���ڵ�洢����Ϣ
typedef struct GBNode
{
	unsigned short	nGBCode;//���ֵ�GB��ת�ɵ�0---6768֮�����
	unsigned short	nBestPrev;//�ýڵ�����ǰ���������0��ʼ��
	double	nCost;//���ۣ����ʵĳ˷����ж���������ɴ��ۣ�ʹ�üӷ�
}GBNode;

typedef enum IsUpdateLangModel
 {
	Update,   //��������ģ��
	NotUpate  //����������ģ��
}IsUpdateLangModel;

typedef struct RecResultStru
{
	unsigned short nGB; //ʶ���ı�ѡ����,����GB���UNICODE��
	unsigned long nDis; //ʶ��ĺ�ı�ѡ������ú��ֵ�ģ��ľ���
}RecResultStru;

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * �������ܣ����ô���ģ�ͽӿڵı��뷽ʽ
 * ����˵����
 * iParamValue[in]:ָ��ΪjtHWR_FUNC_CODEPAGE_GBK��ʾGBK��ָ��ΪjtHWR_FUNC_CODEPAGE_UNICODE��ʾUnicode
 * ����ֵ��  void
 */
void jtLanModel_SetParam_CodePage(long iParamValue);

/*
 * �������ܣ���ȡ���ʶ�����ӿڣ�����дʶ����к���ʹ�� 
 * ����˵����                                                
 * pRecResult����дʶ���������ı�ѡ���У���ά����                
 * nSenLen���þ仰�ĳ���                                          
 * pContext��������֮���ת�Ƹ��ʿ⣬�ļ�ָ����ڴ��еĵ�ַ         
 * pOut�����������ڴ��ɵ��ÿ��٣��ú���ֱ��ʹ��                    
 * bIsUpdateLangModel���Ƿ��������ģ�ͣ�Update���£�NOTUpdate������
 * pNode������ģ�Ͳ����ڴ�ռ䣬���ⲿ����
 * ����ֵ �� -2�����ܼ��ʧ��
 *			 -1���������ʧ��
 *			 1�� �ɹ�
 * ��ע�����Ҫ��������ģ�ͣ��Ƴ�ϵͳʱ���뽫pContext��ָ�ϵ����Կռ� 
 *       ���б��洦��
 */
long jtLanModel_GetBestResult( RecResultStru ** pRecResult, 
								long nSenLen, 
								void * pContext, 
								unsigned short * pOut, 
								IsUpdateLangModel bIsUpdateLangModel,
								GBNode **pNode );

#ifdef __cplusplus
}
#endif

#endif